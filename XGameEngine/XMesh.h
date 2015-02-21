#ifndef _XMesh_H_
#define _XMesh_H_

#include "XGlobal.h"

namespace Xge
{
	class Graphics;

	class Mesh
	{
	public:
		Mesh();
		~Mesh();

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
		BOOL Load(Graphics *Graphics, LPTSTR Filename, LPTSTR TexturePath);
		BOOL Render();
		BOOL Free();

	private:
		Graphics *m_Graphics;

		LPD3DXMESH			m_pMesh;			// 网格模型对象
		D3DMATERIAL9		*m_pMeshMaterials;	// 网格模型材质
		LPDIRECT3DTEXTURE9	*m_pMeshTextures;	// 网格模型纹理
		DWORD				m_dwNumMaterials;	// 网格模型材质数量

		// 包围盒
		D3DXVECTOR3    m_Min, m_Max;
		D3DXVECTOR3    m_Center;
		float          m_Radius;
	};
}

#endif
