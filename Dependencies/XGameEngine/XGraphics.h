#pragma once

#define ReleaseCOM(x) if(x) { x->Release(); x = NULL; }

class cGraphics;

class cTexture;
class cBumpTexture;
class cMaterial;
class cLight;
class cFont;
class cVertexBuffer;

class cWorldPosition;
class cCamera;

// DirectX mesh classes
class cMesh;
class cObject;

// DXUT mesh classes
class cAllocateHierarchy;
class cAnimMesh;
class cSkinMesh;

class cGraphics
{
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

  public:
    cGraphics();
    ~cGraphics();

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
    
    BOOL SetWorldPosition(cWorldPosition *WorldPos);
    BOOL SetCamera(cCamera *Camera);
    BOOL SetLight(long Num, cLight *Light);
    BOOL SetMaterial(cMaterial *Material);
    BOOL SetTexture(short Num, cTexture *Texture);
    
    BOOL SetAmbientLight(char Red, char Green, char Blue);
    BOOL GetAmbientLight(char *Red, char *Green, char *Blue);

    BOOL EnableLight(long Num, BOOL Enable = TRUE);
    BOOL EnableLighting(BOOL Enable = TRUE);
    BOOL EnableZBuffer(BOOL Enable = TRUE);
    BOOL EnableAlphaBlending(BOOL Enable = TRUE, DWORD Src = D3DBLEND_SRCALPHA, DWORD Dest = D3DBLEND_INVSRCALPHA);
    BOOL EnableAlphaTesting(BOOL Enable = TRUE);
};

class cTexture
{
protected:
	cGraphics         *m_Graphics;
	IDirect3DTexture9 *m_Texture;
	unsigned long      m_Width, m_Height;

public:
	cTexture();
	~cTexture();

	IDirect3DTexture9 *GetTextureCOM();

	BOOL Load(cGraphics *Graphics, LPTSTR Filename, DWORD Transparent = 0, D3DFORMAT Format = D3DFMT_UNKNOWN);
	BOOL Create(cGraphics *Graphics, IDirect3DTexture9 *Texture);
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

typedef struct
{
	D3DXVECTOR3 p;
	float tu1, tv1;
	float tu2, tv2;
	static const DWORD FVF;
}sBumpVertex;

class cBumpTexture
{
protected:
	cGraphics         *m_Graphics;
	IDirect3DTexture9 *m_BumpTex;
	IDirect3DTexture9 *m_BackTex;
	IDirect3DVertexBuffer9 *m_pVB;
	unsigned long      m_Width, m_Height;

public:
	cBumpTexture();
	~cBumpTexture();

	BOOL Load(cGraphics *Graphics, LPTSTR Filename, DWORD Transparent = 0, D3DFORMAT Format = D3DFMT_UNKNOWN);
	BOOL Create(cGraphics *Graphics, IDirect3DTexture9 *Texture);
	BOOL Free();

	long GetWidth();
	long GetHeight();

	BOOL Render(float dt);
};

class cMaterial
{
  protected:
    D3DMATERIAL9  m_Material;

  public:
    cMaterial();

    D3DMATERIAL9 *GetMaterial();

    BOOL SetDiffuseColor(unsigned char Red, unsigned char Green, unsigned char Blue);
    BOOL GetDiffuseColor(unsigned char *Red, unsigned char *Green, unsigned char *Blue);

    BOOL SetAmbientColor(unsigned char Red, unsigned char Green, unsigned char Blue);
    BOOL GetAmbientColor(unsigned char *Red, unsigned char *Green, unsigned char *Blue);
    
    BOOL SetSpecularColor(unsigned char Red, unsigned char Green, unsigned char Blue);
    BOOL GetSpecularColor(unsigned char *Red, unsigned char *Green, unsigned char *Blue);
    
    BOOL SetEmissiveColor(unsigned char Red, unsigned char Green, unsigned char Blue);
    BOOL GetEmissiveColor(unsigned char *Red, unsigned char *Green, unsigned char *Blue);

