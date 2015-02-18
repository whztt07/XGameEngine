#pragma once

class cApp : public cApplication
{
private:
	cGraphics		m_Graphics;
	cVertexBuffer	m_VB;

public:
	cApp();

	BOOL Init();
	BOOL Frame();
	BOOL Shutdown();
};

// The 2-D vertex format and descriptor
typedef struct {
	FLOAT x, y, z;     // 2-D coordinates
	FLOAT rhw;         // rhw
	D3DCOLOR Diffuse;  // Diffuse color component
} sVertex;
#define VERTEXFVF (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)
