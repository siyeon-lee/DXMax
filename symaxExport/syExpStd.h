#pragma once
#include <string>
#include <vector>


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

const enum OBJECTTYPE {
	CLASS_GEOM = 0,
	CLASS_BONE,
	CLASS_DUMMY,
	CLASS_BIPED,
};
struct PNCT
{
	Point3 p;
	Point3 n;
	Point4 c;
	Point2 t;

};
struct TriList
{
	PNCT v[3];
	int    iSubIndex;	//iMatIndex
};
struct syTexMap
{
	int   iMapID;					//이게
	TSTR  szName;
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
	Box3	box;
	int		iType;	//바이패드인지 본인지 더미인지 지오메트리인지
	Matrix3    wtm;	//world transform matrix
	D3D_MATRIX matWorld;
	D3D_MATRIX matInvWorld;
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
		iType = CLASS_GEOM;
	}
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
