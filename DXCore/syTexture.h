#pragma once
#include "syStd.h"
class syTexture
{
public:
	syTexture();
	virtual ~syTexture();
public:
	T_STR		m_szName;
	T_STR		m_szPath;
	int			m_iIndex;
	float		m_fScale;
public:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pSRV;
public:
	bool Load(ID3D11Device* pDevice,	std::wstring filename);
	bool    Apply(ID3D11DeviceContext* pContext, UINT iSlot = 0);
public:
	bool Init();
	bool Frame();
	bool Render();
	bool Release();

	bool    Draw(float x, float y, RECT rt, DWORD iMode = SRCCOPY);
	bool    Draw(float x, float y, RECT rt, BLENDFUNCTION bf);
	bool    Draw(RECT rtDesk, RECT rtSrc, BLENDFUNCTION bf);
	bool    Draw(RECT rtDesk, RECT rtSrc, DWORD iMode = SRCCOPY);
	bool    DrawColorKey(float x, float y, RECT rt, COLORREF color);
	void	SetScale(float sc)
	{
		m_fScale = sc;
	};
};

