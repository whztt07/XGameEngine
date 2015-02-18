#include "XGlobal.h"
//#include "rmxftmpl.h"
//#include "rmxfguid.h"

cGraphics::cGraphics()
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

cGraphics::~cGraphics()
{
  Shutdown();
}

BOOL cGraphics::Init()
{
  Shutdown();

  if((m_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
    return FALSE;

  return TRUE;
}

BOOL cGraphics::SetMode(HWND hWnd, BOOL Windowed, BOOL UseZBuffer, long Width, long Height, char BPP)
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

BOOL cGraphics::Shutdown()
{
  ReleaseCOM(m_pSprite);
  ReleaseCOM(m_pD3DDevice);
  ReleaseCOM(m_pD3D);

  return TRUE;
}

IDirect3D9 *cGraphics::GetDirect3DCOM()
{
  return m_pD3D;
}

IDirect3DDevice9 *cGraphics::GetDeviceCOM()
{
  return m_pD3DDevice;
}

ID3DXSprite *cGraphics::GetSpriteCOM()
{
  return m_pSprite;
}

long cGraphics::GetNumDisplayModes(D3DFORMAT Format)
{
  if(m_pD3D == NULL)
    return 0;

  return (long)m_pD3D->GetAdapterModeCount(D3DADAPTER_DEFAULT, Format);
}

BOOL cGraphics::GetDisplayModeInfo(D3DFORMAT Format, long Num, D3DDISPLAYMODE *Mode)
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

char cGraphics::GetFormatBPP(D3DFORMAT Format)
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

BOOL cGraphics::CheckFormat(D3DFORMAT Format, BOOL Windowed, BOOL HAL)
{
  if(FAILED(m_pD3D->CheckDeviceType(D3DADAPTER_DEFAULT, 
              (HAL == TRUE) ? D3DDEVTYPE_HAL : D3DDEVTYPE_REF,
              Format, Format, Windowed)))
    return FALSE;

  return TRUE;
}

BOOL cGraphics::BeginScene()
{
  if(m_pD3DDevice == NULL)
    return FALSE;

  if(FAILED(m_pD3DDevice->BeginScene()))
    return FALSE;

  return TRUE;
}

BOOL cGraphics::EndScene()
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

BOOL cGraphics::BeginSprite()
{
  if(m_pSprite == NULL)
    return FALSE;

  if(FAILED(m_pSprite->Begin(D3DXSPRITE_ALPHABLEND)))
    return FALSE;

  return TRUE;
}

BOOL cGraphics::EndSprite()
{
  if(m_pSprite == NULL)
    return FALSE;

  if(FAILED(m_pSprite->End()))
    return FALSE;

  return TRUE;
}

BOOL cGraphics::Display()
{
  if(m_pD3DDevice == NULL)
    return FALSE;

  if(FAILED(m_pD3DDevice->Present(NULL, NULL, NULL, NULL)))
    return FALSE;

  return TRUE;
}

BOOL cGraphics::Clear(long Color, float ZBuffer)
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

BOOL cGraphics::ClearDisplay(long Color)
{
  if(m_pD3DDevice == NULL)
    return FALSE;

  if(FAILED(m_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, Color, 1.0f, 0)))
    return FALSE;

  return TRUE; 
}  

BOOL cGraphics::ClearZBuffer(float ZBuffer)
{
  if(m_pD3DDevice == NULL || m_ZBuffer == FALSE)
    return FALSE;

  if(FAILED(m_pD3DDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0, ZBuffer, 0)))
    return FALSE;

  return TRUE;
}

long cGraphics::GetWidth()
{
  return m_Width;
}

long cGraphics::GetHeight()
{
  return m_Height;
}

char cGraphics::GetBPP()
{
  return m_BPP;
}

BOOL cGraphics::GetHAL()
{
  return m_HAL;
}
    
BOOL cGraphics::GetZBuffer()
{
  return m_ZBuffer;
}

BOOL cGraphics::SetCamera(cCamera *Camera)
{
  if(m_pD3DDevice == NULL || Camera == NULL)
    return FALSE;

  if(FAILED(m_pD3DDevice->SetTransform(D3DTS_VIEW, Camera->GetMatrix())))
    return FALSE;

  return TRUE;
}

BOOL cGraphics::SetWorldPosition(cWorldPosition *WorldPos)
{
  if(WorldPos == NULL || m_pD3DDevice == NULL)
    return FALSE;

  if(FAILED(m_pD3DDevice->SetTransform(D3DTS_WORLD, WorldPos->GetMatrix(this))))
    return FALSE;

  return TRUE;
}

BOOL cGraphics::SetLight(long Num, cLight *Light)
{
  if(Light == NULL)
    return FALSE;
  if(m_pD3DDevice == NULL)
    return FALSE;

  if(FAILED(m_pD3DDevice->SetLight(Num, Light->GetLight())))
    return FALSE;
  return TRUE;
}

BOOL cGraphics::SetAmbientLight(char Red, char Green, char Blue)
{
	D3DCOLOR Color;

	if(m_pD3DDevice == NULL)
		return FALSE;

	Color = D3DCOLOR_XRGB((m_AmbientRed = Red), (m_AmbientGreen = Green), (m_AmbientBlue = Blue));
	if(FAILED(m_pD3DDevice->SetRenderState(D3DRS_AMBIENT, Color)))
		return FALSE;

	return TRUE;
}

BOOL cGraphics::GetAmbientLight(char *Red, char *Green, char *Blue)
{
  if(Red != NULL)
    *Red = m_AmbientRed;
  if(Green != NULL)
    *Green = m_AmbientGreen;
  if(Blue != NULL)
    *Blue = m_AmbientBlue;

  return TRUE;
}

BOOL cGraphics::SetMaterial(cMaterial *Material)
{
  if(m_pD3DDevice == NULL)
    return FALSE;

  if(Material != NULL) {
    if(FAILED(m_pD3DDevice->SetMaterial(Material->GetMaterial())))
      return FALSE;
  }

  return TRUE;
}

BOOL cGraphics::SetTexture(short Num, cTexture *Texture)
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

BOOL cGraphics::SetPerspective(float FOV, float Aspect, float Near, float Far)
{
	D3DXMATRIX matProjection;

	if(m_pD3DDevice == NULL)
		return FALSE;

	D3DXMatrixPerspectiveFovLH(&matProjection, FOV, Aspect, Near, Far);
	if(FAILED(m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &matProjection)))
		return FALSE;
	return TRUE;
}

BOOL cGraphics::EnableLight(long Num, BOOL Enable)
{
  if(m_pD3DDevice == NULL)
    return FALSE;

  if(FAILED(m_pD3DDevice->LightEnable(Num, Enable)))
    return FALSE;

  return TRUE;
}

BOOL cGraphics::EnableLighting(BOOL Enable)
{
  if(m_pD3DDevice == NULL)
    return FALSE;

  if(FAILED(m_pD3DDevice->SetRenderState(D3DRS_LIGHTING, Enable)))
    return FALSE;

  return TRUE;
}

BOOL cGraphics::EnableZBuffer(BOOL Enable)
{
  if(m_pD3DDevice == NULL || m_ZBuffer == FALSE)
    return FALSE;

  if(FAILED(m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, (Enable == TRUE) ? D3DZB_TRUE : D3DZB_FALSE)))
    return FALSE;

  return TRUE;
}

BOOL cGraphics::EnableAlphaBlending(BOOL Enable, DWORD Src, DWORD Dest)
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

BOOL cGraphics::EnableAlphaTesting(BOOL Enable)
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

cWorldPosition::cWorldPosition()
{
  m_Billboard = FALSE;
  m_matCombine1 = m_matCombine2 = NULL;

  Move(0.0f,0.0f,0.0f);
  Rotate(0.0f,0.0f,0.0f);
  Scale(1.0f, 1.0f, 1.0f);

  Update();
}

BOOL cWorldPosition::Copy(cWorldPosition *DestPos)
{
  DestPos->Move(m_XPos, m_YPos, m_ZPos);
  DestPos->Rotate(m_XRotation, m_YRotation, m_ZRotation);
  DestPos->Scale(m_XScale, m_YScale, m_ZScale);
  DestPos->EnableBillboard(m_Billboard);
 
  return TRUE;
}

BOOL cWorldPosition::Move(float XPos, float YPos, float ZPos)
{
  m_XPos = XPos;
  m_YPos = YPos;
  m_ZPos = ZPos;

  D3DXMatrixTranslation(&m_matTranslation, m_XPos, m_YPos, m_ZPos);

  return TRUE;
}

BOOL cWorldPosition::MoveRel(float XAdd, float YAdd, float ZAdd)
{
  return Move(m_XPos + XAdd, m_YPos + YAdd, m_ZPos + ZAdd);
}

BOOL cWorldPosition::Rotate(float XRot, float YRot, float ZRot)
{
  m_XRotation = XRot;
  m_YRotation = YRot;
  m_ZRotation = ZRot;

  D3DXMatrixRotationYawPitchRoll(&m_matRotation, m_YRotation, m_XRotation, m_ZRotation);

  return TRUE;
}

BOOL cWorldPosition::RotateRel(float XAdd, float YAdd, float ZAdd)
{
  return Rotate(m_XRotation + XAdd, m_YRotation + YAdd, m_ZRotation + ZAdd);
}

BOOL cWorldPosition::Scale(float XScale, float YScale, float ZScale)
{
  m_XScale = XScale;
  m_YScale = YScale;
  m_ZScale = ZScale;

  D3DXMatrixScaling(&m_matScale, XScale, YScale, ZScale);

  return TRUE;
}

BOOL cWorldPosition::ScaleRel(float XAdd, float YAdd, float ZAdd)
{
  return Scale(m_XScale + XAdd, m_YScale + YAdd, m_ZScale + ZAdd);
}

BOOL cWorldPosition::Update(cGraphics *Graphics)
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

BOOL cWorldPosition::EnableBillboard(BOOL Enable)
{
  m_Billboard = Enable;
  return TRUE;
}

