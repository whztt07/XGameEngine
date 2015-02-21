#ifndef _XStateManager_H_
#define _XStateManager_H_

#include "XGlobal.h"

namespace Xge
{
	enum Purposes 
	{
		NOPURPOSE = 0,
		INITPURPOSE,
		SHUTDOWNPURPOSE,
		FRAMEPURPOSE
	};
    
	class StateManager
	{
		typedef struct sState 
		{
			void (*Function)(void *Ptr, long Purpose);
			sState *Next;

			sState()
			{
				Function = NULL;
				Next = NULL;
			}

			~sState()
			{
				delete Next;
			}
		} sState;

	protected:
		sState *m_StateParent;

	public:
		StateManager();
		~StateManager();

		void Push(void (*Function)(void *Ptr, long Purpose), void *DataPtr = NULL);
		BOOL Pop(void *DataPtr = NULL);
		void PopAll(void *DataPtr = NULL);
		BOOL Process(void *DataPtr = NULL);
	};
}

#endif
