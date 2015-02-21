#include "XApplication.h"
#include "XGraphics.h"
#include "XWorldPosition.h"
#include "XCamera.h"
#include "XTexture.h"
#include "XVertexBuffer.h"

class cApp : public Xge::Application
{
private:
	Xge::Graphics	m_Graphics;
	Xge::Camera		m_Camera;

	Xge::VertexBuffer	m_Billboard;
	Xge::Texture		m_BillboardTex;

	Xge::VertexBuffer	m_Floor;
	Xge::Texture		m_FloorTex;

public:
	cApp();

	BOOL Init();
	BOOL Frame();
	BOOL Shutdown();
};

// The 3D vertex format and descriptor
typedef struct {
	FLOAT x, y, z;   // 3D coordinates
	FLOAT u, v;      // Texture coordinates
} s3DVertex;
#define FVF3D (D3DFVF_XYZ | D3DFVF_TEX1)

cApp::cApp()
{
	m_Width = 400;
	m_Height = 400;
	_tcscpy(m_Class, TEXT("Billboard"));
	_tcscpy(m_Caption, TEXT("Billboard demo by pf_d"));
}

BOOL cApp::Init()
{
	// Initialize the graphics device and set display mode
	m_Graphics.Init();
	m_Graphics.SetMode(GethWnd(), TRUE, FALSE);

	s3DVertex BillboardVerts[4] = 
	{
		{ -42.0f, 80.0f, 0.0f, 0.0f, 0.0f },
		{  40.0f, 80.0f, 0.0f, 1.0f, 0.0f },
		{ -40.0f,  0.0f, 0.0f, 0.0f, 1.0f },
		{  40.0f,  0.0f, 0.0f, 1.0f, 1.0f }
	};
	m_Billboard.Create(&m_Graphics, 4, FVF3D, sizeof(s3DVertex));
	m_Billboard.Set(0, 4, BillboardVerts);

	// Transparent对应的颜色Alpha值会被置为0
	m_BillboardTex.Load(&m_Graphics, TEXT(".\\Data\\Billboard.bmp"), D3DCOLOR_RGBA(0, 0, 0, 255));

	s3DVertex FloorVerts[4] = 
	{
		{ -100.0f, 0.0f,  100.0f, 0.0f, 0.0f },
		{  100.0f, 0.0f,  100.0f, 1.0f, 0.0f },
		{ -100.0f, 0.0f, -100.0f, 0.0f, 1.0f },
		{  100.0f, 0.0f, -100.0f, 1.0f, 1.0f }
	};
	m_Floor.Create(&m_Graphics, sizeof(FloorVerts), FVF3D, sizeof(s3DVertex));
	m_Floor.Set(0, sizeof(FloorVerts), FloorVerts);
	
	m_FloorTex.Load(&m_Graphics, TEXT(".\\Data\\Floor.bmp"));
	
	// 展示透明度在0x01以上的颜色
	m_Graphics.GetDeviceCOM()->SetRenderState(D3DRS_ALPHAREF, 0x01);
	m_Graphics.GetDeviceCOM()->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);

	return TRUE;
}

BOOL cApp::Frame()
{
	m_Graphics.Clear();
	
	// Update the view position
	float Angle = (float)timeGetTime() / 2000.0f;
	m_Camera.Point((float)cos(Angle) * 200.0f, 200.0f, (float)sin(Angle) * 200.0f, 0.0f, 0.0f, 0.0f);
	m_Graphics.SetCamera(&m_Camera);

	if(m_Graphics.BeginScene())
	{
		m_Graphics.GetDeviceCOM()->SetTexture(0, m_FloorTex.GetTextureCOM());

		Xge::WorldPosition worldPosition;
		m_Graphics.SetWorldPosition(&worldPosition);
		m_Floor.Render(0, 2, D3DPT_TRIANGLESTRIP);

		// 开启Alpha测试以屏蔽透明色
		m_Graphics.GetDeviceCOM()->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		worldPosition.EnableBillboard(TRUE);
		m_Graphics.SetWorldPosition(&worldPosition);
		m_Graphics.GetDeviceCOM()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		
		m_Graphics.GetDeviceCOM()->SetTexture(0, m_BillboardTex.GetTextureCOM());
		m_Billboard.Render(0, 2, D3DPT_TRIANGLESTRIP);

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
