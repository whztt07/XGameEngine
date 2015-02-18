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
// Desc: ����ṹ
//-----------------------------------------------------------------------------
struct  CUSTOMVERTEX
{
	float x, y, z;    //����λ��
	float u,v ;		  //������������
};
#define D3DFVF_CUSTOMVERTEX   (D3DFVF_XYZ|D3DFVF_TEX1)
