#include "syTextureMgr.h"



syTextureMgr::syTextureMgr()
{
	m_iNumTexture = 0;
}


syTextureMgr::~syTextureMgr()
{
	Release();
}


bool		syTextureMgr::Init()
{
	return true;
}

bool		syTextureMgr::Frame()
{
	return true;
}

bool		syTextureMgr::Render()
{
	for (ITOR itor = m_List.begin(); itor != m_List.end(); itor++)
	{
		syTexture* pData = (*itor).second;
		pData->Render();
	}
	return true;
}

bool		syTextureMgr::Release()
{
	for (ITOR itor = m_List.begin();itor != m_List.end();)
	{
		syTexture* pData = (*itor).second;
		pData->Release();
		delete pData;
		itor = m_List.erase(itor);
	}

	m_List.clear();
	return true;
}

syTexture*		syTextureMgr::GetPtr(int iKey)
{
	ITOR itor = m_List.find(iKey);

	if (itor != m_List.end())
	{
		syTexture* pData = (*itor).second;
		return pData;
	}
	return nullptr;
}

bool		syTextureMgr::Delete(int iKey)
{
	ITOR itor = m_List.find(iKey);
	if (itor != m_List.end())
	{
		syTexture* pData = (*itor).second;
		pData->Release();
		delete pData;
		m_List.erase(itor);
		return true;
	}
	return false;
}

int        syTextureMgr::Load(ID3D11Device* pDevice, T_STR filepath)
{
	//path split
	if (filepath.empty()) return -1;

	TCHAR Drive[MAX_PATH] = { 0, };
	TCHAR Dir[MAX_PATH] = { 0, };
	TCHAR Name[MAX_PATH] = { 0, };
	TCHAR Ext[MAX_PATH] = { 0, };
	_tsplitpath_s(filepath.c_str(), Drive, Dir, Name, Ext);

	T_STR filename = Name;
	filename += Ext;

	T_STR path = Drive;
	path += Dir;

	// 중복제거
	for (ITOR itor = m_List.begin(); itor != m_List.end();itor++)
	{
		syTexture* pData = (*itor).second;
		
		if (pData->m_szPath + pData->m_szName == filepath)		//변화한곳0918 release에러
		{
			return pData->m_iIndex;
		}
	}
	syTexture* pData = new syTexture;
	if (pData->Load(pDevice, filepath) == false)
	{
		delete pData;
		return -1;
	}
	pData->m_szName = filename;
	pData->m_szPath = path;
	pData->m_iIndex = m_iNumTexture++;
	m_List.insert(make_pair(pData->m_iIndex, pData));
	return pData->m_iIndex;
}

bool	syTextureMgr::Apply(ID3D11DeviceContext* pContext, UINT iKey, UINT iSlot)
{
	GetPtr(iKey)->Apply(pContext, iSlot);
	return true;
}