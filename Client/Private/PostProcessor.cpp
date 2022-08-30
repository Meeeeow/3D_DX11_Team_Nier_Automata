#include "stdafx.h"
#include "Client_Defines.h"
#include "PostProcessor.h"
#include "Texture.h"
#include "GameInstance.h"
#include "Renderer.h"


IMPLEMENT_SINGLETON(CPostProcessor)

CPostProcessor::CPostProcessor()
{
}

void CPostProcessor::Set_PP_BeforeUI_SetTexture()
{



}

void CPostProcessor::Set_PP_AfterUI_SetTexture()
{
	for (auto& pIter : m_vecDeliveredTexture)
		Safe_Delete(pIter);
	m_vecDeliveredTexture.clear();

	MYSRV* pMySrv = nullptr;
	pMySrv = new MYSRV;
	strcpy_s(pMySrv->pConstantTextureName, MAX_PATH, "g_texFilter0");
	pMySrv->pShaderResourceView = m_mapTexture[TEXT("Prototype_Component_Texture_255_255_220")]->Get_SRV();
	m_vecDeliveredTexture.emplace_back(pMySrv);
	if (m_pRenderer)
	{
		m_pRenderer->Set_PostProcessing_BeforeUI_SRV(pMySrv);
	}

	pMySrv = new MYSRV;
	strcpy_s(pMySrv->pConstantTextureName, MAX_PATH, "g_texFilter1");
	pMySrv->pShaderResourceView = m_mapTexture[TEXT("Prototype_Component_Texture_255_255_220")]->Get_SRV();
	m_vecDeliveredTexture.emplace_back(pMySrv);
	if (m_pRenderer)
	{
		m_pRenderer->Set_PostProcessing_BeforeUI_SRV(pMySrv);
	}
}


HRESULT CPostProcessor::NativeConstruct(_pGraphicDevice _pGraphicDevice, _pContextDevice _pContextDevice)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	m_pGraphicDevice = _pGraphicDevice; Safe_AddRef(m_pGraphicDevice);
	m_pContextDevice = _pContextDevice; Safe_AddRef(m_pContextDevice);

	m_pRenderer = static_cast<CRenderer*>(pGameInstance->Clone_Component((_uint)LEVEL::STATIC, TEXT("Prototype_Renderer")));


	CTexture* pTexture = nullptr;
	if (FAILED(pGameInstance->Add_Component((_uint)LEVEL::STATIC, TEXT("Prototype_Component_Texture_255_255_220"), pTexture = CTexture::Create(m_pGraphicDevice, m_pContextDevice, TEXT("../Bin/Resources/Effect/Color/255_255_220.dds")))))
		return E_FAIL;
	m_mapTexture[TEXT("Prototype_Component_Texture_255_255_220")] = static_cast<CTexture*>(pGameInstance->Clone_Component((_uint)LEVEL::STATIC, TEXT("Prototype_Component_Texture_255_255_220")));
	if (FAILED(pGameInstance->Add_Component((_uint)LEVEL::STATIC, TEXT("Prototype_Component_Texture_255_255_230"), pTexture = CTexture::Create(m_pGraphicDevice, m_pContextDevice, TEXT("../Bin/Resources/Effect/Color/255_255_230.dds")))))
		return E_FAIL;
	m_mapTexture[TEXT("Prototype_Component_Texture_255_255_230")] = static_cast<CTexture*>(pGameInstance->Clone_Component((_uint)LEVEL::STATIC, TEXT("Prototype_Component_Texture_255_255_220")));


	return S_OK;
}


void CPostProcessor::Free()
{
	for (auto& pIter : m_mapTexture)
		Safe_Release(pIter.second);
	m_mapTexture.clear();

	for (auto& pIter : m_vecDeliveredTexture)
		Safe_Delete(pIter);
	m_vecDeliveredTexture.clear();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pGraphicDevice);
	Safe_Release(m_pContextDevice);

}