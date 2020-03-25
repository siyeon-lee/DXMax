#include "syTexture.h"


bool    syTexture::Draw(RECT rtDesk, RECT rtSrc, DWORD iMode)
{
	return true;
}

bool    syTexture::Draw(RECT rtDesk, RECT rtSrc, BLENDFUNCTION bf)
{
	return true;
}
bool syTexture::Draw(float x, float y, RECT rt, DWORD iMode)
{
	return true;
}
bool    syTexture::Draw(float x, float y, RECT rt,	BLENDFUNCTION bf)
{
	return true;
}
bool    syTexture::DrawColorKey(float x, float y, RECT rt, COLORREF color)
{

	return true;
}

bool syTexture::Init()
{
	return true;
}
bool syTexture::Frame()
{
	return true;
}
bool syTexture::Render()
{
	return true;
}
bool syTexture::Release()
{

	return true;
}

bool syTexture::Load(ID3D11Device* pDevice, std::wstring filename)
{
	HRESULT hr;
	if (FAILED(hr = D3DX11CreateShaderResourceViewFromFile(
		pDevice,
		filename.c_str(),
		NULL,
		NULL,
		&m_pSRV,
		NULL)))
	{
		return false;
	}
	return true;
}
bool syTexture::Apply(ID3D11DeviceContext* pContext, UINT iSlot)
{
	pContext->PSSetShaderResources(iSlot, 1, &m_pSRV);
	return true;
}

syTexture::syTexture()
{
	m_pSRV = nullptr;
	m_fScale = 1;
}


syTexture::~syTexture()
{

}