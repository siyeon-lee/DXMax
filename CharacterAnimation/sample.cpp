#include "sample.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "syStd.h"
#include "syTextureMgr.h"
sample::sample()
{
}


sample::~sample()
{
}




bool sample::Init()
{
	m_cbLight.g_cAmbientMaterial = D3DXVECTOR4(0.1f, 0.1f, 0.1f, 1);
	m_cbLight.g_cDiffuseMaterial = D3DXVECTOR4(0.9, 0.9, 0.9, 1);
	m_cbLight.g_cAmbientLightColor = D3DXVECTOR4(1, 1, 1, 1);
	m_cbLight.g_cDiffuseLightColor = D3DXVECTOR4(1, 1, 1, 1);
	

	m_pConstantBufferLight.Attach(DX::CreateConstantBuffer(
		m_pDevice, &m_cbLight, 1, sizeof(LIGHT_CONSTANT_BUFFER)));
	m_maxObject.m_filename = L"All.SY";
	if (!m_maxObject.Create(m_pDevice, L"../../data/shader/shader.hlsl", nullptr))
	{
		return false;
	}
	m_BoneObj.m_filename = L"f.mat";
	if (!m_BoneObj.Create(m_pDevice, L"../../data/shader/shader.hlsl", nullptr))
	{
		return false;
	}

	m_Ship.m_filename = L"f.skm";
	if (!m_Ship.Create(m_pDevice, L"../../data/shader/character.hlsl", nullptr))
	{
		return false;
	}
	m_knifeBone.m_filename = L"knifeRight.mat";
	if (!m_knifeBone.Create(m_pDevice, L"../../data/shader/shader.hlsl", nullptr))
	{
		return false;
	}

	m_knifeSkm.m_filename = L"knifrRight.skm";
	if (!m_knifeSkm.Create(m_pDevice, L"../../data/shader/character.hlsl", nullptr))
	{
		return false;
	}

	m_CameraBack.Init();
	m_CameraBack.CreateFrustumObject(m_pDevice);
	m_CameraBack.SetViewMatrix(D3DXVECTOR3(0.0f, 0.0f, -10.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f));
	m_CameraBack.SetProjMatrix(D3DX_PI / 4, g_rtClient.right / g_rtClient.bottom, 1.0f, 1000.0f);
	m_pMainCamera = &m_CameraBack;

	return true;
}
bool sample::Frame()
{
	m_vLightVector.x = 1;// cosf(g_fGameTimer) / 2 + 0.5f;
	m_vLightVector.y = 1;// sinf(g_fGameTimer) / 2 + 0.5f;
	m_vLightVector.z = 1;// cosf(g_fGameTimer) * sinf(g_fGameTimer) / 2 + 0.5f;

	D3DXVec3Normalize(&m_vLightVector, &m_vLightVector);
	m_BoneObj.Frame();
	m_Ship.Frame(m_BoneObj.m_calList);
	m_knifeBone.Frame();
	D3DXMATRIX cal;
	D3DXMatrixInverse(&cal,NULL, &m_BoneObj.m_knifecalList);
	//for (auto &a : m_knifeBone.m_calList)
	//{
	//	a =  m_BoneObj.m_knifecalList;
	//}

	m_knifeSkm.Frame(m_knifeBone.m_calList);
	m_maxObject.Frame();
	//m_vLightVector *= -1.0f;
	//for (int i = 0; i < m_ObjList.size(); i++)
	//{
	//	if (!m_ObjList[i].Frame());
	//	{
	//		return false;
	//	};
	//}
	return true;
}
bool sample::Render()
{
	m_cbLight.g_vLightDir.x = m_vLightVector.x;
	m_cbLight.g_vLightDir.y = m_vLightVector.y;
	m_cbLight.g_vLightDir.z = m_vLightVector.z;
	m_cbLight.g_vLightDir.w = 1;

	m_cbLight.g_vEyeDir.x = m_pMainCamera->GetLookVector()->x;
	m_cbLight.g_vEyeDir.y = m_pMainCamera->GetLookVector()->y;
	m_cbLight.g_vEyeDir.z = m_pMainCamera->GetLookVector()->z;
	m_cbLight.g_vEyeDir.w = 100;

	m_cbLight.g_vEyePos.x = m_pMainCamera->GetEyePt()->x;
	m_cbLight.g_vEyePos.y = m_pMainCamera->GetEyePt()->y;
	m_cbLight.g_vEyePos.z = m_pMainCamera->GetEyePt()->z;
	m_cbLight.g_vEyePos.w = 1;
	D3DXMATRIX matInvWorld;
	D3DXMatrixInverse(&matInvWorld, NULL, &m_pMainCamera->m_matView);
	D3DXMatrixTranspose(&matInvWorld, &matInvWorld);
	D3DXMatrixTranspose(&m_cbLight.g_matInvWorld, &matInvWorld);


	//m_pImmediateContext->UpdateSubresource(m_pConstantBufferLight.Get(), 0, NULL, &m_cbLight, 0, 0);
	//m_pImmediateContext->VSSetConstantBuffers(1, 1, m_pConstantBufferLight.GetAddressOf());
	//m_pImmediateContext->PSSetConstantBuffers(1, 1, m_pConstantBufferLight.GetAddressOf());

	//for (int i = 0; i < m_Ship.m_subObject.size(); i++)
	//{
	m_pImmediateContext->PSSetShaderResources(1, 1,
	I_TextureMgr.GetPtr(m_Ship.m_iAttribute)->m_pSRV.GetAddressOf());
	m_Ship.SetMatrix(nullptr, &m_pMainCamera->m_matView, &m_pMainCamera->m_matProj);
	m_Ship.Render(m_pImmediateContext);

	//m_pImmediateContext->PSSetShaderResources(1, 1,
	//	I_TextureMgr.GetPtr(m_knifeSkm.m_iAttribute)->m_pSRV.GetAddressOf());
	//m_knifeSkm.SetMatrix(nullptr, &m_pMainCamera->m_matView, &m_pMainCamera->m_matProj);
	//m_knifeSkm.Render(m_pImmediateContext);

	/*	m_pImmediateContext->PSSetShaderResources(1, 1,
			I_TextureMgr.GetPtr(m_maxObject.m_iAttribute)->m_pSRV.GetAddressOf());
		m_maxObject.SetMatrix(nullptr, &m_pMainCamera->m_matView, &m_pMainCamera->m_matProj);
		m_maxObject.Render(m_pImmediateContext);*/
	//	
	//}
	//
	//for (int i = 0; i < m_ObjList.size(); i++)
	//{
	//	m_ObjList[i].SetMatrix(nullptr, &m_pMainCamera->m_matView, &m_pMainCamera->m_matProj);
	//	m_pImmediateContext->UpdateSubresource(m_pConstantBufferLight.Get(), 0, NULL, &m_cbLight, 0, 0);
	//	m_pImmediateContext->VSSetConstantBuffers(1, 1, m_pConstantBufferLight.GetAddressOf());
	//	m_pImmediateContext->PSSetConstantBuffers(1, 1, m_pConstantBufferLight.GetAddressOf());

	//	m_ObjList[i].Render(m_pImmediateContext);
	//}
	return true;
}
bool sample::Release()
{
	/*for (auto obj : m_ObjList)
	{
		obj.Release();
	}*/
	return true;
}

