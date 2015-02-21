#include "XFont.h"
#include "XGraphics.h"

namespace Xge
{
	Font::Font()
	{
	  m_Font = NULL;
	}

	Font::~Font()
	{
	  Free();
	}

	ID3DXFont *Font::GetFontCOM()
	{
	  return m_Font;
	}

	BOOL Font::Create(Graphics *Graphics, LPTSTR Name, long Size, BOOL Bold, BOOL Italic, BOOL Underline, BOOL Strikeout)
	{
		D3DXFONT_DESC fd;

		if(Graphics == NULL || Name == NULL)
			return FALSE;
		if(Graphics->GetDeviceCOM() == NULL)
			return FALSE;

		// Clear out the font structure
		ZeroMemory(&fd, sizeof(fd));

		// Set the font name and height
		lstrcpy(fd.FaceName, Name);
		fd.Height = -Size;
		fd.Weight = (Bold == TRUE) ? 700 : 0;
		fd.Italic = Italic;

		// Create the font object
		if(FAILED(D3DXCreateFontIndirect(Graphics->GetDeviceCOM(), &fd, &m_Font)))
			return FALSE;
		return TRUE;
	}

	BOOL Font::Free()
	{
	  ReleaseCOM(m_Font);
	  return TRUE;
	}

	BOOL Font::Print(LPTSTR Text, long XPos, long YPos, long Width, long Height, D3DCOLOR Color, DWORD Format)
	{
	  RECT Rect;

	  if(m_Font == NULL)
		return FALSE;

	  if(!Width)
		Width = 65535;
	  if(!Height)
		Height = 65536;

	  Rect.left   = XPos;
	  Rect.top    = YPos;
	  Rect.right  = Rect.left + Width;
	  Rect.bottom = Rect.top + Height;
	  if(FAILED(m_Font->DrawText(NULL, Text, -1, &Rect, Format, Color)))
		return FALSE;
	  return TRUE;
	}
}
