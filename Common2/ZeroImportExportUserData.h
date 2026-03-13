#ifndef _ZEROIMPORTEXPORTUSERDATA_
#define _ZEROIMPORTEXPORTUSERDATA_

#include "Zero.h"

#define MSH_ANIMATION_USER_DATA  "animationCyclesDescription"


class CMSHBlockAnimation;

class AnimationUserDataElem
{
    public:
	char m_AnimationName[32];
	int m_StartFrame;
	int m_EndFrame;
	int m_PlayStyle;
    float m_fps;

    AnimationUserDataElem ()
    {
    	m_AnimationName[0] = 0;
	    m_StartFrame = 1;
    	m_EndFrame = 1;
	    m_PlayStyle = 0;
        m_fps = 30;
    }
};

class AnimationUserDataList
{
public:
	int m_nAnimations;
	AnimationUserDataElem *m_pAnimElements;
	AnimationUserDataList()
	{
		m_nAnimations=0;
		m_pAnimElements=NULL;
	};

	virtual ~AnimationUserDataList()
	{
		if(m_pAnimElements)
			delete[] m_pAnimElements;
	};

	void AddElement(AnimationUserDataElem &element)
	{
		bool bFound=false;
		if(m_nAnimations==0)
		{
			bFound = true;
			m_nAnimations = 1;
			m_pAnimElements = new AnimationUserDataElem[1];
			memcpy(&m_pAnimElements[0], &element, sizeof(AnimationUserDataElem));
		}
		else
		{
			for(int i =0;i<m_nAnimations && !bFound;i++)
			{
				if(strcmpi(m_pAnimElements[i].m_AnimationName,element.m_AnimationName)==0)
				{
					bFound = true;
					memcpy(&m_pAnimElements[i], &element, sizeof(AnimationUserDataElem));
				}
			}
		}

		if(!bFound)
		{
			AnimationUserDataElem *temp=m_pAnimElements;
			m_pAnimElements = new AnimationUserDataElem[m_nAnimations+1];
			memcpy(m_pAnimElements,temp,m_nAnimations*sizeof(AnimationUserDataElem));
			delete[] temp;

			m_nAnimations++;
			memcpy(&m_pAnimElements[m_nAnimations-1],&element, sizeof(AnimationUserDataElem));
		}
	};

	void RemoveElement(int idx)
	{
//		int idx=-1;
/*		for(int i =0;i<m_nAnimations && !bFound;i++)
		{
			if((strcmpi(m_pAnimElements[i].m_AnimationName,element.m_AnimationName)==0) &&
				(m_pAnimElements[i].m_StartFrame==element.m_StartFrame) &&
				(m_pAnimElements[i].m_EndFrame==element.m_EndFrame) &&
				(m_pAnimElements[i].m_PlayStyle==element.m_PlayStyle))
			{
				idx=i;
			}
		}
*/
		if((idx>=0) && (idx<m_nAnimations))
		{
			if(m_nAnimations==1)
			{
				m_nAnimations = 0;
				m_pAnimElements = NULL;
			}
			else
			{
				AnimationUserDataElem *temp=m_pAnimElements;
				m_pAnimElements = new AnimationUserDataElem[m_nAnimations-1];
				memcpy(m_pAnimElements,temp,idx*sizeof(AnimationUserDataElem));
				if(idx!=m_nAnimations-1)
					memcpy(&m_pAnimElements[idx],&temp[idx+1],(m_nAnimations-idx-1)*sizeof(AnimationUserDataElem));
				m_nAnimations--;
				delete[] temp;
			}
		}
	};
};


//struct AnimationCycleDescriptor: public UserData 
//{   
//    unsigned cycleNameCRC;
//    unsigned startFrame;
//    unsigned endFrame;
//    bool loop;
//};

#endif
    
    
            