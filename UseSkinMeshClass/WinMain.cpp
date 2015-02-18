#include "Global.h"

cApp::cApp()
{
	//--------------------------------------------------
	// Change default window settings
	//--------------------------------------------------
	m_Width = 640;
	m_Height = 480;
	_tcscpy(m_Class, TEXT("UseSkinMeshClass"));
	_tcscpy(m_Caption, TEXT("UseSkinMeshClass author pf_d"));
}

BOOL cApp::Init()
{
	// Initialize the graphics device and set display mode
	m_Graphics.Init();
	m_Graphics.SetMode(GethWnd(), TRUE, TRUE);

	// Initialize input and input devices
	m_Input.Init(GethWnd(), GethInst());
	m_Keyboard.Create(&m_Input, KEYBOARD);
	m_Mouse.Create(&m_Input, MOUSE, TRUE);

	//--------------------------------------------------
	// Add your codes below...
	//--------------------------------------------------

	m_pMesh = new cSkinMesh;
	m_pMesh->OnCreate(m_Graphics.GetDeviceCOM(), TEXT("lxq.x"));
	
	// 构造世界矩阵
	D3DXMatrixIdentity(&m_matWorld);

	// 初始摄像机位置
	m_XPos = 0;m_YPos = 25.0f;m_ZPos = -100.0f;

	// 设置投影矩阵
	m_Graphics.SetPerspective(D3DX_PI/4, m_Width/m_Height, 1.0f, 2000.0f);

	// 设置灯光
	m_Light.SetType(D3DLIGHT_DIRECTIONAL);
	m_Light.SetAmbientColor(255, 255, 255);
	m_Light.SetDiffuseColor(255, 255, 255);
	m_Light.SetSpecularColor(255, 255, 255);
	m_Light.Point(0, 0, 0, 1.0f, 1.0f, 1.0f);
	m_Graphics.SetLight(0, &m_Light);
	m_Graphics.EnableLight(0, TRUE);
	m_Graphics.EnableLighting(TRUE);
	m_Graphics.GetDeviceCOM()->SetRenderState(D3DRS_NORMALIZENORMALS, true);  
	m_Graphics.GetDeviceCOM()->SetRenderState(D3DRS_SPECULARENABLE, true);

    //设置环境光
    m_Graphics.GetDeviceCOM()->SetRenderState(D3DRS_AMBIENT, 0x00ffffff);

	return TRUE;
}

BOOL cApp::Frame()
{
	static DWORD  Timer = timeGetTime();
	// Calculate elapsed time (plus speed boost)
	unsigned long Elapsed = (timeGetTime() - Timer) * .1;
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
		//--------------------------------------------------
		// Add your codes below...
		//--------------------------------------------------
		m_pMesh->Render((D3DXMATRIXA16*)&m_matWorld, 0.01f);
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
