#ifndef _XFont_H_
#define _XFont_H_

#include "XGlobal.h"

namespace Xge
{
	class Graphics;

	class Font
	{
	public:
		Font();
		~Font();

		ID3DXFont *GetFontCOM();

		BOOL Create(Graphics *Graphics, LPTSTR Name, long Size = 16, BOOL Bold = FALSE, BOOL Italic = FALSE, BOOL Underline = FALSE, BOOL Strikeout = FALSE);
		BOOL Free();

		BOOL Print(LPTSTR Text, long XPos, long YPos, long Width = 0, long Height = 0, D3DCOLOR Color = 0xFFFFFFFF, DWORD Format = 0);

	private:
		ID3DXFont *m_Font;
	};
}

#endif
