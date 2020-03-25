#pragma once
#include "stdafx.h"
#include <string>
#include <vector>
struct PNCT
{
	Point3 p;
	Point3 n;
	Point4 c;
	Point2 t;

};

typedef struct _D3D_MATRIX {
	union {
		struct {
			float        _11, _12, _13, _14;
			float        _21, _22, _23, _24;
			float        _31, _32, _33, _34;
			float        _41, _42, _43, _44;
		};
		float m[4][4];
	};
}D3D_MATRIX, *LPD3D_MATRIX;

struct TriList
{
	PNCT v[3];
	int    iSubIndex;	//iMatIndex로 바꾸기
};
struct syTexMap		
{
	int   iMapID;					//이게
	TSTR  szName;
};
struct syTexMapEx
{
	int    iMapID;
	TCHAR  szName[32];
};
struct syMtrl
{
	INode*  pINode;
	int   iMtrlID;
	TSTR  szName;
	std::vector<syTexMap>  texList;
	std::vector<syMtrl>    subMtrl;
};
struct syMtrlHeader
{
	int    iMtrlID;
	TCHAR  szName[32];
};
struct syMtrlEx
{
	syMtrlHeader header;
	std::vector<syTexMapEx>  texList;
	std::vector<syMtrlEx>    subMtrl;
};
typedef std::vector<TriList>   vectorTriList;
typedef std::vector<PNCT>   vertexList;
typedef std::vector<int>    IndexList;
struct syAnimTrack
{
	int     i;
	Point3  p;
	Quat    q;
};


struct syMesh
{
	INode*  pINode;
	int     iSubMesh;					
	TSTR    name;
	TSTR    ParentName;
	Matrix3    wtm;	//world transform matrix
	D3D_MATRIX matWorld;
	std::vector<TriList>			triList;
	std::vector<vectorTriList>		bufferList;

	std::vector<vertexList>   vbList;
	std::vector<IndexList>    ibList;

	bool  bAnimatin[3];
	std::vector<syAnimTrack>   animPos;
	std::vector<syAnimTrack>   animRot;
	std::vector<syAnimTrack>   animScl;

	int     iMtrlID;
	syMesh()
	{
		name = L"none";
		ParentName = L"none";
		iMtrlID = -1;
		iSubMesh = 1;
	}
};
struct syMeshHeader
{
	int     iSubMesh;
	int     iMtrlID;
	TCHAR    name[32];
	TCHAR    ParentName[32];
	D3D_MATRIX matWorld;
};
struct syMeshEx
{
	syMeshHeader header;
	std::vector<vertexList>   vbList;
	std::vector<IndexList>    ibList;
};
struct syScene
{
	int   iFirstFrame;  // 0
	int   iLastFrame;   // 100
	int   iFrameSpeed; // 30
	int   iTickPerFrame; // 160
	int   iNumObjects;
	int   iNumMaterials;
};
class syWriter
{
public:
	syWriter();
	virtual ~syWriter();
private:
	FILE*				m_pStream;
	Interface*			m_pMax;
	std::wstring		m_filename;
	INode*				m_pRootNode;
	syScene				m_Scene;

	std::vector<INode*>  m_ObjectList;
	std::vector<syMesh>   m_tMeshList;

	std::vector<Mtl*>    m_MaterialList;
	std::vector<syMtrl>   m_MtrlList;

	Interval		m_Interval;
public:
	void Set(const TCHAR* name ,Interface* pMax);
	bool  Export();
	bool  Convert();
	bool TMNegParity(Matrix3 tm);
	void DumpPoint3(Point3& v0, Point3& v1);
	void   DumpMatrix3(Matrix3& matWorld, D3D_MATRIX& world);
	Point3 GetVertexNormal(Mesh* mesh, int iFace, RVertex* rVertex);
	void GetMaterial(INode* pNode);
	int   FindMaterial(INode* pNode);
	void GetTexture(Mtl* pMtl, syMtrl& tMtrl);

	void PreProcess(INode* pNode);
	void AddObject(INode* pNode);
	void AddMaterial(INode* pNode);
	TriObject* GetTriObjectFromNode(INode* pNode, TimeValue time, bool& deleteite);
	void GetMesh(INode* pNode, syMesh& tMesh);
	void  SetUniqueBuffer(syMesh& tMesh);
	TCHAR*  FixupName(MSTR name);
	int   IsEqulVerteList(PNCT& vertex, vertexList& vList);
	bool	EqualPoint2(Point2 p1, Point2 p2);
	bool	EqualPoint3(Point3 p1, Point3 p2);
	bool	EqualPoint4(Point4 p1, Point4 p2);

	void GetAnimation(INode* pNode,	syMesh& tMesh);
	void ExportAnimation(syMesh& tMesh);
	bool CheckForAnimation(INode* pNode,
		bool&  bPos, bool& bRot, bool& bScl);
	void DumpPosSample(INode* pNode, syMesh& pMesh);
	void DumpRotSample(INode* pNode, syMesh& pMesh);
	void DumpSclSample(INode* pNode, syMesh& pMesh);

};

