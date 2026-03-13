#include "ZeroTypes.h"
#include "ZeroFsm.h"
#include "ZeroDebug.h"

ZeroFsmTrace *zeroFsmTrace = NULL;
ZeroFsmDebug ZeroFsmDebugInstance;


// initialize static members
int ZeroFsmDebug::iNumInstances = 0;


//
// ZeroFsmTransition
//
bool ZeroFsmTransition::operator==( int iEvent ) const
{
	ZeroList< int >::Iterator iter ( limTriggerEvent );
	for ( iter.Begin(); iter; iter++ ) 
	{
		if ( iEvent == *iter ) 
		{
			return true;
		}
	}
	return false;
}



//
// ZeroFsmTransitionContainer
//
ZeroFsmTransitionContainer::ZeroFsmTransitionContainer( ZeroFsm* puFsm )	
	: ZeroUnknown(), lpumTrans(), pumFsmThis( puFsm )
{
}


//	
ZeroFsmTransitionContainer::ZeroFsmTransitionContainer( ZeroFsm* puFsm, 
													    ZeroFsmTransition* puTrans )	
	: ZeroUnknown(), lpumTrans(), pumFsmThis( puFsm )
{
	AddTransition( puTrans );
}


//
ZeroFsmTransitionContainer::~ZeroFsmTransitionContainer()
{
	// release allocated transition memory
	ZeroSimList< ZeroFsmTransition* >::Iterator uIter( lpumTrans );
	for ( uIter.Begin(); uIter; uIter++ )
	{
		(*uIter)->Release();
	}
}


//
void ZeroFsmTransitionContainer::AddTransition( ZeroFsmTransition* puTrans )
{
	lpumTrans.Append( puTrans );
	puTrans->AddRef();
}


//
ZeroFsmTransition* ZeroFsmTransitionContainer::HasTransitionMatchingEvent( int iEvent ) const
{
	// returns NULL if no match

	ZeroSimList< ZeroFsmTransition* >::Iterator uIter( lpumTrans );
	for ( uIter.Begin(); uIter; uIter++ )
	{
		if ( *(*uIter) == iEvent )
		{
			return *uIter;
		}
	}

	// found no match
	return NULL;
}


//
// ZeroFsmState
//
ZeroFsmState::TransitionTest ZeroFsmState::Transition( int iEvent, const ZeroEventData* puEventData, 
													   ZeroArch** ppuArch, 
													   ZeroFsmTransition** ppuTrans )
{
	// cycle through this state's trigger events and test whether any match the event
	ZeroFsmTransitionContainer* puTransCtr;

	const ZeroArch* puArch = pumArchThis;
	do 
	{
		puTransCtr = static_cast< ZeroFsmTransitionContainer* >( puArch->arch );
		if ( puTransCtr != NULL ) 
		{
			ZeroFsmTransition* puTrans = puTransCtr->HasTransitionMatchingEvent( iEvent );
			if ( puTrans != NULL )
			{
				// found event
				// test guard code
				if ( puTrans->Guard( puEventData ) )
				{
					// Note: we do not execute transition action code yet
					// We need to execute current state's exit action code first
					// Caller will execute transition action code
					*ppuTrans = puTrans;
					*ppuArch = puArch->next;
					return TRANSITION_IN_SUB_STATE;				
				}
			}
		}
		puArch = puArch->twin->next;
	} while ( puArch != pumArchThis );

	// no event matched this state's trigger events
	return NO_TRANSITION;
}



//
// ZeroFsmMetaState
//
void ZeroFsmMetaState::SetCurrentSubState( ZeroFsmState* puState )	
{ 
	pumStateCurrent = puState; 

	// reset child state to starting state
	if ( pumStateCurrent != NULL )
	{
		pumStateCurrent->Reset();
	}
}


//
void ZeroFsmMetaState::SetStartSubState( ZeroFsmState* puState )
{ 
	pumStateStart = puState; 

	// set current sub-state so that meta state is "ready to go" when Update() is called
	SetCurrentSubState( pumStateStart );
}


//
void ZeroFsmMetaState::EntryActionInternal()
{
	// first do my entry action
	EntryAction();

	// then do entry action of child state
	pumStateCurrent->EntryActionInternal();
}


