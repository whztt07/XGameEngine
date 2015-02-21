#ifndef _XGraphics_H_
#define _XGraphics_H_

#include "XGlobal.h"

namespace Xge
{
	class Camera;
	class Light;
	class Material;
	class Texture;
	class WorldPosition;

	class Graphics
	{
	public:
		Graphics();
		~Graphics();

		IDirect3D9       *GetDirect3DCOM();
		IDirect3DDevice9 *GetDeviceCOM();
		ID3DXSprite      *GetSpriteCOM();

		BOOL Init();
		BOOL Shutdown();

		BOOL SetMode(HWND hWnd, BOOL Windowed = TRUE, BOOL UseZBuffer = FALSE, long Width = 0, long Height = 0, char BPP = 0);
		long GetNumDisplayModes(D3DFORMAT Format);
		BOOL GetDisplayModeInfo(D3DFORMAT Format, long Num, D3DDISPLAYMODE *Mode);
		char GetFormatBPP(D3DFORMAT Format);
		BOOL CheckFormat(D3DFORMAT Format, BOOL Windowed, BOOL HAL);

		BOOL Display();

		BOOL BeginScene();
		BOOL EndScene();

		BOOL BeginSprite();
		BOOL EndSprite();

		BOOL Clear(long Color = 0, float ZBuffer = 1.0f);
		BOOL ClearDisplay(long Color = 0);
		BOOL ClearZBuffer(float ZBuffer = 1.0f);

		long GetWidth();
		long GetHeight();
		char GetBPP();
		BOOL GetHAL();
		BOOL GetZBuffer();

		BOOL SetPerspective(float FOV=D3DX_PI / 4.0f, float Aspect=1.3333f, float Near=1.0f, float Far=10000.0f);
    
		BOOL SetWorldPosition(WorldPosition *WorldPos);
		BOOL SetCamera(Camera *Camera);
		BOOL SetLight(long Num, Light *Light);
		BOOL SetMaterial(Material *Material);
		BOOL SetTexture(short Num, Texture *Texture);
    
		BOOL SetAmbientLight(char Red, char Green, char Blue);
		BOOL GetAmbientLight(char *Red, char *Green, char *Blue);

		BOOL EnableLight(long Num, BOOL Enable = TRUE);
		BOOL EnableLighting(BOOL Enable = TRUE);
		BOOL EnableZBuffer(BOOL Enable = TRUE);
		BOOL EnableAlphaBlending(BOOL Enable = TRUE, DWORD Src = D3DBLEND_SRCALPHA, DWORD Dest = D3DBLEND_INVSRCALPHA);
		BOOL EnableAlphaTesting(BOOL Enable = TRUE);

	protected:
		HWND              m_hWnd;
		IDirect3D9       *m_pD3D;
		IDirect3DDevice9 *m_pD3DDevice;
		ID3DXSprite      *m_pSprite;

		D3DDISPLAYMODE    m_d3ddm;

		BOOL              m_Windowed;
		BOOL              m_ZBuffer;
		BOOL              m_HAL;

		long              m_Width;
		long              m_Height;
		char              m_BPP;

		char              m_AmbientRed;
		char              m_AmbientGreen;
		char              m_AmbientBlue;
	};
}

#endif
