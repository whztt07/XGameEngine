#ifndef _XTexture_H_
#define _XTexture_H_

#include "XGlobal.h"

namespace Xge
{
	class Graphics;

	class Texture
	{
	protected:
		Graphics         *m_Graphics;
		IDirect3DTexture9 *m_Texture;
		unsigned long      m_Width, m_Height;

	public:
		Texture();
		~Texture();

		IDirect3DTexture9 *GetTextureCOM();

		BOOL Load(Graphics *Graphics, LPTSTR Filename, DWORD Transparent = 0, D3DFORMAT Format = D3DFMT_UNKNOWN);
		BOOL Create(Graphics *Graphics, IDirect3DTexture9 *Texture);
		BOOL Free();

		BOOL      IsLoaded();

		long      GetWidth();
		long      GetHeight();
		D3DFORMAT GetFormat();

		BOOL Blit(long DestX, long DestY,                         \
					long SrcX = 0, long SrcY = 0,                   \
					long Width = 0, long Height = 0,                \
					float XScale = 1.0f, float YScale = 1.0f,       \
					D3DCOLOR Color = 0xFFFFFFFF);
	};
}

#endif
