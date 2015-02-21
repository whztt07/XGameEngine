#include "XGraphics.h"
#include "XCamera.h"
#include "XLight.h"
#include "XMaterial.h"
#include "XTexture.h"
#include "XWorldPosition.h"

namespace Xge
{
	Graphics::Graphics()
	{
	  m_hWnd = NULL;
  
	  m_pD3D = NULL;
	  m_pD3DDevice = NULL;
	  m_pSprite = NULL;
	  m_AmbientRed = m_AmbientGreen = m_AmbientBlue = 255;

	  m_Width  = 0;
	  m_Height = 0;
	  m_BPP    = 0;

	  m_Windowed = TRUE;
	  m_ZBuffer  = FALSE;
	  m_HAL      = FALSE;
	}

	Graphics::~Graphics()
	{
	  Shutdown();
	}

	BOOL Graphics::Init()
	{
	  Shutdown();

	  if((m_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
		return FALSE;

	  return TRUE;
	}

	BOOL Graphics::SetMode(HWND hWnd, BOOL Windowed, BOOL UseZBuffer, long Width, long Height, char BPP)
	{
		D3DPRESENT_PARAMETERS	d3dpp;
		D3DCAPS9				d3dCaps;
		DWORD					Behavior;
		D3DFORMAT             Format, AltFormat;
		RECT                  WndRect, ClientRect;
		long                  WndWidth, WndHeight;
		float                 Aspect;

		// Error checking
		if((m_hWnd = hWnd) == NULL)
			return FALSE;
		if(m_pD3D == NULL)
			return FALSE;

		// Get the current display format
		if(FAILED(m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &m_d3ddm)))
			return FALSE;

		// Configure width
		if(!Width) 
		{
			// Default to screen width if fullscreen
			if(Windowed == FALSE) 
			{
				m_Width = m_d3ddm.Width;
			} 
			else 
			{
				// Otherwise grab from client size
				GetClientRect(m_hWnd, &ClientRect);
				m_Width = ClientRect.right;
			}
		} 
		else 
		{
			m_Width = Width;
		}

		// Configure height
		if(!Height) 
		{
			// Default to screen height if fullscreen
			if(Windowed == FALSE) 
			{
				m_Height = m_d3ddm.Height;
			} 
			else 
			{
				// Otherwise grab from client size
				GetClientRect(m_hWnd, &ClientRect);
				m_Height = ClientRect.bottom;
			}
		} 
		else 
		{
			m_Height = Height;
		}  


		// Configure BPP
		if(!(m_BPP = BPP) || Windowed == TRUE) 
		{
			if(!(m_BPP = GetFormatBPP(m_d3ddm.Format)))
				return FALSE;
		}

		// Resize client window if using windowed mode
		if(Windowed == TRUE) 
		{
			GetWindowRect(m_hWnd, &WndRect);
			GetClientRect(m_hWnd, &ClientRect);

			WndWidth  = (WndRect.right  - (ClientRect.right  - m_Width))  - WndRect.left;
			WndHeight = (WndRect.bottom - (ClientRect.bottom - m_Height)) - WndRect.top;

			MoveWindow(m_hWnd, WndRect.left, WndRect.top, WndWidth, WndHeight, TRUE);
		}

		// Clear presentation structure
		ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));

		// Default to no hardware acceleration detected
		m_HAL = FALSE;

