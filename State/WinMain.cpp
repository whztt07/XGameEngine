#include "Global.h"

void cApp::Func1(void *Ptr, long Purpose)
{
	cApp *App = (cApp*)Ptr;
	if(Purpose == SHUTDOWNPURPOSE)
		MB(L"1");
}

void cApp::Func2(void *Ptr, long Purpose)
{
	cApp *App = (cApp*)Ptr;
	if(Purpose == SHUTDOWNPURPOSE)
		MB(L"2");
}

void cApp::Func3(void *Ptr, long Purpose)
{
	cApp *App = (cApp*)Ptr;
	if(Purpose == SHUTDOWNPURPOSE)
		MB(L"3");
}


BOOL cApp::Init()
{
	m_StateManager.Push(Func1);
	m_StateManager.Push(Func2);
	m_StateManager.Push(Func3);
	return TRUE;
}

BOOL cApp::Frame()
{
	return m_StateManager.Pop(this);
}

int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int nCmdShow)
{
	cApp App;
	return App.Run();
}
