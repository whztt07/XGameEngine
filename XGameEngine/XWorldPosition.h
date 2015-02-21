#ifndef _XWorldPosition_H_
#define _XWorldPosition_H_

#include "XGlobal.h"

namespace Xge
{
	class Graphics;

	class WorldPosition
	{
	  protected:
		BOOL  m_Billboard;

		float m_XPos,      m_YPos,      m_ZPos;
		float m_XRotation, m_YRotation, m_ZRotation;
		float m_XScale,    m_YScale,    m_ZScale;
    
		D3DXMATRIX m_matWorld;
		D3DXMATRIX m_matScale;
		D3DXMATRIX m_matRotation;
		D3DXMATRIX m_matTranslation;
		D3DXMATRIX *m_matCombine1;
		D3DXMATRIX *m_matCombine2;

		BOOL Update(Graphics *Graphics = NULL);
	  public:
		WorldPosition();

		D3DXMATRIX *GetMatrix(Graphics *Graphics = NULL);
		BOOL SetCombineMatrix1(D3DXMATRIX *Matrix = NULL);
		BOOL SetCombineMatrix2(D3DXMATRIX *Matrix = NULL);

		BOOL Copy(WorldPosition *DestPos);

		BOOL Move(float XPos, float YPos, float ZPos);
		BOOL MoveRel(float XAdd, float YAdd, float ZAdd);

		BOOL Rotate(float XRot, float YRot, float ZRot);
		BOOL RotateRel(float XAdd, float YAdd, float ZAdd);

		BOOL Scale(float XScale, float YScale, float ZScale);
		BOOL ScaleRel(float XAdd, float YAdd, float ZAdd);

		BOOL EnableBillboard(BOOL Enable = TRUE);

		float GetXPos();
		float GetYPos();
		float GetZPos();
		float GetXRotation();
		float GetYRotation();
		float GetZRotation();
		float GetXScale();
		float GetYScale();
		float GetZScale();
	};
}

#endif
