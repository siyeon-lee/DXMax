#pragma once
#include "syPlaneObject.h"
#include "syTextureMgr.h"
struct syScene
{
	int   iFirstFrame;  // 0
	int   iLastFrame;   // 100
	int   iFrameSpeed; // 30
	int   iTickPerFrame; // 160
	int   iNumObjects;
	int   iNumMaterials;
};
struct syAnimTrack
{
	int				i;
	D3DXVECTOR3		p;
	D3DXQUATERNION	q;
};


struct syTexMap		//
{
	int   iMapID;  //iTexIndex
	T_STR  szName;
};

struct syMtrl
{
	T_STR		szName;
	int         iTexIndex;
	std::vector<syTexMap>  texList;
	std::vector< syMtrl> subMtrl;
};
struct syMesh
{
	T_STR		m_szName;
	T_STR		m_ParentzName;
	syMesh*		m_pParent;
	int			iMtrlID;
	SY_BOX		m_box;//

	D3DXMATRIX m_matWorld;
	D3DXMATRIX	m_matInvWorld;
	D3DXMATRIX  m_matCalculation;

	int iNumFace;// iNumFace;
	int ibufferNum;		//bufferList.size(),	=	1

	vector<vector<PNCT_VERTEX>>		vbList;
	vector<vector<DWORD>>			ibList;
	vector<syDxHelperEx>			m_renderObjList;

	std::vector<syAnimTrack>   animPos;
	std::vector<syAnimTrack>   animRot;
	std::vector<syAnimTrack>   animScl;

	// 0 frame 분해된 에니메이션 트랙의 행렬
	D3DXMATRIX m_matAnimSelf;
	D3DXMATRIX m_matPos;
	D3DXMATRIX m_matRot;
	D3DXMATRIX m_matScl;
	D3DXVECTOR3 m_vPosTrack;
	D3DXVECTOR3 m_vScaleTrack;
	D3DXQUATERNION m_qRotTrack;



};
struct syBMesh
{

	T_STR    m_szName;
	T_STR    m_ParentzName;
	syBMesh* m_pParent;
	int     iMtrlID;

	D3DXMATRIX m_matWorld;
	D3DXMATRIX m_matInvWorld;
	D3DXMATRIX  m_matCalculation;
	D3DXMATRIX InvGetNodeTM;//

	int iNumFace;// iNumFace;
	int ibufferNum;		//bufferList.size(),=1


	vector<vector<PNCTI8W8_VERTEX>>		vbList;
	vector<vector<int>>					ibList;
	vector<syDxHelperEx>				m_renderObjList;

	std::vector<syAnimTrack>   animPos;
	std::vector<syAnimTrack>   animRot;
	std::vector<syAnimTrack>   animScl;


	// 0 frame 분해된 에니메이션 트랙의 행렬
	D3DXMATRIX m_matAnimSelf;
	D3DXMATRIX m_matPos;
	D3DXMATRIX m_matRot;
	D3DXMATRIX m_matScl;
	D3DXVECTOR3 m_vPosTrack;
	D3DXVECTOR3 m_vScaleTrack;
	D3DXQUATERNION m_qRotTrack;

};

//struct syMtrl
//{
//	T_STR		szName;
//	T_STR		m_ParentzName;
//	int         iTexIndex;
//	std::vector<syTexMap>  texList;
//	std::vector< syMtrl> subMtrl;
//};
//struct syBMesh
//{
//
//	int     iSubMesh;
//	T_STR    name;
//	T_STR    ParentName;
//
//	D3DXMATRIX matWorld;
//	D3DXMATRIX matInvWorld;
//
//
//	vector<vector<PNCTI8W8_VERTEX>>		vbList;
//	vector<vector<vector<int>>>			ibList;
//	vector<syDxHelperEx>			m_renderObjList;
//
//	std::vector<syAnimTrack>   animPos;
//	std::vector<syAnimTrack>   animRot;
//	std::vector<syAnimTrack>   animScl;
//	int     iMtrlID;
//};