D3DXMATRIX *cWorldPosition::GetMatrix(cGraphics *Graphics)
{
  Update(Graphics);
  return &m_matWorld;
}

BOOL cWorldPosition::SetCombineMatrix1(D3DXMATRIX *Matrix)
{
  m_matCombine1 = Matrix;
  return TRUE;
}

BOOL cWorldPosition::SetCombineMatrix2(D3DXMATRIX *Matrix)
{
  m_matCombine2 = Matrix;
  return TRUE;
}

float cWorldPosition::GetXPos()
{
  return m_XPos;
}

float cWorldPosition::GetYPos()
{
  return m_YPos;
}

float cWorldPosition::GetZPos()
{
  return m_ZPos;
}

float cWorldPosition::GetXRotation()
{
  return m_XRotation;
}

float cWorldPosition::GetYRotation()
{
  return m_YRotation;
}

float cWorldPosition::GetZRotation()
{
  return m_ZRotation;
}

float cWorldPosition::GetXScale()
{
  return m_XScale;
}

float cWorldPosition::GetYScale()
{
  return m_YScale;
}

float cWorldPosition::GetZScale()
{
  return m_ZScale;
}

cCamera::cCamera()
{
  Move(0.0f,0.0f,0.0f);
  Rotate(0.0f,0.0f,0.0f);
  Update();
}

BOOL cCamera::Move(float XPos, float YPos, float ZPos)
{
  m_XPos = XPos;
  m_YPos = YPos;
  m_ZPos = ZPos;

  D3DXMatrixTranslation(&m_matTranslation, -m_XPos, -m_YPos, -m_ZPos);

  return TRUE;
}

BOOL cCamera::MoveRel(float XAdd, float YAdd, float ZAdd)
{
  return Move(m_XPos + XAdd, m_YPos + YAdd, m_ZPos + ZAdd);
}

BOOL cCamera::Rotate(float XRot, float YRot, float ZRot)
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

BOOL cCamera::RotateRel(float XAdd, float YAdd, float ZAdd)
{
  return Rotate(m_XRot + XAdd, m_YRot + YAdd, m_ZRot + ZAdd);
}

BOOL cCamera::Point(float XEye, float YEye, float ZEye, float XAt, float YAt, float ZAt)
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

BOOL cCamera::SetStartTrack()
{
  m_StartXPos = m_XPos;
  m_StartYPos = m_YPos;
  m_StartZPos = m_ZPos;
  m_StartXRot = m_XRot;
  m_StartYRot = m_YRot;
  m_StartZRot = m_ZRot;
  return TRUE;
}

BOOL cCamera::SetEndTrack()
{
  m_EndXPos = m_XPos;
  m_EndYPos = m_YPos;
  m_EndZPos = m_ZPos;
  m_EndXRot = m_XRot;
  m_EndYRot = m_YRot;
  m_EndZRot = m_ZRot;
  return TRUE;
}

BOOL cCamera::Track(float Time, float Length)
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

BOOL cCamera::Update()
{
  D3DXMatrixMultiply(&m_matWorld, &m_matTranslation, &m_matRotation);
  return TRUE;
}

D3DXMATRIX *cCamera::GetMatrix()
{
  Update();
  return &m_matWorld;
}

float cCamera::GetXPos()
{
  return m_XPos;
}

float cCamera::GetYPos()
{
  return m_YPos;
}

float cCamera::GetZPos()
{
  return m_ZPos;
}

float cCamera::GetXRotation()
{
  return m_XRot;
}

float cCamera::GetYRotation()
{
  return m_YRot;
}

float cCamera::GetZRotation()
{
  return m_ZRot;
}

cFont::cFont()
{
  m_Font = NULL;
}

cFont::~cFont()
{
  Free();
}

ID3DXFont *cFont::GetFontCOM()
{
  return m_Font;
}

BOOL cFont::Create(cGraphics *Graphics, LPTSTR Name, long Size, BOOL Bold, BOOL Italic, BOOL Underline, BOOL Strikeout)
{
	D3DXFONT_DESC fd;

	if(Graphics == NULL || Name == NULL)
		return FALSE;
	if(Graphics->GetDeviceCOM() == NULL)
		return FALSE;

	// Clear out the font structure
	ZeroMemory(&fd, sizeof(fd));

	// Set the font name and height
	lstrcpy(fd.FaceName, Name);
	fd.Height = -Size;
	fd.Weight = (Bold == TRUE) ? 700 : 0;
	fd.Italic = Italic;

	// Create the font object
	if(FAILED(D3DXCreateFontIndirect(Graphics->GetDeviceCOM(), &fd, &m_Font)))
		return FALSE;
	return TRUE;
}

BOOL cFont::Free()
{
  ReleaseCOM(m_Font);
  return TRUE;
}

BOOL cFont::Print(LPTSTR Text, long XPos, long YPos, long Width, long Height, D3DCOLOR Color, DWORD Format)
{
  RECT Rect;

  if(m_Font == NULL)
    return FALSE;

  if(!Width)
    Width = 65535;
  if(!Height)
    Height = 65536;

  Rect.left   = XPos;
  Rect.top    = YPos;
  Rect.right  = Rect.left + Width;
  Rect.bottom = Rect.top + Height;
  if(FAILED(m_Font->DrawText(NULL, Text, -1, &Rect, Format, Color)))
    return FALSE;
  return TRUE;
}

//-----------------------------------------------------------------------------
// Desc: 从绝对路径中提取文件名
//-----------------------------------------------------------------------------
void RemovePathFromFileName(LPSTR fullPath, LPTSTR fileName, DWORD fileNameLen)
{
	//先将fullPath的类型变换为LPWSTR
	WCHAR wszBuf[MAX_PATH];
	MultiByteToWideChar( CP_ACP, 0, fullPath, -1, wszBuf, MAX_PATH );
	wszBuf[MAX_PATH-1] = L'\0';

	WCHAR* wszFullPath = wszBuf;

	//从绝对路径中提取文件名
	LPWSTR pch=wcsrchr(wszFullPath,'\\');
	if (pch)
		_tcscpy_s(fileName, fileNameLen, ++pch);
	else
		_tcscpy_s(fileName, fileNameLen, wszFullPath);
}

cMesh::cMesh()
{
	m_Graphics = NULL;

	m_pMesh				= NULL;			
	m_pMeshMaterials	= NULL;
	m_pMeshTextures		= NULL;
	m_dwNumMaterials	= 0;
	
	m_Radius = 0;
}

cMesh::~cMesh()
{
	Free();
}

LPD3DXMESH cMesh::GetMesh()
{
	return m_pMesh;
}

DWORD cMesh::GetNumMaterials()
{
	return m_dwNumMaterials;
}

D3DMATERIAL9* cMesh::GetMeshMaterials()
{
	return m_pMeshMaterials;
}

LPDIRECT3DTEXTURE9* cMesh::GetMeshTextures()
{
	return m_pMeshTextures;
}

BOOL cMesh::GetBounds(D3DXVECTOR3 *Min, D3DXVECTOR3 *Max, float *Radius)
{
	if(Min != NULL)
		*Min = m_Min;
	if(Max != NULL)
		*Max = m_Max;
	if(Radius != NULL)
		*Radius = m_Radius;

	return TRUE;
}

BOOL cMesh::GetBounds(float *MinX, float *MinY, float *MinZ, float *MaxX, float *MaxY, float *MaxZ, float *Radius)
{
	if(MinX != NULL)
		*MinX = m_Min.x;
	if(MinY != NULL)
		*MinY = m_Min.y;
	if(MinZ != NULL)
		*MinZ = m_Min.z;

	if(MaxX != NULL)
		*MaxX = m_Max.x;
	if(MaxY != NULL)
		*MaxY = m_Max.y;
	if(MaxZ != NULL)
		*MaxZ = m_Max.z;

	if(Radius != NULL)
		*Radius = m_Radius;

	return TRUE;
}

BOOL cMesh::Load(cGraphics *Graphics, LPTSTR Filename, LPTSTR TexturePath)
{
	m_Graphics = Graphics;
	// 从.X文件中加载网格模型
	LPD3DXBUFFER pD3DXMaterialBuffer;
	if(FAILED(D3DXLoadMeshFromX(Filename, D3DXMESH_MANAGED,
								Graphics->GetDeviceCOM(), NULL,
								&pD3DXMaterialBuffer, NULL, &m_dwNumMaterials,
								&m_pMesh)))
	{
		return FALSE;
	}

	BYTE  **Ptr;
	// 计算包围盒与包围圆
	if(SUCCEEDED(m_pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&Ptr))) {
		D3DXComputeBoundingBox((D3DXVECTOR3*)Ptr, m_pMesh->GetNumVertices(), 
                            m_pMesh->GetNumBytesPerVertex(), &m_Min, &m_Max);

		D3DXComputeBoundingSphere((D3DXVECTOR3*)Ptr, m_pMesh->GetNumVertices(), 
								m_pMesh->GetNumBytesPerVertex(),
								&D3DXVECTOR3(0.0f,0.0f,0.0f), &m_Radius);

		m_pMesh->UnlockVertexBuffer();
    }

	// 建立存储区
	D3DXMATERIAL *d3dxMaterials = (D3DXMATERIAL*)pD3DXMaterialBuffer->GetBufferPointer();
	m_pMeshMaterials = new D3DMATERIAL9[m_dwNumMaterials];
	if(m_pMeshMaterials == NULL)
		return FALSE;
	m_pMeshTextures = new LPDIRECT3DTEXTURE9[m_dwNumMaterials];
	if(m_pMeshTextures == NULL)
		return FALSE;

	// 逐块提取网格模型材质属性和纹理文件名
	for(DWORD i=0; i<m_dwNumMaterials; i++)
	{
		// 材质属性
		m_pMeshMaterials[i] = d3dxMaterials[i].MatD3D;
		// 设置材质的环境光反射系数
		// 因为材质本身没有设置环境光反射系数
		m_pMeshMaterials[i].Ambient = m_pMeshMaterials[i].Diffuse;

		m_pMeshTextures[i] = NULL;
		if(d3dxMaterials[i].pTextureFilename != NULL &&
			strlen(d3dxMaterials[i].pTextureFilename) > 0)
		{
			//获取纹理文件名
			TCHAR TextureFilename[256];
			_stprintf_s(TextureFilename, TEXT("%s\\"), TexturePath);
			RemovePathFromFileName(d3dxMaterials[i].pTextureFilename, TextureFilename+_tcslen(TextureFilename), 256);

			if(FAILED(D3DXCreateTextureFromFile(m_Graphics->GetDeviceCOM(), TextureFilename, &m_pMeshTextures[i])))
			{
				//////////////////////////////////////////////////
				// Warning 加载纹理失败
				//////////////////////////////////////////////////
			}
		}
	}

	// 释放在加载模型文件时创建的保存模型材质和纹理数据的缓冲区对象
	pD3DXMaterialBuffer->Release();
	return TRUE;
}

