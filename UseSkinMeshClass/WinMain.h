#pragma once

class cApp : public cApplication
{
public:
	cGraphics		m_Graphics;
	D3DXMATRIX		m_matWorld;
	cCamera			m_Camera;
	cLight			m_Light;

	cInput          m_Input;     // Input object
    cInputDevice    m_Keyboard;  // Keyboard device object
    cInputDevice    m_Mouse;     // Moues device object
	float		m_XPos, m_YPos, m_ZPos;  // Camera position

	//--------------------------------------------------
	// Add your definations below...
	//--------------------------------------------------

	cSkinMesh *m_pMesh;

public:
	cApp();

	BOOL Init();
	BOOL Frame();
	BOOL Shutdown();
};
