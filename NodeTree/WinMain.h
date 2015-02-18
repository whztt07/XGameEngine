#pragma once

class cApp : public cApplication
{
private:
	cGraphics	m_Graphics;
	cCamera		m_Camera;
	cLight		m_Light;

	cInput          m_Input;     // Input object
    cInputDevice    m_Keyboard;  // Keyboard device object
    cInputDevice    m_Mouse;     // Moues device object
	float		m_XPos, m_YPos, m_ZPos;  // Camera position

	//--------------------------------------------------
	// Add your definations below...
	//--------------------------------------------------
	cSkyBox m_SkyBox;

	cMesh           m_Mesh;
    cNodeTreeMesh   m_NodeTreeMesh;

public:
	cApp();

	BOOL Init();
	BOOL Frame();
	BOOL Shutdown();
};
