#include "Global.h"

class cApp : public cApplication
{
private:
	cGraphics		m_Graphics;
	cVertexBuffer	m_VB;

public:
	cApp();

	BOOL Init();
	BOOL Frame();
};

typedef struct
{
	FLOAT x, y, z, rhw;
	DWORD color;
} CUSTOMVERTEX;
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)

cApp::cApp()
{
	m_XPos = 200;
	m_YPos = 100;

	m_Width = 600;
	m_Height = 500;

	_tcscpy(m_Class, TEXT("VertexBuffer"));
	_tcscpy(m_Caption, TEXT("cVertexBuffer demo by pf_d"));
}

BOOL cApp::Init()
{
	// Initialize the graphics device and set display mode
	m_Graphics.Init();
	m_Graphics.SetMode(GethWnd(), TRUE, TRUE);

	CUSTOMVERTEX Vertices[] = 
	{
		{100.0f, 400.0f, 0.5f, 1.0f, 0xffff0000},
		{300.0f, 50.0f, 0.5f, 1.0f, 0xff00ff00},
		{500.0f, 400.0f, 0.5f, 1.0f, 0xff0000ff}
	};

	m_VB.Create(&m_Graphics, sizeof(Vertices), D3DFVF_CUSTOMVERTEX, sizeof(CUSTOMVERTEX));
	m_VB.Set(0, sizeof(Vertices), Vertices);

	return TRUE;
}

BOOL cApp::Frame()
{
	m_Graphics.Clear(D3DCOLOR_XRGB(45, 50, 170));
	if(m_Graphics.BeginScene())
	{
		m_VB.Render(0, 1, D3DPT_TRIANGLELIST);
		m_Graphics.EndScene();
	}
	m_Graphics.Display();

	return TRUE;
}

int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int nCmdShow)
{
	cApp App;
	return App.Run();
}
