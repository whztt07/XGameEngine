#include "Global.h"

class cApp : public cApplication
{
private:
	cGraphics		m_Graphics;
	cWorldPosition	m_World;
	cCamera			m_Camera;

	cInput			m_Input;
	cInputDevice	m_Keyboard;

	cMesh		m_Mesh;
	cObject		m_Object;

public:
	cApp();

	BOOL Init();
	BOOL Frame();
	BOOL Shutdown();
};

cApp::cApp()
{
	m_Width = 480;
	m_Height = 600;
	_tcscpy(m_Class, TEXT("XFile"));
	_tcscpy(m_Caption, TEXT("Mesh control demo by pf_d"));
}

BOOL cApp::Init()
{
	// Initialize the graphics device and set display mode
	m_Graphics.Init();
	m_Graphics.SetMode(GethWnd(), TRUE, TRUE);

	m_Input.Init(GethWnd(), GethInst());
	m_Keyboard.Create(&m_Input, KEYBOARD, TRUE);

	m_World.Move(-30.0f, 30.0f, 0);
	m_Graphics.SetWorldPosition(&m_World);

	m_Camera.Point(0, 50.0f, -150.0f, 0, 50.0f, 0);
	m_Camera.Rotate(0, 0, 0);
	m_Graphics.SetCamera(&m_Camera);

	m_Mesh.Load(&m_Graphics, TEXT(".\\Warrior.x"), TEXT("."));
	m_Object.Create(&m_Graphics, &m_Mesh);

	return TRUE;
}

BOOL cApp::Frame()
{
	static DWORD  Timer = timeGetTime();
	// Calculate elapsed time (plus speed boost)
	unsigned long Elapsed = timeGetTime() - Timer;
	// 30fps
	if(Elapsed < 33.0f)
		return TRUE;
	Timer = timeGetTime();

	float XAdd = 0, YAdd = 0, ZAdd = 0;
	if(m_Keyboard.Read())
	{
		do
		{
			if(m_Keyboard.GetKeyState(KEY_A))
			{
				XAdd = 1.0f;
				break;
			}
			if(m_Keyboard.GetKeyState(KEY_S))
			{
				YAdd = 1.0f;
				break;
			}
			if(m_Keyboard.GetKeyState(KEY_D))
			{
				ZAdd = 1.0f;
				break;
			}
			if(m_Keyboard.GetKeyState(KEY_Z))
			{
				XAdd = -1.0f;
				break;
			}
			if(m_Keyboard.GetKeyState(KEY_X))
			{
				YAdd = -1.0f;
				break;
			}
			if(m_Keyboard.GetKeyState(KEY_C))
			{
				ZAdd = -1.0f;
				break;
			}
		}while(0);
	}
	m_World.RotateRel(XAdd, YAdd, ZAdd);
	m_Graphics.SetWorldPosition(&m_World);

	m_Graphics.Clear(D3DCOLOR_RGBA(0,64,128,255));
	if(m_Graphics.BeginScene())
	{
		m_Mesh.Render();
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
