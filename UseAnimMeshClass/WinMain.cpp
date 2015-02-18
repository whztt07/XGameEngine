#include "Global.h"

class cApp : public cApplication
{
private:
	cGraphics		m_Graphics;
	cWorldPosition	m_World;
	cCamera			m_Camera;
	cLight			m_Light;

	cInput          m_Input;     // Input object
    cInputDevice    m_Keyboard;  // Keyboard device object
    cInputDevice    m_Mouse;     // Moues device object
	float		m_XPos, m_YPos, m_ZPos;  // Viewpoint position

	cAnimMesh		*m_pMesh;

public:
	cApp();

	BOOL Init();
	BOOL Frame();
	BOOL Shutdown();

	cGraphics *GetGraphics();
};

cApp::cApp()
{
	m_Width = 640;
	m_Height = 480;
	_tcscpy_s(m_Class, TEXT("UseAnimMeshClass"));
	_tcscpy_s(m_Caption, TEXT("UseAnimMeshClass demo by pf_d"));
}

BOOL cApp::Init()
{
	HRESULT hr = S_OK;

	// Initialize the graphics device and set display mode
	m_Graphics.Init();
	m_Graphics.SetMode(GethWnd(), TRUE, TRUE);

	// Initialize input and input devices
	m_Input.Init(GethWnd(), GethInst());
	m_Keyboard.Create(&m_Input, KEYBOARD);
	m_Mouse.Create(&m_Input, MOUSE, TRUE);

	m_pMesh = new cAnimMesh();
	V_RETURN(m_pMesh->OnCreate(m_Graphics.GetDeviceCOM(), L"emr.x"));

	// 构造世界变换
	m_World.Rotate(-D3DX_PI/2, 0, 0);
	m_World.Move(0, -8.0f, 0);
	m_Graphics.SetWorldPosition(&m_World);

	// 初始摄像机位置
	m_XPos = 0;m_YPos = 0;m_ZPos = -50.0f;

	// 设置投影矩阵
	m_Graphics.SetPerspective(D3DX_PI/4, (float)(m_Width / m_Height), 1.0f, 100.0f);

	// 设置灯光
	m_Light.SetType(D3DLIGHT_DIRECTIONAL);
	m_Light.SetDiffuseColor(255, 255, 255);
	m_Light.Point(0, 0, 0, 0, -1.0f, 1.0f);
	m_Graphics.SetLight(0, &m_Light);
	m_Graphics.EnableLight(0, TRUE);
	m_Graphics.EnableLighting(TRUE);

    //设置环境光
    m_Graphics.GetDeviceCOM()->SetRenderState( D3DRS_AMBIENT, 0x00ffffff);

	return TRUE;
}

BOOL cApp::Frame()
{
	static DWORD  Timer = timeGetTime();
	// Calculate elapsed time (plus speed boost)
	unsigned long Elapsed = (long)((timeGetTime() - Timer) * .1f);
	Timer = timeGetTime();

	float XMove = 0, ZMove = 0;
	// Process movement
	if(m_Keyboard.Read())
	{
		// Process input and update everything.
		// ESC quits program
		if(m_Keyboard.GetKeyState(KEY_ESC) == TRUE)
			return FALSE;

		// Move viewpoint forward
		if(m_Keyboard.GetKeyState(KEY_W) == TRUE) 
		{
			XMove = (float)sin(m_Camera.GetYRotation()) * Elapsed;
			ZMove = (float)cos(m_Camera.GetYRotation()) * Elapsed;
		}

		// Move viewpoint backward
		if(m_Keyboard.GetKeyState(KEY_S) == TRUE) 
		{
			XMove = -(float)sin(m_Camera.GetYRotation()) * Elapsed;
			ZMove = -(float)cos(m_Camera.GetYRotation()) * Elapsed;
		}

		// Strafe left
		if(m_Keyboard.GetKeyState(KEY_A) == TRUE) 
		{
			XMove=(float)sin(m_Camera.GetYRotation()-1.57f)*Elapsed;
			ZMove=(float)cos(m_Camera.GetYRotation()-1.57f)*Elapsed;
		}

		// Strafe right
		if(m_Keyboard.GetKeyState(KEY_D) == TRUE) 
		{
			XMove=(float)sin(m_Camera.GetYRotation()+1.57f)*Elapsed;
			ZMove=(float)cos(m_Camera.GetYRotation()+1.57f)*Elapsed;
		}
	}

	m_XPos += XMove;
	m_ZPos += ZMove;
	m_Camera.Move(m_XPos + XMove, m_YPos, m_ZPos + ZMove);

	// Process rotation
	if(m_Mouse.Read())
	{
		m_Camera.RotateRel((float)m_Mouse.GetYDelta() / 200.0f,
						(float)m_Mouse.GetXDelta() / 200.0f,
						0.0f);
	}

	// Set camera
	m_Graphics.SetCamera(&m_Camera);

	m_Graphics.Clear(D3DCOLOR_ARGB(0, 45, 50, 170));
	if(m_Graphics.BeginScene())
	{
		m_pMesh->Render((D3DXMATRIXA16*)m_World.GetMatrix(&m_Graphics), 0.00001f);
		m_Graphics.EndScene();
	}
	m_Graphics.Display();

	return TRUE;
}

BOOL cApp::Shutdown()
{
	return TRUE;
}

cGraphics *cApp::GetGraphics()
{
	return &m_Graphics;
}

int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int nCmdShow)
{
	cApp App;
	return App.Run();
}