BOOL cMesh::Render()
{
	if(m_pMesh == NULL || m_pMeshMaterials == NULL || m_pMeshTextures == NULL)
		return FALSE;

	for(DWORD i=0; i<m_dwNumMaterials; i++)
	{
		if(m_pMeshTextures[i] == NULL)
			continue;
		m_Graphics->GetDeviceCOM()->SetMaterial(&m_pMeshMaterials[i]);
		m_Graphics->GetDeviceCOM()->SetTexture(0, m_pMeshTextures[i]);
		m_pMesh->DrawSubset(i);
	}
	return TRUE;
}

BOOL cMesh::Free()
{
	// 释放网格模型材质
	if(m_pMeshMaterials != NULL)
		delete[] m_pMeshMaterials;

	// 释放网格模型纹理
	if(m_pMeshTextures)
	{
		for(DWORD i=0; i<m_dwNumMaterials; i++)
		{
			if(m_pMeshTextures[i])
				m_pMeshTextures[i]->Release();
		}
		delete[] m_pMeshTextures;
	}

	// 释放网格模型
	if(m_pMesh != NULL)
		m_pMesh->Release();

	return TRUE;
}

cObject::cObject()
{
  m_Graphics       = NULL;
  m_Mesh           = NULL;
}

cObject::~cObject()
{
  Free();
}

BOOL cObject::Create(cGraphics *Graphics, cMesh *Mesh)
{
  if((m_Graphics = Graphics) == NULL)
    return FALSE;
  m_Mesh = Mesh;

  Move(0.0f, 0.0f, 0.0f);
  Rotate(0.0f, 0.0f, 0.0f);
  Scale(1.0f, 1.0f, 1.0f);

  return TRUE;
}

BOOL cObject::Free()
{
  m_Graphics	 = NULL;
  m_Mesh         = NULL;

  return TRUE;
}

BOOL cObject::EnableBillboard(BOOL Enable)
{
  m_Pos.EnableBillboard(Enable);
  return TRUE;
}

BOOL cObject::Move(float XPos, float YPos, float ZPos)
{
  return m_Pos.Move(XPos, YPos, ZPos);
}

BOOL cObject::MoveRel(float XAdd, float YAdd, float ZAdd)
{
  return m_Pos.MoveRel(XAdd, YAdd, ZAdd);
}

BOOL cObject::Rotate(float XRot, float YRot, float ZRot)
{
  return m_Pos.Rotate(XRot, YRot, ZRot);
}

BOOL cObject::RotateRel(float XAdd, float YAdd, float ZAdd)
{
  return m_Pos.RotateRel(XAdd, YAdd, ZAdd);
}

BOOL cObject::Scale(float XScale, float YScale, float ZScale)
{
  return m_Pos.Scale(XScale, YScale, ZScale);
}

BOOL cObject::ScaleRel(float XAdd, float YAdd, float ZAdd)
{
  return m_Pos.ScaleRel(XAdd, YAdd, ZAdd);
}

D3DXMATRIX *cObject::GetMatrix()
{
  return m_Pos.GetMatrix(); 
}

BOOL cObject::SetMesh(cMesh *Mesh)
{
  m_Mesh = Mesh;

  return TRUE;
}

cMesh *cObject::GetMesh()
{
  return m_Mesh;
}

float cObject::GetXPos()
{
  return m_Pos.GetXPos();
}

float cObject::GetYPos()
{
  return m_Pos.GetYPos();
}

float cObject::GetZPos()
{
  return m_Pos.GetZPos();
}

float cObject::GetXRotation()
{
  return m_Pos.GetXRotation();
}

float cObject::GetYRotation()
{
  return m_Pos.GetYRotation();
}

float cObject::GetZRotation()
{
  return m_Pos.GetZRotation();
}

float cObject::GetXScale()
{
  return m_Pos.GetXScale();
}

float cObject::GetYScale()
{
  return m_Pos.GetYScale();
}

float cObject::GetZScale()
{
  return m_Pos.GetZScale();
}

BOOL cObject::GetBounds(float *MinX, float *MinY, float *MinZ, float *MaxX, float *MaxY, float *MaxZ, float *Radius)
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

BOOL cObject::Render()
{
  D3DXMATRIX Matrix;
  
  // Error checking
  if(m_Graphics == NULL || m_Mesh == NULL)
    return FALSE;

  // Update the world position
  m_Graphics->GetDeviceCOM()->SetTransform(D3DTS_WORLD, m_Pos.GetMatrix(m_Graphics));

  XV(m_Mesh->Render());

  return TRUE;
}

//-----------------------------------------------------------------------------
// Desc: 
//-----------------------------------------------------------------------------
HRESULT AllocateName( LPCSTR Name, LPSTR *pNewName )
{
    UINT cbLength;

    if( Name != NULL )
    {
		cbLength = (UINT)strlen(Name) + 1;
        *pNewName = new CHAR[cbLength];
        if (*pNewName == NULL)
            return E_OUTOFMEMORY;
        memcpy( *pNewName, Name, cbLength*sizeof(CHAR) );
    }
    else
    {
        *pNewName = NULL;
    }

    return S_OK;
}

//-----------------------------------------------------------------------------
//Desc: 生成蒙皮网格模型(含有每个顶点的混合权重、索引和一个骨骼组合表)
//-----------------------------------------------------------------------------
HRESULT GenerateSkinnedMesh(D3DXMESHCONTAINER_DERIVED *pMeshContainer)
{
	HRESULT hr = S_OK;

	if (pMeshContainer->pSkinInfo == NULL)
		return hr;

	SAFE_RELEASE( pMeshContainer->MeshData.pMesh );
	SAFE_RELEASE( pMeshContainer->pBoneCombinationBuf );

	if (FAILED(pMeshContainer->pSkinInfo->ConvertToBlendedMesh(
		pMeshContainer->pOrigMesh, 
		D3DXMESH_MANAGED | D3DXMESHOPT_VERTEXCACHE,
		pMeshContainer->pAdjacency,
		NULL, NULL, NULL,
		&pMeshContainer->NumInfl,
		&pMeshContainer->NumAttributeGroups,
		&pMeshContainer->pBoneCombinationBuf,
		&pMeshContainer->MeshData.pMesh)))
		return E_FAIL;
	return hr;

}

//-----------------------------------------------------------------------------
// Desc: 创建框架, 仅仅是分配内存和初始化,还没有对其成员赋予合适的值
//-----------------------------------------------------------------------------
HRESULT cAllocateHierarchy::CreateFrame(LPCSTR Name, LPD3DXFRAME *ppNewFrame)
{
    HRESULT hr = S_OK;
    D3DXFRAME_DERIVED *pFrame;
	
    *ppNewFrame = NULL;
	
    pFrame = new D3DXFRAME_DERIVED;  //创建框架结构对象
    if (pFrame == NULL) 
    {
        return E_OUTOFMEMORY;
    }
	
	//为框架指定名称
    hr = AllocateName(Name, (LPSTR*)&pFrame->Name);
	if (FAILED(hr))
	{
		delete pFrame;
		return hr;
	}
	
	//初始化D3DXFRAME_DERIVED结构其它成员变量
    D3DXMatrixIdentity(&pFrame->TransformationMatrix);
    D3DXMatrixIdentity(&pFrame->CombinedTransformationMatrix);
	
    pFrame->pMeshContainer = NULL;
    pFrame->pFrameSibling = NULL;
    pFrame->pFrameFirstChild = NULL;
	
    *ppNewFrame = pFrame;
    pFrame = NULL;

	return hr;
}

