#include "Global.h"

cApp::cApp()
{
	m_Width = 400;
	m_Height = 400;
	_tcscpy(m_Class, TEXT("Alpha"));
	_tcscpy(m_Caption, TEXT("Alpha blend demo by pf_d"));
}

BOOL cApp::Init()
{
	// Initialize the graphics device and set display mode
	m_Graphics.Init();
	m_Graphics.SetMode(GethWnd(), TRUE, FALSE);

	sVertex Verts[8] = {
  		/*
  			实心的矩形 
  		*/
		{ 100.0f, 100.0f, 1.0f, 1.0f, D3DCOLOR_RGBA(0,64,128,255) },
		{ 300.0f, 100.0f, 1.0f, 1.0f, D3DCOLOR_RGBA(0,64,128,255) },
		{ 100.0f, 300.0f, 1.0f, 1.0f, D3DCOLOR_RGBA(0,64,128,255) },
		{ 300.0f, 300.0f, 1.0f, 1.0f, D3DCOLOR_RGBA(0,64,128,255) },
		/*
	  		半透明的矩形 
		*/
		{  50.0f, 150.0f, 1.0f, 1.0f, D3DCOLOR_RGBA(128,0,0,128)  },
		{ 350.0f, 150.0f, 1.0f, 1.0f, D3DCOLOR_RGBA(128,0,0,128)  },
		{  50.0f, 350.0f, 1.0f, 1.0f, D3DCOLOR_RGBA(128,0,0,128)  },
		{ 350.0f, 350.0f, 1.0f, 1.0f, D3DCOLOR_RGBA(128,0,0,128)  }
	};

	// 通常在Write时使用XV进行防御式检查
	m_VB.Create(&m_Graphics, 8, VERTEXFVF, sizeof(sVertex));
	XV(m_VB.Set(0, 8, Verts));

	return TRUE;
}

BOOL cApp::Frame()
{
	m_Graphics.Clear();
	if(m_Graphics.BeginScene())
	{
		m_VB.Render(0, 2, D3DPT_TRIANGLESTRIP);

		m_Graphics.GetDeviceCOM()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_Graphics.GetDeviceCOM()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_Graphics.GetDeviceCOM()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		m_VB.Render(4, 2, D3DPT_TRIANGLESTRIP);
		m_Graphics.GetDeviceCOM()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
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
