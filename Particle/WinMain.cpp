#include "Global.h"

// The particle vertex format and descriptor
typedef struct {
	FLOAT		x, y, z;    // 3D coordinates
	D3DCOLOR	Diffuse;	// Color
	FLOAT		u, v;       // Texture coordinates
} sVertex;
#define VERTEXFVF (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1)

// Create a structure for tracking particles
typedef struct sParticle {
	float    XPos, YPos, ZPos;  // Coordinates
	float    XAdd, YAdd, ZAdd;  // Movement values
	float    Red, Green, Blue;  // Colors
	long     Timer, Counter;    // Current and update counter

	sParticle()
	{
		// Position particle at origin
		XPos = YPos = ZPos = 0.0f;

		// Get a random update counter
		Counter = rand() % 50 + 10;
		Timer = 0;

		// Get a random speed
		XAdd = (float)(rand() % 11) - 5.0f;
		YAdd = (float)(rand() % 11) - 5.0f;
		ZAdd = (float)(rand() % 11) - 5.0f;

		// Get a random color
		Red   = (float)(rand() % 101) / 100.0f;
		Green = (float)(rand() % 101) / 100.0f;
		Blue  = (float)(rand() % 101) / 100.0f;
	}
} sParticle;
#define PARTICLE_COUNT 512

class cApp : public cApplication
{
private:
	IDirect3D9			*m_pD3D;
	IDirect3DDevice9	*m_pD3DDevice;

	sParticle				*m_pParticles;
	IDirect3DVertexBuffer9	*m_pParticleVB;
	IDirect3DTexture9		*m_pParticleTexture;

	void SetupParticles();

public:
	cApp();

	BOOL Init();
	BOOL Frame();
	BOOL Shutdown();
};

cApp::cApp()
{
	m_Width = 800;
	m_Height = 480;
	_tcscpy(m_Class, TEXT("Particle"));
	_tcscpy(m_Caption, TEXT("Particle demo by pf_d"));
}