//-----------------------------------------------------------------------------
// Desc: 创建网格容器对象, 保存网格模型数据
//-----------------------------------------------------------------------------
HRESULT cAllocateHierarchy::CreateMeshContainer(LPCSTR Name, 
                                                CONST D3DXMESHDATA *pMeshData,
                                                CONST D3DXMATERIAL *pMaterials, 
                                                CONST D3DXEFFECTINSTANCE *pEffectInstances, 
                                                DWORD NumMaterials, 
                                                CONST DWORD *pAdjacency, 
                                                LPD3DXSKININFO pSkinInfo, 
                                                LPD3DXMESHCONTAINER *ppNewMeshContainer) 
{
    HRESULT hr;
    D3DXMESHCONTAINER_DERIVED *pMeshContainer = NULL;
    UINT NumFaces;       //网格中的面数,在填充网格容器结构的邻接信息成员时使用
    UINT iMaterial;      //纹理操作时的循环变量
	UINT cBones;         //当前网格模型骨骼总数
    LPDIRECT3DDEVICE9 pd3dDevice = NULL;
    LPD3DXMESH pMesh    = NULL;
    *ppNewMeshContainer = NULL;
	
    if (pMeshData->Type != D3DXMESHTYPE_MESH)
    {
        return E_FAIL;
    }
	
    pMesh = pMeshData->pMesh;
	
    if (pMesh->GetFVF() == 0)
    {
        return E_FAIL;
    }

	//为网格容器分配内存
    pMeshContainer = new D3DXMESHCONTAINER_DERIVED;
    if (pMeshContainer == NULL)
    {
        return E_OUTOFMEMORY;
    }
    memset(pMeshContainer, 0, sizeof(D3DXMESHCONTAINER_DERIVED));
	
	//填充网格容器结构D3DXMESHCONTAINER_DERIVED的成员

	//为网格指定名称
	hr = AllocateName(Name, &pMeshContainer->Name);
	if (FAILED(hr))
	{
		DestroyMeshContainer(pMeshContainer);
		return hr;
	}      
	
    pMesh->GetDevice(&pd3dDevice);
    NumFaces = pMesh->GetNumFaces();

	//确保网格顶点包含法线
    if (!(pMesh->GetFVF() & D3DFVF_NORMAL))
    {
        pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;
        hr = pMesh->CloneMeshFVF( pMesh->GetOptions(), 
			                      pMesh->GetFVF() | D3DFVF_NORMAL, 
			                      pd3dDevice, 
			                      &pMeshContainer->MeshData.pMesh );
		if (FAILED(hr))
		{
			SAFE_RELEASE(pd3dDevice);
			DestroyMeshContainer(pMeshContainer);
			return hr;
		}

		pMesh = pMeshContainer->MeshData.pMesh;
        D3DXComputeNormals( pMesh, NULL );
    }
    else 
    {
        pMeshContainer->MeshData.pMesh = pMesh;
        pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;
        pMesh->AddRef();
    }
	
    //为网格模型准备材质和纹理
    pMeshContainer->NumMaterials = max(1, NumMaterials); 
    pMeshContainer->pMaterials = new D3DXMATERIAL[pMeshContainer->NumMaterials];
    pMeshContainer->ppTextures = new LPDIRECT3DTEXTURE9[pMeshContainer->NumMaterials];
    pMeshContainer->pAdjacency = new DWORD[NumFaces*3];
    if ((pMeshContainer->pAdjacency == NULL) || (pMeshContainer->pMaterials == NULL)
		|| (pMeshContainer->ppTextures == NULL))
	{
		hr = E_OUTOFMEMORY;
		SAFE_RELEASE(pd3dDevice);
		DestroyMeshContainer(pMeshContainer);
		return hr;
	}

	memcpy(pMeshContainer->pAdjacency, pAdjacency, sizeof(DWORD) * NumFaces*3);  
    memset(pMeshContainer->ppTextures, 0, sizeof(LPDIRECT3DTEXTURE9) * pMeshContainer->NumMaterials);
	
   if (NumMaterials > 0)            
    {
		//复制材质属性, 设置材质环境光属性
        memcpy(pMeshContainer->pMaterials, pMaterials, sizeof(D3DXMATERIAL) * NumMaterials); 
		pMeshContainer->pMaterials->MatD3D.Ambient = pMeshContainer->pMaterials->MatD3D.Diffuse;

		//提取纹理文件, 创建纹理对象
		for (iMaterial = 0; iMaterial < NumMaterials; iMaterial++) 
        {
            if (pMeshContainer->pMaterials[iMaterial].pTextureFilename != NULL)
            {
				WCHAR strTexturePath[MAX_PATH];
                WCHAR wszBuf[MAX_PATH];
				//从纹理文件路径提取纹理文件名
				RemovePathFromFileName(pMeshContainer->pMaterials[iMaterial].pTextureFilename, wszBuf, MAX_PATH);
                //根据纹理文件名从事先指定的路径查找纹理文件
				DXUTFindDXSDKMediaFileCch( strTexturePath, MAX_PATH, wszBuf );
                if( FAILED( D3DXCreateTextureFromFile( pd3dDevice, strTexturePath,
                                                        &pMeshContainer->ppTextures[iMaterial] ) ) )
                    pMeshContainer->ppTextures[iMaterial] = NULL;

                pMeshContainer->pMaterials[iMaterial].pTextureFilename = NULL;
			}
        }
    }
    else
    {
        pMeshContainer->pMaterials[0].pTextureFilename = NULL;
        memset(&pMeshContainer->pMaterials[0].MatD3D, 0, sizeof(D3DMATERIAL9));
        pMeshContainer->pMaterials[0].MatD3D.Diffuse.r = 0.5f;
        pMeshContainer->pMaterials[0].MatD3D.Diffuse.g = 0.5f;
        pMeshContainer->pMaterials[0].MatD3D.Diffuse.b = 0.5f;
        pMeshContainer->pMaterials[0].MatD3D.Specular = pMeshContainer->pMaterials[0].MatD3D.Diffuse;
		pMeshContainer->pMaterials[0].MatD3D.Ambient  = pMeshContainer->pMaterials[0].MatD3D.Diffuse;
    }

	SAFE_RELEASE(pd3dDevice);

	//如果当前网格包含蒙皮信息
    if (pSkinInfo != NULL)
    {
		//加载蒙皮网格信息
        pMeshContainer->pSkinInfo = pSkinInfo; 
        pSkinInfo->AddRef();
		
		//保留原网格信息
        pMeshContainer->pOrigMesh = pMesh;
        pMesh->AddRef();

		//获取骨骼数量
        cBones = pSkinInfo->GetNumBones();

		//为每块骨骼分配保存初始变换矩阵的内存空间
        pMeshContainer->pBoneOffsetMatrices = new D3DXMATRIX[cBones];
        if (pMeshContainer->pBoneOffsetMatrices == NULL) 
        {
			hr = E_OUTOFMEMORY;
			DestroyMeshContainer(pMeshContainer);
			return hr;
		}
        
		//保存每块骨骼的初始变换矩阵
        for (UINT iBone = 0; iBone < cBones; iBone++)
        {
            pMeshContainer->pBoneOffsetMatrices[iBone] = *(pMeshContainer->pSkinInfo->GetBoneOffsetMatrix(iBone));
        }
		
		//生成蒙皮网格模型
        hr = GenerateSkinnedMesh(pMeshContainer); 
        if (FAILED(hr))
		{
			DestroyMeshContainer(pMeshContainer);
			return hr;
		}
    }

    *ppNewMeshContainer = pMeshContainer;
    pMeshContainer = NULL;
    return hr;
}


//-----------------------------------------------------------------------------
// Desc: 释放框架
//-----------------------------------------------------------------------------
HRESULT cAllocateHierarchy::DestroyFrame(LPD3DXFRAME pFrameToFree) 
{
	if(pFrameToFree != NULL)
	{
		SAFE_DELETE_ARRAY( pFrameToFree->Name );
		SAFE_DELETE( pFrameToFree );
	}
    return S_OK; 
}


//-----------------------------------------------------------------------------
// Desc: 释放网格容器
//-----------------------------------------------------------------------------
HRESULT cAllocateHierarchy::DestroyMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase)
{
	if(pMeshContainerBase == NULL)
		return S_OK;

    UINT iMaterial;
	// 先转为扩展型以免内存泄漏
    D3DXMESHCONTAINER_DERIVED *pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerBase;
	
    SAFE_DELETE_ARRAY( pMeshContainer->Name );
    SAFE_DELETE_ARRAY( pMeshContainer->pAdjacency );
    SAFE_DELETE_ARRAY( pMeshContainer->pMaterials );
	
    if (pMeshContainer->ppTextures != NULL)
    {
        for (iMaterial = 0; iMaterial < pMeshContainer->NumMaterials; iMaterial++)
        {
            SAFE_RELEASE( pMeshContainer->ppTextures[iMaterial] );
        }
    }
    SAFE_DELETE_ARRAY( pMeshContainer->ppTextures );

    SAFE_RELEASE( pMeshContainer->MeshData.pMesh );
    SAFE_RELEASE( pMeshContainer->pSkinInfo );
    SAFE_DELETE( pMeshContainer );
    return S_OK;
}

//--------------------------------------------------------------------
// Desc: 构造函数和析构函数  Construction/Destruction
//--------------------------------------------------------------------
cAnimMesh::cAnimMesh()
{
	m_bPlayAnim          = true;
	m_pd3dDevice         = NULL;
    m_pAnimController    = NULL;
    m_pFrameRoot         = NULL;

	m_pAlloc = new cAllocateHierarchy();
}


//-----------------------------------------------------------------------------
// Desc: 构造函数和析构函数 
//-----------------------------------------------------------------------------
cAnimMesh::~cAnimMesh()
{
	D3DXFrameDestroy(m_pFrameRoot, m_pAlloc);
    SAFE_RELEASE(m_pAnimController);
	delete m_pAlloc;
}


//-----------------------------------------------------------------------------
// Desc:创建并加载蒙皮网格模型
//-----------------------------------------------------------------------------
HRESULT cAnimMesh::OnCreate(LPDIRECT3DDEVICE9 pD3DDevice, WCHAR *strFileName)
{
	HRESULT hr;
	m_pd3dDevice = pD3DDevice;
	V_RETURN(LoadFromXFile(strFileName));
	return S_OK;
}


//-----------------------------------------------------------------------------
// Desc: 从文件加载蒙皮网格模型
//-----------------------------------------------------------------------------
HRESULT cAnimMesh::LoadFromXFile(WCHAR *strFileName)
{
    HRESULT hr;

	//根据文件名, 从指定的路经查找文件
	WCHAR strPath[MAX_PATH];
	DXUTFindDXSDKMediaFileCch( strPath, sizeof(strPath) / sizeof(WCHAR), strFileName );

	//从.X文件加载层次框架和动画数据
    V_RETURN(D3DXLoadMeshHierarchyFromX(strPath, D3DXMESH_MANAGED, m_pd3dDevice, 
		                            m_pAlloc, NULL, &m_pFrameRoot, &m_pAnimController));
	
	//计算框架对象的边界球
    V_RETURN(D3DXFrameCalculateBoundingSphere(m_pFrameRoot, &m_vObjectCenter, &m_fObjectRadius));

	return S_OK;
}


