#include "Global.h"

cApp::cApp()
{
	//--------------------------------------------------
	// Change default window settings
	//--------------------------------------------------
	m_Width = 640;
	m_Height = 480;
	_tcscpy(m_Class, TEXT("NodeTree"));
	_tcscpy(m_Caption, TEXT("NodeTree by pf_d"));
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

	// 初始摄像机位置
	m_XPos = 0;m_YPos = 500.0f;m_ZPos = 0;

	// 光照
	m_Light.SetType(D3DLIGHT_DIRECTIONAL);
	m_Light.SetDiffuseColor(255, 255, 255);
	m_Light.Point(0, 0, 0, 1.0f, -1.0f, 1.0f);
	m_Graphics.SetLight(0, &m_Light);
	m_Graphics.EnableLight(0, TRUE);
	m_Graphics.EnableLighting(TRUE);

	//设置环境光
    m_Graphics.GetDeviceCOM()->SetRenderState( D3DRS_AMBIENT, 0x00ffffff);
	//--------------------------------------------------
	// Add your codes below...
	//--------------------------------------------------
	

	// Load the mesh and create an NodeTree mesh from it
	m_Mesh.Load(&m_Graphics, TEXT(".\\Data\\Level.x"), TEXT(".\\Data\\"));
	m_NodeTreeMesh.Create(&m_Graphics, &m_Mesh, QUADTREE);

	// Setup the sky box
	//m_SkyBox.Create(&m_Graphics);
	//for(int i=0;i<6;i++)
	//	m_SkyBox.LoadTexture(i, TEXT(".\\Data\\Stars.bmp"));

	return TRUE;
}

BOOL cApp::Frame()
{
	cFrustum      Frustum;

	static DWORD  Timer = timeGetTime();
	// Calculate elapsed time (plus speed boost)
	unsigned long Elapsed = (timeGetTime() - Timer);
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
	Frustum.Construct(&m_Graphics, 1000.0f);

	m_Graphics.Clear(D3DCOLOR_ARGB(0, 45, 50, 170));
	if(m_Graphics.BeginScene())
	{
		//--------------------------------------------------
		// Add your codes below...
		//--------------------------------------------------
		//m_Graphics.EnableZBuffer(FALSE);
		//m_Graphics.EnableLighting(FALSE);
		//m_SkyBox.Render(&m_Camera);

		m_Graphics.EnableZBuffer(TRUE);
		m_Graphics.EnableLighting(TRUE);
		m_NodeTreeMesh.Render(&Frustum);
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
