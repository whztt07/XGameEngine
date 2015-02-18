#include "Global.h"

//-----------------------------------------------------------------------------
// Desc: 
//-----------------------------------------------------------------------------
HRESULT cAllocateHierarchy::AllocateName( LPCSTR Name, LPSTR *pNewName )
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
// Desc: 从绝对路径中提取文件名
//-----------------------------------------------------------------------------
void cAllocateHierarchy::RemovePathFromFileName(LPSTR fullPath, LPTSTR fileName)
{
	//先将fullPath的类型变换为LPWSTR
	WCHAR wszBuf[MAX_PATH];
	MultiByteToWideChar( CP_ACP, 0, fullPath, -1, wszBuf, MAX_PATH );
	wszBuf[MAX_PATH-1] = L'\0';

	WCHAR* wszFullPath = wszBuf;

	//从绝对路径中提取文件名
	LPWSTR pch=wcsrchr(wszFullPath,'\\');
	if (pch)
		lstrcpy(fileName, ++pch);
	else
		lstrcpy(fileName, wszFullPath);
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
				RemovePathFromFileName(pMeshContainer->pMaterials[iMaterial].pTextureFilename, wszBuf);
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