    BOOL  SetPower(float Power);
    float GetPower(float Power);
};

class cLight
{
  protected:
    D3DLIGHT9 m_Light;

  public:
    cLight();

    D3DLIGHT9 *GetLight();

    BOOL SetType(D3DLIGHTTYPE Type);

    BOOL Move(float XPos, float YPos, float ZPos);
    BOOL MoveRel(float XPos, float YPos, float ZPos);
    BOOL GetPos(float *XPos, float *YPos, float *ZPos);

    BOOL Point(float XFrom, float YFrom, float ZFrom,
               float XAt,   float YAt,   float ZAt);
    BOOL GetDirection(float *XDir, float *YDir, float *ZDir);

    BOOL SetDiffuseColor(unsigned char Red, unsigned char Green, unsigned char Blue);
    BOOL GetDiffuseColor(unsigned char *Red, unsigned char *Green, unsigned char *Blue);

    BOOL SetSpecularColor(unsigned char Red, unsigned char Green, unsigned char Blue);
    BOOL GetSpecularColor(unsigned char *Red, unsigned char *Green, unsigned char *Blue);
    
    BOOL SetAmbientColor(unsigned char Red, unsigned char Green, unsigned char Blue);
    BOOL GetAmbientColor(unsigned char *Red, unsigned char *Green, unsigned char *Blue);

    BOOL SetRange(float Range);
    float GetRange();
    
    BOOL SetFalloff(float Falloff);
    float GetFalloff();
    
    BOOL SetAttenuation0(float Attenuation);
    float GetAttenuation0();

    BOOL SetAttenuation1(float Attenuation);
    float GetAttenuation1();

    BOOL SetAttenuation2(float Attenuation);
    float GetAttenuation2();
    
    BOOL SetTheta(float Theta);
    float GetTheta();
    
    BOOL SetPhi(float Phi);
    float GetPhi();
};

class cWorldPosition
{
  protected:
    BOOL  m_Billboard;

    float m_XPos,      m_YPos,      m_ZPos;
    float m_XRotation, m_YRotation, m_ZRotation;
    float m_XScale,    m_YScale,    m_ZScale;
    
    D3DXMATRIX m_matWorld;
    D3DXMATRIX m_matScale;
    D3DXMATRIX m_matRotation;
    D3DXMATRIX m_matTranslation;
    D3DXMATRIX *m_matCombine1;
    D3DXMATRIX *m_matCombine2;

	BOOL Update(cGraphics *Graphics = NULL);
  public:
    cWorldPosition();

    D3DXMATRIX *GetMatrix(cGraphics *Graphics = NULL);
    BOOL SetCombineMatrix1(D3DXMATRIX *Matrix = NULL);
    BOOL SetCombineMatrix2(D3DXMATRIX *Matrix = NULL);

    BOOL Copy(cWorldPosition *DestPos);

    BOOL Move(float XPos, float YPos, float ZPos);
    BOOL MoveRel(float XAdd, float YAdd, float ZAdd);

    BOOL Rotate(float XRot, float YRot, float ZRot);
    BOOL RotateRel(float XAdd, float YAdd, float ZAdd);

    BOOL Scale(float XScale, float YScale, float ZScale);
    BOOL ScaleRel(float XAdd, float YAdd, float ZAdd);

    BOOL EnableBillboard(BOOL Enable = TRUE);

    float GetXPos();
    float GetYPos();
    float GetZPos();
    float GetXRotation();
    float GetYRotation();
    float GetZRotation();
    float GetXScale();
    float GetYScale();
    float GetZScale();
};

class cCamera
{
  protected:
    float m_XPos, m_YPos, m_ZPos;
    float m_XRot, m_YRot, m_ZRot;

    float m_StartXPos, m_StartYPos, m_StartZPos;
    float m_StartXRot, m_StartYRot, m_StartZRot;

    float m_EndXPos, m_EndYPos, m_EndZPos;
    float m_EndXRot, m_EndYRot, m_EndZRot;
    
