#include "XWorldPosition.h"
#include "XGraphics.h"

namespace Xge
{
	WorldPosition::WorldPosition()
	{
	  m_Billboard = FALSE;
	  m_matCombine1 = m_matCombine2 = NULL;

	  Move(0.0f,0.0f,0.0f);
	  Rotate(0.0f,0.0f,0.0f);
	  Scale(1.0f, 1.0f, 1.0f);

	  Update();
	}

	BOOL WorldPosition::Copy(WorldPosition *DestPos)
	{
	  DestPos->Move(m_XPos, m_YPos, m_ZPos);
	  DestPos->Rotate(m_XRotation, m_YRotation, m_ZRotation);
	  DestPos->Scale(m_XScale, m_YScale, m_ZScale);
	  DestPos->EnableBillboard(m_Billboard);
 
	  return TRUE;
	}

	BOOL WorldPosition::Move(float XPos, float YPos, float ZPos)
	{
	  m_XPos = XPos;
	  m_YPos = YPos;
	  m_ZPos = ZPos;

	  D3DXMatrixTranslation(&m_matTranslation, m_XPos, m_YPos, m_ZPos);

	  return TRUE;
	}

	BOOL WorldPosition::MoveRel(float XAdd, float YAdd, float ZAdd)
	{
	  return Move(m_XPos + XAdd, m_YPos + YAdd, m_ZPos + ZAdd);
	}

	BOOL WorldPosition::Rotate(float XRot, float YRot, float ZRot)
	{
	  m_XRotation = XRot;
	  m_YRotation = YRot;
	  m_ZRotation = ZRot;

	  D3DXMatrixRotationYawPitchRoll(&m_matRotation, m_YRotation, m_XRotation, m_ZRotation);

	  return TRUE;
	}

	BOOL WorldPosition::RotateRel(float XAdd, float YAdd, float ZAdd)
	{
	  return Rotate(m_XRotation + XAdd, m_YRotation + YAdd, m_ZRotation + ZAdd);
	}

	BOOL WorldPosition::Scale(float XScale, float YScale, float ZScale)
	{
	  m_XScale = XScale;
	  m_YScale = YScale;
	  m_ZScale = ZScale;

	  D3DXMatrixScaling(&m_matScale, XScale, YScale, ZScale);

	  return TRUE;
	}

	BOOL WorldPosition::ScaleRel(float XAdd, float YAdd, float ZAdd)
	{
	  return Scale(m_XScale + XAdd, m_YScale + YAdd, m_ZScale + ZAdd);
	}

	BOOL WorldPosition::Update(Graphics *Graphics)
	{
	  D3DXMATRIX matView, matTransposed;

	  // Setup billboarding matrix
	  if(m_Billboard == TRUE) {
		if(Graphics != NULL && Graphics->GetDeviceCOM() != NULL) {
		  Graphics->GetDeviceCOM()->GetTransform(D3DTS_VIEW, &matView);
		  D3DXMatrixTranspose(&matTransposed, &matView);
		  matTransposed._41 = matTransposed._42 = matTransposed._43 = matTransposed._14 = matTransposed._24 = matTransposed._34 = 0.0f;
		} else {
		  D3DXMatrixIdentity(&matTransposed);
		}
	  }

	  // Combine scaling and rotation matrices first
	  D3DXMatrixMultiply(&m_matWorld, &m_matScale, &m_matRotation);

	  // Apply billboard matrix
	  if(m_Billboard == TRUE)
		D3DXMatrixMultiply(&m_matWorld, &m_matWorld, &matTransposed);  

	  // Combine with translation matrix
	  D3DXMatrixMultiply(&m_matWorld, &m_matWorld, &m_matTranslation);

	  // Combine with combined matrices (if any)
	  if(m_matCombine1 != NULL) 
		D3DXMatrixMultiply(&m_matWorld, &m_matWorld, m_matCombine1);
	  if(m_matCombine2 != NULL) 
		D3DXMatrixMultiply(&m_matWorld, &m_matWorld, m_matCombine2);

	  return TRUE;
	}

	BOOL WorldPosition::EnableBillboard(BOOL Enable)
	{
	  m_Billboard = Enable;
	  return TRUE;
	}

	D3DXMATRIX *WorldPosition::GetMatrix(Graphics *Graphics)
	{
	  Update(Graphics);
	  return &m_matWorld;
	}

	BOOL WorldPosition::SetCombineMatrix1(D3DXMATRIX *Matrix)
	{
	  m_matCombine1 = Matrix;
	  return TRUE;
	}

	BOOL WorldPosition::SetCombineMatrix2(D3DXMATRIX *Matrix)
	{
	  m_matCombine2 = Matrix;
	  return TRUE;
	}

	float WorldPosition::GetXPos()
	{
	  return m_XPos;
	}

	float WorldPosition::GetYPos()
	{
	  return m_YPos;
	}

	float WorldPosition::GetZPos()
	{
	  return m_ZPos;
	}

	float WorldPosition::GetXRotation()
	{
	  return m_XRotation;
	}

	float WorldPosition::GetYRotation()
	{
	  return m_YRotation;
	}

	float WorldPosition::GetZRotation()
	{
	  return m_ZRotation;
	}

	float WorldPosition::GetXScale()
	{
	  return m_XScale;
	}

	float WorldPosition::GetYScale()
	{
	  return m_YScale;
	}

	float WorldPosition::GetZScale()
	{
	  return m_ZScale;
	}
}