		// Setup Windowed or fullscreen usage
		if((m_Windowed = Windowed) == TRUE) 
		{
			d3dpp.Windowed         = TRUE;
			d3dpp.SwapEffect       = D3DSWAPEFFECT_DISCARD;
			d3dpp.BackBufferFormat = m_d3ddm.Format;

			// See if card supports HAL
			if(CheckFormat(m_d3ddm.Format, TRUE, TRUE) == TRUE) 
			{
				m_HAL = TRUE;
			} 
			else 
			{
				// Return error if not emulated
				if(CheckFormat(m_d3ddm.Format, TRUE, FALSE) == FALSE)
					return FALSE;
			}
		} 
		else 
		{
			d3dpp.Windowed   = FALSE;
			d3dpp.SwapEffect = D3DSWAPEFFECT_FLIP;
  
			d3dpp.BackBufferWidth  = m_Width;
			d3dpp.BackBufferHeight = m_Height;
			d3dpp.FullScreen_RefreshRateInHz      = D3DPRESENT_RATE_DEFAULT;
			d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE; // or D3DPRESENT_INTERVAL_DEFAULT or D3DPRESENT_INTERVAL_IMMEDIATE
			// Figure display format to use
			if(m_BPP == 32) 
			{
				Format    = D3DFMT_X8R8G8B8;
				AltFormat = D3DFMT_X8R8G8B8;
			}
			if(m_BPP == 24) 
			{
				Format    = D3DFMT_R8G8B8;
				AltFormat = D3DFMT_R8G8B8;
			}
			if(m_BPP == 16) 
			{
				Format    = D3DFMT_R5G6B5;
				AltFormat = D3DFMT_X1R5G5B5;
			}
			if(m_BPP == 8) 
			{
				Format    = D3DFMT_P8;
				AltFormat = D3DFMT_P8;
			}

			// Check for HAL device
			if(CheckFormat(Format, FALSE, TRUE) == TRUE) 
			{
				m_HAL = TRUE;
			} 
			else 
			{
				// Check for HAL device in alternate format
				if(CheckFormat(AltFormat, FALSE, TRUE) == TRUE) 
				{
					m_HAL = TRUE;
					Format = AltFormat;
				} 
				else 
				{
					// Check for Emulation device
					if(CheckFormat(Format, FALSE, FALSE) == FALSE) 
					{
						// Check for Emulation device in alternate format
						if(CheckFormat(AltFormat, FALSE, FALSE) == FALSE)
							return FALSE;
						else
							Format = AltFormat;
					}
				}
			}

			d3dpp.BackBufferFormat = Format;
		}

		// Setup Zbuffer format - 16 bit
		if((m_ZBuffer = UseZBuffer) == TRUE) 
		{
			d3dpp.EnableAutoDepthStencil = TRUE;
			d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
		} 
		else 
		{
			d3dpp.EnableAutoDepthStencil = FALSE;
		}