//-----------------------------------------------------------------------------
// Desc: 更新框架并绘制框架
//       (1)用m_pAnimController->AdvanceTime()设置时间，m_pAnimController是
//          类LPD3DXANIMATIONCONTROLLER的一个对象
//       (2)用函数CSkinMesh::UpdateFrameMatrices()更新框架
//       (3)用函数CSkinMesh::DrawFrame()绘制框架
//-----------------------------------------------------------------------------
HRESULT cAnimMesh::Render(D3DXMATRIXA16* matWorld, double fElapsedAppTime)
{
    if( 0.0f == fElapsedAppTime ) 
        return S_OK;

	if (m_bPlayAnim && m_pAnimController != NULL)
		m_pAnimController->AdvanceTime( fElapsedAppTime, NULL );

	UpdateFrameMatrices(m_pFrameRoot, matWorld);  //调用子函数
	DrawFrame(m_pFrameRoot);  //调用子函数

	return S_OK;
}


//-----------------------------------------------------------------------------
// Desc:计算各个骨骼的组合变换矩阵
//-----------------------------------------------------------------------------
VOID cAnimMesh::UpdateFrameMatrices(LPD3DXFRAME pFrameBase, LPD3DXMATRIX pParentMatrix)
{
    D3DXFRAME_DERIVED *pFrame = (D3DXFRAME_DERIVED*)pFrameBase;

    if (pParentMatrix != NULL)
        D3DXMatrixMultiply(&pFrame->CombinedTransformationMatrix, &pFrame->TransformationMatrix, pParentMatrix);
    else
        pFrame->CombinedTransformationMatrix = pFrame->TransformationMatrix;

    if (pFrame->pFrameSibling != NULL)
    {
        UpdateFrameMatrices(pFrame->pFrameSibling, pParentMatrix);
    }

    if (pFrame->pFrameFirstChild != NULL)
    {
        UpdateFrameMatrices(pFrame->pFrameFirstChild, &pFrame->CombinedTransformationMatrix);
    }
}


//-----------------------------------------------------------------------------
// Desc: 绘制框架.
//       先用CSkinMesh::DrawMeshContainer()绘制一个LPD3DXMESHCONTAINER类型
//       的变量pMeshContainer.然后递归绘制同一级框架和子一级框架。
//-----------------------------------------------------------------------------
VOID cAnimMesh::DrawFrame(LPD3DXFRAME pFrame)
{
    LPD3DXMESHCONTAINER pMeshContainer;
	
    pMeshContainer = pFrame->pMeshContainer;
    while (pMeshContainer != NULL)
    {
        DrawMeshContainer(pMeshContainer, pFrame); //调用子函数
        pMeshContainer = pMeshContainer->pNextMeshContainer;
    }
	
    if (pFrame->pFrameSibling != NULL)
    {
        DrawFrame(pFrame->pFrameSibling);
    }
	
    if (pFrame->pFrameFirstChild != NULL)
    {
        DrawFrame(pFrame->pFrameFirstChild);
    }
}


//-----------------------------------------------------------------------------
// Name: DrawMeshContainer()
// Desc: Called to render a mesh in the hierarchy
//-----------------------------------------------------------------------------
VOID cAnimMesh::DrawMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase, LPD3DXFRAME pFrameBase)
{
	D3DXMESHCONTAINER_DERIVED *pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerBase;
	D3DXFRAME_DERIVED *pFrame = (D3DXFRAME_DERIVED*)pFrameBase;
	UINT iMaterial;

	m_pd3dDevice->SetTransform(D3DTS_WORLD, &pFrame->CombinedTransformationMatrix);

	for (iMaterial = 0; iMaterial < pMeshContainer->NumMaterials; iMaterial++)
	{
		m_pd3dDevice->SetMaterial( &pMeshContainer->pMaterials[iMaterial].MatD3D );
		m_pd3dDevice->SetTexture( 0, pMeshContainer->ppTextures[iMaterial] );
		pMeshContainer->MeshData.pMesh->DrawSubset(iMaterial);
	}
}

//-----------------------------------------------------------------------------
// Desc: 释放蒙皮网格模型
//-----------------------------------------------------------------------------
HRESULT cAnimMesh::OnDestory()
{
	delete this;
	return S_OK;
}

//--------------------------------------------------------------------
// Desc: 构造函数和析构函数  Construction/Destruction
//--------------------------------------------------------------------
cSkinMesh::cSkinMesh()
{
	m_bPlayAnim          = true;
	m_pd3dDevice         = NULL;
    m_pAnimController    = NULL;
    m_pFrameRoot         = NULL;
    m_pBoneMatrices      = NULL;
    m_NumBoneMatricesMax = 0;

	m_pAlloc = new cAllocateHierarchy();
}


//-----------------------------------------------------------------------------
// Desc: 构造函数和析构函数 
//-----------------------------------------------------------------------------
cSkinMesh::~cSkinMesh()
{
	D3DXFrameDestroy(m_pFrameRoot, m_pAlloc);
    SAFE_RELEASE(m_pAnimController);
	delete m_pAlloc;
}


//-----------------------------------------------------------------------------
// Desc:创建并加载蒙皮网格模型
//-----------------------------------------------------------------------------
HRESULT cSkinMesh::OnCreate(LPDIRECT3DDEVICE9 pD3DDevice, WCHAR *strFileName)
{
	HRESULT hr;
	m_pd3dDevice = pD3DDevice;
	hr = LoadFromXFile(strFileName);
	if(FAILED(hr))
		return hr;
	return S_OK;
}


//-----------------------------------------------------------------------------
// Desc: 从文件加载蒙皮网格模型
//-----------------------------------------------------------------------------
HRESULT cSkinMesh::LoadFromXFile(WCHAR *strFileName)
{
    HRESULT hr;

	//根据文件名, 从指定的路经查找文件
	WCHAR strPath[MAX_PATH];
	DXUTFindDXSDKMediaFileCch( strPath, sizeof(strPath) / sizeof(WCHAR), strFileName );

	//从.X文件加载层次框架和动画数据
    V_RETURN(D3DXLoadMeshHierarchyFromX(strPath, D3DXMESH_MANAGED, m_pd3dDevice, 
		                            m_pAlloc, NULL, &m_pFrameRoot, &m_pAnimController));

	//建立各级框架的组合变换矩阵
    V_RETURN(SetupBoneMatrixPointers(m_pFrameRoot));  
	
	//计算框架对象的边界球
    hr = D3DXFrameCalculateBoundingSphere(m_pFrameRoot, &m_vObjectCenter, &m_fObjectRadius);
	if (FAILED(hr))
        return hr;

	return S_OK;
}


//--------------------------------------------------------------------------
// Desc: 仅在LoadFromXFile中调用。调用子函数SetupBoneMatrixPointersOnMesh()
//       安置好各级框架(实际上是各个骨骼)的组合变换矩阵。
// 注意: 在这里其实并没有计算出各个骨骼的组合变换矩阵，只是为每个矩阵开辟了相应
//       的存储空间，真正的计算是在函数CSkinMesh::UpdateFrameMatrices()中完成的。
//---------------------------------------------------------------------------
HRESULT cSkinMesh::SetupBoneMatrixPointers(LPD3DXFRAME pFrame)
{
    HRESULT hr;

    if (pFrame->pMeshContainer != NULL)
    {
        hr = SetupBoneMatrixPointersOnMesh(pFrame->pMeshContainer);  //调用子函数
        if (FAILED(hr))
            return hr;
    }

    if (pFrame->pFrameSibling != NULL)
    {
        hr = SetupBoneMatrixPointers(pFrame->pFrameSibling);   //递归
        if (FAILED(hr))
            return hr;
    }

    if (pFrame->pFrameFirstChild != NULL)
    {
        hr = SetupBoneMatrixPointers(pFrame->pFrameFirstChild);  //递归
        if (FAILED(hr))
            return hr;
    }

    return S_OK;
}


//-----------------------------------------------------------------------------
// Desc: 仅在SetupBoneMatrixPointers()中被调用，设置每个骨骼的组合变换矩阵
//-----------------------------------------------------------------------------
HRESULT cSkinMesh::SetupBoneMatrixPointersOnMesh(LPD3DXMESHCONTAINER pMeshContainerBase)
{
    UINT iBone, cBones;  // cBones表示骨骼数量，iBone表示循环变量
    D3DXFRAME_DERIVED *pFrame;

	//先强制转为扩展型
    D3DXMESHCONTAINER_DERIVED *pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerBase;

    // 只有蒙皮网格模型才有骨骼矩阵
    if (pMeshContainer->pSkinInfo != NULL)
    {
		//得到骨骼数量
        cBones = pMeshContainer->pSkinInfo->GetNumBones();

		//申请存储骨骼矩阵的空间
        pMeshContainer->ppBoneMatrixPtrs = new D3DXMATRIX*[cBones];     
		if (pMeshContainer->ppBoneMatrixPtrs == NULL)
            return E_OUTOFMEMORY;

        for (iBone = 0; iBone < cBones; iBone++)
        {
			//找到框架
            pFrame = (D3DXFRAME_DERIVED*)D3DXFrameFind(m_pFrameRoot, pMeshContainer->pSkinInfo->GetBoneName(iBone));
            if (pFrame == NULL)
                return E_FAIL;

			//将框架的组合变换矩阵赋值给对应的骨骼的复合变换矩阵
            pMeshContainer->ppBoneMatrixPtrs[iBone] = &pFrame->CombinedTransformationMatrix;
		}
    }

    return S_OK;
}

//-----------------------------------------------------------------------------
// Desc: 切换动画
//-----------------------------------------------------------------------------
BOOL cSkinMesh::SwitchAnim(LPSTR AnimName)
{
	LPD3DXANIMATIONSET pAS;
	if(FAILED(m_pAnimController->GetAnimationSetByName(AnimName, &pAS)))
		return FALSE;
	if(FAILED(m_pAnimController->SetTrackAnimationSet( 0, pAS)))
		return FALSE;
	return TRUE;
}

