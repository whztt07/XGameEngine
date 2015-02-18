#pragma once

class cApp : public cApplication
{
private:
	cGraphics		m_Graphics;
	//--------------------------------------------------
	// Add your definations below...
	//--------------------------------------------------

	cTexture m_Texs[25];
	cVertexBuffer m_VB;

public:
	cApp();

	BOOL Init();
	BOOL Frame();
	BOOL Shutdown();
};

//-----------------------------------------------------------------------------
// Desc: 顶点结构
//-----------------------------------------------------------------------------
struct  CUSTOMVERTEX
{
	float x, y, z;    //顶点位置
	float u,v ;		  //顶点纹理坐标
};
#define D3DFVF_CUSTOMVERTEX   (D3DFVF_XYZ|D3DFVF_TEX1)
