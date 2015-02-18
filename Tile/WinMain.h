#pragma once

class cApp : public cApplication
{
private:
	cGraphics		m_Graphics;
	
	cTiles m_Tiles;
	cMap m_Map;

	cInput          m_Input;     // Input object
    cInputDevice    m_Keyboard;  // Keyboard device object

	//--------------------------------------------------
	// Add your definations below...
	//--------------------------------------------------
public:
	cApp();

	BOOL Init();
	BOOL Frame();
	BOOL Shutdown();
};