//-----------------------------------------------------------------------------
// Desc: 更新框架并绘制框架
//       (1)用m_pAnimController->AdvanceTime()设置时间，m_pAnimController是
//          类LPD3DXANIMATIONCONTROLLER的一个对象
//       (2)用函数CSkinMesh::UpdateFrameMatrices()更新框架
//       (3)用函数CSkinMesh::DrawFrame()绘制框架
//-----------------------------------------------------------------------------
HRESULT cSkinMesh::Render(D3DXMATRIXA16* matWorld, float fElapsedAppTime)
{
    if( 0.0f == fElapsedAppTime ) 
        return S_OK;

	if (m_bPlayAnim && m_pAnimController != NULL)
		m_pAnimController->AdvanceTime( fElapsedAppTime, NULL );

	UpdateFrameMatrices(m_pFrameRoot, matWorld);  //调用子函数
	DrawFrame(m_pFrameRoot);  //调用子函数

	return S_OK;
}


//-----------------------------------------------------------------------------
// Desc:计算各个骨骼的组合变换矩阵
//-----------------------------------------------------------------------------
VOID cSkinMesh::UpdateFrameMatrices(LPD3DXFRAME pFrameBase, LPD3DXMATRIX pParentMatrix)
{
    D3DXFRAME_DERIVED *pFrame = (D3DXFRAME_DERIVED*)pFrameBase;

    if (pParentMatrix != NULL)
        D3DXMatrixMultiply(&pFrame->CombinedTransformationMatrix, &pFrame->TransformationMatrix, pParentMatrix);
    else
        pFrame->CombinedTransformationMatrix = pFrame->TransformationMatrix;

    if (pFrame->pFrameSibling != NULL)
    {
        UpdateFrameMatrices(pFrame->pFrameSibling, pParentMatrix);
    }

    if (pFrame->pFrameFirstChild != NULL)
    {
        UpdateFrameMatrices(pFrame->pFrameFirstChild, &pFrame->CombinedTransformationMatrix);
    }
}


//-----------------------------------------------------------------------------
// Desc: 绘制框架.
//       先用CSkinMesh::DrawMeshContainer()绘制一个LPD3DXMESHCONTAINER类型
//       的变量pMeshContainer.然后递归绘制同一级框架和子一级框架。
//-----------------------------------------------------------------------------
VOID cSkinMesh::DrawFrame(LPD3DXFRAME pFrame)
{
    LPD3DXMESHCONTAINER pMeshContainer;
	
    pMeshContainer = pFrame->pMeshContainer;
    while (pMeshContainer != NULL)
    {
        DrawMeshContainer(pMeshContainer, pFrame); //调用子函数
        pMeshContainer = pMeshContainer->pNextMeshContainer;
    }
	
    if (pFrame->pFrameSibling != NULL)
    {
        DrawFrame(pFrame->pFrameSibling);
    }
	
    if (pFrame->pFrameFirstChild != NULL)
    {
        DrawFrame(pFrame->pFrameFirstChild);
    }
}


//-----------------------------------------------------------------------------
// Name: DrawMeshContainer()
// Desc: Called to render a mesh in the hierarchy
//-----------------------------------------------------------------------------
VOID cSkinMesh::DrawMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase, LPD3DXFRAME pFrameBase)
{
	D3DXMESHCONTAINER_DERIVED* pMeshContainer = ( D3DXMESHCONTAINER_DERIVED* )pMeshContainerBase;
	D3DXFRAME_DERIVED* pFrame = ( D3DXFRAME_DERIVED* )pFrameBase;
	UINT iMaterial;
	UINT NumBlend;
	UINT iAttrib;
	DWORD AttribIdPrev;
	LPD3DXBONECOMBINATION pBoneComb;

	UINT iMatrixIndex;
	D3DXMATRIXA16 matTemp;

	D3DCAPS9 d3dCaps;
	m_pd3dDevice->GetDeviceCaps( &d3dCaps );

	// first check for skinning
	if( pMeshContainer->pSkinInfo != NULL )
	{
		AttribIdPrev = UNUSED32;
		pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>( pMeshContainer->pBoneCombinationBuf->GetBufferPointer() );

		// Draw using default vtx processing of the device (typically HW)
		for( iAttrib = 0; iAttrib < pMeshContainer->NumAttributeGroups; iAttrib++ )
		{
			NumBlend = 0;
			for( DWORD i = 0; i < pMeshContainer->NumInfl; ++i )
			{
				if( pBoneComb[iAttrib].BoneId[i] != UINT_MAX )
				{
					NumBlend = i;
				}
			}

			if( d3dCaps.MaxVertexBlendMatrices >= NumBlend + 1 )
			{
				// first calculate the world matrices for the current set of blend weights and get the accurate count of the number of blends
				for( DWORD i = 0; i < pMeshContainer->NumInfl; ++i )
				{
					iMatrixIndex = pBoneComb[iAttrib].BoneId[i];
					if( iMatrixIndex != UINT_MAX )
					{
						D3DXMatrixMultiply( &matTemp, &pMeshContainer->pBoneOffsetMatrices[iMatrixIndex],
							pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex] );
						m_pd3dDevice->SetTransform( D3DTS_WORLDMATRIX( i ), &matTemp );
					}
				}

				m_pd3dDevice->SetRenderState( D3DRS_VERTEXBLEND, NumBlend );

				// lookup the material used for this subset of faces
				if( ( AttribIdPrev != pBoneComb[iAttrib].AttribId ) || ( AttribIdPrev == UNUSED32 ) )
				{
					m_pd3dDevice->SetMaterial( &pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D );
					m_pd3dDevice->SetTexture( 0, pMeshContainer->ppTextures[pBoneComb[iAttrib].AttribId] );
					AttribIdPrev = pBoneComb[iAttrib].AttribId;
				}

				// draw the subset now that the correct material and matrices are loaded
				pMeshContainer->MeshData.pMesh->DrawSubset( iAttrib );
			}
		}
		m_pd3dDevice->SetRenderState( D3DRS_VERTEXBLEND, 0 );
	}
	else  // standard mesh, just draw it after setting material properties
	{
		m_pd3dDevice->SetTransform( D3DTS_WORLD, &pFrame->CombinedTransformationMatrix );

		for( iMaterial = 0; iMaterial < pMeshContainer->NumMaterials; iMaterial++ )
		{
			m_pd3dDevice->SetMaterial( &pMeshContainer->pMaterials[iMaterial].MatD3D );
			m_pd3dDevice->SetTexture( 0, pMeshContainer->ppTextures[iMaterial] );
			pMeshContainer->MeshData.pMesh->DrawSubset( iMaterial );
		}
	}
}


//-----------------------------------------------------------------------------
// Desc: 释放蒙皮网格模型
//-----------------------------------------------------------------------------
HRESULT cSkinMesh::OnDestory()
{
	delete this;
	return S_OK;
}

cTexture::cTexture()
{
  m_Graphics = NULL;
  m_Texture = NULL;
  m_Width = m_Height = 0;
}

cTexture::~cTexture()
{
  Free();
}

BOOL cTexture::Load(cGraphics *Graphics, LPTSTR Filename, DWORD Transparent, D3DFORMAT Format)
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

BOOL cTexture::Create(cGraphics *Graphics, IDirect3DTexture9 *Texture)
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

BOOL cTexture::Free()
{
  ReleaseCOM(m_Texture);
  m_Graphics = NULL;
  m_Width = m_Height = 0;

  return TRUE;
}

BOOL cTexture::IsLoaded()
{
  if(m_Texture == NULL)
    return FALSE;
  return TRUE;
}

IDirect3DTexture9 *cTexture::GetTextureCOM()
{
  return m_Texture;
}

long cTexture::GetWidth()
{
  D3DSURFACE_DESC d3dsd;

  if(m_Texture == NULL)
    return 0;

  if(FAILED(m_Texture->GetLevelDesc(0, &d3dsd)))
    return 0;

  return d3dsd.Width;
}

long cTexture::GetHeight()
{
  D3DSURFACE_DESC d3dsd;

  if(m_Texture == NULL)
    return 0;

  if(FAILED(m_Texture->GetLevelDesc(0, &d3dsd)))
    return 0;

  return d3dsd.Height;
}

D3DFORMAT cTexture::GetFormat()
{
  D3DSURFACE_DESC d3dsd;

  if(m_Texture == NULL)
    return D3DFMT_UNKNOWN;

  if(FAILED(m_Texture->GetLevelDesc(0, &d3dsd)))
    return D3DFMT_UNKNOWN;

  return d3dsd.Format;
}

