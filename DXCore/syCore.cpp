#include "syCore.h"


bool syCore::Init()
{
	return true;
}
bool syCore::PreFrame()
{
	return true;
}

bool syCore::Frame()
{
	return true;
}
bool syCore::PostFrame()
{
	return true;
}


bool syCore::PreRender()
{
	//syDevice::Render();
	float clearColor[] = { 0.83f,0.5f,0.463f,1 };
	//float clearColor[] = { 1.0f,1.0f,1.0f,1.0f };
	m_pImmediateContext->ClearRenderTargetView(m_pRTView, clearColor);
	m_pImmediateContext->ClearDepthStencilView(
		m_pDepthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f, 0);
	if (m_bWireFrameRender)
	{
		DX::ApplyRS(m_pImmediateContext, DX::syDxState::m_pRSWireFrame);
	}
	else
	{
		DX::ApplyRS(m_pImmediateContext, DX::syDxState::m_pRSSolidFrame);
	}

	DX::ApplyDSS(m_pImmediateContext, DX::syDxState::m_pDSSDepthEnable);
	DX::ApplyBS(m_pImmediateContext, DX::syDxState::m_pAlphaBlend);
	DX::ApplySS(m_pImmediateContext, DX::syDxState::m_pSSWrapLinear);
	m_DirAxis.SetMatrix(
		nullptr,
		(D3DXMATRIX*)&m_pMainCamera->m_matView,
		(D3DXMATRIX*)&m_pMainCamera->m_matProj);
	m_DirAxis.Render(m_pImmediateContext);


	// sky
	D3DXMATRIX matSkyWorld;
	D3DXMatrixScaling(&matSkyWorld, 10, 10, 10);
	D3DXMATRIX matSkyView = m_pMainCamera->m_matView;
	matSkyView._41 = 0.0f;
	matSkyView._42 = 0.0f;
	matSkyView._43 = 0.0f;
	m_SkyBox.SetMatrix(&matSkyWorld, &matSkyView,
		&m_pMainCamera->m_matProj);
	//m_SkyBox.Render(m_pImmediateContext);
	return true;
}
bool syCore::Render()
{
	return true;
}
bool syCore::PostRender()
{
	if (m_bDebug == true)
	{
 		DebugRender();
	}
	m_dxWrite.Render();
	m_dxWrite.m_dataList.clear();
	m_pSwapChain->Present(0, 0);

	return true;
}
bool syCore::DebugRender()
{
	m_Timer.Render();
	I_Input.Render();
	I_SoundMgr.Render();

	return true;
}
bool syCore::Release()
{
	return true;
}
bool syCore::TCoreInit()
{
	
	m_Timer.Init();
	I_Input.Init();
	I_SoundMgr.Init();
	m_SkyBox.Init();
	if (syDevice::InitDevice() == false)
	{
		return false;
	}

	syDxState::SetState(m_pDevice);

	m_dxWrite.Init();
	IDXGISurface1*   m_pBuffer;
	m_pSwapChain->GetBuffer(0,
		__uuidof(IDXGISurface1),
		(LPVOID*)&m_pBuffer);

	m_dxWrite.Set(m_hWnd,
		g_rtClient.right,
		g_rtClient.bottom, m_pBuffer);

	if (m_pBuffer) m_pBuffer->Release();

	if (!m_DirAxis.Create(m_pDevice,
		L"../../data/shader/Line.hlsl",
		nullptr))
	{
		return false;
	}
	m_DefaultCamera.Init();
	D3DXVECTOR3 Eye(0.0f, 5.0f, -5.0f);
	D3DXVECTOR3 At(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 Up(0.0f, 1.0f, 0.0f);
	m_DefaultCamera.SetViewMatrix(Eye, At, Up);
	m_DefaultCamera.SetProjMatrix(D3DX_PI / 4,
		(float)g_rtClient.right / (float)g_rtClient.bottom,
		1.0f,
		3000.0f);	//수정 0903
	m_pMainCamera = &m_DefaultCamera;

	if (!m_SkyBox.Create(m_pDevice,
		L"../../data/shader/skybox.hlsl",
		L"../../data/sky/mariosky.png"))
	{
		return false;
	}
	return Init();
}
bool syCore::TCoreFrame()
{
	if (g_ActionInput.bSpace == KEY_PUSH)
	{
 		m_bDebug = !m_bDebug;
	}
	if (::g_ActionInput.bMiddleClick == KEY_PUSH)
	{
		m_bWireFrameRender = !m_bWireFrameRender;
	}
	PreFrame();
	if (!m_Timer.Frame())
	{
		return false;
	}
		I_Input.Frame();
		I_SoundMgr.Frame();

		if (m_bDebug)
		{
			m_dxWrite.m_dataList.clear();
			syTextArray data;
			data.data = m_Timer.m_csBuffer;
			data.color = D2D1::ColorF(1, 0, 0, 1);		//수정요망
			m_dxWrite.m_dataList.push_back(data);
			m_dxWrite.Frame();
		}
		m_pMainCamera->Frame();

		Frame();
	return PostFrame();
	/*}
	return false;*/
}
bool syCore::TCoreRender()
{
	PreRender();
	Render();
	PostRender();
	return true;
}
bool syCore::TCoreRelease()
{

	Release();

	m_Timer.Release();
	I_Input.Release();
	I_SoundMgr.Release();
	m_dxWrite.Release();
	m_SkyBox.Release();
	syDevice::ReleaseDevice();
	return true;
}
void syCore::MessageProc(MSG msg)
{
	I_Input.MsgProc(msg);
}
LRESULT	syCore::MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (m_pMainCamera == nullptr) return 1;
	return m_pMainCamera->MsgProc(hWnd, msg, wParam, lParam);
}
bool syCore::Run()
{
	TCoreInit();
	MSG msg = { 0, };
	// WM_QUIT -> FALSE
	// 메세지 큐에서 하나하나씩 가져온다.
	//while (GetMessage(&msg, nullptr, 0, 0))
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg); // 메세지 번역
			DispatchMessage(&msg);	// 메세지 프로시져 전달	
			MessageProc(msg);
		}
		else
		{
			// 게임로직
			TCoreFrame(); // 게임계산
			TCoreRender();// 게임드로우		
			//	Sleep(1);
		}
	}
	TCoreRelease(); // 게임 종료 
	return true;
}

bool syCore::CreateResource()
{
	return true;
}
bool syCore::DeleteResource()
{
	return true;
}
HRESULT syCore::DeleteDXResource()
{
	m_dxWrite.DiscardDeviceResource();

	DeleteResource();
	return S_OK;
}
HRESULT syCore::CreateDXResource()
{
	IDXGISurface1*   m_pBuffer;
	m_pSwapChain->GetBuffer(0,
		__uuidof(IDXGISurface1),
		(LPVOID*)&m_pBuffer);

	m_dxWrite.CreateDeviceResources(m_pBuffer);

	if (m_pBuffer) m_pBuffer->Release();

	CreateResource();
	return S_OK;
}
syCore::syCore()
{
	m_bDebug = false;
	m_bWireFrameRender = false;
}


syCore::~syCore()
{
}
