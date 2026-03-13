// ZeroFsm.h
#ifndef __ZeroFsm__
#define __ZeroFsm__

#include "ZeroUnknown.h"
#include "ZeroGraph.h"
#include "ZeroList.h"
#include "ZeroDebug.h"

class ZeroFsm;
class MessagePort;
class ZeroEventData;



/*============================================================================
 CLASS:		ZeroFsmDebug
 PURPOSE:   This class is used to output debug strings to a text file
 NOTES:     There should only ever be one instance of this class
=============================================================================*/
class ZeroFsmDebug : public ZeroUnknown
{
	private:
        //-----------------------
        // variable declarations
        //-----------------------
		char		acFileName[50];
		FILE*		pFile;
		static int	iNumInstances;

		int			iNumOutputs;
		int			iNumCharWrites;

	public:
        //-----------------------
        // function declarations
        //-----------------------
		ZeroFsmDebug() : ZeroUnknown()
		{
			#ifdef _DEBUG

			#if (PLATFORM != PLATFORM_GAMECUBE)
			iNumInstances ++;
			ZeroAssert (( iNumInstances == 1, "There can only be one instance of this class!"));

#if (PLATFORM == PLATFORM_XBOX)
      strcpy( acFileName, "d:\\FSMDebugOut.txt" );
#else
			strcpy( acFileName, "FSMDebugOut.txt" );
#endif
			pFile = fopen( acFileName, "wb" );
			ZeroWarn (( pFile != NULL, "Could not open file for writing" ));

			iNumOutputs = 0;
			iNumCharWrites = 0;
			#endif
			
			#endif
		}

	   ~ZeroFsmDebug()
		{
			#ifdef _DEBUG

			if ( pFile != NULL )
			{
				char* TempString = "------------ABOVE IS THE LAST LINE PRINTED---------------";

				fwrite( TempString, 1, strlen( TempString ), pFile );

				fclose( pFile );
				pFile = NULL;
			}			

			#endif
		}

		void	WriteDebugString( char *fmt, ... );
};

extern ZeroFsmDebug ZeroFsmDebugInstance;



//
// ZeroFsmTransition
//
// ZeroFsmTransitions are stored in ZeroFsmTransitionContainers
class ZeroFsmTransition : public ZeroUnknown
{
	protected:
// *** this is instance specific ***
// the fsm that this transition is a member of
ZeroFsm*	pumFsmThis;

	// list of events that can trigger this transition
	ZeroSimList< int >	limTriggerEvent;

//	virtual void	WriteDebugString( char *fmt, ... );


	public:
	ZeroFsmTransition( const char* strName, ZeroFsm* puFsm )	
		:ZeroUnknown (strName), pumFsmThis (puFsm), limTriggerEvent()
	{
	}

	ZeroFsmTransition( const char* strName, ZeroFsm* puFsm, int iEvent )	
		:ZeroUnknown (strName), pumFsmThis( puFsm ), limTriggerEvent()
	{
		AddTriggerEvent( iEvent );
	}

	void AddTriggerEvent( int iEvent )
	{
		limTriggerEvent.Append ( iEvent);
	}

	virtual bool	Guard( const ZeroEventData* puEventData )	{ return true; }
	virtual void	Action( const ZeroEventData* puEventData )	{}

	bool operator==( int iEvent ) const;

	ZeroFsm*		GetFsm() { return pumFsmThis; }
	const ZeroFsm*	GetFsm() const { return pumFsmThis; }
};


//
// ZeroFsmTransitionContainer
//
// ZeroFsmTransitionContainer are stored in arches that connect nodes of the graph
class ZeroFsmTransitionContainer : public ZeroUnknown
{
	// Note: I'm not currently storing the src and dest states in this object.
	// This is because it doesn't look like this is feasible, given the composition of the graph 
	// class (it would require deriving from the ZeroArch class which isn't really a transition)

	// class owns the allocated memory for the transitions
	protected:
// *** this is instance specific ***
// the fsm that this transition container is a member of
ZeroFsm*	pumFsmThis;

	// list of transitions
	ZeroSimList< ZeroFsmTransition* > lpumTrans;

	public:
	ZeroFsmTransitionContainer( ZeroFsm* puFsm );	
	ZeroFsmTransitionContainer( ZeroFsm* puFsm, ZeroFsmTransition* puTrans );	
	~ZeroFsmTransitionContainer();

	void AddTransition( ZeroFsmTransition* puTrans );
	ZeroFsmTransition* HasTransitionMatchingEvent( int iEvent ) const;
};


//
// ZeroFsmState
//
class ZeroFsmState : public ZeroUnknown
{
	//
	protected:
// *** this is instance specific ***
// the fsm that this state is a member of
ZeroFsm*	pumFsmThis;

	// ptr to the state's arch representation in the graph (Note: the state is referenced by 
	// possibly multiple arches, but this particular arch ptr represents the first one that was 
	// created)
	const ZeroArch*	pumArchThis;