BOOL cTexture::Blit(long DestX, long DestY,                   \
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

cBumpTexture::cBumpTexture()
{
  m_Graphics = NULL;
  m_BumpTex = NULL;
  m_BackTex = NULL;
  m_pVB = NULL;
  m_Width = m_Height = 0;
}

cBumpTexture::~cBumpTexture()
{
	Free();
}

const DWORD sBumpVertex::FVF = D3DFVF_XYZ | D3DFVF_TEX2;

BOOL cBumpTexture::Load(cGraphics *Graphics, LPTSTR Filename, DWORD Transparent, D3DFORMAT Format)
{
	Free();

	if((m_Graphics = Graphics) == NULL)
		return FALSE;
	if(Graphics->GetDeviceCOM() == NULL)
		return FALSE;
	if(Filename == NULL)
		return FALSE;

	HRESULT hr = S_OK;

	D3DCAPS9 d3dCaps;
	m_Graphics->GetDeviceCOM()->GetDeviceCaps(&d3dCaps);

	D3DDISPLAYMODE d3dDM;
	m_Graphics->GetDirect3DCOM()->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3dDM);

	// 检查设备是否支持凹凸纹理映射
	if(0 == (d3dCaps.TextureOpCaps & D3DTEXOPCAPS_BUMPENVMAP))
		return FALSE;
	// 检查设备是否支持D3DFMT_V8U8格式的纹理
	V_RETURN(m_Graphics->GetDirect3DCOM()->CheckDeviceFormat(
		d3dCaps.AdapterOrdinal, d3dCaps.DeviceType, d3dDM.Format,
		0, D3DRTYPE_TEXTURE, D3DFMT_V8U8));

	UINT iWidth  = 256;
	UINT iHeight = 256;

	// 创建凹凸纹理对象
	V_RETURN( m_Graphics->GetDeviceCOM()->CreateTexture( iWidth, iHeight, 1, 0,
		D3DFMT_V8U8, D3DPOOL_MANAGED, &m_BumpTex, NULL ) );

	// 填充纹理元素
	D3DLOCKED_RECT lrDst;
	m_BumpTex->LockRect( 0, &lrDst, 0, 0 );
	BYTE* pDst = (BYTE*)lrDst.pBits;

	for( DWORD y=0; y<iHeight; y++ )
	{
		for( DWORD x=0; x<iWidth; x++ )
		{
			FLOAT fx = x/(FLOAT)iWidth  - 0.5f;
			FLOAT fy = y/(FLOAT)iHeight - 0.5f;

			CHAR iDu = (CHAR)(64*cosf(4.0f*(fx+fy)*D3DX_PI));
			CHAR iDv = (CHAR)(64*sinf(4.0f*(fx+fy)*D3DX_PI));

			pDst[2*x+0] = iDu;
			pDst[2*x+1] = iDv;
		}
		pDst += lrDst.Pitch;
	}

	m_BumpTex->UnlockRect(0);

	// 创建背景纹理对象
	// V_RETURN( D3DXCreateTextureFromFile( m_Graphics->GetDeviceCOM(), Filename, &m_BackTex ));
	if(FAILED(D3DXCreateTextureFromFileEx(	Graphics->GetDeviceCOM(),
											Filename, D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT,
											0, Format, D3DPOOL_MANAGED, 
											D3DX_FILTER_TRIANGLE, D3DX_FILTER_TRIANGLE,
											Transparent, NULL, NULL, &m_BackTex	)))
	{
		return FALSE;
	}

	m_Width = GetWidth();
	m_Height = GetHeight();

	// 创建顶点缓冲区
	V_RETURN( m_Graphics->GetDeviceCOM()->CreateVertexBuffer( 4*sizeof(sBumpVertex),
		                                      D3DUSAGE_WRITEONLY, sBumpVertex::FVF,
		                                      D3DPOOL_MANAGED, &m_pVB, NULL ));

	// 填充顶点缓冲区
	sBumpVertex* v;
	m_pVB->Lock( 0, 0, (void**)&v, 0 );
	v[0].p = D3DXVECTOR3(-60.0f,-60.0f, 0.0f );
	v[1].p = D3DXVECTOR3(-60.0f, 60.0f, 0.0f ); 
	v[2].p = D3DXVECTOR3( 60.0f,-60.0f, 0.0f ); 
	v[3].p = D3DXVECTOR3( 60.0f, 60.0f, 0.0f ); 
	v[0].tu2 = 0.000f;
	v[0].tv2 = 1.0f;
	v[1].tu2 = 0.000f;
	v[1].tv2 = 0.0f;
	v[2].tu2 = 1.000f; 
	v[2].tv2 = 1.0f;
	v[3].tu2 = 1.000f;
	v[3].tv2 = 0.0f;
	m_pVB->Unlock();

	return TRUE;
}

BOOL cBumpTexture::Create(cGraphics *Graphics, IDirect3DTexture9 *Texture)
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
	m_Graphics->GetDeviceCOM()->CreateTexture(m_Width, m_Height, 0, 0, Desc.Format, D3DPOOL_MANAGED, &m_BackTex, NULL);

	Texture->LockRect(0, &SrcRect, NULL, D3DLOCK_READONLY);
	m_BackTex->LockRect(0, &DestRect, NULL, 0);

	memcpy(DestRect.pBits, SrcRect.pBits, SrcRect.Pitch * m_Height);

	m_BackTex->UnlockRect(0);
	Texture->UnlockRect(0);

	return TRUE;
}

BOOL cBumpTexture::Free()
{
	ReleaseCOM(m_pVB);
	ReleaseCOM(m_BackTex);
	ReleaseCOM(m_BumpTex);

	m_Graphics = NULL;
	m_Width = m_Height = 0;

	return TRUE;
}

long cBumpTexture::GetWidth()
{
	D3DSURFACE_DESC d3dsd;

	if(m_BackTex == NULL)
		return 0;

	if(FAILED(m_BackTex->GetLevelDesc(0, &d3dsd)))
		return 0;

	return d3dsd.Width;
}

long cBumpTexture::GetHeight()
{
	D3DSURFACE_DESC d3dsd;

	if(m_BackTex == NULL)
		return 0;

	if(FAILED(m_BackTex->GetLevelDesc(0, &d3dsd)))
		return 0;

	return d3dsd.Height;
}

inline DWORD F2DW( FLOAT f ) { return *((DWORD*)&f); }
BOOL cBumpTexture::Render(float dt)
{
	IDirect3DDevice9 *pd3dDevice = m_Graphics->GetDeviceCOM();

	// 实时更新纹理坐标
	sBumpVertex* vWaterVertices;
	m_pVB->Lock( 0, 0, (void**)&vWaterVertices, 0 );
	vWaterVertices[0].tu1 = 0.000f; 
	vWaterVertices[0].tv1 = 0.5f*(float)dt + 2.0f;
	vWaterVertices[1].tu1 = 0.000f; 
	vWaterVertices[1].tv1 = 0.5f*(float)dt;
	vWaterVertices[2].tu1 = 1.000f; 
	vWaterVertices[2].tv1 = 0.5f*(float)dt;
	vWaterVertices[3].tu1 = 1.000f; 
	vWaterVertices[3].tv1 = 0.5f*(float)dt + 2.0f;
	m_pVB->Unlock();

	// 设置纹理状态
	pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_BUMPENVMAP );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_BUMPENVMAT00, F2DW(0.01f) );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_BUMPENVMAT01, F2DW(0.00f) );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_BUMPENVMAT10, F2DW(0.00f) );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_BUMPENVMAT11, F2DW(0.01f) );

	pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1 );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	pd3dDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );

	//渲染水下效果
	pd3dDevice->SetTexture( 0, m_BumpTex );
	pd3dDevice->SetTexture( 1, m_BackTex );
	pd3dDevice->SetFVF( sBumpVertex::FVF );
	pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof(sBumpVertex) );
	pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

	return TRUE;
}

cMaterial::cMaterial()
{
  // Set a default material (white)
  ZeroMemory(&m_Material, sizeof(D3DMATERIAL9));
  SetDiffuseColor(255, 255, 255);
  SetAmbientColor(255, 255, 255);
  SetSpecularColor(255, 255, 255);
  SetEmissiveColor(0, 0, 0);
  SetPower(1.0f);
}

BOOL cMaterial::SetDiffuseColor(unsigned char Red, unsigned char Green, unsigned char Blue)
{
	m_Material.Diffuse.r = 1.0f / 255.0f * (float)Red;
	m_Material.Diffuse.g = 1.0f / 255.0f * (float)Green;
	m_Material.Diffuse.b = 1.0f / 255.0f * (float)Blue;

	return TRUE;
}

BOOL cMaterial::GetDiffuseColor(unsigned char *Red, unsigned char *Green, unsigned char *Blue)
{
	if(Red != NULL)
		*Red = (unsigned char)(255.0f * m_Material.Diffuse.r);

	if(Green != NULL)
		*Green = (unsigned char)(255.0f * m_Material.Diffuse.g);

	if(Blue != NULL)
		*Blue = (unsigned char)(255.0f * m_Material.Diffuse.b);

	return TRUE;
}

BOOL cMaterial::SetAmbientColor(unsigned char Red, unsigned char Green, unsigned char Blue)
{
	m_Material.Ambient.r = 1.0f / 255.0f * (float)Red;
	m_Material.Ambient.g = 1.0f / 255.0f * (float)Green;
	m_Material.Ambient.b = 1.0f / 255.0f * (float)Blue;

	return TRUE;
}

BOOL cMaterial::GetAmbientColor(unsigned char *Red, unsigned char *Green, unsigned char *Blue)
{
	if(Red != NULL)
		*Red = (unsigned char)(255.0f * m_Material.Ambient.r);

	if(Green != NULL)
		*Green = (unsigned char)(255.0f * m_Material.Ambient.g);

	if(Blue != NULL)
		*Blue = (unsigned char)(255.0f * m_Material.Ambient.b);

	return TRUE;
}

BOOL cMaterial::SetSpecularColor(unsigned char Red, unsigned char Green, unsigned char Blue)
{
	m_Material.Specular.r = 1.0f / 255.0f * (float)Red;
	m_Material.Specular.g = 1.0f / 255.0f * (float)Green;
	m_Material.Specular.b = 1.0f / 255.0f * (float)Blue;

	return TRUE;
}

BOOL cMaterial::GetSpecularColor(unsigned char *Red, unsigned char *Green, unsigned char *Blue)
{
	if(Red != NULL)
		*Red = (char)(255.0f * m_Material.Specular.r);

	if(Green != NULL)
		*Green = (char)(255.0f * m_Material.Specular.g);

	if(Blue != NULL)
		*Blue = (char)(255.0f * m_Material.Specular.b);

	return TRUE;
}

BOOL cMaterial::SetEmissiveColor(unsigned char Red, unsigned char Green, unsigned char Blue)
{
	m_Material.Emissive.r = 1.0f / 255.0f * (float)Red;
	m_Material.Emissive.g = 1.0f / 255.0f * (float)Green;
	m_Material.Emissive.b = 1.0f / 255.0f * (float)Blue;

	return TRUE;
}

BOOL cMaterial::GetEmissiveColor(unsigned char *Red, unsigned char *Green, unsigned char *Blue)
{
	if(Red != NULL)
		*Red = (char)(255.0f * m_Material.Emissive.r);

	if(Green != NULL)
		*Green = (char)(255.0f * m_Material.Emissive.g);

	if(Blue != NULL)
		*Blue = (char)(255.0f * m_Material.Emissive.b);

	return TRUE;
}

BOOL cMaterial::SetPower(float Power)
{
	m_Material.Power = Power;
	return TRUE;
}

float cMaterial::GetPower(float Power)
{
	return m_Material.Power;
}