//
void ZeroFsmMetaState::ExitActionInternal()
{
	// first do exit action of child state
	pumStateCurrent->ExitActionInternal();

	// then do my exit action
	ExitAction();
}


//
ZeroFsmState::TransitionTest ZeroFsmMetaState::Transition( int iEvent, 
														   const ZeroEventData* puEventData, 
														   ZeroArch** ppuArchNew,
														   ZeroFsmTransition** ppuTrans )
{
	TransitionTest uTestThis;
	TransitionTest uTestSubState = GetCurrentSubState()->Transition( iEvent, puEventData, 
																	 ppuArchNew, ppuTrans );
	switch ( uTestSubState )
	{
		case NO_TRANSITION:
		{
			// can *this* state handle it
			uTestThis = ZeroFsmState::Transition( iEvent, puEventData, ppuArchNew, ppuTrans );

			break;
		}
		case TRANSITION_IN_SUB_STATE:
		{
			// current sub state is triggered by event

			ZeroFsmState* puStateNew = static_cast< ZeroFsmState* >( (*ppuArchNew)->node );
			// display transition info
			GetFsm()->LogPlayerOrTeamOwnerName();
			GetFsm()->LogTransitionName( iEvent, *ppuTrans );
			ZeroFsmDebugInstance.WriteDebugString("from old state %s to new state %s\n", 
				( GetCurrentSubState() != NULL ) ? GetCurrentSubState()->GetName() : "null", 
				puStateNew->GetName() );

			// exit action for old state (hierarchical)
			GetCurrentSubState()->ExitActionInternal();

			// update sub-state (hierarchical)
			SetCurrentSubState( puStateNew );

			// if a terminate had been requested, can we now terminate?
			if ( GetFsm()->GetTerminateRequested() && !GetFsm()->GetBlockTerminate() )
			{
				// can now terminate
				GetFsm()->SetFinished( true );
			}

			// if finished flag gets set (during any of the transitions), do not execute transition
			// code or entry action code
			if ( !GetFsm()->IsFinished() )
			{
				// Transition action
				(*ppuTrans)->Action( puEventData );

				// entry action for new state (hierarchical)
				GetCurrentSubState()->EntryActionInternal();
			}

			// since we've now handled the transition, the caller of this function should not deal
			// with anything
			uTestThis = TRANSITION_IN_DEEPER_STATE;

			break;
		}
		case TRANSITION_IN_DEEPER_STATE:
		{
			// transition occurred, but it has already been handled at a deeper level of the fsm
			uTestThis = TRANSITION_IN_DEEPER_STATE;
			break;
		}
		default:
		{
			// provide this case to (hopefully) avoid a compiler warning regarding to not 
			// initializing uTestSubState
			_ASSERTE( false );
			uTestThis = NO_TRANSITION;
		}
	}

	return uTestThis;
}


//
void ZeroFsmMetaState::UpdateInternal( float dt )
{
	// first call update on child state
	pumStateCurrent->UpdateInternal( dt );

	// then update myself
	Update( dt );
}


//
// ZeroFsmRouter
//
void ZeroFsmRouter::EntryAction() 
{
	int iInternalEvent = Route();
	GetFsm()->SetInternalEvent( iInternalEvent );
}


//
// ZeroFsm
//
ZeroFsm::ZeroFsm( ZeroUnknown *puOwner, const char* strName )
	: ZeroGraph(true), pumOwner( puOwner )
{
	if ( strName ) 
	{
		imId = CalcStrID(strName);
	}
	else
	{
		imId = 0;
	}

	// create container meta state
	pumState = new (true) ZeroFsmMetaState( "DummyMetaState", this );
	AddState( pumState );
	// release state ptr since AddState() transferred ownership to the fsm
	pumState->Release();

	fmStarted = false;
	SetFinished( false );

	fmInternalEvent = false;
	fmTerminateRequested = false;
	SetBlockTerminate( false );
}


//
void ZeroFsm::SetStartState( ZeroFsmState* puState )
{
	pumState->SetStartSubState( puState );
}


//
void ZeroFsm::AddState( ZeroFsmState* puState )
{
	// add to graph
	AddLink( puState, NULL );

	// Set "this-arch" ptr
	ZeroArch* puArch = FindNode( puState );
	_ASSERTE( puArch != NULL );
	puState->SetThisArch( puArch );
}


