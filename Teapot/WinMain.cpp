#include "Global.h"

class cApp : public cApplication
{
private:
	cGraphics	m_Graphics;
	cCamera		m_Camera;

	cLight		m_Light;
	cMaterial	m_Material;
	LPD3DXMESH	m_pMesh;

public:
	cApp();

	BOOL Init();
	BOOL Frame();
	BOOL Shutdown();
};

cApp::cApp()
{
	m_Width = 400;
	m_Height = 400;
	_tcscpy(m_Class, TEXT("Teapot"));
	_tcscpy(m_Caption, TEXT("Teapot demo by pf_d"));
}

BOOL cApp::Init()
{
	// Initialize the graphics device and set display mode
	m_Graphics.Init();
	m_Graphics.SetMode(GethWnd(), TRUE, TRUE);
	
	m_Camera.Point(0, 1.0f, -4.0f, 0, 0, 0);
	m_Graphics.SetCamera(&m_Camera);

	m_Graphics.GetDeviceCOM()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_Graphics.GetDeviceCOM()->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
	m_Graphics.GetDeviceCOM()->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);
    //m_Graphics.GetDeviceCOM()->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	// 模型
	D3DXCreateTeapot(m_Graphics.GetDeviceCOM(), &m_pMesh, NULL);

	// 材质...
	m_Graphics.SetMaterial(&m_Material);
	
	// 光照...
	m_Light.SetType(D3DLIGHT_DIRECTIONAL);
	m_Light.SetDiffuseColor(255, 255, 255);
	m_Light.SetAmbientColor(77, 77, 77);
	m_Light.SetSpecularColor(77, 77, 77);
	m_Light.Point(-2.0f, 2.0f, -2.0f, 0, 0, 0);
	m_Graphics.SetLight(0, &m_Light);

	m_Graphics.GetDeviceCOM()->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
	m_Graphics.GetDeviceCOM()->SetRenderState(D3DRS_SPECULARENABLE, TRUE);
	m_Graphics.EnableLight(0, TRUE);
	m_Graphics.EnableLighting(TRUE);

	return TRUE;
}

BOOL cApp::Frame()
{
	m_Graphics.Clear(D3DCOLOR_XRGB(45, 50, 170));
	if(m_Graphics.BeginScene())
	{
		// 旋转
		cWorldPosition WorldPosition;
		WorldPosition.Rotate(0, timeGetTime()/2000.0f, 0);
		m_Graphics.SetWorldPosition(&WorldPosition);
		
		m_pMesh->DrawSubset(0);
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
