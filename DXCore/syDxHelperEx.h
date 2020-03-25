#pragma once
#include <d3d11.h>
#include <d3dx11.h>
#include <dxgi.h>
#include <wrl.h> // windows runtime c++ templaste library
#include <d3dx10math.h>
#include <D3Dcompiler.h>
//#include <DirectXMath.h>	//sdk 필요없는 수학라이브러리
#include "syDxState.h"

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "dxgi.lib")
#if defined(_DEBUG)
#pragma comment (lib, "d3dx10d.lib")
#else
#pragma comment (lib, "d3dx10.lib")
#endif
namespace DX
{
	HRESULT CompilerShaderFromFile(
		const TCHAR* sFileName,
		LPCSTR szEntryPoint,
		LPCSTR szShaderModel,
		ID3DBlob** ppBlobOut);

	ID3D11VertexShader* LoadVertexShaderFile(
		ID3D11Device* pd3dDevice,
		const void* pShaderFileData,
		ID3DBlob** ppBlobOut = nullptr,
		const char* pFuntionName = 0,
		bool bBinary = false);

	ID3D11PixelShader* LoadPixelShaderFile(
		ID3D11Device* pd3dDevice,
		const void* pShaderFileData,
		ID3DBlob** ppBlobOut = nullptr,
		const char* pFuntionName = 0,
		bool bBinary = false);

	ID3D11InputLayout* CreateInputLayout(
		ID3D11Device* pd3dDevice,
		DWORD dwSize, LPCVOID lpData,
		D3D11_INPUT_ELEMENT_DESC* layout,
		UINT numElements);

	ID3D11Buffer* CreateVertexBuffer(
		ID3D11Device* pd3dDevice,
		void * pData,
		UINT iNumCount,
		UINT iSize,
		bool  bDynamic = false);
	ID3D11Buffer* CreateIndexBuffer(ID3D11Device* pd3dDevice,
		void * pData,
		UINT iNumCount,
		UINT iSize,
		bool  bDynamic = false);
	ID3D11Buffer* CreateConstantBuffer(
		ID3D11Device* pd3dDevice,
		void * pData,
		UINT iNumCount,
		UINT iSize,
		bool  bDynamic = false);


	class syDxHelperEx
	{
	public:
		syDxHelperEx();
		virtual ~syDxHelperEx();
	public:
		UINT	m_iNumVertex;
		UINT	m_iNumIndex;
		UINT	m_iVertexSize;
	public:	
		Microsoft::WRL::ComPtr<ID3D11Buffer>				m_pVertexBuffer;	//버텍스 버퍼
		Microsoft::WRL::ComPtr<ID3D11Buffer>				m_pIndexBuffer;		//인덱스버퍼
		Microsoft::WRL::ComPtr<ID3D11Buffer>				m_pConstantBuffer;	//상수버퍼
		Microsoft::WRL::ComPtr<ID3D11VertexShader>			m_pVertexShader;	//버텍스 쉐이더
		Microsoft::WRL::ComPtr<ID3D11PixelShader>			m_pPixelShader;		//픽셀 쉐이더
		Microsoft::WRL::ComPtr<ID3D11PixelShader>			m_pPixelShaderSplat; //픽셀 스플랜더쉐이더
		Microsoft::WRL::ComPtr<ID3D11InputLayout>			m_pInputLayout;		//레이아웃
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_pSRV;
		Microsoft::WRL::ComPtr<ID3DBlob>					m_pVSBlob;

	public:
		void PreRender(ID3D11DeviceContext* pContext, UINT iSize);
		bool Render(ID3D11DeviceContext* pContext);
		void PostRender(ID3D11DeviceContext* pContext, UINT iCount);
	};


}

