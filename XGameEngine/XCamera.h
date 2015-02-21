#ifndef _XCamera_H_
#define _XCamera_H_

#include "XGlobal.h"

namespace Xge
{
	class Camera
	{
	public:
		Camera();

		D3DXMATRIX *GetMatrix(); // Get view transformation matrix
		BOOL Update();           // Update transformation matrix

		BOOL Move(float XPos, float YPos, float ZPos);
		BOOL MoveRel(float XAdd, float YAdd, float ZAdd);

		BOOL Rotate(float XRot, float YRot, float ZRot);
		BOOL RotateRel(float XAdd, float YAdd, float ZAdd);

		BOOL Point(float XEye, float YEye, float ZEye, float XAt, float YAt, float ZAt);

		BOOL SetStartTrack();
		BOOL SetEndTrack();
		BOOL Track(float Time, float Length);

		float GetXPos();
		float GetYPos();
		float GetZPos();
		float GetXRotation();
		float GetYRotation();
		float GetZRotation();

	protected:
		float m_XPos, m_YPos, m_ZPos;
		float m_XRot, m_YRot, m_ZRot;

		float m_StartXPos, m_StartYPos, m_StartZPos;
		float m_StartXRot, m_StartYRot, m_StartZRot;

		float m_EndXPos, m_EndYPos, m_EndZPos;
		float m_EndXRot, m_EndYRot, m_EndZRot;
    
		D3DXMATRIX m_matWorld;
		D3DXMATRIX m_matTranslation;
		D3DXMATRIX m_matRotation;
	};
}

#endif