BOOL cApp::Init()
{
	D3DPRESENT_PARAMETERS d3dpp;
	D3DDISPLAYMODE        d3ddm;
	D3DXMATRIX            matView, matProj;

	// Do a windowed mode initialization of Direct3D
	if((m_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
		return FALSE;
	if(FAILED(m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
		return FALSE;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = d3ddm.Format;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	if(FAILED(m_pD3D->CreateDevice(	D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, GethWnd(),
									D3DCREATE_SOFTWARE_VERTEXPROCESSING,
									&d3dpp, &m_pD3DDevice)))
		return FALSE;

	// Set the render states
	m_pD3DDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);

	// Set ambient light to highest level (to see particles)
	m_pD3DDevice->SetRenderState(D3DRS_AMBIENT, 0xFFFFFFFF);

	// Create and set the projection transformation
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI/4, 1.0f, 1.0f, 1000.0f);
	m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &matProj);

	// Create and set the view transformation
	D3DXMatrixLookAtLH(&matView, &D3DXVECTOR3(0.0f, 0.0f, -500.0f), &D3DXVECTOR3(0.0f, 0.0f, 0.0f), &D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	m_pD3DDevice->SetTransform(D3DTS_VIEW, &matView);

	// Create the vertices
	SetupParticles();

	return TRUE;
}

void cApp::SetupParticles()
{
	void *Ptr;
	sVertex Verts[4] = 
	{
		{ -50.0f, 50.0f, 0.0f, 0xFFFFFFFF, 0.0f, 0.0f },
		{  50.0f, 50.0f, 0.0f, 0xFFFFFFFF, 1.0f, 0.0f },
		{ -50.0f,  0.0f, 0.0f, 0xFFFFFFFF, 0.0f, 1.0f },
		{  50.0f,  0.0f, 0.0f, 0xFFFFFFFF, 1.0f, 1.0f }
	};

	// Create vertex buffers and set data
	m_pD3DDevice->CreateVertexBuffer(sizeof(Verts)*4, 0, VERTEXFVF, D3DPOOL_DEFAULT, &m_pParticleVB, NULL);
	m_pParticleVB->Lock(0, 0, (void**)&Ptr, 0);
	memcpy(Ptr, Verts, sizeof(Verts));
	m_pParticleVB->Unlock();

	// Get textures 
	D3DXCreateTextureFromFile(m_pD3DDevice, TEXT("Particle.bmp"), &m_pParticleTexture);

	// Create some particles
	m_pParticles = new sParticle[PARTICLE_COUNT]();
}

BOOL cApp::Frame()
{
	D3DXMATRIX matView, matWorld, matTransposed, matTrans;
	static D3DMATERIAL9 Mat;
	static BOOL InitMat = TRUE;
	static DWORD Counter = timeGetTime();

	// Limit to 30fps
	if(timeGetTime() < Counter+33)
		return TRUE;
	Counter = timeGetTime();

	// Configure the material if first time called
	if(InitMat == TRUE) 
	{
		InitMat = FALSE;
		ZeroMemory(&Mat, sizeof(Mat));
		Mat.Diffuse.a = Mat.Ambient.a = 0.5f;
	}

	// Clear device backbuffer
	m_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0,64,128,255), 1.0f, 0);

	if(SUCCEEDED(m_pD3DDevice->BeginScene())) 
	{

		// Set the particle source, shader, and texture
		m_pD3DDevice->SetStreamSource(0, m_pParticleVB, 0, sizeof(sVertex));
		m_pD3DDevice->SetFVF(VERTEXFVF);
		m_pD3DDevice->SetTexture(0, m_pParticleTexture);

		// Get and set the transposed view matrix (billboard technique)
		m_pD3DDevice->GetTransform(D3DTS_VIEW, &matView);
		D3DXMatrixTranspose(&matTransposed, &matView);

		// Enable alpha blending
		m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

		// Loop through all particles and draw them
		for(short i=0;i<PARTICLE_COUNT;i++) 
		{

			// Move particle first
			m_pParticles[i].XPos += m_pParticles[i].XAdd;
			m_pParticles[i].YPos += m_pParticles[i].YAdd;
			m_pParticles[i].ZPos += m_pParticles[i].ZAdd;

			// Reverse movements if past counter
			if((m_pParticles[i].Timer += 1) >= m_pParticles[i].Counter) 
			{
				m_pParticles[i].Timer = 0;
				m_pParticles[i].XAdd *= -1.0f;
				m_pParticles[i].YAdd *= -1.0f;
				m_pParticles[i].ZAdd *= -1.0f;
			}

			// Setup the particle's world transformation
			D3DXMatrixTranslation(&matTrans, m_pParticles[i].XPos, m_pParticles[i].YPos, m_pParticles[i].ZPos);
			D3DXMatrixMultiply(&matWorld, &matTrans, &matTransposed);
			m_pD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);

			// Set the particle's material
			Mat.Diffuse.r = Mat.Ambient.r = m_pParticles[i].Red;
			Mat.Diffuse.g = Mat.Ambient.g = m_pParticles[i].Green;
			Mat.Diffuse.b = Mat.Ambient.b = m_pParticles[i].Blue;
			m_pD3DDevice->SetMaterial(&Mat);

			// Draw the particle
			m_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
		}

		// Clear the texture usage
		m_pD3DDevice->SetTexture(0, NULL);

		// End the scene
		m_pD3DDevice->EndScene();
	}

	// Display the scene
	m_pD3DDevice->Present(NULL, NULL, NULL, NULL);

	return TRUE;
}

BOOL cApp::Shutdown()
{
	// Free particles
	if(m_pParticles != NULL)
		delete [] m_pParticles;

	// Release textures and vertex buffers
	if(m_pParticleTexture != NULL)
		m_pParticleTexture->Release();

	if(m_pParticleVB != NULL)
		m_pParticleVB->Release();

	// Release device and 3D objects
	if(m_pD3DDevice != NULL)
		m_pD3DDevice->Release();

	if(m_pD3D != NULL)
		m_pD3D->Release();

	return TRUE;
}

int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int nCmdShow)
{
	cApp App;
	return App.Run();
}
