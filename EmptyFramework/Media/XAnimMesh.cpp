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
// Desc: �Ӿ���·������ȡ�ļ���
//-----------------------------------------------------------------------------
void cAllocateHierarchy::RemovePathFromFileName(LPSTR fullPath, LPTSTR fileName)
{
	//�Ƚ�fullPath�����ͱ任ΪLPWSTR
	WCHAR wszBuf[MAX_PATH];
	MultiByteToWideChar( CP_ACP, 0, fullPath, -1, wszBuf, MAX_PATH );
	wszBuf[MAX_PATH-1] = L'\0';

	WCHAR* wszFullPath = wszBuf;

	//�Ӿ���·������ȡ�ļ���
	LPWSTR pch=wcsrchr(wszFullPath,'\\');
	if (pch)
		lstrcpy(fileName, ++pch);
	else
		lstrcpy(fileName, wszFullPath);
}


//-----------------------------------------------------------------------------
// Desc: �������, �����Ƿ����ڴ�ͳ�ʼ��,��û�ж����Ա������ʵ�ֵ
//-----------------------------------------------------------------------------
HRESULT cAllocateHierarchy::CreateFrame(LPCSTR Name, LPD3DXFRAME *ppNewFrame)
{
    HRESULT hr = S_OK;
    D3DXFRAME_DERIVED *pFrame;
	
    *ppNewFrame = NULL;
	
    pFrame = new D3DXFRAME_DERIVED;  //������ܽṹ����
    if (pFrame == NULL) 
    {
        return E_OUTOFMEMORY;
    }
	
	//Ϊ���ָ������
    hr = AllocateName(Name, (LPSTR*)&pFrame->Name);
	if (FAILED(hr))
	{
		delete pFrame;
		return hr;
	}
	
	//��ʼ��D3DXFRAME_DERIVED�ṹ������Ա����
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
// Desc: ����������������, ��������ģ������
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
    UINT NumFaces;       //�����е�����,��������������ṹ���ڽ���Ϣ��Աʱʹ��
    UINT iMaterial;      //�������ʱ��ѭ������
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

	//Ϊ�������������ڴ�
    pMeshContainer = new D3DXMESHCONTAINER_DERIVED;
    if (pMeshContainer == NULL)
    {
        return E_OUTOFMEMORY;
    }
    memset(pMeshContainer, 0, sizeof(D3DXMESHCONTAINER_DERIVED));
	
	//������������ṹD3DXMESHCONTAINER_DERIVED�ĳ�Ա

	//Ϊ����ָ������
	hr = AllocateName(Name, &pMeshContainer->Name);
	if (FAILED(hr))
	{
		DestroyMeshContainer(pMeshContainer);
		return hr;
	}      
	
    pMesh->GetDevice(&pd3dDevice);
    NumFaces = pMesh->GetNumFaces();

	//ȷ�����񶥵��������
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
	
    //Ϊ����ģ��׼�����ʺ�����
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
		//���Ʋ�������, ���ò��ʻ���������
        memcpy(pMeshContainer->pMaterials, pMaterials, sizeof(D3DXMATERIAL) * NumMaterials); 
		pMeshContainer->pMaterials->MatD3D.Ambient = pMeshContainer->pMaterials->MatD3D.Diffuse;
       
		//��ȡ�����ļ�, �����������
		for (iMaterial = 0; iMaterial < NumMaterials; iMaterial++) 
        {
            if (pMeshContainer->pMaterials[iMaterial].pTextureFilename != NULL)
            {
				WCHAR strTexturePath[MAX_PATH];
                WCHAR wszBuf[MAX_PATH];
				//�������ļ�·����ȡ�����ļ���
				RemovePathFromFileName(pMeshContainer->pMaterials[iMaterial].pTextureFilename, wszBuf);
                //���������ļ���������ָ����·�����������ļ�
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
// Desc: �ͷſ��
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
// Desc: �ͷ���������
//-----------------------------------------------------------------------------
HRESULT cAllocateHierarchy::DestroyMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase)
{
	if(pMeshContainerBase == NULL)
		return S_OK;

    UINT iMaterial;
	// ��תΪ��չ�������ڴ�й©
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
// Desc: ���캯������������  Construction/Destruction
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
// Desc: ���캯������������ 
//-----------------------------------------------------------------------------
cAnimMesh::~cAnimMesh()
{
	D3DXFrameDestroy(m_pFrameRoot, m_pAlloc);
    SAFE_RELEASE(m_pAnimController);
	delete m_pAlloc;
}


//-----------------------------------------------------------------------------
// Desc:������������Ƥ����ģ��
//-----------------------------------------------------------------------------
HRESULT cAnimMesh::OnCreate(LPDIRECT3DDEVICE9 pD3DDevice, WCHAR *strFileName)
{
	HRESULT hr;
	m_pd3dDevice = pD3DDevice;
	V_RETURN(LoadFromXFile(strFileName));
	return S_OK;
}


//-----------------------------------------------------------------------------
// Desc: ���ļ�������Ƥ����ģ��
//-----------------------------------------------------------------------------
HRESULT cAnimMesh::LoadFromXFile(WCHAR *strFileName)
{
    HRESULT hr;

	//�����ļ���, ��ָ����·�������ļ�
	WCHAR strPath[MAX_PATH];
	DXUTFindDXSDKMediaFileCch( strPath, sizeof(strPath) / sizeof(WCHAR), strFileName );

	//��.X�ļ����ز�ο�ܺͶ�������
    V_RETURN(D3DXLoadMeshHierarchyFromX(strPath, D3DXMESH_MANAGED, m_pd3dDevice, 
		                            m_pAlloc, NULL, &m_pFrameRoot, &m_pAnimController));
	
	//�����ܶ���ı߽���
    V_RETURN(D3DXFrameCalculateBoundingSphere(m_pFrameRoot, &m_vObjectCenter, &m_fObjectRadius));

	return S_OK;
}


//-----------------------------------------------------------------------------
// Desc: ���¿�ܲ����ƿ��
//       (1)��m_pAnimController->AdvanceTime()����ʱ�䣬m_pAnimController��
//          ��LPD3DXANIMATIONCONTROLLER��һ������
//       (2)�ú���CSkinMesh::UpdateFrameMatrices()���¿��
//       (3)�ú���CSkinMesh::DrawFrame()���ƿ��
//-----------------------------------------------------------------------------
HRESULT cAnimMesh::Render(D3DXMATRIXA16* matWorld, double fElapsedAppTime)
{
    if( 0.0f == fElapsedAppTime ) 
        return S_OK;

	if (m_bPlayAnim && m_pAnimController != NULL)
		m_pAnimController->AdvanceTime( fElapsedAppTime, NULL );

	UpdateFrameMatrices(m_pFrameRoot, matWorld);  //�����Ӻ���
	DrawFrame(m_pFrameRoot);  //�����Ӻ���

	return S_OK;
}


//-----------------------------------------------------------------------------
// Desc:���������������ϱ任����
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
// Desc: ���ƿ��.
//       ����CSkinMesh::DrawMeshContainer()����һ��LPD3DXMESHCONTAINER����
//       �ı���pMeshContainer.Ȼ��ݹ����ͬһ����ܺ���һ����ܡ�
//-----------------------------------------------------------------------------
VOID cAnimMesh::DrawFrame(LPD3DXFRAME pFrame)
{
    LPD3DXMESHCONTAINER pMeshContainer;
	
    pMeshContainer = pFrame->pMeshContainer;
    while (pMeshContainer != NULL)
    {
        DrawMeshContainer(pMeshContainer, pFrame); //�����Ӻ���
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
// Desc: �ͷ���Ƥ����ģ��
//-----------------------------------------------------------------------------
HRESULT cAnimMesh::OnDestory()
{
	delete this;
	return S_OK;
}
