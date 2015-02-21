#include "XStateManager.h"

namespace Xge
{
	StateManager::StateManager() 
	{ 
		m_StateParent = NULL; 
	}

	StateManager::~StateManager() 
	{
		PopAll();
	}

	// Push a function on to the stack
	void StateManager::Push(void (*Function)(void *Ptr, long Purpose), void *DataPtr)
	{
		sState *StatePtr;

		// Don't push a NULL value
		if(Function != NULL) 
		{
			// Allocate a new state and push it on stack
			StatePtr = new sState();

			StatePtr->Function = Function;
			StatePtr->Next = m_StateParent;

			m_StateParent = StatePtr;

			// Call state with init purpose
			StatePtr->Function(DataPtr, INITPURPOSE);
		}
	}

	BOOL StateManager::Pop(void *DataPtr)
	{
		sState *StatePtr;

		// Remove the head of stack (if any)
		if((StatePtr = m_StateParent) != NULL) 
		{
			// First call with shutdown purpose
			m_StateParent->Function(DataPtr, SHUTDOWNPURPOSE);

			m_StateParent = StatePtr->Next;
			StatePtr->Next = NULL;
			delete StatePtr;
		}

		// return TRUE if more states exist, FALSE otherwise
		if(m_StateParent == NULL)
			return FALSE;
		return TRUE;
	}

	void StateManager::PopAll(void *DataPtr)
	{
		while(Pop(DataPtr) == TRUE);
	}

	BOOL StateManager::Process(void *DataPtr)
	{ 
		// return an error if no more states
		if(m_StateParent == NULL)
			return FALSE;

		// Process the top-most state
		m_StateParent->Function(DataPtr, FRAMEPURPOSE); 

		return TRUE;
	}
}
