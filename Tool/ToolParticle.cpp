#include "stdafx.h"
#include "ToolParticle.h"

#include "GameInstance.h"

CToolParticle::CToolParticle(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CToolParticle::CToolParticle(const CToolParticle & rhs)
	: CGameObject(rhs)
{
}


HRESULT CToolParticle::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CToolParticle::NativeConstruct(void * pArg)
{
	PARTICLEDESC* pDesc = (PARTICLEDESC*)pArg;

	Update_ByDesc(*pDesc);

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE::POSITION, XMVectorSet(m_tParticleDesc.vPosition.x, m_tParticleDesc.vPosition.y, m_tParticleDesc.vPosition.z, 1.f));
	m_pTransformCom->Set_Scale(XMVectorSet(m_tParticleDesc.vScale.x, m_tParticleDesc.vScale.y, m_tParticleDesc.vScale.z, 0.f));
	m_pTransformCom->Rotation_Degree(m_tParticleDesc.vRotation);

	m_tParticleVar.bPlay = false;
	m_tParticleVar.fLifeTime = 0.f;
	return S_OK;
}

_int CToolParticle::Tick(_double TimeDelta)
{
	if (Get_State() == OBJSTATE::DEAD) return (_int)OBJSTATE::DEAD;

	_float fTimeDelta = static_cast<_float>(TimeDelta);

	//CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);


	if (m_tParticleDesc.tPointInst_Desc.bRepeat)
	{
			//if (m_tParticleDesc.tPointInst_Desc.bRepeat)
			//{
			//	Play();
			//	//Stop();
			//}
			//else
			//{
			//	//Init_Var();
			//	// return (_int)OBJSTATE::DEAD;
			//}
	}
	else
	{
		if (m_tParticleVar.fLifeTime > m_tParticleDesc.tPointInst_Desc.fLifeTime)
		{
			Stop();
			//m_pModelCom->Init();
		}
	}
	

	if (m_tParticleVar.bPlay)
	{
		m_tParticleVar.fLifeTime += fTimeDelta;


		m_pModelCom->Update(TimeDelta);

		m_tParticleVar.fMultiTextureElapsedTime += fTimeDelta;
		if (m_tParticleDesc.fMultiTextureInterval < m_tParticleVar.fMultiTextureElapsedTime)
		{
			if (++m_tParticleVar.iMultiTextureIndex > m_tParticleDesc.iTextureNumber - 1)
			{
				if (m_tParticleDesc.bMultiTextureRepeat)
				{
					m_tParticleVar.iMultiTextureIndex = 0;
				}
				else
				{
					--m_tParticleVar.iMultiTextureIndex;
				}
			}
		}
	}

	//RELEASE_INSTANCE(CGameInstance);
	return _int();
}

_int CToolParticle::LateTick(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::ALPHABLEND, this);

	return _int();
}

HRESULT CToolParticle::Render()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;


	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(0);

	return S_OK;
}

HRESULT CToolParticle::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, TEXT("Prototype_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, TEXT("Prototype_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, m_tParticleDesc.szTextureProto, TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;



	/* For.Com_Model */
	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, TEXT("Prototype_Component_VIBuffer_PointInstance"), TEXT("Com_Model"), (CComponent**)&m_pModelCom, &m_tParticleDesc.tPointInst_Desc)))
		return E_FAIL;
		

	return S_OK;
}

