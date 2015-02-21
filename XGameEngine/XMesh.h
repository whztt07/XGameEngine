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
		// ���������ɻ�õ�ǰĿ¼��Filename�����ӵ���ǰĿ¼����ָ���ļ�
		//------------------------------------------------------------
		BOOL Load(Graphics *Graphics, LPTSTR Filename, LPTSTR TexturePath);
		BOOL Render();
		BOOL Free();

	private:
		Graphics *m_Graphics;

		LPD3DXMESH			m_pMesh;			// ����ģ�Ͷ���
		D3DMATERIAL9		*m_pMeshMaterials;	// ����ģ�Ͳ���
		LPDIRECT3DTEXTURE9	*m_pMeshTextures;	// ����ģ������
		DWORD				m_dwNumMaterials;	// ����ģ�Ͳ�������

		// ��Χ��
		D3DXVECTOR3    m_Min, m_Max;
		D3DXVECTOR3    m_Center;
		float          m_Radius;
	};
}

#endif
