#pragma once
#include "stdafx.h"
#include "syExpStd.h"

class syWriter
{
public:
	syWriter();
	virtual ~syWriter();
public:
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
	virtual void Set(const TCHAR* name ,Interface* pMax);
	virtual bool  Export();
	virtual bool  Convert();
	virtual void PreProcess(INode* pNode);

	bool TMNegParity(Matrix3 tm);
	void DumpPoint3(Point3& v0, Point3& v1);
	void   DumpMatrix3(Matrix3& matWorld, D3D_MATRIX& world);
	Point3 GetVertexNormal(Mesh* mesh, int iFace, RVertex* rVertex);
	void GetMaterial(INode* pNode);
	int   FindMaterial(INode* pNode);
	void GetTexture(Mtl* pMtl, syMtrl& tMtrl);


	void AddObject(INode* pNode);
	void AddMaterial(INode* pNode);
	TriObject* GetTriObjectFromNode(INode* pNode, TimeValue time, bool& deleteite);
	void GetMesh(INode* pNode, syMesh& tMesh);
	virtual void  SetUniqueBuffer(syMesh& tMesh);
	TCHAR*  FixupName(MSTR name);
	virtual int   IsEqulVerteList(PNCT& vertex, vertexList& vList);
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

	TCHAR* SaveFileDlg(TCHAR* szExt, TCHAR* szTitle);
};
class syMaxWriter : public syWriter
{
public:
	static syMaxWriter& Get()
	{
		static syMaxWriter theExp;
		return theExp;
	}
public:
	syMaxWriter() {}
	virtual ~syMaxWriter() {}
};
