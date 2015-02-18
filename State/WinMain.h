#pragma once

// Macro to ease the use of MessageBox function
#define MB(s) MessageBox(NULL, s, s, MB_OK);

class cApp : public cApplication
{
private:
	cStateManager m_StateManager;

	// State function prototypes - must follow this prototype!
	static void Func1(void *Ptr, long Purpose);
	static void Func2(void *Ptr, long Purpose);
	static void Func3(void *Ptr, long Purpose);

public:
	BOOL Init();
	BOOL Frame();
};
