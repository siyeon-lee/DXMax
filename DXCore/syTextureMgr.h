#pragma once
#include "syTexture.h"
class syTextureMgr : public sySingleton<syTextureMgr>
{
	friend class sySingleton<syTextureMgr>;
	typedef std::map<int, syTexture*>::iterator ITOR;
	// 사전식 컨테이너 < 키, 값 >
	std::map<int, syTexture*>   m_List;

public:
	int					m_iNumTexture;
public:
	bool			Init();
	bool			Frame();
	bool			Render();
	bool			Release();

	int				Load(ID3D11Device* pDevice, T_STR filePath);
	syTexture*		GetPtr(int iKey);
	bool			Delete(int iKey);
	bool			Apply(ID3D11DeviceContext* pContext, UINT iKey, UINT iSlot);
private:
	syTextureMgr();
public:
	~syTextureMgr();
};


#define I_TextureMgr syTextureMgr::GetInstance()
