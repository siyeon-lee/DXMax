#pragma once
#include "syMatrix.h"
#include "syModel.h"

class syPlaneObject : public syModel
{
public:
	syPlaneObject() {};
	virtual ~syPlaneObject() {};

public:
	virtual HRESULT CreateVertexData() override;
	virtual HRESULT CreateIndexData() override;
	void SetColor(D3DXVECTOR4 retColor);

};


class syDirectionLineObject : public syModel
{
public:
	syDirectionLineObject() {};
	virtual ~syDirectionLineObject() {};

public:
	virtual HRESULT CreateVertexData() override;
	virtual bool  PostRender(ID3D11DeviceContext* pContext);
};

class syLineObject : public syModel
{
public:
	virtual HRESULT CreateVertexData() override;
	virtual bool  PostRender(ID3D11DeviceContext* pContext);
	void  Draw(ID3D11DeviceContext* pContext,
		D3DXVECTOR3 v0,
		D3DXVECTOR3 v1,
		D3DXVECTOR4 vColor);
public:
	syLineObject() {};
	virtual ~syLineObject() {};
};

class syBoxObject : public syModel
{
public:
	syBoxObject() {};
	virtual ~syBoxObject() {};

public:
	virtual HRESULT CreateVertexData() override;
	virtual HRESULT CreateIndexData() override;
};
class syCircleObject : public syModel
{
public:
	syCircleObject(int num) { iNum = num; };
	syCircleObject() { iNum = 20; };
	virtual ~syCircleObject() {};

public:
	int iNum = 20;
	virtual HRESULT CreateVertexData() override;
	virtual HRESULT CreateIndexData() override;
};
class syTriangleObject : public syModel
{
public:
	syTriangleObject() {};
	virtual ~syTriangleObject() {};

public:
	virtual HRESULT CreateVertexData() override;

};