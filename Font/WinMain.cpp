#include "Global.h"

class cApp : public cApplication
{
private:
	cGraphics	m_Graphics;
	cFont		m_Font;

public:
	cApp();

	BOOL Init();
	BOOL Frame();
	BOOL Shutdown();
};

cApp::cApp()
{
	_tcscpy(m_Class, TEXT("Font"));
	_tcscpy(m_Caption, TEXT("Font demo by pf_d"));
}

BOOL cApp::Init()
{
	// Initialize the graphics device and set display mode
	m_Graphics.Init();
	m_Graphics.SetMode(GethWnd(), TRUE, TRUE);

	m_Font.Create(&m_Graphics, TEXT("Arial"), 26);

	return TRUE;
}

BOOL cApp::Frame()
{
	m_Graphics.Clear();
	if(m_Graphics.BeginScene())
	{
		m_Font.Print(TEXT("Hello,world!"), 0, 0);
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
