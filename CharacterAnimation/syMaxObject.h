#pragma once
#include "syModel.h"
#include "syObjStd.h"
#include "syBoneObj.h"
#include "sySkmObj.h"
class syMaxObject : public syModel
{
public:
	syMaxObject() { /*m_fElapseTime = 0.0f;*/ };
	virtual ~syMaxObject() {};
public:
	int						m_iAttribute;
	float					m_fElapseTime;
	wstring					m_filename;

	syScene					m_Scene;
	std::vector<syMesh>		m_tMeshList;
	std::vector<syMtrl>		m_MtrlList;
public:
	bool Frame() override;
	virtual HRESULT CreateVertexData() override;
	virtual HRESULT CreateIndexData() override;
	virtual HRESULT CreateVertexBuffer()override;
	virtual HRESULT CreateIndexBuffer()override;
	virtual HRESULT LoadTextures(ID3D11Device* pd3dDevice, const TCHAR* pLoadTextureFile)  override;
	virtual bool  Render(ID3D11DeviceContext* pContext)override;
	virtual bool  PreRender(ID3D11DeviceContext* pContext)override;
	virtual bool  PostRender(ID3D11DeviceContext* pContext)override;
	/*void		getMaxObject(vector<syMesh> m_ObjList, int ObjNum, vector<syMtrl> m_TexList, int TexNum);
	T_STR		m_resourceName;
	vector <syMaxObject> m_subObject;*/
public:
	void Interpolate(syMesh& tMesh, D3DXMATRIX &matParent, float fElapseTime);
	bool GetAnimationTrack(float fElapseTime,
		std::vector<syAnimTrack>& trackList,
		syAnimTrack& StartTrack,
		syAnimTrack& EndTrack);
	/*virtual HRESULT LoadTextures(ID3D11Device* pd3dDevice, const TCHAR* pLoadTextureFile) override;*/
};