D3DMATERIAL9 *cMaterial::GetMaterial()
{
	return &m_Material;
}

cLight::cLight()
{
	// Set a default light to point
	ZeroMemory(&m_Light, sizeof(D3DLIGHT9));
	SetType(D3DLIGHT_POINT);
	Move(0.0f, 0.0f, 0.0f);
	SetRange(1000.0f);
}

BOOL cLight::SetType(D3DLIGHTTYPE Type)
{
  m_Light.Type = Type;
  return TRUE;
}

BOOL cLight::Move(float XPos, float YPos, float ZPos)
{
  m_Light.Position.x = XPos;
  m_Light.Position.y = YPos;
  m_Light.Position.z = ZPos;
  return TRUE;
}

BOOL cLight::MoveRel(float XPos, float YPos, float ZPos)
{
  m_Light.Position.x += XPos;
  m_Light.Position.y += YPos;
  m_Light.Position.z += ZPos;
  return TRUE;
}

BOOL cLight::GetPos(float *XPos, float *YPos, float *ZPos)
{
  if(XPos != NULL)
    *XPos = m_Light.Position.x;
  if(YPos != NULL)
    *YPos = m_Light.Position.y;
  if(ZPos != NULL)
    *ZPos = m_Light.Position.z;
  return TRUE;
}

BOOL cLight::Point(float XFrom, float YFrom, float ZFrom,
                   float XAt,   float YAt,   float ZAt)
{
  D3DXVECTOR3 vecSrc;
  D3DXVECTOR3 vecDest;

  // Move the light
  Move(XFrom, YFrom, ZFrom);

  // Calculate vector between angles
  D3DXVECTOR3 VecDir = 
	  D3DXVECTOR3(XAt - XFrom, YAt - YFrom, ZAt - ZFrom);
  D3DXVec3Normalize((D3DXVECTOR3*)&m_Light.Direction, &VecDir);

  return TRUE;
}

BOOL cLight::GetDirection(float *XDir, float *YDir, float *ZDir)
{
  if(XDir != NULL)
    *XDir = m_Light.Direction.x;
  if(YDir != NULL)
    *YDir = m_Light.Direction.y;
  if(ZDir != NULL)
    *ZDir = m_Light.Direction.z;
  return TRUE;
}

BOOL cLight::SetDiffuseColor(unsigned char Red, unsigned char Green, unsigned char Blue)
{
  m_Light.Diffuse.r = 1.0f / 255.0f * (float)Red;
  m_Light.Diffuse.g = 1.0f / 255.0f * (float)Green;
  m_Light.Diffuse.b = 1.0f / 255.0f * (float)Blue;

  return TRUE;
}

BOOL cLight::GetDiffuseColor(unsigned char *Red, unsigned char *Green, unsigned char *Blue)
{
  if(Red != NULL)
    *Red = (char)(255.0f * m_Light.Diffuse.r);

  if(Green != NULL)
    *Green = (char)(255.0f * m_Light.Diffuse.g);

  if(Blue != NULL)
    *Blue = (char)(255.0f * m_Light.Diffuse.b);

  return TRUE;
}

BOOL cLight::SetSpecularColor(unsigned char Red, unsigned char Green, unsigned char Blue)
{
  m_Light.Specular.r = 1.0f / 255.0f * (float)Red;
  m_Light.Specular.g = 1.0f / 255.0f * (float)Green;
  m_Light.Specular.b = 1.0f / 255.0f * (float)Blue;

  return TRUE;
}

BOOL cLight::GetSpecularColor(unsigned char *Red, unsigned char *Green, unsigned char *Blue)
{
  if(Red != NULL)
    *Red = (char)(255.0f * m_Light.Specular.r);

  if(Green != NULL)
    *Green = (char)(255.0f * m_Light.Specular.g);

  if(Blue != NULL)
    *Blue = (char)(255.0f * m_Light.Specular.b);

  return TRUE;
}

BOOL cLight::SetAmbientColor(unsigned char Red, unsigned char Green, unsigned char Blue)
{
  m_Light.Ambient.r = 1.0f / 255.0f * (float)Red;
  m_Light.Ambient.g = 1.0f / 255.0f * (float)Green;
  m_Light.Ambient.b = 1.0f / 255.0f * (float)Blue;

  return TRUE;
}

BOOL cLight::GetAmbientColor(unsigned char *Red, unsigned char *Green, unsigned char *Blue)
{
  if(Red != NULL)
    *Red = (char)(255.0f * m_Light.Ambient.r);

  if(Green != NULL)
    *Green = (char)(255.0f * m_Light.Ambient.g);

  if(Blue != NULL)
    *Blue = (char)(255.0f * m_Light.Ambient.b);

  return TRUE;
}

BOOL cLight::SetRange(float Range)
{
  m_Light.Range = Range;
  return TRUE;
}

float cLight::GetRange()
{
  return m_Light.Range;
}

BOOL cLight::SetFalloff(float Falloff)
{
  m_Light.Falloff = Falloff;
  return TRUE;
}

float cLight::GetFalloff()
{
  return m_Light.Falloff;
}

BOOL cLight::SetAttenuation0(float Attenuation)
{
  m_Light.Attenuation0 = Attenuation;
  return TRUE;
}

float cLight::GetAttenuation0()
{
  return m_Light.Attenuation0;
}

BOOL cLight::SetAttenuation1(float Attenuation)
{
  m_Light.Attenuation1 = Attenuation;
  return TRUE;
}

float cLight::GetAttenuation1()
{
  return m_Light.Attenuation1;
}

BOOL cLight::SetAttenuation2(float Attenuation)
{
  m_Light.Attenuation2 = Attenuation;
  return TRUE;
}

float cLight::GetAttenuation2()
{
  return m_Light.Attenuation2;
}

BOOL cLight::SetTheta(float Theta)
{
  m_Light.Theta = Theta;
  return TRUE;
}

float cLight::GetTheta()
{
  return m_Light.Theta;
}

BOOL cLight::SetPhi(float Phi)
{
  m_Light.Phi = Phi;
  return TRUE;
}

float cLight::GetPhi()
{
  return m_Light.Phi;
}

D3DLIGHT9 *cLight::GetLight()
{
  return &m_Light;
}

cVertexBuffer::cVertexBuffer()
{
  m_Graphics = NULL;
  m_pVB = NULL;
  m_NumVertices = 0;
  m_FVF = 0;
  m_Locked = FALSE;
  m_Ptr = NULL;
}

cVertexBuffer::~cVertexBuffer()
{
  Free();
}

IDirect3DVertexBuffer9 *cVertexBuffer::GetVertexBufferCOM()
{
  return m_pVB;
}

unsigned long cVertexBuffer::GetVertexSize()
{
  return D3DXGetFVFVertexSize(m_FVF);
}

unsigned long cVertexBuffer::GetVertexFVF()
{
  return m_FVF;
}

unsigned long cVertexBuffer::GetNumVertices()
{
  return m_NumVertices;
}

BOOL cVertexBuffer::Create(cGraphics *Graphics, unsigned long NumVertices, DWORD Descriptor, long VertexSize)
{
  Free();

  if((m_Graphics = Graphics) == NULL)
    return FALSE;
  if(m_Graphics->GetDeviceCOM() == NULL)
    return FALSE;
  if(!(m_NumVertices = NumVertices) || !(m_FVF = Descriptor) || !(m_VertexSize = VertexSize))
    return FALSE;

  if(FAILED(m_Graphics->GetDeviceCOM()->CreateVertexBuffer(
       m_NumVertices * m_VertexSize,
       0, m_FVF,
       D3DPOOL_DEFAULT, &m_pVB, NULL)))
    return FALSE;

  return TRUE;
}

BOOL cVertexBuffer::Free()
{
  Unlock();
  ReleaseCOM(m_pVB);
  m_Graphics = NULL;
  m_NumVertices = 0;
  m_FVF = 0;
  m_Locked = FALSE;
  m_Ptr = NULL;

  return TRUE;
}

BOOL cVertexBuffer::Set(unsigned long FirstVertex, unsigned long NumVertices, void *VertexList)
{
  if(m_Graphics == NULL || VertexList == NULL || m_pVB == NULL)
    return FALSE;
  if(m_Graphics->GetDeviceCOM() == NULL)
    return FALSE;

  // Lock the vertex buffer
  if(Lock(FirstVertex, NumVertices) == FALSE)
    return FALSE;

  // Copy vertices to vertex buffer
  memcpy(m_Ptr, VertexList, NumVertices * m_VertexSize);

  // Unlock vertex buffer
  if(Unlock() == FALSE)
    return FALSE;

  return TRUE;
}

BOOL cVertexBuffer::Render(unsigned long FirstVertex, unsigned long NumPrimitives, DWORD Type)
{
	if(m_Graphics->GetDeviceCOM() == NULL || m_pVB == NULL)
		return FALSE;

	m_Graphics->GetDeviceCOM()->SetStreamSource(0, m_pVB, 0, m_VertexSize);
	m_Graphics->GetDeviceCOM()->SetFVF(m_FVF);
	m_Graphics->GetDeviceCOM()->DrawPrimitive((D3DPRIMITIVETYPE)Type, FirstVertex, NumPrimitives);
  
	return TRUE;
}

BOOL cVertexBuffer::Lock(unsigned long FirstVertex, unsigned long NumVertices)
{
	if(m_pVB == NULL)
		return FALSE;

	if(FAILED(m_pVB->Lock(FirstVertex * m_VertexSize, NumVertices * m_VertexSize, (void**)&m_Ptr, 0)))
		return FALSE;

	m_Locked = TRUE;

	return TRUE;
}

BOOL cVertexBuffer::Unlock()
{ 
  if(m_pVB == NULL)
    return FALSE;

  if(FAILED(m_pVB->Unlock()))
    return FALSE;

  m_Locked = FALSE;

  return TRUE;
}

BOOL cVertexBuffer::IsLoaded()
{
  return (m_pVB == NULL) ? FALSE : TRUE;
}

void *cVertexBuffer::GetPtr()
{
  return (void*)m_Ptr;
}