	void	SetThisArch( const ZeroArch* puArch ) { pumArchThis = puArch; }
	virtual void	EntryActionInternal() { EntryAction(); }
	virtual void	ExitActionInternal() { ExitAction(); }
	virtual void	UpdateInternal( float dt ) { Update( dt ); }
//	virtual void	WriteDebugString( char *fmt, ... );


	public:
	enum TransitionTest
	{
		NO_TRANSITION, // no transition occurred in any sub-states
		TRANSITION_IN_SUB_STATE, // transition occurred in first sub-state
		TRANSITION_IN_DEEPER_STATE // transition occurred in a sub-state deeper than the first sub-state
	};

	ZeroFsmState( const char *strName, ZeroFsm* puFsm )
		: ZeroUnknown (strName), pumFsmThis( puFsm ), pumArchThis( NULL )
	{
	}

	ZeroFsm*		GetFsm() { return pumFsmThis; }
	const ZeroFsm*	GetFsm() const { return pumFsmThis; }

	DLL_DECLSPEC virtual TransitionTest	Transition( int iEvent, const ZeroEventData* puEventData, 
													ZeroArch** ppuArch, 
													ZeroFsmTransition** ppuTrans );
	DLL_DECLSPEC virtual void	EntryAction() {}
	DLL_DECLSPEC virtual void	ExitAction() {}
	DLL_DECLSPEC virtual void	Reset()	{} // does nothing in this base class
	DLL_DECLSPEC virtual void	Update( float dt ) {}

	// is this state blocking a terminate event (this feature is used by extension classes)
	DLL_DECLSPEC virtual bool	IsBlockingTerminate() const	{ return false; }

	friend class ZeroFsm;
	friend class ZeroFsmMetaState;
};

/* State "Template"
class "StateName" : public ZeroFsmState
{
	private:
	// data goes here

	public:
	void EntryAction() 
	{
		// code goes here
	}

	void ExitAction()
	{
		// code goes here
	}

	void Update( float dt )
	{
		// code goes here
	}
};
*/


//
// ZeroFsmMetaState
//
class ZeroFsmMetaState : public ZeroFsmState
{
	protected:
	ZeroFsmState*	pumStateStart;

// *** this is instance specific ***
ZeroFsmState* pumStateCurrent;

	const ZeroFsmState*	GetCurrentSubState() const	{ return pumStateCurrent; } 
	ZeroFsmState*		GetCurrentSubState()		{ return pumStateCurrent; } 
	DLL_DECLSPEC void	
		SetCurrentSubState( ZeroFsmState* puState );

	// this version of the constructor is necessary because when an fsm is created, it needs to 
	// create a meta state container BEFORE it knows the start state
	ZeroFsmMetaState( const char *strName, ZeroFsm* puFsm )
		: ZeroFsmState( strName, puFsm )
	{
		SetStartSubState( NULL );
	}


	public:
	ZeroFsmMetaState( const char *strName, ZeroFsm* puFsm, ZeroFsmState* puStateStart )
		: ZeroFsmState( strName, puFsm )
	{
		SetStartSubState( puStateStart );
	}

	DLL_DECLSPEC const ZeroFsmState*	GetStartSubState() const	{ return pumStateStart; } 
	DLL_DECLSPEC void					SetStartSubState( ZeroFsmState* puState );	

	DLL_DECLSPEC TransitionTest	Transition( int iEvent, const ZeroEventData* puEventData, 
											ZeroArch** ppuArch, ZeroFsmTransition** ppuTrans );
	DLL_DECLSPEC virtual void	EntryActionInternal();
	DLL_DECLSPEC virtual void	ExitActionInternal();

	// set current state back to start state
	DLL_DECLSPEC virtual void	Reset()	{ pumStateCurrent = pumStateStart; }

	DLL_DECLSPEC virtual void	UpdateInternal( float dt );

	friend class ZeroFsm;
};
/* State "Template"
class "StateName" : public ZeroFsmMetaState
{
	private:
	// data goes here

	public:
	void EntryAction() 
	{
		// code goes here
	}

	void ExitAction()
	{
		// code goes here
	}

	void Update( float dt )
	{
		// code goes here
	}
};
*/


//
// ZeroFsmRouter
//
class ZeroFsmRouter : public ZeroFsmState
{
	protected:
	// The below functions are protected because they are banned in the Router class
	void	ExitAction() {}
	void	Update( float dt ) {}
	DLL_DECLSPEC virtual void	EntryAction();

	public:
	ZeroFsmRouter( const char *strName, ZeroFsm* puFsm )
		: ZeroFsmState( strName, puFsm )
	{
	}

	DLL_DECLSPEC virtual int	Route() = 0;
};
/* Router "Template"
class "RouterName" : public ZeroFsmRouter
{
	private:
	// data goes here

	public:
	void Route() 
	{
		// code goes here
	}
};
*/


