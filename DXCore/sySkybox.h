#pragma once
#include "syPlaneObject.h"
class sySkybox : public syBoxObject
{
public:
	sySkybox();
	virtual ~sySkybox();
private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pTexSRV[6];	
public:
	bool  PostRender(ID3D11DeviceContext* pContext) override;
	HRESULT LoadTextures(ID3D11Device* pd3dDevice, const TCHAR* pLoadTextureFile);
};

