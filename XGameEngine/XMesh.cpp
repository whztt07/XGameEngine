#include "XMesh.h"
#include "XGraphics.h"

namespace Xge
{
	//-----------------------------------------------------------------------------
	// Desc: 从绝对路径中提取文件名
	//-----------------------------------------------------------------------------
	static void RemovePathFromFileName(LPSTR fullPath, LPTSTR fileName, DWORD fileNameLen)
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

	Mesh::Mesh()
	{
		m_Graphics = NULL;

		m_pMesh				= NULL;			
		m_pMeshMaterials	= NULL;
		m_pMeshTextures		= NULL;
		m_dwNumMaterials	= 0;
	
		m_Radius = 0;
	}

	Mesh::~Mesh()
	{
		Free();
	}

	LPD3DXMESH Mesh::GetMesh()
	{
		return m_pMesh;
	}

	DWORD Mesh::GetNumMaterials()
	{
		return m_dwNumMaterials;
	}

	D3DMATERIAL9* Mesh::GetMeshMaterials()
	{
		return m_pMeshMaterials;
	}

	LPDIRECT3DTEXTURE9* Mesh::GetMeshTextures()
	{
		return m_pMeshTextures;
	}

	BOOL Mesh::GetBounds(D3DXVECTOR3 *Min, D3DXVECTOR3 *Max, float *Radius)
	{
		if(Min != NULL)
			*Min = m_Min;
		if(Max != NULL)
			*Max = m_Max;
		if(Radius != NULL)
			*Radius = m_Radius;

		return TRUE;
	}

	BOOL Mesh::GetBounds(float *MinX, float *MinY, float *MinZ, float *MaxX, float *MaxY, float *MaxZ, float *Radius)
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

	BOOL Mesh::Load(Graphics *Graphics, LPTSTR Filename, LPTSTR TexturePath)
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

	BOOL Mesh::Render()
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

	BOOL Mesh::Free()
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
}
