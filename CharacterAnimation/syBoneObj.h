#pragma once
#include "syModel.h"
#include "syObjStd.h"

class syBoneObj : public syModel
{
public:
	syBoneObj() { m_fElapseTime = 0.0f; };
	virtual ~syBoneObj() {};
public:
	int						m_iAttribute;
	float					m_fElapseTime;
	wstring					m_filename;

	syScene					m_Scene;
	std::vector<syMesh>		m_tMeshList;
	std::vector<D3DXMATRIX>		m_calList;
	D3DXMATRIX					m_knifecalList;

public:
	bool Frame() override;
	virtual HRESULT CreateVertexData() override;
	virtual HRESULT CreateIndexData() override;
	virtual HRESULT CreateVertexBuffer()override;
	virtual HRESULT CreateIndexBuffer()override;
	virtual HRESULT LoadTextures(ID3D11Device* pd3dDevice, const TCHAR* pLoadTextureFile)  override;
	//virtual bool  Render(ID3D11DeviceContext* pContext)override;
	virtual bool  PreRender(ID3D11DeviceContext* pContext)override;
	virtual bool  PostRender(ID3D11DeviceContext* pContext)override;

public:
	void Interpolate(syMesh& tMesh, D3DXMATRIX &matParent, float fElapseTime);
	bool GetAnimationTrack(float fElapseTime,
		std::vector<syAnimTrack>& trackList,
		syAnimTrack& StartTrack,
		syAnimTrack& EndTrack);

};

