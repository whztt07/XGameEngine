#include "XTexture.h"
#include "XGraphics.h"

namespace Xge
{
	Texture::Texture()
	{
	  m_Graphics = NULL;
	  m_Texture = NULL;
	  m_Width = m_Height = 0;
	}

	Texture::~Texture()
	{
	  Free();
	}

	BOOL Texture::Load(Graphics *Graphics, LPTSTR Filename, DWORD Transparent, D3DFORMAT Format)
	{
		Free();

		if((m_Graphics = Graphics) == NULL)
			return FALSE;
		if(Graphics->GetDeviceCOM() == NULL)
			return FALSE;
		if(Filename == NULL)
			return FALSE;

		if(FAILED(D3DXCreateTextureFromFileEx(	Graphics->GetDeviceCOM(),
												Filename, D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT,
												0, Format, D3DPOOL_MANAGED, 
												D3DX_FILTER_TRIANGLE, D3DX_FILTER_TRIANGLE,
												Transparent, NULL, NULL, &m_Texture	)))
		{
			return FALSE;
		}
		//if(FAILED(D3DXCreateTextureFromFile( Graphics->GetDeviceCOM(), Filename, &m_Texture )))
		//	return FALSE;

		m_Width = GetWidth();
		m_Height = GetHeight();

		return TRUE;
	}

	BOOL Texture::Create(Graphics *Graphics, IDirect3DTexture9 *Texture)
	{
	  D3DLOCKED_RECT SrcRect, DestRect;
	  D3DSURFACE_DESC Desc;

	  Free();

	  if((m_Graphics = Graphics) == NULL)
		return FALSE;

	  if(Texture == NULL)
		return TRUE;
    
	  // Copy texture over
	  Texture->GetLevelDesc(0, &Desc);
	  m_Width  = Desc.Width;
	  m_Height = Desc.Height;
	  m_Graphics->GetDeviceCOM()->CreateTexture(m_Width, m_Height, 0, 0, Desc.Format, D3DPOOL_MANAGED, &m_Texture, NULL);

	  Texture->LockRect(0, &SrcRect, NULL, D3DLOCK_READONLY);
	  m_Texture->LockRect(0, &DestRect, NULL, 0);

	  memcpy(DestRect.pBits, SrcRect.pBits, SrcRect.Pitch * m_Height);

	  m_Texture->UnlockRect(0);
	  Texture->UnlockRect(0);

	  return TRUE;
	}

	BOOL Texture::Free()
	{
		ReleaseCOM(m_Texture);
	  m_Graphics = NULL;
	  m_Width = m_Height = 0;

	  return TRUE;
	}

	BOOL Texture::IsLoaded()
	{
	  if(m_Texture == NULL)
		return FALSE;
	  return TRUE;
	}

	IDirect3DTexture9 *Texture::GetTextureCOM()
	{
	  return m_Texture;
	}

	long Texture::GetWidth()
	{
	  D3DSURFACE_DESC d3dsd;

	  if(m_Texture == NULL)
		return 0;

	  if(FAILED(m_Texture->GetLevelDesc(0, &d3dsd)))
		return 0;

	  return d3dsd.Width;
	}

	long Texture::GetHeight()
	{
	  D3DSURFACE_DESC d3dsd;

	  if(m_Texture == NULL)
		return 0;

	  if(FAILED(m_Texture->GetLevelDesc(0, &d3dsd)))
		return 0;

	  return d3dsd.Height;
	}

	D3DFORMAT Texture::GetFormat()
	{
	  D3DSURFACE_DESC d3dsd;

	  if(m_Texture == NULL)
		return D3DFMT_UNKNOWN;

	  if(FAILED(m_Texture->GetLevelDesc(0, &d3dsd)))
		return D3DFMT_UNKNOWN;

	  return d3dsd.Format;
	}

	BOOL Texture::Blit(long DestX, long DestY,                   \
						long SrcX, long SrcY,                     \
						long Width, long Height,                  \
						float XScale, float YScale,               \
						D3DCOLOR Color)
	{
	  RECT Rect;
	  ID3DXSprite *pSprite;

	  if(m_Texture == NULL)
		return FALSE;
	  if(m_Graphics == NULL)
		return FALSE;
	  if((pSprite = m_Graphics->GetSpriteCOM()) == NULL)
		return FALSE;

	  if(!Width)
		Width = m_Width;
	  if(!Height)
		Height = m_Height;

	  Rect.left = SrcX;
	  Rect.top  = SrcY;
	  Rect.right = Rect.left + Width;
	  Rect.bottom = Rect.top + Height;

	  if(FAILED(pSprite->Begin(D3DXSPRITE_ALPHABLEND)))
		  return FALSE;

	  D3DXMATRIX Transform;
	  D3DXMatrixScaling(&Transform, XScale, YScale, 1.0f);
	  pSprite->SetTransform(&Transform);
	  pSprite->Draw(m_Texture, &Rect, NULL, &D3DXVECTOR3((float)DestX, (float)DestY, 0), Color);

	  pSprite->End();

	  return TRUE;
	}
}
