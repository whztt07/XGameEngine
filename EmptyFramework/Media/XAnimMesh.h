#pragma once

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
};

//-----------------------------------------------------------------------------
// Desc: 该类用来从.X文件加载框架层次和网格模型数据
//-----------------------------------------------------------------------------
class cAllocateHierarchy : public ID3DXAllocateHierarchy
{
private:
	HRESULT AllocateName(LPCSTR Name, LPSTR *pNewName);
	void    RemovePathFromFileName(LPSTR fullPath, LPTSTR fileName);

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
