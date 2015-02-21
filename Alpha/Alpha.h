#pragma once

#include "XApplication.h"
#include "XGraphics.h"
#include "XVertexBuffer.h"

class Alpha : public Xge::Application
{
private:
	Xge::Graphics		m_Graphics;
	Xge::VertexBuffer	m_VB;

public:
	Alpha();

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
