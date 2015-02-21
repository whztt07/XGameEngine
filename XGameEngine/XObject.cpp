#include "XObject.h"
#include "XGraphics.h"
#include "XMesh.h"

namespace Xge
{
	Object::Object()
	{
	  m_Graphics       = NULL;
	  m_Mesh           = NULL;
	}

	Object::~Object()
	{
	  Free();
	}

	BOOL Object::Create(Graphics *Graphics, Mesh *Mesh)
	{
	  if((m_Graphics = Graphics) == NULL)
		return FALSE;
	  m_Mesh = Mesh;

	  Move(0.0f, 0.0f, 0.0f);
	  Rotate(0.0f, 0.0f, 0.0f);
	  Scale(1.0f, 1.0f, 1.0f);

	  return TRUE;
	}

	BOOL Object::Free()
	{
	  m_Graphics	 = NULL;
	  m_Mesh         = NULL;

	  return TRUE;
	}

	BOOL Object::EnableBillboard(BOOL Enable)
	{
	  m_Pos.EnableBillboard(Enable);
	  return TRUE;
	}

	BOOL Object::Move(float XPos, float YPos, float ZPos)
	{
	  return m_Pos.Move(XPos, YPos, ZPos);
	}

	BOOL Object::MoveRel(float XAdd, float YAdd, float ZAdd)
	{
	  return m_Pos.MoveRel(XAdd, YAdd, ZAdd);
	}

	BOOL Object::Rotate(float XRot, float YRot, float ZRot)
	{
	  return m_Pos.Rotate(XRot, YRot, ZRot);
	}

	BOOL Object::RotateRel(float XAdd, float YAdd, float ZAdd)
	{
	  return m_Pos.RotateRel(XAdd, YAdd, ZAdd);
	}

	BOOL Object::Scale(float XScale, float YScale, float ZScale)
	{
	  return m_Pos.Scale(XScale, YScale, ZScale);
	}

	BOOL Object::ScaleRel(float XAdd, float YAdd, float ZAdd)
	{
	  return m_Pos.ScaleRel(XAdd, YAdd, ZAdd);
	}

	D3DXMATRIX *Object::GetMatrix()
	{
	  return m_Pos.GetMatrix(); 
	}

	BOOL Object::SetMesh(Mesh *Mesh)
	{
	  m_Mesh = Mesh;

	  return TRUE;
	}

	Mesh *Object::GetMesh()
	{
	  return m_Mesh;
	}

	float Object::GetXPos()
	{
	  return m_Pos.GetXPos();
	}

	float Object::GetYPos()
	{
	  return m_Pos.GetYPos();
	}

	float Object::GetZPos()
	{
	  return m_Pos.GetZPos();
	}

	float Object::GetXRotation()
	{
	  return m_Pos.GetXRotation();
	}

	float Object::GetYRotation()
	{
	  return m_Pos.GetYRotation();
	}

	float Object::GetZRotation()
	{
	  return m_Pos.GetZRotation();
	}

	float Object::GetXScale()
	{
	  return m_Pos.GetXScale();
	}

	float Object::GetYScale()
	{
	  return m_Pos.GetYScale();
	}

	float Object::GetZScale()
	{
	  return m_Pos.GetZScale();
	}

	BOOL Object::GetBounds(float *MinX, float *MinY, float *MinZ, float *MaxX, float *MaxY, float *MaxZ, float *Radius)
	{
	  float Length, XScale, YScale, ZScale;

	  if(m_Mesh == NULL)
		return FALSE;

	  m_Mesh->GetBounds(MinX, MinY, MinZ, MaxX, MaxY, MaxZ, Radius);

	  // Scale bounds
	  XScale = m_Pos.GetXScale();
	  YScale = m_Pos.GetYScale();
	  ZScale = m_Pos.GetZScale();

	  if(MinX != NULL)
		*MinX *= XScale;
	  if(MinY != NULL)
		*MinY *= YScale;
	  if(MinZ != NULL)
		*MinZ *= ZScale;

	  if(MaxX != NULL)
		*MaxX *= XScale;
	  if(MaxY != NULL)
		*MaxY *= YScale;
	  if(MaxZ != NULL)
		*MaxZ *= ZScale;

	  if(Radius != NULL) {
		Length = (float)sqrt(XScale*XScale+YScale*YScale+ZScale*ZScale);
		(*Radius) *= Length;
	  }

	  return TRUE;
	}

	BOOL Object::Render()
	{
	  D3DXMATRIX Matrix;
  
	  // Error checking
	  if(m_Graphics == NULL || m_Mesh == NULL)
		return FALSE;

	  // Update the world position
	  m_Graphics->GetDeviceCOM()->SetTransform(D3DTS_WORLD, m_Pos.GetMatrix(m_Graphics));

	  m_Mesh->Render();

	  return TRUE;
	}
}
