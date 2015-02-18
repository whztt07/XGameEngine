#include "Global.h"

cApp::cApp()
{
	//--------------------------------------------------
	// Change default window settings
	//--------------------------------------------------
	m_Width = 640;
	m_Height = 480;
	lstrcpy(m_Class, L"BumpUnderWater");
	lstrcpy(m_Caption, L"BumpUnderWater by pf_d");
}

BOOL cApp::Init()
{
	// Initialize the graphics device and set display mode
	m_Graphics.Init();
	m_Graphics.SetMode(GethWnd(), TRUE, FALSE);

	IDirect3DDevice9 *pd3dDevice = m_Graphics.GetDeviceCOM();

	//设置世界矩阵
	D3DXMATRIXA16 matWorld;
	D3DXMatrixIdentity( &matWorld );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	//设置观察矩阵
	D3DXMATRIXA16 matView;
	D3DXVECTOR3 vEyePt( 0.0f, 0.0f,-100.0f );
    D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
    pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

	//设置投影矩阵
	D3DXMATRIXA16 matProj;
	float fAspectRatio = (float)m_Width / (float)m_Width;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, fAspectRatio, 1.0f, 200.0f );
    pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

	//--------------------------------------------------
	// Add your codes below...
	//--------------------------------------------------
	XV(m_BumpTex.Load(&m_Graphics, TEXT("Back.jpg")));

	return TRUE;
}

static float StartTimer;
BOOL cApp::Frame()
{
	static DWORD  Timer = timeGetTime();
	// Calculate elapsed time (plus speed boost)
	unsigned long Elapsed = timeGetTime() - Timer;
	if(Elapsed < 33.0f)
		return TRUE;
	Timer = timeGetTime();

	m_Graphics.Clear(D3DCOLOR_ARGB(0, 45, 50, 170));
	if(m_Graphics.BeginScene())
	{
		//--------------------------------------------------
		// Add your codes below...
		//--------------------------------------------------
		m_BumpTex.Render(StartTimer+=0.03f);
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