    D3DXMATRIX m_matWorld;
    D3DXMATRIX m_matTranslation;
    D3DXMATRIX m_matRotation;

  public:
    cCamera();

    D3DXMATRIX *GetMatrix(); // Get view transformation matrix
    BOOL Update();           // Update transformation matrix

    BOOL Move(float XPos, float YPos, float ZPos);
    BOOL MoveRel(float XAdd, float YAdd, float ZAdd);

    BOOL Rotate(float XRot, float YRot, float ZRot);
    BOOL RotateRel(float XAdd, float YAdd, float ZAdd);

    BOOL Point(float XEye, float YEye, float ZEye, float XAt, float YAt, float ZAt);

    BOOL SetStartTrack();
    BOOL SetEndTrack();
    BOOL Track(float Time, float Length);

    float GetXPos();
    float GetYPos();
    float GetZPos();
    float GetXRotation();
    float GetYRotation();
    float GetZRotation();
};

class cFont
{
  private:
    ID3DXFont *m_Font;

  public:
    cFont();
    ~cFont();

    ID3DXFont *GetFontCOM();

	BOOL Create(cGraphics *Graphics, LPTSTR Name, long Size = 16, BOOL Bold = FALSE, BOOL Italic = FALSE, BOOL Underline = FALSE, BOOL Strikeout = FALSE);
    BOOL Free();

    BOOL Print(LPTSTR Text, long XPos, long YPos, long Width = 0, long Height = 0, D3DCOLOR Color = 0xFFFFFFFF, DWORD Format = 0);
};

class cMesh
{
private:
	cGraphics *m_Graphics;

	LPD3DXMESH			m_pMesh;			// 网格模型对象
	D3DMATERIAL9		*m_pMeshMaterials;	// 网格模型材质
	LPDIRECT3DTEXTURE9	*m_pMeshTextures;	// 网格模型纹理
	DWORD				m_dwNumMaterials;	// 网格模型材质数量

	// 包围盒
	D3DXVECTOR3    m_Min, m_Max;
	D3DXVECTOR3    m_Center;
	float          m_Radius;

public:
	cMesh();
	~cMesh();

	LPD3DXMESH			GetMesh();
	DWORD				GetNumMaterials();
	D3DMATERIAL9*		GetMeshMaterials();
	LPDIRECT3DTEXTURE9*	GetMeshTextures();
	
	BOOL GetBounds(D3DXVECTOR3 *Min, D3DXVECTOR3 *Max, float *Radius);
	BOOL GetBounds(float *MinX, float *MinY, float *MinZ, float *MaxX, float *MaxY, float *MaxZ, float *Radius);

	//------------------------------------------------------------
	// TCHAR CurrDir[MAX_PATH+1];
	// DWORD CPL = GetCurrentDirectory(MAX_PATH, CurrDir);
	// if (CPL == NULL)
	//	 return E_FAIL;
	// 上述代码块可获得当前目录，Filename将附加到当前目录后以指定文件
	//------------------------------------------------------------
	BOOL Load(cGraphics *Graphics, LPTSTR Filename, LPTSTR TexturePath);
	BOOL Render();
	BOOL Free();
};

class cObject
{
  protected:
    cGraphics     *m_Graphics;
    cMesh         *m_Mesh;
    cWorldPosition m_Pos;

    BOOL           m_Billboard;

  public:
    cObject();
    ~cObject();

    BOOL Create(cGraphics *Graphics, cMesh *Mesh = NULL);
    BOOL Free();

    BOOL EnableBillboard(BOOL Enable = TRUE);
    BOOL AttachToObject(cObject *Object, char *FrameName = NULL);

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

    BOOL SetMesh(cMesh *Mesh);
    cMesh *GetMesh();

