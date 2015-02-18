#include "Global.h"

class cApp : public cApplication
{
private:
	cGraphics		m_Graphics;
	cWorldPosition	m_WorldPosition;
	cCamera			m_Camera;
	
	cMaterial		m_Material;
	cLight			m_Light;
	cVertexBuffer	m_VB;

	void InitGeomtry();
	void InitLight();

public:
	cApp();

	BOOL Init();
	BOOL Frame();
	BOOL Shutdown();
};

typedef struct
{
	D3DXVECTOR3 Position;
	D3DXVECTOR3 Normal;
} CUSTOMVERTEX;
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ | D3DFVF_NORMAL)

#define WHITE D3DCOLOR_ARGB(0, 1.0f, 1.0f, 1.0f)

cApp::cApp()
{
	m_Width = 640;
	m_Height = 480;
	_tcscpy(m_Class, TEXT("Pyramid"));
	_tcscpy(m_Caption, TEXT("Pyramid by pf_d"));
}

BOOL cApp::Init()
{
	// Initialize the graphics device and set display mode
	m_Graphics.Init();
	m_Graphics.SetMode(GethWnd(), TRUE, TRUE);
	m_Graphics.SetPerspective(D3DX_PI/2.0f, m_Width / m_Height);

	m_Camera.Point(0, 1.0f, -3.0f, 0, 0, 0);
	m_Graphics.SetCamera(&m_Camera);

	InitGeomtry();
	InitLight();

	return TRUE;
}

void cApp::InitGeomtry()
{
	m_VB.Create(&m_Graphics, 12, D3DFVF_CUSTOMVERTEX, sizeof(CUSTOMVERTEX));
	if(m_VB.Lock())
	{
		CUSTOMVERTEX *pVertices = (CUSTOMVERTEX*)m_VB.GetPtr();
		// Front face
		pVertices[0].Position	= D3DXVECTOR3(-1.0f, 0.0f, -1.0f); 
		pVertices[0].Normal		= D3DXVECTOR3(0.0f, 0.707f, -0.707f);
		pVertices[1].Position	= D3DXVECTOR3(0.0f, 1.0f,  0.0f);
		pVertices[1].Normal		= D3DXVECTOR3(0.0f, 0.707f, -0.707f);
		pVertices[2].Position	= D3DXVECTOR3(1.0f, 0.0f, -1.0f);
		pVertices[2].Normal		= D3DXVECTOR3(0.0f, 0.707f, -0.707f);
		// Left face
		pVertices[3].Position	= D3DXVECTOR3(-1.0f, 0.0f,  1.0f);
		pVertices[3].Normal		= D3DXVECTOR3(-0.707f, 0.707f, 0.0f);
		pVertices[4].Position	= D3DXVECTOR3(0.0f, 1.0f,  0.0f);
		pVertices[4].Normal		= D3DXVECTOR3(-0.707f, 0.707f, 0.0f);
		pVertices[5].Position	= D3DXVECTOR3(-1.0f, 0.0f, -1.0f);
		pVertices[5].Normal		= D3DXVECTOR3(-0.707f, 0.707f, 0.0f);
		// Right face
		pVertices[6].Position	= D3DXVECTOR3( 1.0f, 0.0f, -1.0f);
		pVertices[6].Normal		= D3DXVECTOR3( 0.707f, 0.707f, 0.0f);
		pVertices[7].Position	= D3DXVECTOR3( 0.0f, 1.0f,  0.0f);
		pVertices[7].Normal		= D3DXVECTOR3( 0.707f, 0.707f, 0.0f);
		pVertices[8].Position	= D3DXVECTOR3( 1.0f, 0.0f,  1.0f);
		pVertices[8].Normal		= D3DXVECTOR3( 0.707f, 0.707f, 0.0f);
		// Back face
		pVertices[9].Position	= D3DXVECTOR3(1.0f, 0.0f,  1.0f);
		pVertices[9].Normal		= D3DXVECTOR3(0.0f, 0.707f, 0.707f);
		pVertices[10].Position	= D3DXVECTOR3( 0.0f, 1.0f,  0.0f);
		pVertices[10].Normal	= D3DXVECTOR3(0.0f, 0.707f, 0.707f);
		pVertices[11].Position	= D3DXVECTOR3(-1.0f, 0.0f,  1.0f);
		pVertices[11].Normal	= D3DXVECTOR3(0.0f, 0.707f, 0.707f);
		m_VB.Unlock();
	}
}

void cApp::InitLight()
{
	// 材质
	m_Graphics.SetMaterial(&m_Material);

	// 方向光
	m_Light.SetType(D3DLIGHT_DIRECTIONAL);
	m_Light.SetDiffuseColor(255, 255, 255);
	m_Light.SetAmbientColor(140, 140, 140);
	m_Light.SetSpecularColor(140, 140, 140);
	m_Light.Point(0, 5, -5, 1.0f, 0, 0);
	m_Graphics.SetLight(0, &m_Light);
	m_Graphics.EnableLight(0, TRUE);

	m_Graphics.GetDeviceCOM()->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
	m_Graphics.GetDeviceCOM()->SetRenderState(D3DRS_SPECULARENABLE, TRUE);
	m_Graphics.EnableLighting(TRUE);
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

		m_VB.Render(0, 4, D3DPT_TRIANGLELIST);
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