//
ZeroFsmTransitionContainer* ZeroFsm::FindConnection( ZeroFsmState* puStateFrom, 
													 ZeroFsmState* puStateTo )
{
	// Test whether an arch exists between two states (in either direction)
	// Note: an arch will exist if a transition has been created betwen the two states (no matter
	// which direction the transition went)
	ZeroArch* puArchStart = FindNode( puStateFrom );
	if ( puArchStart != NULL ) 
	{
		ZeroArch* puArch = puArchStart;
		do 
		{
			if ( puArch->twin->node == puStateTo )
			{
				// found connection
				_ASSERTE( puArch->arch != NULL );
				return static_cast< ZeroFsmTransitionContainer* >( puArch->arch );
			}
			puArch = puArch->twin->next;
		} while ( puArch != puArchStart );
	}

	// did not find connection
	return NULL;
}


//
void ZeroFsm::AddTransition( ZeroFsmTransition* puTrans, ZeroFsmState* puStateFrom, 
							 ZeroFsmState* puStateTo )
{
	// check whether a connection/arch already exists between the two states (in either direction)
	ZeroFsmTransitionContainer* puTransCtrFromTo = FindConnection( puStateFrom, puStateTo );
	if ( puTransCtrFromTo == NULL )
	{
		// no connection previously existed
		// create connection from puStateFrom to puStateTo
		// Note: we deliberately do not add puTrans to the below constructor because we will add 
		// it later (after the conditional ends)
		puTransCtrFromTo = new (true) ZeroFsmTransitionContainer( this );

		// Also need to create connection from puStateTo to puStateFrom (since the graph will 
		// automatically create arches between them, in both directions.  If we later call 
		// AddTransition(puTrans2, puStateTo, puStateFrom) then a connection will exist)
		// Check for transition to self - if this is the case then do not create another transition
		// (the graph won't like it since it will try to create the same arch)
		ZeroFsmTransitionContainer* puTransCtrToFrom = NULL;
		if ( puStateFrom != puStateTo )
		{
			// no self transition
			puTransCtrToFrom = new (true) ZeroFsmTransitionContainer( this );
		}
		
		AddLink( puStateFrom, puTransCtrFromTo, puStateTo, puTransCtrToFrom );
		// release transitions since AddLink added a reference in the graph class
		puTransCtrFromTo->Release();
		if ( puTransCtrToFrom != NULL )
		{
			puTransCtrToFrom->Release();
		}
	}

	// add transition to container
	puTransCtrFromTo->AddTransition( puTrans );
}


//
bool ZeroFsm::Terminate()
{
	if ( GetBlockTerminate() )
	{
		// cannot terminate
		fmTerminateRequested = true;
		return false;
	}
	else
	{
		// can terminate

		// debugging
		if (zeroFsmTrace)
			(*zeroFsmTrace)(this, 2);

		// this is the last time the fsm will be updated. Need to execute ExitAction code
		pumState->ExitActionInternal();

		SetFinished( true );

		return true;
	}
}


