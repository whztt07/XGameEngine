#include "Global.h"

cApp::cApp()
{
	//--------------------------------------------------
	// Change default window settings
	//--------------------------------------------------
	m_Width = 384;
	m_Height = 384;
	_tcscpy_s(m_Class, TEXT("Tile"));
	_tcscpy_s(m_Caption, TEXT("Tile demo by pf_d"));
}

BOOL cApp::Init()
{
	// Initialize the graphics device and set display mode
	m_Graphics.Init();
	m_Graphics.SetMode(GethWnd(), TRUE, FALSE);

	// Initialize input and input devices
	m_Input.Init(GethWnd(), GethInst());
	m_Keyboard.Create(&m_Input, KEYBOARD);

	//--------------------------------------------------
	// Add your codes below...
	//--------------------------------------------------
	// Create and load the tile set
	m_Tiles.Create(&m_Graphics, 1);
	m_Tiles.Load(0, TEXT("Tiles.bmp"), 64, 64);

	// Create and set the map
	char MapData[16][16] = 
	{ 
		{ 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0 },
		{ 1, 2, 2, 1, 2, 2, 0, 2, 2, 2, 2, 2, 2, 2, 2, 0 },
		{ 2, 2, 2, 2, 2, 2, 0, 2, 2, 2, 2, 2, 2, 0, 2, 0 },
		{ 2, 2, 2, 2, 2, 0, 0, 2, 2, 2, 2, 0, 0, 0, 2, 0 },
		{ 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0 },
		{ 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0 },
		{ 3, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 0 },
		{ 3, 0, 2, 2, 2, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 0 },
		{ 0, 0, 2, 2, 2, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 2, 2, 1, 1, 0, 0, 2, 2, 0, 0, 2, 2, 2, 2, 0 },
		{ 0, 1, 2, 2, 2, 0, 0, 2, 2, 0, 0, 2, 1, 1, 2, 0 },
		{ 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0 },
		{ 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0 },
		{ 0, 0, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
	};
	m_Map.Create(1, 16, 16);
	m_Map.SetMapData(0, (char*)&MapData);
	m_Map.UseTiles(&m_Tiles);

	return TRUE;
}

BOOL cApp::Frame()
{
	static DWORD  Timer = timeGetTime();
	// Calculate elapsed time (plus speed boost)
	unsigned long Elapsed = timeGetTime() - Timer;
	// 30Fps
	if(Elapsed < 33.0f)
		return TRUE;
	Timer = timeGetTime();

	static long XPos = 0, YPos = 0;

	// Press arrows to scroll map around
    if(GetAsyncKeyState(VK_LEFT))
      XPos -= 8;
    if(GetAsyncKeyState(VK_RIGHT))
      XPos += 8;
    if(GetAsyncKeyState(VK_UP))
      YPos -= 8;
    if(GetAsyncKeyState(VK_DOWN))
      YPos += 8;

    // Bounds check map coordinates
    if(XPos < 0)
      XPos = 0;
    if(XPos > 640)
      XPos = 640;
    if(YPos < 0)
      YPos = 0;
    if(YPos > 640)
      YPos = 640;

	m_Graphics.Clear(D3DCOLOR_ARGB(0, 45, 50, 170));
	if(m_Graphics.BeginScene())
	{
		//--------------------------------------------------
		// Add your codes below...
		//--------------------------------------------------
		// Draw the map
		m_Map.Render(XPos, YPos, 6, 6, 0);
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
