#pragma once
#include "syWriter.h"

struct PNCTWI_VERTEX
{
	Point3 p;
	Point3 n;
	Point4 c;
	Point2 t;
	float i1[4];
	float w1[4];
	float i2[4];
	float w2[4];
};

struct syBipedVertex
{
	int    m_iNumWeight;
	std::vector<BYTE>  m_IDList;
	std::vector<float>  m_WeightList;
};

struct BTriList
{
	PNCTWI_VERTEX   v[3];
	int    iSubIndex;
};
typedef std::vector<BTriList>   BvectorTriList;
typedef std::vector<PNCTWI_VERTEX>   BvertexList;

struct syBMesh
{
	INode*  pINode;
	int     iSubMesh;
	TSTR    name;
	TSTR    ParentName;
	Matrix3    wtm;
	D3D_MATRIX matWorld;
	D3D_MATRIX matInvWorld;
	std::vector<BTriList>			triList;
	std::vector<BvectorTriList>		bufferList;
	std::vector<syBipedVertex>		bipedList;

	std::vector<BvertexList>   vbList;
	std::vector<IndexList>    ibList;

	bool  bAnimatin[3];
	std::vector<syAnimTrack>   animPos;
	std::vector<syAnimTrack>   animRot;
	std::vector<syAnimTrack>   animScl;

	int     iMtrlID;
	syBMesh()
	{
		name = L"none";
		ParentName = L"none";
		iMtrlID = -1;
		iSubMesh = 1;
	}
};

class sySkinExp : public syWriter
{
	std::vector<syBMesh>   m_bMeshList;
public:
	sySkinExp();
	virtual ~sySkinExp();
	static sySkinExp& Get()
	{
		static sySkinExp theExp;
		return theExp;
	}
public:
	void Set(const TCHAR* name, Interface* pMax) override;
	bool  Export() override;
	bool  Convert() override;

public:
	void GetMesh(INode* pNode, syBMesh& tMesh);
	void  SetUniqueBuffer(syBMesh& tMesh);
	int   IsEqulVerteList(PNCTWI_VERTEX& vertex, BvertexList& vList);
public:
	void SetBippedInfo(INode* pNode, syBMesh& tMesh);
	Modifier*  FindModifier(
		INode* pNode, Class_ID classID);
	void ExportPhysiqueData(INode* pNode, Modifier*, syBMesh&);
	void ExportSkinData(INode* pNode, Modifier*, syBMesh&);
	int GetFindIndex(INode* node);
};

