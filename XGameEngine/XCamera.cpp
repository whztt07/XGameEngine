#include "XCamera.h"

namespace Xge
{
	Camera::Camera()
	{
	  Move(0.0f,0.0f,0.0f);
	  Rotate(0.0f,0.0f,0.0f);
	  Update();
	}

	BOOL Camera::Move(float XPos, float YPos, float ZPos)
	{
	  m_XPos = XPos;
	  m_YPos = YPos;
	  m_ZPos = ZPos;

	  D3DXMatrixTranslation(&m_matTranslation, -m_XPos, -m_YPos, -m_ZPos);

	  return TRUE;
	}

	BOOL Camera::MoveRel(float XAdd, float YAdd, float ZAdd)
	{
	  return Move(m_XPos + XAdd, m_YPos + YAdd, m_ZPos + ZAdd);
	}

	BOOL Camera::Rotate(float XRot, float YRot, float ZRot)
	{
	  D3DXMATRIX matXRotation, matYRotation, matZRotation;

	  m_XRot = XRot;
	  m_YRot = YRot;
	  m_ZRot = ZRot;

	  D3DXMatrixRotationX(&matXRotation, -m_XRot);
	  D3DXMatrixRotationY(&matYRotation, -m_YRot);
	  D3DXMatrixRotationZ(&matZRotation, -m_ZRot);

	  m_matRotation = matZRotation;
	  D3DXMatrixMultiply(&m_matRotation, &m_matRotation, &matYRotation);
	  D3DXMatrixMultiply(&m_matRotation, &m_matRotation, &matXRotation);

	  return TRUE;
	}

	BOOL Camera::RotateRel(float XAdd, float YAdd, float ZAdd)
	{
	  return Rotate(m_XRot + XAdd, m_YRot + YAdd, m_ZRot + ZAdd);
	}

	BOOL Camera::Point(float XEye, float YEye, float ZEye, float XAt, float YAt, float ZAt)
	{
	  float XRot, YRot, XDiff, YDiff, ZDiff;

	  // Calculate angles between points
	  XDiff = XAt - XEye;
	  YDiff = YAt - YEye;
	  ZDiff = ZAt - ZEye;
	  XRot = (float)atan2(-YDiff, sqrt(XDiff*XDiff+ZDiff*ZDiff));
	  YRot = (float)atan2(XDiff, ZDiff);

	  Move(XEye, YEye, ZEye);
	  Rotate(XRot, YRot, 0.0f);

	  return TRUE;
	}

	BOOL Camera::SetStartTrack()
	{
	  m_StartXPos = m_XPos;
	  m_StartYPos = m_YPos;
	  m_StartZPos = m_ZPos;
	  m_StartXRot = m_XRot;
	  m_StartYRot = m_YRot;
	  m_StartZRot = m_ZRot;
	  return TRUE;
	}

	BOOL Camera::SetEndTrack()
	{
	  m_EndXPos = m_XPos;
	  m_EndYPos = m_YPos;
	  m_EndZPos = m_ZPos;
	  m_EndXRot = m_XRot;
	  m_EndYRot = m_YRot;
	  m_EndZRot = m_ZRot;
	  return TRUE;
	}

	BOOL Camera::Track(float Time, float Length)
	{
	  float x, y, z;
	  float TimeOffset;

	  TimeOffset = Length * Time;

	  x = (m_EndXPos - m_StartXPos) / Length * TimeOffset;
	  y = (m_EndYPos - m_StartYPos) / Length * TimeOffset;
	  z = (m_EndZPos - m_StartZPos) / Length * TimeOffset;
	  Move(m_StartXPos + x, m_StartYPos + y, m_StartZPos + z);

	  x = (m_EndXRot - m_StartXRot) / Length * TimeOffset;
	  y = (m_EndYRot - m_StartYRot) / Length * TimeOffset;
	  z = (m_EndZRot - m_StartZRot) / Length * TimeOffset;
	  Rotate(m_StartXRot + x, m_StartYRot + y, m_StartZRot + z);

	  return TRUE;
	}

	BOOL Camera::Update()
	{
	  D3DXMatrixMultiply(&m_matWorld, &m_matTranslation, &m_matRotation);
	  return TRUE;
	}

	D3DXMATRIX *Camera::GetMatrix()
	{
	  Update();
	  return &m_matWorld;
	}

	float Camera::GetXPos()
	{
	  return m_XPos;
	}

	float Camera::GetYPos()
	{
	  return m_YPos;
	}

	float Camera::GetZPos()
	{
	  return m_ZPos;
	}

	float Camera::GetXRotation()
	{
	  return m_XRot;
	}

	float Camera::GetYRotation()
	{
	  return m_YRot;
	}

	float Camera::GetZRotation()
	{
	  return m_ZRot;
	}
}
