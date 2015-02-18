#include "Global.h"

class cApp : public cApplication
{
private:
	cGraphics m_Graphics;
	CDXUTMeshFile *m_pMesh;

public:
	cApp();

	BOOL Init();
	BOOL Frame();
	BOOL Shutdown();
};

cApp::cApp()
{
	m_Width = 640;
	m_Height = 480;
	_tcscpy_s(m_Class, TEXT("UseCDXUTMeshClass"));
	_tcscpy_s(m_Caption, TEXT("UseCDXUTMeshClass demo by pf_d"));
}

BOOL cApp::Init()
{
	// Initialize the graphics device and set display mode
	m_Graphics.Init();
	m_Graphics.SetMode(GethWnd(), TRUE, TRUE);

	m_pMesh = new CDXUTMeshFile;
	m_pMesh->Create(m_Graphics.GetDeviceCOM(), TEXT("heli.x"));

	//设置观察矩阵
	D3DXMATRIXA16 matView;
	D3DXVECTOR3 vEyePt( 0.0f, 10.0f,-20 );
    D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
	m_Graphics.GetDeviceCOM()->SetTransform( D3DTS_VIEW, &matView );

	D3DXMATRIXA16 matProj;
	D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, (float)(m_Width / m_Height), 1.0f, 100.0f );
	m_Graphics.GetDeviceCOM()->SetTransform( D3DTS_PROJECTION, &matProj );

	//设置灯光
    D3DXVECTOR3 vecDir;
    D3DLIGHT9 light;
    ZeroMemory( &light, sizeof(D3DLIGHT9) );
    light.Type       = D3DLIGHT_DIRECTIONAL;
    light.Diffuse.r  = 1.0f;
    light.Diffuse.g  = 1.0f;
    light.Diffuse.b  = 1.0f;
    vecDir = D3DXVECTOR3(0.0f, -1.0f, 1.0f);
    D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &vecDir );
    m_Graphics.GetDeviceCOM()->SetLight( 0, &light );
    m_Graphics.GetDeviceCOM()->LightEnable( 0, true );
    m_Graphics.GetDeviceCOM()->SetRenderState( D3DRS_LIGHTING, TRUE );

    //设置环境光
    m_Graphics.GetDeviceCOM()->SetRenderState( D3DRS_AMBIENT, 0x00808080);

	return TRUE;
}

BOOL cApp::Frame()
{
	m_Graphics.Clear(D3DCOLOR_ARGB(0, 45, 50, 170));
	if(m_Graphics.BeginScene())
	{
		D3DXMATRIXA16 matWorld;
		D3DXMatrixRotationY( &matWorld, timeGetTime()/1000.0f );
		m_Graphics.GetDeviceCOM()->SetTransform( D3DTS_WORLD, &matWorld );

		m_pMesh->Render(m_Graphics.GetDeviceCOM());
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