//
// ZeroFsm
//
class ZeroFsm: public ZeroGraph
{
//	virtual void	WriteDebugString( char *fmt, ... );

	//
	protected:

	// owner (this is typically a controller)
	ZeroUnknown*		pumOwner;

	// name crc
	unsigned			imId;

	// Fsm creates an outer meta state, within which the user's states are stored
	ZeroFsmMetaState*	pumState;

	// flag to indicate if this is the first time the fsm has been updated
	// (in which case, need to execute EntryAction code)
	bool				fmStarted;

	// flag to indicate whether fsm has finished
	bool				fmFinished;

	// are there any internal events
	bool				fmInternalEvent;
	// the internal event
	int					imInternalEvent;

	// flag to indicate that this fsm should block a terminate request
	bool				fmBlockTerminate;

	// flag to indicate that the fsm should terminate when we exit the blocking state
	bool				fmTerminateRequested;

	ZeroFsmTransitionContainer*	FindConnection( ZeroFsmState* puStateFrom, ZeroFsmState* puStateTo );

	//
	public:
	DLL_DECLSPEC ZeroFsm( ZeroUnknown *puOwner, const char* strName );

	DLL_DECLSPEC void	SetStartState( ZeroFsmState* puState );

	DLL_DECLSPEC bool	IsFinished() const { return fmFinished; }
	DLL_DECLSPEC void	SetFinished( bool f ) { fmFinished = f; }

	DLL_DECLSPEC void	SetInternalEvent( int i )	
						{
							fmInternalEvent = true;
							imInternalEvent = i; 
						}

	DLL_DECLSPEC bool	Terminate();
	DLL_DECLSPEC bool	GetBlockTerminate()	const { return fmBlockTerminate; }
	DLL_DECLSPEC void	SetBlockTerminate( bool f )	{ fmBlockTerminate = f; }
	DLL_DECLSPEC bool	GetTerminateRequested() const	{ return fmTerminateRequested; }

	DLL_DECLSPEC const char*			GetName() const	{ return InvertStrID( imId ); }
	DLL_DECLSPEC const ZeroUnknown*		GetOwner() const	{ return pumOwner; }
	DLL_DECLSPEC ZeroUnknown*			GetOwner() 	{ return pumOwner; }

	DLL_DECLSPEC void	AddState( ZeroFsmState* puState );
	DLL_DECLSPEC void	AddRouter( ZeroFsmRouter* puRouter )	{ AddState( puRouter ); }
	DLL_DECLSPEC void	AddTransition( ZeroFsmTransition* puTrans, ZeroFsmState* puStateFrom, 
									   ZeroFsmState* puStateTo );

	// create the fsm with states and transitions
	DLL_DECLSPEC virtual void	Program() {}

	// Transition and Update functionality has been combined to solve the problem of the first
	// update, when the fsm must execute EntryAction code.  If the two were separate and 
	// Transition was called before the first Update then we could have moved to a non-starting
	// state before executing the EntryAction code.
	DLL_DECLSPEC void	Update( float dt, const ZeroList< int >& liEvents, 
								const ZeroList< ZeroEventData* >& lupEventData );

	DLL_DECLSPEC virtual void	LogPlayerOrTeamOwnerName();
	DLL_DECLSPEC virtual void	LogTransitionName( int iEvent, const ZeroFsmTransition* puTrans );
};

typedef void (ZeroFsmTrace)(ZeroFsm *fsm, int action);

extern DLL_DECLSPEC ZeroFsmTrace *zeroFsmTrace;

/* FSM "Template"
class "FsmName" : public ZeroFsm
{
	private:
	// data goes here

	public:
	void Program() 
	{
		// define states

		// connect states

		// set start state
	}
};
*/


//
// ZeroEventData
//

class ZeroEventData : public ZeroUnknown
{
	public:
	ZeroEventData() 
		: ZeroUnknown("ctrlEVENT_DATA_NAME")
	{
	}

	virtual void	Push( MessagePort* puPort ) const	{}
	virtual void	Pop( MessagePort* puPort )	{}
};


//
// ZeroEventFactory
//

class ZeroEventFactory : public ZeroUnknown
{
	public:
	ZeroEventFactory() 
		: ZeroUnknown("ctrlFACTORY_EVENT_NAME")
	{
	}

	virtual ZeroEventData*	ProcessEvent( MessagePort* puPort, int iEvent )	
							{ 
								return new (true) ZeroEventData(); 
							}
};


//
// ZeroFsmData
//

class ZeroFsmData : public ZeroUnknown
{
	public:
	ZeroFsmData() 
		: ZeroUnknown("ctrlFSM_DATA_NAME")
	{
	}

	virtual void	Push( MessagePort* puPort ) const	{}
	virtual void	Pop( MessagePort* puPort )	{}
};


#endif //#define __ZeroFsm__
