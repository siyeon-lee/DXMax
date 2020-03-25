#pragma once
#include "syWriter.h"
class syMatrixExp : public syWriter
{
public:
	syMatrixExp();
	virtual ~syMatrixExp();
public:
	static syMatrixExp& Get()
	{
		static syMatrixExp theExp;
		return theExp;
	}
public:
	void  PreProcess(INode* pNode) override;
	bool  Export()override;
	bool  Convert()override;
	void  SetUniqueBuffer(syMesh& tMesh) override;
};

