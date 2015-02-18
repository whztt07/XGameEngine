#pragma once

class cApplication
{
private:
	HINSTANCE     m_hInst;
	HWND          m_hWnd;

protected:
	TCHAR          m_Class[MAX_PATH];
	TCHAR          m_Caption[MAX_PATH];

	WNDCLASSEX    m_wcex;

	DWORD         m_Style;
	DWORD         m_XPos;
	DWORD         m_YPos;
	DWORD         m_Width;
	DWORD         m_Height;

public:
	cApplication();

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
	virtual cGraphics *GetGraphics() { return NULL; }
};
static cApplication *g_pApplication;
static long FAR PASCAL AppWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

enum Purposes 
{
	NOPURPOSE = 0,
	INITPURPOSE,
	SHUTDOWNPURPOSE,
	FRAMEPURPOSE
};
    
class cStateManager
{
	typedef struct sState 
	{
		void (*Function)(void *Ptr, long Purpose);
		sState *Next;

		sState()
		{
			Function = NULL;
			Next = NULL;
		}

		~sState()
		{
			delete Next;
		}
	} sState;

protected:
	sState *m_StateParent;

public:
	cStateManager();
	~cStateManager();

	void Push(void (*Function)(void *Ptr, long Purpose), void *DataPtr = NULL);
	BOOL Pop(void *DataPtr = NULL);
	void PopAll(void *DataPtr = NULL);
	BOOL Process(void *DataPtr = NULL);
};

static VOID XGETrace(const CHAR* strFile, DWORD dwLine)
{ 
	CHAR MsgBuf[256];
	sprintf_s(MsgBuf, "File:%s\nLine:%d", strFile, dwLine);
	MessageBoxA(NULL, MsgBuf, "Return FALSE!", MB_OK);
}
