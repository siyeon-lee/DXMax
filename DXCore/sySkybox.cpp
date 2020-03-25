#include "sySkybox.h"
#include "syTextureMgr.h"


sySkybox::sySkybox()
{
}


sySkybox::~sySkybox()
{
}
bool  sySkybox::PostRender(ID3D11DeviceContext* pContext)
{
	DX::ApplySS(pContext, DX::syDxState::m_pSSWrapPoint);
	/*for (int iSub = 0; iSub < 6; iSub++)
	{
		pContext->PSSetShaderResources(0, 1,m_pTexSRV[iSub].GetAddressOf());
		pContext->DrawIndexed(6, 6* iSub, 0);
	}*/
	pContext->PSSetShaderResources(2, 6,m_pTexSRV[0].GetAddressOf());
	pContext->DrawIndexed(36, 0, 0);
	DX::ApplySS(pContext, DX::syDxState::m_pSSWrapLinear);

	return true;
}
HRESULT sySkybox::LoadTextures(ID3D11Device* pd3dDevice, const TCHAR* pLoadTextureFile)
{
	HRESULT hr = S_OK;
	const TCHAR* g_szSkyTextures[] =
	{
	
		L"../../data/sky/mariosky.png",
		L"../../data/sky/mariosky.png",
		L"../../data/sky/mariosky.png",
		L"../../data/sky/mariosky.png",
		L"../../data/sky/mariosky.png",
		L"../../data/sky/mariosky.png"
	};
	for (int iSub = 0; iSub < 6; iSub++)
	{
		int iIndex = I_TextureMgr.Load(pd3dDevice, g_szSkyTextures[iSub]);
		if (iIndex >= 0)
		{
			m_pTexSRV[iSub]=(I_TextureMgr.GetPtr(iIndex)->m_pSRV);
		}
	}
	return hr;

}
