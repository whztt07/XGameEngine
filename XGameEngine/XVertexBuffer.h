#ifndef _XVertexBuffer_H_
#define _XVertexBuffer_H_

#include "XGlobal.h"

namespace Xge
{
	class Graphics;

	class VertexBuffer
	{
	  private:
		Graphics              *m_Graphics;
		IDirect3DVertexBuffer9 *m_pVB;

		DWORD                   m_NumVertices;
		DWORD                   m_VertexSize;
		DWORD                   m_FVF;

		BOOL                    m_Locked;
		char                   *m_Ptr;

	  public:
		VertexBuffer();
		~VertexBuffer();

		IDirect3DVertexBuffer9 *GetVertexBufferCOM();
		unsigned long           GetVertexSize();
		unsigned long           GetVertexFVF();
		unsigned long           GetNumVertices();

		BOOL Create(Graphics *Graphics, unsigned long NumVertices, DWORD Descriptor, long VertexSize);
		BOOL Free();

		BOOL IsLoaded();

		BOOL Set(unsigned long FirstVertex, unsigned long NumVertices, void *VertexList);
		BOOL Render(unsigned long FirstVertex, unsigned long NumPrimitives, DWORD Type);

		BOOL  Lock(unsigned long FirstVertex = 0, unsigned long NumVertices = 0);
		BOOL  Unlock();
		void *GetPtr();
	};
}

#endif
