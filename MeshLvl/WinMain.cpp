#include "Global.h"

class cApp : public cApplication
{
private:
	cGraphics	m_Graphics;
	cCamera		m_Camera;

	cMesh           m_RoomMeshes[2];	// Meshes
    cObject         m_RoomObjects[8];	// Objects

	cInput          m_Input;			// Input object
    cInputDevice    m_Keyboard;			// Keyboard device object
    cInputDevice    m_Mouse;			// Moues device object
	
	float           m_XPos, m_ZPos;		// Viewpoint position

public:
	cApp();

	BOOL Init();
	BOOL Frame();
	BOOL Shutdown();
};

cApp::cApp()
{
	m_Width  = 640;
	m_Height = 480;
	m_Style  = WS_BORDER|WS_CAPTION|WS_MINIMIZEBOX|WS_SYSMENU;

	_tcscpy(m_Class, TEXT("MeshLvl"));
	_tcscpy(m_Caption, TEXT("MeshLvl demo by pf_d"));
}

BOOL cApp::Init()
{
	// Initialize the graphics device and set display mode
	m_Graphics.Init();
	m_Graphics.SetMode(GethWnd(), TRUE, TRUE);
	m_Graphics.SetPerspective(D3DX_PI/4, 1.3333f, 1.0f, 10000.0f);
	ShowMouse(TRUE);

	// Initialize input and input devices
	m_Input.Init(GethWnd(), GethInst());
	m_Keyboard.Create(&m_Input, KEYBOARD);
	m_Mouse.Create(&m_Input, MOUSE, TRUE);

	// Load the room meshes
	XV(m_RoomMeshes[0].Load(&m_Graphics, TEXT(".\\Data\\Corridor.x"), TEXT(".\\Data")));
	XV(m_RoomMeshes[1].Load(&m_Graphics, TEXT(".\\Data\\Room.x"), TEXT(".\\Data")));

	// Setup the room objects
	m_RoomObjects[0].Create(&m_Graphics, &m_RoomMeshes[1]);
	m_RoomObjects[1].Create(&m_Graphics, &m_RoomMeshes[0]);
	m_RoomObjects[2].Create(&m_Graphics, &m_RoomMeshes[1]);
	m_RoomObjects[3].Create(&m_Graphics, &m_RoomMeshes[0]);
	m_RoomObjects[4].Create(&m_Graphics, &m_RoomMeshes[0]);
	m_RoomObjects[5].Create(&m_Graphics, &m_RoomMeshes[1]);
	m_RoomObjects[6].Create(&m_Graphics, &m_RoomMeshes[0]);
	m_RoomObjects[7].Create(&m_Graphics, &m_RoomMeshes[1]);

	// Move room objects into position
	m_RoomObjects[0].Move(-2000.0f, 0.0f,  2000.0f);
	m_RoomObjects[1].Move(    0.0f, 0.0f,  2000.0f);
	m_RoomObjects[2].Move( 2000.0f, 0.0f,  2000.0f);
	m_RoomObjects[3].Move(-2000.0f, 0.0f,     0.0f);
	m_RoomObjects[4].Move( 2000.0f, 0.0f,     0.0f);
	m_RoomObjects[5].Move(-2000.0f, 0.0f, -2000.0f);
	m_RoomObjects[6].Move(    0.0f, 0.0f, -2000.0f);
	m_RoomObjects[7].Move( 2000.0f, 0.0f, -2000.0f);

	// Rotate room objects into proper position
	m_RoomObjects[1].Rotate(0.0f,  1.57f, 0.0f);
	m_RoomObjects[2].Rotate(0.0f,  1.57f, 0.0f);
	m_RoomObjects[5].Rotate(0.0f, -1.57f, 0.0f);
	m_RoomObjects[6].Rotate(0.0f, -1.57f, 0.0f);
	m_RoomObjects[7].Rotate(0.0f,  3.14f, 0.0f);

	// Position view in a room
	m_XPos = -2000.0f;
	m_ZPos = -2000.0f;

	return TRUE;
}

BOOL cApp::Frame()
{
	static DWORD  Timer = timeGetTime();
	unsigned long Elapsed;
	float         XMove, ZMove;
	short         i;

	// Calculate elapsed time (plus speed boost)
	Elapsed = (timeGetTime() - Timer) * 2;
	Timer = timeGetTime();

	// Get input
	m_Keyboard.Read();
	m_Mouse.Read();

	// Process input and update everything.
	// ESC quits program
	if(m_Keyboard.GetKeyState(KEY_ESC) == TRUE)
		return FALSE;

	// Process movement
	XMove = ZMove = 0.0f;

	// Process keyboard input to move view

	// Move viewpoint forward
	if(m_Keyboard.GetKeyState(KEY_W) == TRUE) {
		XMove = (float)sin(m_Camera.GetYRotation()) * Elapsed;
		ZMove = (float)cos(m_Camera.GetYRotation()) * Elapsed;
	}

	// Move viewpoint backward
	if(m_Keyboard.GetKeyState(KEY_S) == TRUE) {
		XMove = -(float)sin(m_Camera.GetYRotation()) * Elapsed;
		ZMove = -(float)cos(m_Camera.GetYRotation()) * Elapsed;
	}

	// Strafe left
	if(m_Keyboard.GetKeyState(KEY_A) == TRUE) {
		XMove=(float)sin(m_Camera.GetYRotation()-1.57f)*Elapsed;
		ZMove=(float)cos(m_Camera.GetYRotation()-1.57f)*Elapsed;
	}

	// Strafe right
	if(m_Keyboard.GetKeyState(KEY_D) == TRUE) {
		XMove=(float)sin(m_Camera.GetYRotation()+1.57f)*Elapsed;
		ZMove=(float)cos(m_Camera.GetYRotation()+1.57f)*Elapsed;
	}

	// Update view coordinates
	m_XPos += XMove;
	m_ZPos += ZMove;

	// Position camera and rotate based on mouse position
	m_Camera.Move(m_XPos + XMove, 400.0f, m_ZPos + ZMove);
	m_Camera.RotateRel((float)m_Mouse.GetYDelta() / 200.0f,
						(float)m_Mouse.GetXDelta() / 200.0f,
						0.0f);

	// Set camera
	m_Graphics.SetCamera(&m_Camera);

	// Render everything
	m_Graphics.Clear(D3DCOLOR_RGBA(0,64,128,255));
	if(m_Graphics.BeginScene()) 
	{
		// Render each room
		for(int i=0;i<8;i++)
			XV(m_RoomObjects[i].Render());
		m_Graphics.EndScene();
	}
	m_Graphics.Display();

	return TRUE;
}

BOOL cApp::Shutdown()
{
	// Shutdown input
	m_Mouse.Free();
	m_Keyboard.Free();
	m_Input.Shutdown();

	// Shutdown graphics
	m_Graphics.Shutdown();

	return TRUE;
}

int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int nCmdShow)
{
	cApp App;
	return App.Run();
}