//
void ZeroFsm::Update( float dt, const ZeroList< int >& liEvents, 
					  const ZeroList< ZeroEventData* >& lupEventData )
{
	// if fsm has already finished, do not try to update it again (otherwise it will call update 
	// code on the last state it was in, and it will call exit action code)
	if ( !IsFinished() )
	{
		if ( !fmStarted )
		{
			// debugging
			if (zeroFsmTrace)
				(*zeroFsmTrace)(this, 1);

			// this is the first time the fsm has been updated. Need to execute EntryAction code
			pumState->EntryActionInternal();

			// set flag
			fmStarted = true;
		}

		// both lists should be the same size
		_ASSERTE( liEvents.GetCount() == lupEventData.GetCount() );

		// Note: there could initially be an internal events (if the start state was a Router)

		ZeroArch* puArch = NULL;
		ZeroFsmTransition* puTrans = NULL;
		ZeroList< int >::Iterator uIter( liEvents );
		ZeroList< ZeroEventData* >::Iterator uIterData( lupEventData );
		uIter.Begin();
		uIterData.Begin();
		bool fEventsRemain = fmInternalEvent || ( uIter != NULL && uIterData != NULL );
		// if finished flag gets set (during any of the transitions), do not test any more 
		// transitions
		while ( !IsFinished() && fEventsRemain )
		{
			// process any internal events before the external event
			// Note: we use a while loop, in case any internal events are created during the 
			// internal event transitions.
			while ( fmInternalEvent )
			{
				fmInternalEvent = false;
				// process intenal event to determine new current state
				ZeroFsmState::TransitionTest uResult;
				uResult = pumState->Transition( imInternalEvent, NULL, &puArch, &puTrans );
				ZeroAssert (( uResult != ZeroFsmState::NO_TRANSITION, "A transition has been requested, but no transtion leads out of this current state" ));
			}

			// if finished flag gets set (during any of the transitions), do not test any more 
			// transitions
			if ( !IsFinished() )
			{			
				if ( uIter != NULL && uIterData != NULL )
				{
					// process external event to determine new current state
					int iEvent = *uIter;
					ZeroEventData*& puEventData = *uIterData;
					pumState->Transition( iEvent, puEventData, &puArch, &puTrans );
					uIter++;
					uIterData++;
				}
			}

			fEventsRemain = fmInternalEvent || ( uIter != NULL && uIterData != NULL );
		}

		// call update on current state
		if ( !IsFinished() )
		{
			pumState->UpdateInternal( dt );

			// if current state's update results in the fsm finishing then need to call exit action 
			if ( IsFinished() )
			{
				// this is the last time the fsm will be updated. Need to execute ExitAction code
				pumState->ExitActionInternal();
			}	
		}

		if ( IsFinished() )
		{
			// debugging
			if (zeroFsmTrace)
				(*zeroFsmTrace)(this, 2);
		}	
	}
}


//
void ZeroFsm::LogPlayerOrTeamOwnerName() 
{ 
	ZeroFsmDebugInstance.WriteDebugString("Unknown "); 
}


//
void ZeroFsm::LogTransitionName( int iEvent, const ZeroFsmTransition* puTrans ) 
{
	ZeroFsmDebugInstance.WriteDebugString("Transition %d (%s) ", iEvent, puTrans->GetName() );
}



//
// ZeroFsmDebug
//

/******************************************************************************
 FUNCTION:  WriteDebugString
 PURPOSE:   Used to write data to a file for debug purposes.
 INPUT:     fmt is a string with optional printf tokens
 OUTPUT:    none (except for the text that is written to the debug file)
 NOTES:		
******************************************************************************/
void ZeroFsmDebug::WriteDebugString( char* fmt, ... )
{
#if defined(_DEBUG) && (PLATFORM == PLATFORM_X86_WIN32)

    //-----------------------
	// create and clear a temporary string
    //-----------------------
	char Text[2048];
	memset( Text, 0, sizeof(Text) );


    //-----------------------
	// fill in the string using the printf
	// style arguements
    //-----------------------
	va_list v_args;
	va_start (v_args, fmt);     
	vsprintf(Text, fmt, v_args);
	va_end (v_args);            


    //-----------------------
	// if we've printed enough lines
	// write out a bunch of "buffer" spaces, and
	// then reset the file pointer to the start
	// of the file.
    //-----------------------
	if ( iNumOutputs > 2000 )
	{
		iNumOutputs = 0;

		char	Temp[2048];
		memset( Temp, ' ', sizeof(Temp) );
		Temp[2046] = '\r';
		Temp[2047] = '\n';

		for ( int i = 0; i < 50; i ++ )
			fwrite( Temp, 1, strlen( Temp ), pFile );


		fseek( pFile, 0, SEEK_SET );
	}


    //-----------------------
	// write the text to the file
    //-----------------------
	fwrite( Text, 1, strlen( Text ), pFile );


    //-----------------------
	// Detect if the text that was just written
	// contains a newline or return character.  
	// If so, overwrite it with our own special
	// "carriage return" string.
    //-----------------------
	for (char* ptr = Text; *ptr; ptr++)
	{
		if ( *ptr == '\n' || *ptr == '\r' )
		{
			fseek( pFile, -1L, SEEK_CUR );
			char*	Temp = "\r\n";
			fwrite( Temp, 1, strlen( Temp ), pFile );

			iNumOutputs ++;
			return;
		}
	}

#endif
}
