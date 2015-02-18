#pragma once

class cApp : public cApplication
{
private:
	cGraphics		m_Graphics;
	cBumpTexture	m_BumpTex;
	//--------------------------------------------------
	// Add your definations below...
	//--------------------------------------------------

public:
	cApp();

	BOOL Init();
	BOOL Frame();
	BOOL Shutdown();
};
