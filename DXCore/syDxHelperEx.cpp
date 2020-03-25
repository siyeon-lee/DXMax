#include "syDxHelperEx.h"

namespace DX 
{
	HRESULT CompilerShaderFromFile(const TCHAR* sFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
	{
		HRESULT hr;
		//3. Á¤Á¡ ½¦ÀÌ´õ¿Í ÇÈ¼¿ ½¦ÀÌ´õ ÆÄÀÏ ÀÛ¼º
		//4. Á¤Á¡ ½¦ÀÌ´õ¿Í ÇÈ¼¿ ½¦ÀÌ´õ ÄÄÆÄÀÏ¹× °´Ã¼ »ý¼º
		DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )	
		dwShaderFlags |= D3DCOMPILE_DEBUG;
		dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;

#endif
		ID3D10Blob* pErrorMsgs;
		if (FAILED(hr = D3DX11CompileFromFile(sFileName, NULL, NULL, szEntryPoint, szShaderModel,
			dwShaderFlags, 0, NULL,
			ppBlobOut,
			&pErrorMsgs,
			NULL)))
		{
			if (pErrorMsgs != nullptr)
			{
				OutputDebugStringA((char*)pErrorMsgs->GetBufferPointer());
				if (pErrorMsgs) pErrorMsgs->Release();
			}
		}
		return hr;
	}

	ID3D11VertexShader* LoadVertexShaderFile(ID3D11Device* pd3dDevice, const void* pShaderFileData,
		ID3DBlob** ppBlobOut, const char* pFuntionName, bool bBinary)
	{
		HRESULT hr;
		ID3DBlob* pBlob = NULL;
		ID3D11VertexShader* pVertexShader;
		DWORD dwSize = 0;
		LPCVOID lpData = 0;
		if (bBinary)
		{
			pBlob = *ppBlobOut;
			if (pBlob == nullptr) return nullptr;
			dwSize = pBlob->GetBufferSize();
			lpData = pBlob->GetBufferPointer();
		}
		else //bBinary==false
		{
			if (pFuntionName == 0)
			{
				if (FAILED(hr = CompilerShaderFromFile((TCHAR*)pShaderFileData,
					"VS", "vs_5_0", &pBlob)))
				{
					return nullptr;
				}
			}
			else //pFuntionName !=0
			{
				if (FAILED(hr = CompilerShaderFromFile((TCHAR*)pShaderFileData,
					pFuntionName, "vs_5_0", &pBlob)))
				{
					return nullptr;
				}
			}
			dwSize = pBlob->GetBufferSize();
			lpData = pBlob->GetBufferPointer();
		}
		if (FAILED(hr = pd3dDevice->CreateVertexShader(lpData, dwSize, NULL, &pVertexShader)))
		{
			pBlob->Release();
			return nullptr;
		}

		if (ppBlobOut == nullptr)
		{
			pBlob->Release();
		}
		else
		{
			*ppBlobOut = pBlob;
		}
		return pVertexShader;
	}

	ID3D11PixelShader* LoadPixelShaderFile(ID3D11Device* pd3dDevice, const void* pShaderFileData, ID3DBlob** ppBlobOut,	const char* pFuntionName, bool bBinary) 
	{
		HRESULT hr;
		ID3DBlob* pBlob = NULL;
		ID3D11PixelShader* pShader;
		DWORD dwSize = 0;
		LPCVOID lpData = 0;
		if (bBinary)
		{
			pBlob = *ppBlobOut;
			if (pBlob == nullptr) return nullptr;
			dwSize = pBlob->GetBufferSize();
			lpData = pBlob->GetBufferPointer();
		}
		else //bBinary==false
		{
			if (pFuntionName == 0)
			{
				if (FAILED(hr = CompilerShaderFromFile((TCHAR*)pShaderFileData,
					"PS", "ps_5_0", &pBlob)))
				{
					return nullptr;
				}
			}
			else //pFuntionName !=0
			{
				if (FAILED(hr = CompilerShaderFromFile((TCHAR*)pShaderFileData,
					pFuntionName, "ps_5_0", &pBlob)))
				{
					return nullptr;
				}
			}
			dwSize = pBlob->GetBufferSize();
			lpData = pBlob->GetBufferPointer();
		}
		if (FAILED(hr = pd3dDevice->CreatePixelShader(lpData, dwSize, NULL, &pShader)))
		{
			pBlob->Release();
			return nullptr;
		}

		if (ppBlobOut == nullptr)
		{
			pBlob->Release();
		}
		else
		{
			*ppBlobOut = pBlob;
		}
		return pShader;
	}

	ID3D11InputLayout* CreateInputLayout(ID3D11Device* pd3dDevice, DWORD dwSize, LPCVOID lpData,D3D11_INPUT_ELEMENT_DESC* layout, UINT numElements)
	{
		HRESULT hr;
		ID3D11InputLayout* pInputLayout = nullptr;
		if (FAILED(hr = pd3dDevice->CreateInputLayout(layout,
			numElements,
			lpData,
			dwSize, &pInputLayout)))
		{
			return nullptr;
		}
		return pInputLayout;
	}

