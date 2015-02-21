#ifndef _XObject_H_
#define _XObject_H_

#include "XGlobal.h"
#include "XWorldPosition.h"

namespace Xge
{
	class Graphics;
	class Mesh;

	class Object
	{
	public:
		Object();
		~Object();

		BOOL Create(Graphics *Graphics, Mesh *Mesh = NULL);
		BOOL Free();

		BOOL EnableBillboard(BOOL Enable = TRUE);
		BOOL AttachToObject(Object *Object, char *FrameName = NULL);

		BOOL Move(float XPos, float YPos, float ZPos);
		BOOL MoveRel(float XAdd, float YAdd, float ZAdd);

		BOOL Rotate(float XRot, float YRot, float ZRot);
		BOOL RotateRel(float XAdd, float YAdd, float ZAdd);

		BOOL Scale(float XScale, float YScale, float ZScale);
		BOOL ScaleRel(float XAdd, float YAdd, float ZAdd);

		D3DXMATRIX *GetMatrix();

		float GetXPos();
		float GetYPos();
		float GetZPos();
		float GetXRotation();
		float GetYRotation();
		float GetZRotation();
		float GetXScale();
		float GetYScale();
		float GetZScale();

		BOOL  GetBounds(float *MinX, float *MinY, float *MinZ, float *MaxX, float *MaxY, float *MaxZ, float *Radius);

		BOOL SetMesh(Mesh *Mesh);
		Mesh *GetMesh();

		BOOL Render();

	protected:
		Graphics     *m_Graphics;
		Mesh         *m_Mesh;
		WorldPosition m_Pos;

		BOOL           m_Billboard;
	};
}

#endif
