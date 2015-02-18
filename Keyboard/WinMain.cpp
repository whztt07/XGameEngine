#include "Global.h"

class cApp : public cApplication
{
private:
	cGraphics m_Graphics;

	cInput			m_Input;
	cInputDevice	m_Keyboard;

public:
	cApp();

	BOOL Init();
	BOOL Frame();
	BOOL Shutdown();
};

cApp::cApp()
{
	_tcscpy(m_Class, TEXT("KeyBoard"));
	_tcscpy(m_Caption, TEXT("DirectInput demo by pf_d"));
}

BOOL cApp::Init()
{
	// Initialize the graphics device and set display mode
	m_Graphics.Init();
	m_Graphics.SetMode(GethWnd(), TRUE, TRUE);

	m_Input.Init(GethWnd(), GethInst());
	m_Keyboard.Create(&m_Input, KEYBOARD, TRUE);

	return TRUE;
}

BOOL cApp::Frame()
{
	if(m_Keyboard.Read())
	{
		if(m_Keyboard.GetKeyState(KEY_A))
		{
			MessageBox(NULL, TEXT("Pressed!"), NULL, MB_OK);
		}
	}

	m_Graphics.Clear();
	if(m_Graphics.BeginScene())
	{
		// ...
		m_Graphics.EndScene();
	}
	m_Graphics.Display();

	return TRUE;
}

BOOL cApp::Shutdown()
{
	return TRUE;
}

int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int nCmdShow)
{
	cApp App;
	return App.Run();
}
