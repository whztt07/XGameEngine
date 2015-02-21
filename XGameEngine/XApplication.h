#ifndef _XApplication_H_
#define _XApplication_H_

#include "XGlobal.h"

namespace Xge
{
	class Graphics;

	class Application
	{
	public:
		Application();

		HWND      GethWnd();
		HINSTANCE GethInst();

		BOOL Run();
		BOOL Error(BOOL Fatal, LPCTSTR Text, ...);

		BOOL Move(long XPos, long YPos);
		BOOL Resize(long Width, long Height);

		BOOL ShowMouse(BOOL Show = TRUE);

		virtual LRESULT CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return DefWindowProc(hWnd, uMsg, wParam, lParam); }
		virtual BOOL Init()       { return TRUE; }
		virtual BOOL Shutdown()   { return TRUE; }
		virtual BOOL Frame()      { return TRUE; }
		virtual Graphics *GetGraphics() { return NULL; }

	protected:
		TCHAR          m_Class[MAX_PATH];
		TCHAR          m_Caption[MAX_PATH];

		WNDCLASSEX    m_wcex;

		DWORD         m_Style;
		DWORD         m_XPos;
		DWORD         m_YPos;
		DWORD         m_Width;
		DWORD         m_Height;

	private:
		HINSTANCE     m_hInst;
		HWND          m_hWnd;
	};

	static Application *g_pApplication;
	static long FAR PASCAL AppWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
}

#endif
