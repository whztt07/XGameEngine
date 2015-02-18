#include "Global.h"

cApp::cApp()
{
	//--------------------------------------------------
	// Change default window settings
	//--------------------------------------------------
	m_Width = 320;
	m_Height = 240;
	_tcscpy_s(m_Class, TEXT("TextureAnimation"));
	_tcscpy_s(m_Caption, TEXT("TextureAnimation demo by pf_d"));
}

BOOL cApp::Init()
{
	// Initialize the graphics device and set display mode
	m_Graphics.Init();
	m_Graphics.SetMode(GethWnd(), TRUE, FALSE);

	//--------------------------------------------------
	// Add your codes below...
	//--------------------------------------------------
	TCHAR Filename[256];
	for(int i=0; i<25; i++)
	{
		_stprintf(Filename, TEXT(".\\Media\\explode%02d.dds"), i);
		XV(m_Texs[i].Load(&m_Graphics, Filename, D3DCOLOR_RGBA(0, 0, 0, 255)));
	}

	CUSTOMVERTEX vertices[] =
    {
		{ -1.0f, -1.0f, 0.0f,  0.0f,  1.0f, },
        { -1.0f,  1.0f, 0.0f,  0.0f,  0.0f, },
        {  1.0f, -1.0f, 0.0f,  1.0f,  1.0f, },
		{  1.0f,  1.0f, 0.0f,  1.0f,  0.0f, }
    };

	XV(m_VB.Create(&m_Graphics, 4, D3DFVF_CUSTOMVERTEX, sizeof(CUSTOMVERTEX)));
	XV(m_VB.Set(0, 4, vertices));

	//设置观察矩阵
	D3DXMATRIXA16 matView;
	D3DXVECTOR3 vEyePt( 0.0f, 0.0f, -5 );
    D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
	m_Graphics.GetDeviceCOM()->SetTransform( D3DTS_VIEW, &matView );

	//设置投影矩阵
	D3DXMATRIXA16 matProj;
	float fAspectRatio = (float)(m_Width / m_Height);
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, fAspectRatio, 1.0f, 100.0f );
    m_Graphics.GetDeviceCOM()->SetTransform( D3DTS_PROJECTION, &matProj );

	// Alpha混合设置, 设置混合系数
	m_Graphics.GetDeviceCOM()->SetRenderState( D3DRS_ALPHABLENDENABLE,   true );
	m_Graphics.GetDeviceCOM()->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
	m_Graphics.GetDeviceCOM()->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	return TRUE;
}

BOOL cApp::Frame()
{
	static DWORD  Timer = timeGetTime();
	// Calculate elapsed time (plus speed boost)
	unsigned long Elapsed = (timeGetTime() - Timer) * .1f;
	if(Elapsed < 8.0f)
		return TRUE;
	Timer = timeGetTime();

	m_Graphics.Clear(D3DCOLOR_ARGB(0, 45, 50, 170));
	if(m_Graphics.BeginScene())
	{
		//--------------------------------------------------
		// Add your codes below...
		//--------------------------------------------------
		// 计算当前纹理索引
		static int    iCurTex = 0;
		iCurTex++;
		if( iCurTex > 24 )
                iCurTex = 0;

		//设置当前纹理
		XV(m_Graphics.SetTexture(0, &m_Texs[iCurTex]));
		//渲染图形
		XV(m_VB.Render(0, 2, D3DPT_TRIANGLESTRIP));

		m_Graphics.EndScene();
	}
	m_Graphics.Display();

	return TRUE;
}

BOOL cApp::Shutdown()
{
	return TRUE;
}

int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int nCmdShow)
{
	cApp App;
	return App.Run();
}
