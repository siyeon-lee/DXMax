#pragma once
#include "syDevice.h"
#include "syTimer.h"
#include "syInput.h"
#include "sySoundMgr.h"
#include "syDirectWrite.h"
#include "syDxHelperEx.h"
#include "syPlaneObject.h"
#include "syDebugCamera.h"
#include "sySkybox.h"
class syCore : public syDevice
{
	DWORD beforeTick = 0;
public:
	sySkybox		m_SkyBox;
	syDirectWrite	m_dxWrite;
	syTimer			m_Timer;
	bool			m_bDebug;
	bool			m_bWireFrameRender;
	syDebugCamera*		m_pMainCamera;
	syDebugCamera		m_DefaultCamera;
	syDirectionLineObject m_DirAxis;
public:
	virtual bool Init();
	virtual bool PreFrame();
	virtual bool Frame();
	virtual bool PostFrame();
	virtual bool PreRender();
	virtual bool Render();
	virtual bool PostRender();
	virtual bool Release();
	virtual bool DebugRender();
	virtual bool CreateResource();
	virtual bool DeleteResource();

public:
	bool TCoreInit();
	bool TCoreFrame();
	bool TCoreRender();
	bool TCoreRelease();
	void MessageProc(MSG msg);
	LRESULT	MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	virtual HRESULT DeleteDXResource() override;
	virtual HRESULT CreateDXResource() override;
public:
	bool Run();
public:
	syCore();
	virtual ~syCore();
};