		// Get the current device caps
		if(FAILED(m_pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, m_HAL == TRUE ? D3DDEVTYPE_HAL : D3DDEVTYPE_REF, &d3dCaps)))
			return FALSE;
		if( d3dCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )  
			Behavior = D3DCREATE_HARDWARE_VERTEXPROCESSING;
		else  
			Behavior = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

		// Create the Direct3D Device object
		if(FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, 
					(m_HAL == TRUE) ? D3DDEVTYPE_HAL : D3DDEVTYPE_REF, 
					hWnd, Behavior,
					&d3dpp, &m_pD3DDevice))) 
		{
			// Try to create Direct3D without ZBuffer support 
			// if selected and first call failed.
			if(m_ZBuffer == TRUE) 
			{
				m_ZBuffer = FALSE;
				d3dpp.EnableAutoDepthStencil = FALSE;

				if(FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, 
							(m_HAL == TRUE) ? D3DDEVTYPE_HAL : D3DDEVTYPE_REF, 
							hWnd, Behavior,
							&d3dpp, &m_pD3DDevice)))
					return FALSE;
			} 
			else                                   
				return FALSE;
		}

		// Set default rendering states
		EnableLighting(FALSE);
		EnableZBuffer(m_ZBuffer);
		EnableAlphaBlending(FALSE);
		EnableAlphaTesting(FALSE);

		// Enable texture rendering stages and filter types
		m_pD3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		m_pD3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		m_pD3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		m_pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		m_pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);

		// Calculate the aspect ratio based on window size
		Aspect = (float)m_Height / (float)m_Width;
		SetPerspective(D3DX_PI/4.0f, Aspect, 1.0f, 10000.0f);

		// Create a sprite interface
		if(FAILED(D3DXCreateSprite(m_pD3DDevice, &m_pSprite)))
			return FALSE;

		return TRUE;
	}

	BOOL Graphics::Shutdown()
	{
	  ReleaseCOM(m_pSprite);
	  ReleaseCOM(m_pD3DDevice);
	  ReleaseCOM(m_pD3D);

	  return TRUE;
	}

	IDirect3D9 *Graphics::GetDirect3DCOM()
	{
	  return m_pD3D;
	}

	IDirect3DDevice9 *Graphics::GetDeviceCOM()
	{
	  return m_pD3DDevice;
	}

	ID3DXSprite *Graphics::GetSpriteCOM()
	{
	  return m_pSprite;
	}

	long Graphics::GetNumDisplayModes(D3DFORMAT Format)
	{
	  if(m_pD3D == NULL)
		return 0;

	  return (long)m_pD3D->GetAdapterModeCount(D3DADAPTER_DEFAULT, Format);
	}

	BOOL Graphics::GetDisplayModeInfo(D3DFORMAT Format, long Num, D3DDISPLAYMODE *Mode)
	{
	  long Max;

	  if(m_pD3D == NULL)
		return FALSE;

	  Max = GetNumDisplayModes(Format);
	  if(Num >= Max)
		return FALSE;

	  if(FAILED(m_pD3D->EnumAdapterModes(D3DADAPTER_DEFAULT, Format,
				Num, Mode)))
		return FALSE;

	  return TRUE;
	}

	char Graphics::GetFormatBPP(D3DFORMAT Format)
	{
	  switch(Format) {
		// 32 bit modes
		case D3DFMT_A8R8G8B8:
		case D3DFMT_X8R8G8B8:
		  return 32;
		  break;

		// 24 bit modes
		case D3DFMT_R8G8B8:
		  return 24;
		  break;

		// 16 bit modes
		case D3DFMT_R5G6B5:
		case D3DFMT_X1R5G5B5:
		case D3DFMT_A1R5G5B5:
		case D3DFMT_A4R4G4B4:
		  return 16;
		  break;

		// 8 bit modes
		case D3DFMT_A8P8:
		case D3DFMT_P8:
		  return 8;
		  break;

		default:
		   return 0;
	  }
	}

	BOOL Graphics::CheckFormat(D3DFORMAT Format, BOOL Windowed, BOOL HAL)
	{
	  if(FAILED(m_pD3D->CheckDeviceType(D3DADAPTER_DEFAULT, 
				  (HAL == TRUE) ? D3DDEVTYPE_HAL : D3DDEVTYPE_REF,
				  Format, Format, Windowed)))
		return FALSE;

	  return TRUE;
	}

	BOOL Graphics::BeginScene()
	{
	  if(m_pD3DDevice == NULL)
		return FALSE;

	  if(FAILED(m_pD3DDevice->BeginScene()))
		return FALSE;

	  return TRUE;
	}

	BOOL Graphics::EndScene()
	{
	  short i;

	  // Error checking
	  if(m_pD3DDevice == NULL)
		return FALSE;

	  // Release all textures
	  for(i=0;i<8;i++)
		m_pD3DDevice->SetTexture(i, NULL);

	  // End the scene
	  if(FAILED(m_pD3DDevice->EndScene()))
		return FALSE;

	  return TRUE;
	}

	BOOL Graphics::BeginSprite()
	{
	  if(m_pSprite == NULL)
		return FALSE;

	  if(FAILED(m_pSprite->Begin(D3DXSPRITE_ALPHABLEND)))
		return FALSE;

	  return TRUE;
	}

	BOOL Graphics::EndSprite()
	{
	  if(m_pSprite == NULL)
		return FALSE;

	  if(FAILED(m_pSprite->End()))
		return FALSE;

	  return TRUE;
	}

	BOOL Graphics::Display()
	{
	  if(m_pD3DDevice == NULL)
		return FALSE;

	  if(FAILED(m_pD3DDevice->Present(NULL, NULL, NULL, NULL)))
		return FALSE;

	  return TRUE;
	}

	BOOL Graphics::Clear(long Color, float ZBuffer)
	{
	  if(m_pD3DDevice == NULL)
		return FALSE;

	  // Only clear screen if no zbuffer
	  if(m_ZBuffer == FALSE)
		return ClearDisplay(Color);

	  // Clear display and zbuffer
	  if(FAILED(m_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, Color, ZBuffer, 0)))
		return FALSE;

	  return TRUE; 
	}

	BOOL Graphics::ClearDisplay(long Color)
	{
	  if(m_pD3DDevice == NULL)
		return FALSE;

	  if(FAILED(m_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, Color, 1.0f, 0)))
		return FALSE;

	  return TRUE; 
	}  

	BOOL Graphics::ClearZBuffer(float ZBuffer)
	{
	  if(m_pD3DDevice == NULL || m_ZBuffer == FALSE)
		return FALSE;

	  if(FAILED(m_pD3DDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0, ZBuffer, 0)))
		return FALSE;

	  return TRUE;
	}

	long Graphics::GetWidth()
	{
	  return m_Width;
	}

	long Graphics::GetHeight()
	{
	  return m_Height;
	}

	char Graphics::GetBPP()
	{
	  return m_BPP;
	}

	BOOL Graphics::GetHAL()
	{
	  return m_HAL;
	}
    
	BOOL Graphics::GetZBuffer()
	{
	  return m_ZBuffer;
	}

	BOOL Graphics::SetCamera(Camera *Camera)
	{
	  if(m_pD3DDevice == NULL || Camera == NULL)
		return FALSE;

	  if(FAILED(m_pD3DDevice->SetTransform(D3DTS_VIEW, Camera->GetMatrix())))
		return FALSE;

	  return TRUE;
	}

	BOOL Graphics::SetWorldPosition(WorldPosition *WorldPos)
	{
	  if(WorldPos == NULL || m_pD3DDevice == NULL)
		return FALSE;

	  if(FAILED(m_pD3DDevice->SetTransform(D3DTS_WORLD, WorldPos->GetMatrix(this))))
		return FALSE;

	  return TRUE;
	}

	BOOL Graphics::SetLight(long Num, Light *Light)
	{
	  if(Light == NULL)
		return FALSE;
	  if(m_pD3DDevice == NULL)
		return FALSE;

	  if(FAILED(m_pD3DDevice->SetLight(Num, Light->GetLight())))
		return FALSE;
	  return TRUE;
	}

	BOOL Graphics::SetAmbientLight(char Red, char Green, char Blue)
	{
		D3DCOLOR Color;

		if(m_pD3DDevice == NULL)
			return FALSE;

		Color = D3DCOLOR_XRGB((m_AmbientRed = Red), (m_AmbientGreen = Green), (m_AmbientBlue = Blue));
		if(FAILED(m_pD3DDevice->SetRenderState(D3DRS_AMBIENT, Color)))
			return FALSE;

		return TRUE;
	}

	BOOL Graphics::GetAmbientLight(char *Red, char *Green, char *Blue)
	{
	  if(Red != NULL)
		*Red = m_AmbientRed;
	  if(Green != NULL)
		*Green = m_AmbientGreen;
	  if(Blue != NULL)
		*Blue = m_AmbientBlue;

	  return TRUE;
	}

	BOOL Graphics::SetMaterial(Material *Material)
	{
	  if(m_pD3DDevice == NULL)
		return FALSE;

	  if(Material != NULL) {
		if(FAILED(m_pD3DDevice->SetMaterial(Material->GetMaterial())))
		  return FALSE;
	  }

	  return TRUE;
	}

	BOOL Graphics::SetTexture(short Num, Texture *Texture)
	{
	  // Error checking
	  if(m_pD3DDevice == NULL || Num < 0 || Num > 7)
		return FALSE;

	  if(Texture == NULL) {
		// Clear the texture
		if(FAILED(m_pD3DDevice->SetTexture(Num, NULL)))
		  return NULL;
	  } else {
		// Set the texture
		if(FAILED(m_pD3DDevice->SetTexture(Num, Texture->GetTextureCOM())))
		  return FALSE;
	  }

	  return TRUE;
	}

	BOOL Graphics::SetPerspective(float FOV, float Aspect, float Near, float Far)
	{
		D3DXMATRIX matProjection;

		if(m_pD3DDevice == NULL)
			return FALSE;

		D3DXMatrixPerspectiveFovLH(&matProjection, FOV, Aspect, Near, Far);
		if(FAILED(m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &matProjection)))
			return FALSE;
		return TRUE;
	}

	BOOL Graphics::EnableLight(long Num, BOOL Enable)
	{
	  if(m_pD3DDevice == NULL)
		return FALSE;

	  if(FAILED(m_pD3DDevice->LightEnable(Num, Enable)))
		return FALSE;

	  return TRUE;
	}

	BOOL Graphics::EnableLighting(BOOL Enable)
	{
	  if(m_pD3DDevice == NULL)
		return FALSE;

	  if(FAILED(m_pD3DDevice->SetRenderState(D3DRS_LIGHTING, Enable)))
		return FALSE;

	  return TRUE;
	}

	BOOL Graphics::EnableZBuffer(BOOL Enable)
	{
	  if(m_pD3DDevice == NULL || m_ZBuffer == FALSE)
		return FALSE;

	  if(FAILED(m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, (Enable == TRUE) ? D3DZB_TRUE : D3DZB_FALSE)))
		return FALSE;

	  return TRUE;
	}

	BOOL Graphics::EnableAlphaBlending(BOOL Enable, DWORD Src, DWORD Dest)
	{
	  if(m_pD3DDevice == NULL)
		return FALSE;

	  // Enable or disable
	  if(FAILED(m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, Enable)))
		return FALSE;

	  // Set blend type
	  if(Enable == TRUE) {
		m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND,  Src);
		m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, Dest);
	  }

	  return TRUE;
	}

	BOOL Graphics::EnableAlphaTesting(BOOL Enable)
	{
	  if(m_pD3DDevice == NULL)
		return FALSE;

	  if(FAILED(m_pD3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, Enable)))
		return FALSE;

	  // Set test type
	  if(Enable == TRUE) {
		m_pD3DDevice->SetRenderState(D3DRS_ALPHAREF, 0x08);
		m_pD3DDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
	  }

	  return TRUE;
	}
}
