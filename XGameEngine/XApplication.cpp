#include "XApplication.h"

namespace Xge
{
	Application::Application()
	{
		// Save instance handle
		// Current = this;
		g_pApplication = this;

		// Get the instance handle
		m_hInst = GetModuleHandle(NULL);

		// Set a default window class and caption
		lstrcpy(m_Class, L"AppClass");
		lstrcpy(m_Caption, L"Application Caption");

		// Set default window style, position, width, height
		// m_Style  = WS_OVERLAPPEDWINDOW;
		m_Style = WS_OVERLAPPED | WS_SYSMENU;
		m_XPos   = 0;
		m_YPos   = 0;
		m_Width  = 256;
		m_Height = 256;

		// Set default WNDCLASSEX structure
		m_wcex.cbSize        = sizeof(WNDCLASSEX);
		m_wcex.style         = CS_CLASSDC;
		m_wcex.lpfnWndProc   = AppWindowProc;
		m_wcex.cbClsExtra    = 0;
		m_wcex.cbWndExtra    = 0;
		m_wcex.hInstance     = m_hInst;
		m_wcex.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
		m_wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
		m_wcex.hbrBackground = NULL;
		m_wcex.lpszMenuName  = NULL;
		m_wcex.lpszClassName = m_Class;
		m_wcex.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);
	}

	HWND Application::GethWnd()
	{
		return m_hWnd;
	}

	HINSTANCE Application::GethInst()
	{
		return m_hInst;
	}

	BOOL Application::Run()
	{
		MSG Msg;

		// Register window class
		if(!RegisterClassEx(&m_wcex))
			return FALSE;

		// Create the Main Window
		m_hWnd = CreateWindow(m_Class, m_Caption,
								m_Style, 
								m_XPos, m_YPos,
								m_Width, m_Height,
								NULL, NULL, m_hInst, NULL);
		if(!m_hWnd)
			return FALSE;

		// Show and update the window
		ShowWindow(m_hWnd, SW_NORMAL);
		UpdateWindow(m_hWnd);

		// Make sure client area is correct size
		Resize(m_Width, m_Height);

		// Initialize COM
		CoInitialize(NULL);

		if(Init() == TRUE) 
		{
			// Enter the message pump
			ZeroMemory(&Msg, sizeof(MSG));
			while(Msg.message != WM_QUIT) 
			{
				// Handle Windows messages (if any)
				if(PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE)) 
				{
					TranslateMessage(&Msg);
					DispatchMessage(&Msg);
				} else {
					// Do per-frame processing, break on FALSE return value
					if(Frame() == FALSE)
						break;
				}
			}
		}

		Shutdown();

		// Shutdown COM
		CoUninitialize();

		// Unregister the window class
		UnregisterClass(m_Class, m_hInst);

		return TRUE;
	}

	BOOL Application::Error(BOOL Fatal, LPCTSTR Text, ...)
	{
		TCHAR CaptionText[12];
		TCHAR ErrorText[2048];
		va_list valist;

		// Build the message box caption based on fatal flag
		if(Fatal == FALSE)
			lstrcpy(CaptionText, L"Error");
		else 
			lstrcpy(CaptionText, L"Fatal Error");

		// Build variable text buffer
		va_start(valist, Text);
		vswprintf_s(ErrorText, sizeof(ErrorText), Text, valist);
		va_end(valist);

		// Display the message box
		MessageBox(NULL, ErrorText, CaptionText, MB_OK | MB_ICONEXCLAMATION);

		// Post a quit message if error was fatal
		if(Fatal == TRUE)
			PostQuitMessage(0);

		return TRUE;
	}

	BOOL Application::Move(long XPos, long YPos)
	{
		RECT ClientRect;

		GetClientRect(m_hWnd, &ClientRect);
		MoveWindow(m_hWnd, XPos, YPos, ClientRect.right, ClientRect.bottom, TRUE);

		return TRUE;
	}

	BOOL Application::Resize(long Width, long Height)
	{
		RECT WndRect, ClientRect;
		long WndWidth, WndHeight;

		GetWindowRect(m_hWnd, &WndRect);
		GetClientRect(m_hWnd, &ClientRect);

		WndWidth  = (WndRect.right  - (ClientRect.right  - Width))  - WndRect.left;
		WndHeight = (WndRect.bottom - (ClientRect.bottom - Height)) - WndRect.top;

		MoveWindow(m_hWnd, WndRect.left, WndRect.top, WndWidth, WndHeight, TRUE);

		return TRUE;
	}

	BOOL Application::ShowMouse(BOOL Show)
	{
		ShowCursor(Show);
		return TRUE;
	}

	// The message procedure - empty except for destroy message
	long FAR PASCAL AppWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch(uMsg) 
		{
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

		default: 
			return g_pApplication->MsgProc(hWnd, uMsg, wParam, lParam);
		}
	}
}
