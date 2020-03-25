#pragma once
#include "syModel.h"
#include "syObjStd.h"
#include "syBoneObj.h"
struct CB_BoneAnim
{
	D3DXMATRIX	g_pMatrix[255];
};
class sySkmObj : public syModel
{
public:
	sySkmObj() { /*m_fElapseTime = 0.0f;*/ };
	virtual ~sySkmObj() {};
	public:

		int						m_iAttribute;
		float					m_fElapseTime;
		wstring					m_filename;
		
		syScene					m_Scene;
		std::vector<syBMesh>	m_tMeshList;
		std::vector<syMtrl>		m_MtrlList;
		CB_BoneAnim				m_AniList;
		vector<D3DXMATRIX>		m_NodeTMList;
		Microsoft::WRL::ComPtr<ID3D11Buffer>	m_cbNodeTM;
	public:
		bool Init() override;
		bool Frame() override;
		bool Frame(std::vector<D3DXMATRIX>		m_calList);
		virtual HRESULT CreateVertexData() override;
		virtual HRESULT CreateIndexData() override;
		virtual HRESULT CreateVertexBuffer()override;
		virtual HRESULT CreateIndexBuffer()override;
		virtual HRESULT CreateConstantBuffer()override;
		virtual HRESULT SetInputLayout();
		virtual HRESULT LoadTextures(ID3D11Device* pd3dDevice, const TCHAR* pLoadTextureFile)  override;
		virtual bool  Render(ID3D11DeviceContext* pContext)override;
		virtual bool  PreRender(ID3D11DeviceContext* pContext)override;
		virtual bool  PostRender(ID3D11DeviceContext* pContext)override;
		/*void		getMaxObject(vector<syMesh> m_ObjList, int ObjNum, vector<syMtrl> m_TexList, int TexNum);
		T_STR		m_resourceName;
		vector <sySkmObj> m_subObject;*/
	public:
		void Interpolate(syBMesh& tMesh, D3DXMATRIX &matParent, float fElapseTime);
		bool GetAnimationTrack(float fElapseTime,
			std::vector<syAnimTrack>& trackList,
			syAnimTrack& StartTrack,
			syAnimTrack& EndTrack);
		/*virtual HRESULT LoadTextures(ID3D11Device* pd3dDevice, const TCHAR* pLoadTextureFile) override;*/
};