    BOOL Render();
};

//-----------------------------------------------------------------------------
// Desc: 继承自DXDXFRAME结构的结构
//-----------------------------------------------------------------------------
struct D3DXFRAME_DERIVED: public D3DXFRAME
{
    D3DXMATRIXA16 CombinedTransformationMatrix;
};

//-----------------------------------------------------------------------------
// Desc: 继承自D3DXMESHCONTAINER结构的结构
//-----------------------------------------------------------------------------
struct D3DXMESHCONTAINER_DERIVED: public D3DXMESHCONTAINER
{
    LPDIRECT3DTEXTURE9*  ppTextures;            //纹理数组
    LPD3DXMESH           pOrigMesh;             //原始网格
	DWORD                NumInfl;               //每个顶点最多受多少骨骼的影响
    DWORD                NumAttributeGroups;    //属性组数量,即子网格数量
    LPD3DXBUFFER         pBoneCombinationBuf;   //骨骼结合表
    D3DXMATRIX**         ppBoneMatrixPtrs;      //存放骨骼的组合变换矩阵
    D3DXMATRIX*          pBoneOffsetMatrices;   //存放骨骼的初始变换矩阵
	DWORD                NumPaletteEntries;     //骨骼数量上限
	bool                 UseSoftwareVP;         //标识是否使用软件顶点处理
};

//-----------------------------------------------------------------------------
// Desc: 该类用来从.X文件加载框架层次和网格模型数据
//-----------------------------------------------------------------------------
class cAllocateHierarchy : public ID3DXAllocateHierarchy
{
public:
	//------------------------------------------------------------------------
	// CreateFrame:
	// ------------
	// Requests allocation of a frame object.
	//
	// Parameters:
	//  Name
	//		Name of the frame to be created
	//	ppNewFrame
	//		Returns the created frame object
	//
	//------------------------------------------------------------------------
    STDMETHOD(CreateFrame)(THIS_ LPCSTR Name, 
                            LPD3DXFRAME *ppNewFrame);

	//------------------------------------------------------------------------
	// CreateMeshContainer:
	// --------------------
	// Requests allocation of a mesh container object.
	//
	// Parameters:
	//  Name
	//		Name of the mesh
	//	pMesh
	//		Pointer to the mesh object if basic polygon data found
	//	pPMesh
	//		Pointer to the progressive mesh object if progressive mesh data found
	//	pPatchMesh
	//		Pointer to the patch mesh object if patch data found
	//	pMaterials
	//		Array of materials used in the mesh
	//	pEffectInstances
	//		Array of effect instances used in the mesh
	//	NumMaterials
	//		Num elements in the pMaterials array
	//	pAdjacency
	//		Adjacency array for the mesh
	//	pSkinInfo
	//		Pointer to the skininfo object if the mesh is skinned
	//	pBoneNames
	//		Array of names, one for each bone in the skinned mesh. 
	//		The numberof bones can be found from the pSkinMesh object
	//	pBoneOffsetMatrices
	//		Array of matrices, one for each bone in the skinned mesh.
	//
	//------------------------------------------------------------------------
    STDMETHOD(CreateMeshContainer)(THIS_ 
        LPCSTR Name, 
        CONST D3DXMESHDATA *pMeshData, 
        CONST D3DXMATERIAL *pMaterials, 
        CONST D3DXEFFECTINSTANCE *pEffectInstances, 
        DWORD NumMaterials, 
        CONST DWORD *pAdjacency, 
        LPD3DXSKININFO pSkinInfo, 
        LPD3DXMESHCONTAINER *ppNewMeshContainer);

	//------------------------------------------------------------------------
	// DestroyFrame:
	// -------------
	// Requests de-allocation of a frame object.
	//
	// Parameters:
	//  pFrameToFree
	//		Pointer to the frame to be de-allocated
	//
	//------------------------------------------------------------------------
    STDMETHOD(DestroyFrame)(THIS_ LPD3DXFRAME pFrameToFree); 