HRESULT CToolParticle::SetUp_ConstantTable()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	m_pModelCom->SetUp_RawValue("g_matWorld", &m_pTransformCom->Get_WorldMatrix4x4Transpose(), sizeof(_matrix));
	m_pModelCom->SetUp_RawValue("g_matView", &pGameInstance->Get_Transpose(CPipeLine::TRANSFORM::D3DTS_VIEW), sizeof(_matrix));
	m_pModelCom->SetUp_RawValue("g_matProj", &pGameInstance->Get_Transpose(CPipeLine::TRANSFORM::D3DTS_PROJ), sizeof(_matrix));

	//const LIGHTDESC*	pLightDesc = pGameInstance->Get_Light(L"LightDir");

	//m_pModelCom->SetUp_RawValue("g_vLightDir", (void*)&pLightDesc->vDirection, sizeof(_float4));
	//m_pModelCom->SetUp_RawValue("g_vLightDiffuse", (void*)&pLightDesc->vDiffuse, sizeof(_float4));
	//m_pModelCom->SetUp_RawValue("g_vLightSpecular", (void*)&pLightDesc->vSpecular, sizeof(_float4));
	//m_pModelCom->SetUp_RawValue("g_vLightAmbient", (void*)&pLightDesc->vAmbient, sizeof(_float4));
	m_pModelCom->SetUp_RawValue("g_vCamPos", &pGameInstance->Get_CameraPosition(), sizeof(_float4));
	m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV(m_tParticleVar.iMultiTextureIndex));

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CToolParticle::Update_ByDesc(const PARTICLEDESC & _tDesc)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	if ((_tcscmp(_tDesc.szTextureName, L"")
		&& _tcscmp(m_tParticleDesc.szTextureName, _tDesc.szTextureName))
		|| !m_pTextureCom)
	{
		if (m_pTextureCom)
			Safe_Release(m_pTextureCom);

		m_pTextureCom = static_cast<CTexture*>(pGameInstance->Clone_Component((_uint)LEVEL::STATIC, _tDesc.szTextureProto));
		if (!m_pTextureCom) return assert(false);
		Safe_AddRef(m_pTextureCom);
	}
	
	_tcscpy_s(m_tParticleDesc.szParticleName, _tDesc.szParticleName);
	memcpy(&m_tParticleDesc.vPosition, &_tDesc.vPosition, sizeof(_float3));
	memcpy(&m_tParticleDesc.vScale, &_tDesc.vScale, sizeof(_float3));
	memcpy(&m_tParticleDesc.vRotation, &_tDesc.vRotation, sizeof(_float3));

	memcpy(&m_tParticleDesc.iTextureNumber, &_tDesc.iTextureNumber, sizeof(_int));
	_tcscpy_s(m_tParticleDesc.szTextureName, _tDesc.szTextureName);
	_tcscpy_s(m_tParticleDesc.szTextureProto, _tDesc.szTextureProto);
	_tcscpy_s(m_tParticleDesc.szTexturePath, _tDesc.szTexturePath);

	memcpy(&m_tParticleDesc.fMultiTextureInterval, &_tDesc.fMultiTextureInterval, sizeof(_float));
	memcpy(&m_tParticleDesc.bMultiTextureRepeat, &_tDesc.bMultiTextureRepeat, sizeof(_bool));

	if (m_tParticleDesc.tParticleShaderDesc.vecShaderName.size() > 0)
	{
		for (_int i = 0; i < m_tParticleDesc.tParticleShaderDesc.vecShaderName.size(); i++)
		{
			Safe_Delete(m_tParticleDesc.tParticleShaderDesc.vecShaderName[i]);
		}
		m_tParticleDesc.tParticleShaderDesc.vecShaderName.clear();
	}

	for (_int i = 0; i < _tDesc.tParticleShaderDesc.vecShaderName.size(); ++i)
	{
		SHADERNAME* pShaderName = new SHADERNAME;
		_tcscpy_s(pShaderName->szName, _tDesc.tParticleShaderDesc.vecShaderName[i]->szName);
		m_tParticleDesc.tParticleShaderDesc.vecShaderName.emplace_back(pShaderName);
	}

	memcpy(&m_tParticleDesc.tPointInst_Desc, &_tDesc.tPointInst_Desc, sizeof(POINTINST_DESC));



	//////////////////////////
	if (m_pTransformCom)
	{
		m_pTransformCom->Set_State(CTransform::STATE::POSITION, XMVectorSet(m_tParticleDesc.vPosition.x, m_tParticleDesc.vPosition.y, m_tParticleDesc.vPosition.z, 1.f));
		m_pTransformCom->LookAt(pGameInstance->Get_CameraPosition());
		m_pTransformCom->Set_Scale(XMVectorSet(m_tParticleDesc.vScale.x, m_tParticleDesc.vScale.y, m_tParticleDesc.vScale.z, 0.f));
		m_pTransformCom->Rotation_Degree(m_tParticleDesc.vRotation);
	}

	if (m_pModelCom)
	{
		m_pModelCom->Set_Desc(m_tParticleDesc.tPointInst_Desc);
		m_pModelCom->Update_ByDesc();
	}

}

void CToolParticle::Play()
{
	Init_Var();
	m_tParticleVar.bPlay = true;
}

void CToolParticle::Stop()
{
	m_tParticleVar.bPlay = false;
}

void CToolParticle::Init_Var()
{
	m_tParticleVar.fMultiTextureElapsedTime = 0.f;
	m_tParticleVar.iMultiTextureIndex = 0;
	m_tParticleVar.fLifeTime = 0.f;
	//m_tParticleVar.bPlay = false;

	dynamic_cast<CVIBuffer_PointInstance*>(m_pModelCom)->Init();
}

CToolParticle * CToolParticle::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CToolParticle* pGameInstance = new CToolParticle(pDevice, pDeviceContext);

	if (FAILED(pGameInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CToolParticle");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

CGameObject * CToolParticle::Clone(void * pArg)
{
	CToolParticle* pGameInstance = new CToolParticle(*this);

	if (FAILED(pGameInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Clone CToolParticle");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CToolParticle::Free()
{
	__super::Free();


	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	if (m_tParticleDesc.tParticleShaderDesc.vecShaderName.size() > 0)
	{
		for (_int i = 0; i < m_tParticleDesc.tParticleShaderDesc.vecShaderName.size(); ++i)
		{
			Safe_Delete(m_tParticleDesc.tParticleShaderDesc.vecShaderName[i]);
		}
		m_tParticleDesc.tParticleShaderDesc.vecShaderName.clear();
	}
	
}
