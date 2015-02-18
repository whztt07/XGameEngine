#include "Global.h"

class cApp : public cApplication
{
private:
	cGraphics		m_Graphics;
	cCamera			m_Camera;

	cMaterial		m_Material;
	cLight			m_Light;
	cVertexBuffer	m_VB;

	void InitGeometry();
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

cApp::cApp()
{
	m_XPos = 200;
	m_YPos = 100;
	m_Width = 500;
	m_Height = 500;
	_tcscpy(m_Class, TEXT("Light"));
	_tcscpy(m_Caption, TEXT("Light demo by pf_d"));
}

BOOL cApp::Init()
{
	// Initialize the graphics device and set display mode
	m_Graphics.Init();
	m_Graphics.SetMode(GethWnd(), TRUE, TRUE);

	m_Camera.Point(0, 3.0f, -5.0f, 0, 0, 0);
	m_Graphics.SetCamera(&m_Camera);

	// 设置剔除模式为不剔除任何面（正面和侧面）
	m_Graphics.GetDeviceCOM()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// 启用深度测试
	m_Graphics.EnableZBuffer(TRUE);

	InitGeometry();
	InitLight();

	return TRUE;
}

void cApp::InitGeometry()
{
	m_VB.Create(&m_Graphics, 100, D3DFVF_CUSTOMVERTEX, sizeof(CUSTOMVERTEX));
	if(m_VB.Lock())
	{
		CUSTOMVERTEX *pVertices = (CUSTOMVERTEX*)m_VB.GetPtr();
		for(DWORD i=0; i<50; i++)
		{
			FLOAT Theta = (2*D3DX_PI*i)/(50-1);
			pVertices[2*i+0].Position	= D3DXVECTOR3(sinf(Theta), -1.0f, cosf(Theta));
			pVertices[2*i+0].Normal		= D3DXVECTOR3(sinf(Theta), 0.0f, cosf(Theta));
			pVertices[2*i+1].Position	= D3DXVECTOR3(sinf(Theta), 1.0f, cosf(Theta));
			pVertices[2*i+1].Normal		= D3DXVECTOR3(sinf(Theta), 0.0f, cosf(Theta));
		}
		m_VB.Unlock();
	}
}

void cApp::InitLight()
{
	// 材质
	m_Material.SetAmbientColor(255, 255, 0);
	m_Material.SetDiffuseColor(255, 255, 0);
	m_Graphics.SetMaterial(&m_Material);

	// 方向光
	m_Light.SetType(D3DLIGHT_DIRECTIONAL);
	m_Light.SetDiffuseColor(255, 255, 255);
	m_Light.Point(0, 0, 0, cosf(timeGetTime()/350.0f), 1.0f, sinf(timeGetTime()/350.0f));
	m_Graphics.SetLight(0, &m_Light);
	m_Graphics.EnableLight(0, TRUE);

	// 环境光
	m_Graphics.SetAmbientLight(0x50, 0x50, 0x50);
	m_Graphics.EnableLighting(TRUE);
}

BOOL cApp::Frame()
{
	m_Graphics.Clear(D3DCOLOR_XRGB(45, 50, 170));
	if(m_Graphics.BeginScene())
	{
		InitLight();
		m_VB.Render(0, 2*50-2, D3DPT_TRIANGLESTRIP);
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