	//------------------------------------------------------------------------
	// DestroyMeshContainer:
	// ---------------------
	// Requests de-allocation of a mesh container object.
	//
	// Parameters:
	//  pMeshContainerToFree
	//		Pointer to the mesh container object to be de-allocated
	//
	//------------------------------------------------------------------------
    STDMETHOD(DestroyMeshContainer)(THIS_ LPD3DXMESHCONTAINER pMeshContainerToFree); 
};


class cAnimMesh
{
private:
	cAllocateHierarchy*         m_pAlloc;
	LPDIRECT3DDEVICE9			m_pd3dDevice;
	LPD3DXFRAME					m_pFrameRoot;

public:
	D3DXVECTOR3   m_vObjectCenter;
	float         m_fObjectRadius;
	BOOL		  m_bPlayAnim;
	LPD3DXANIMATIONCONTROLLER	m_pAnimController;

private:
	HRESULT LoadFromXFile(WCHAR* strFileName);
	VOID    UpdateFrameMatrices( LPD3DXFRAME pFrameBase, LPD3DXMATRIX pParentMatrix ); 

	VOID DrawFrame(LPD3DXFRAME pFrame);
	VOID DrawMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase, LPD3DXFRAME pFrameBase);

public:
	HRESULT OnCreate(LPDIRECT3DDEVICE9 pD3DDevice, WCHAR* strFileName);
	HRESULT Render(D3DXMATRIXA16* matWorld, double fElapsedAppTime);
	HRESULT OnDestory();

public:
	cAnimMesh();
	virtual ~cAnimMesh();
};

class cSkinMesh
{
private:
	cAllocateHierarchy*         m_pAlloc;
	LPDIRECT3DDEVICE9			m_pd3dDevice;
	LPD3DXFRAME					m_pFrameRoot;
	D3DXMATRIXA16*              m_pBoneMatrices;
	UINT                        m_NumBoneMatricesMax;

private:
	HRESULT SetupBoneMatrixPointers( LPD3DXFRAME pFrame );
	HRESULT SetupBoneMatrixPointersOnMesh( LPD3DXMESHCONTAINER pMeshContainerBase );

	HRESULT LoadFromXFile(WCHAR* strFileName);
	VOID    UpdateFrameMatrices( LPD3DXFRAME pFrameBase, LPD3DXMATRIX pParentMatrix ); 

	VOID DrawFrame(LPD3DXFRAME pFrame);
	VOID DrawMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase, LPD3DXFRAME pFrameBase);

public:
	D3DXVECTOR3   m_vObjectCenter;
	float         m_fObjectRadius;
	BOOL		  m_bPlayAnim;
	LPD3DXANIMATIONCONTROLLER	m_pAnimController;

public:
	HRESULT OnCreate(LPDIRECT3DDEVICE9 pD3DDevice, WCHAR* strFileName);
	BOOL	SwitchAnim(LPSTR AnimName);
	HRESULT Render(D3DXMATRIXA16* matWorld, float fElapsedAppTime);
	HRESULT OnDestory();

public:
	cSkinMesh();
	virtual ~cSkinMesh();
};

class cVertexBuffer
{
  private:
    cGraphics              *m_Graphics;
    IDirect3DVertexBuffer9 *m_pVB;

    DWORD                   m_NumVertices;
    DWORD                   m_VertexSize;
    DWORD                   m_FVF;

    BOOL                    m_Locked;
    char                   *m_Ptr;

  public:
    cVertexBuffer();
    ~cVertexBuffer();

    IDirect3DVertexBuffer9 *GetVertexBufferCOM();
    unsigned long           GetVertexSize();
    unsigned long           GetVertexFVF();
    unsigned long           GetNumVertices();

    BOOL Create(cGraphics *Graphics, unsigned long NumVertices, DWORD Descriptor, long VertexSize);
    BOOL Free();

    BOOL IsLoaded();

    BOOL Set(unsigned long FirstVertex, unsigned long NumVertices, void *VertexList);
    BOOL Render(unsigned long FirstVertex, unsigned long NumPrimitives, DWORD Type);

	BOOL  Lock(unsigned long FirstVertex = 0, unsigned long NumVertices = 0);
    BOOL  Unlock();
    void *GetPtr();
};