	ID3D11Buffer* CreateVertexBuffer(ID3D11Device* pd3dDevice, void * pData, UINT iNumCount, UINT iSize, bool  bDynamic)
	{
		HRESULT hr;
		ID3D11Buffer* pBuffer = nullptr;
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
		bd.ByteWidth = iSize * iNumCount;
		if (bDynamic)
		{
			bd.Usage = D3D11_USAGE_DYNAMIC;
			bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		}
		else
		{
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.CPUAccessFlags = 0;
		}
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(D3D11_SUBRESOURCE_DATA));
		sd.pSysMem = pData;

		if (pData != nullptr)
		{
			if (FAILED(hr = pd3dDevice->CreateBuffer(
				&bd, &sd,
				&pBuffer)))
			{
				return nullptr;
			}
		}
		else 
		{
			if (FAILED(hr = pd3dDevice->CreateBuffer(
				&bd, NULL, &pBuffer)))
			{
				return nullptr;
			}
		}

		return pBuffer;
	}

	ID3D11Buffer* CreateIndexBuffer(ID3D11Device* pd3dDevice, void * pData, UINT iNumCount, UINT iSize, bool  bDynamic)
	{
		HRESULT hr;
		ID3D11Buffer* pBuffer = nullptr;
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
		bd.ByteWidth = iSize * iNumCount;
		if (bDynamic)
		{
			bd.Usage = D3D11_USAGE_DYNAMIC;
			bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		}
		else
		{
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.CPUAccessFlags = 0;
		}
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(D3D11_SUBRESOURCE_DATA));
		sd.pSysMem = pData;

		if (pData != nullptr)
		{
			if (FAILED(hr = pd3dDevice->CreateBuffer(
				&bd, &sd,
				&pBuffer)))
			{
				return nullptr;
			}
		}
		else
		{
			if (FAILED(hr = pd3dDevice->CreateBuffer(
				&bd, NULL, &pBuffer)))
			{
				return nullptr;
			}
		}
		return pBuffer;
	}

	ID3D11Buffer* CreateConstantBuffer(ID3D11Device* pd3dDevice, void * pData,	UINT iNumCount,	UINT iSize,	bool  bDynamic)
	{
		HRESULT hr;
		ID3D11Buffer* pBuffer = nullptr;
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
		bd.ByteWidth = iSize * iNumCount;
		if (bDynamic)
		{
			bd.Usage = D3D11_USAGE_DYNAMIC;
			bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		}
		else
		{
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.CPUAccessFlags = 0;
		}
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(D3D11_SUBRESOURCE_DATA));
		sd.pSysMem = pData;

		if (pData != nullptr)
		{
			if (FAILED(hr = pd3dDevice->CreateBuffer(
				&bd, &sd,
				&pBuffer)))
			{
				return nullptr;
			}
		}
		else
		{
			if (FAILED(hr = pd3dDevice->CreateBuffer(
				&bd, NULL, &pBuffer)))
			{
				return nullptr;
			}
		}
		return pBuffer;
	}

	void syDxHelperEx::PreRender(ID3D11DeviceContext* pContext, UINT iSize)
	{
		/*DX::ApplyBS(pContext, DX::syDxState::m_pAlphaBlend);
		DX::ApplySS(pContext, DX::syDxState::m_pSSWrapLinear);
		pContext->VSSetConstantBuffers(0, 1, m_pConstantBuffer.GetAddressOf());
		pContext->PSSetShaderResources(0, 1, m_pSRV.GetAddressOf());
		pContext->PSSetShaderResources(1, 1, m_pSRV.GetAddressOf());
		pContext->IASetInputLayout(m_pInputLayout.Get());
		pContext->VSSetShader(m_pVertexShader.Get(), NULL, 0);
		pContext->PSSetShader(m_pPixelShader.Get(), NULL, 0);*/

		pContext->VSSetConstantBuffers(0, 1,
			m_pConstantBuffer.GetAddressOf());
		pContext->PSSetShaderResources(0, 1,
			m_pSRV.GetAddressOf());
		//pContext->PSSetShaderResources(1, 1, m_pSRV.GetAddressOf());
		pContext->IASetInputLayout(m_pInputLayout.Get());
		pContext->VSSetShader(m_pVertexShader.Get(), NULL, 0);
		pContext->PSSetShader(m_pPixelShader.Get(), NULL, 0);

		UINT stride = iSize;
		UINT offset = 0;
		pContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(),
			&stride, &offset);
		pContext->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);


		pContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	}
	bool syDxHelperEx::Render(ID3D11DeviceContext* pContext)
	{
		PreRender(pContext, m_iVertexSize);
		PostRender(pContext, m_iNumIndex);
		return true;
	}
	void syDxHelperEx::PostRender(ID3D11DeviceContext* pContext, UINT iCount)
	{
		if (iCount == 0) iCount = m_iNumIndex;
		if (iCount != 0)
			pContext->DrawIndexed(iCount, 0, 0);
		else
			pContext->Draw(m_iNumVertex, 0);
	}

	syDxHelperEx::syDxHelperEx()
	{
		m_iNumVertex = 0;
		m_iNumIndex = 0;
	}


	syDxHelperEx::~syDxHelperEx()
	{
	}
}


