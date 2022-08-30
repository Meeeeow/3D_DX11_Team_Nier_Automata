#include "stdafx.h"
#include "ToolEffectMeshObject.h"
#include "VIBuffer_Pointbillboard.h"
#include "GameInstance.h"
#include "ToolDefines.h"
#include "MainFrm.h"
#include "ToolView.h"

void CToolEffectMeshObject::Stop()
{
	ZeroMemory(&m_tEffectMeshVariables, sizeof(EFFECTMESHVARIABLES));
	Init_PatternData();
	Init_Texture();
	Init_ShaderVariables();
	Init_FadeData();
}

CToolEffectMeshObject::CToolEffectMeshObject(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CToolEffectMeshObject::CToolEffectMeshObject(const CToolEffectMeshObject & rhs)
	: CGameObject(rhs)
{
}


HRESULT CToolEffectMeshObject::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CToolEffectMeshObject::NativeConstruct(void * pArg)
{
	EFFECTMESHDESC* pDesc = (EFFECTMESHDESC*)pArg;

	memcpy(&m_tDesc.tInfo, &pDesc->tInfo, sizeof(EFFECTMESHDESC::EFFECTMESHINFO));
	memcpy(&m_tDesc.tFade_InOut, &pDesc->tFade_InOut, sizeof(EFFECTMESHDESC::FADE_INOUT));
	memcpy(&m_tDesc.tPattern, &pDesc->tPattern, sizeof(EFFECTMESHDESC::EFFECTMESHPATTERN));
	memcpy(&m_tDesc.tTexture, &pDesc->tTexture, sizeof(EFFECTMESHDESC::EFFECTMESHTEXTURE));
	memcpy(&m_tDesc.tUV_Animation, &pDesc->tUV_Animation, sizeof(EFFECTMESHDESC::UV_ANIMATION));
	memcpy(&m_tDesc.tUV_Sprite, &pDesc->tUV_Sprite, sizeof(EFFECTMESHDESC::UV_SPRITE));

	for (_int i = 0; i < m_tDesc.tEffectMeshShader.vecShaderName.size(); ++i)
		Safe_Delete(m_tDesc.tEffectMeshShader.vecShaderName[i]);
	m_tDesc.tEffectMeshShader.vecShaderName.clear();

	for (_int i = 0; i < pDesc->tEffectMeshShader.vecShaderName.size(); ++i)
	{
		SHADERNAME* tShaderName = new SHADERNAME;
		_tcscpy_s(tShaderName->szName, pDesc->tEffectMeshShader.vecShaderName[i]->szName);
		m_tDesc.tEffectMeshShader.vecShaderName.emplace_back(tShaderName);
	}

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE::POSITION, XMVectorSet(m_tDesc.tInfo.vPosition.x, m_tDesc.tInfo.vPosition.y, m_tDesc.tInfo.vPosition.z, 1.f));
	m_pTransformCom->Set_Scale(XMVectorSet(m_tDesc.tInfo.vScale.x, m_tDesc.tInfo.vScale.y, m_tDesc.tInfo.vScale.z, 0.f));
	m_pTransformCom->Rotation_Degree(m_tDesc.tInfo.vRotation);

	return S_OK;
}

_int CToolEffectMeshObject::Tick(_double TimeDelta)
{
	if (m_eState == OBJSTATE::DEAD)
		return (_int)OBJSTATE::DEAD;

	const _float fTimeDelta = static_cast<_float>(TimeDelta);

	if (m_tEffectMeshVariables.bPlay)
	{
		m_tEffectMeshVariables.fLifeTime += fTimeDelta;




		if (m_tEffectMeshVariables.fLifeTime > m_tEffectMeshVariables.tCreateDeathVariables.vCreateDeathTime.y)
		{
			m_tEffectMeshVariables.bRenderOn = false;

			if (m_tEffectMeshVariables.tCreateDeathVariables.bCreateDeathRepeat)
			{
				m_tEffectMeshVariables.fLifeTime = 0.f;
			}
			else
			{
				m_tEffectMeshVariables.bPlay = false;
				Stop();
			}
			Init_Datas();
		}
		else if (m_tEffectMeshVariables.fLifeTime > m_tEffectMeshVariables.tCreateDeathVariables.vCreateDeathTime.x)
		{
			m_tEffectMeshVariables.bRenderOn = true;

			Update_TextureIndex(fTimeDelta);
			Pattern(fTimeDelta);
			Update_Fade(fTimeDelta);
		}
	}


	switch (m_tDesc.tInfo.iMeshKind)
	{
	case (_int)Client::EffectMeshDescription::MESH_KIND::IMAGELESS_MESH:
		break;
	case (_int)Client::EffectMeshDescription::MESH_KIND::MESH:
		break;
	case (_int)Client::EffectMeshDescription::MESH_KIND::RECT:
		if (m_tDesc.tTexture.bDefaultTexture && m_pDefaultTextureCom)
		{
			m_iPointbillboardPass = (_int)POINTBILLBOARD_PASS::BASE;
		}
		else
		{
			m_iPointbillboardPass = (_int)POINTBILLBOARD_PASS::EMPTY;
		}


		break;
	default:
		break;
	}


	if (m_tDesc.tUV_Animation.bUVAnimation)
	{
		m_tEffectMeshVariables.tUVAnimVar.vAnimValue.x += m_tDesc.tUV_Animation.vUV_Val.x * fTimeDelta;
		m_tEffectMeshVariables.tUVAnimVar.vAnimValue.y += m_tDesc.tUV_Animation.vUV_Val.y * fTimeDelta;

		if (m_tEffectMeshVariables.tUVAnimVar.vAnimValue.x > 1.f)
			m_tEffectMeshVariables.tUVAnimVar.vAnimValue.x = 0;
		if (m_tEffectMeshVariables.tUVAnimVar.vAnimValue.y > 1.f)
			m_tEffectMeshVariables.tUVAnimVar.vAnimValue.y = 0;

	}


	Compute_CamDistanceForAlpha(m_pTransformCom);
	return _int();
}

_int CToolEffectMeshObject::LateTick(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	if (m_tEffectMeshVariables.bPlay)
	{
		if (m_tEffectMeshVariables.bRenderOn)
		{
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::ALPHABLEND, this);
		}
		else
		{
			int a = 0;
		}
	}
	else
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::ALPHABLEND, this);
	}

	return _int();
}

HRESULT CToolEffectMeshObject::Render()
{
	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	switch (m_tDesc.tInfo.iMeshKind)
	{
	case (_int)Client::EffectMeshDescription::MESH_KIND::MESH:
	{
		if (nullptr == m_pModelCom)
			return E_FAIL;

		_uint		iNumMeshContainers = m_pModelCom->Get_NumMeshContainers();

		if (FAILED(m_pModelCom->Bind_Buffers()))
			return E_FAIL;

		for (_uint i = 0; i < iNumMeshContainers; ++i)
		{
			m_pModelCom->Set_MaterialTexture(i, aiTextureType_DIFFUSE, "g_texDiffuse");
			//m_pModelCom->Set_MaterialTexture(i, aiTextureType_NORMALS, "g_NormalTexture");
			m_pModelCom->Render(i, m_iModelPass);
		}
	}
		break;
	case (_int)Client::EffectMeshDescription::MESH_KIND::IMAGELESS_MESH:
		if (nullptr == m_pModelCom)
			return E_FAIL;
		{
			_uint		iNumMeshContainers = m_pModelCom->Get_NumMeshContainers();

			if (FAILED(m_pModelCom->Bind_Buffers()))
				return E_FAIL;

			for (_uint i = 0; i < iNumMeshContainers; ++i)
			{
				//m_pModelCom->Set_MaterialTexture(i, aiTextureType_DIFFUSE, "g_texDiffuse");
				//m_pModelCom->Set_MaterialTexture(i, aiTextureType_NORMALS, "g_NormalTexture");
				m_pModelCom->Render(i, m_iImagelessModelPass);
			}
		}
		break;
	case (_int)Client::EffectMeshDescription::MESH_KIND::RECT:
		//if (m_tEffectMeshVariables.tTextureVaribles.iDefaultTextureIndex != -1)
		//{
		//	m_pPointbillboardCom->Render(m_iPointbillboardPass);
		//}
		m_pPointbillboardCom->Render(m_iPointbillboardPass);


		break;
	}




	return S_OK;
}

HRESULT CToolEffectMeshObject::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, TEXT("Prototype_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, TEXT("Prototype_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Model*/
	switch (m_tDesc.tInfo.iMeshKind)
	{
	case (_int)Client::EffectMeshDescription::MESH_KIND::MESH:
		if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, m_tDesc.tInfo.szMeshPrototypeName, TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case (_int)Client::EffectMeshDescription::MESH_KIND::IMAGELESS_MESH:
		if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, m_tDesc.tInfo.szMeshPrototypeName, TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case (_int)Client::EffectMeshDescription::MESH_KIND::RECT:
		if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, TEXT("Prototype_Component_VIBuffer_Pointbillboard"), TEXT("Com_Model"), (CComponent**)&m_pPointbillboardCom)))
			return E_FAIL;
		break;
	default:
		break;
	}

	/* For.Com_DefaultTexture*/
	if (m_tDesc.tTexture.bDefaultTexture)
	{
		if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, m_tDesc.tTexture.szDefaultTexturePrototypeName, TEXT("Com_DefaultTexture"), (CComponent**)&m_pDefaultTextureCom)))
			return E_FAIL;
	}

	/* For.Com_AlphaOneTexture*/
	if (m_tDesc.tTexture.bAlphaOneTexture)
	{
		if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, m_tDesc.tTexture.szAlphaOneTexturePrototypeName, TEXT("Com_OneAlphaTexture"), (CComponent**)&m_pAlphaOneTextureCom)))
			return E_FAIL;
	}

	/* For.Com_MaskTexture*/
	if (m_tDesc.tTexture.bMaskTexture)
	{
		if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, m_tDesc.tTexture.szMaskTexturePrototypeName, TEXT("Com_MaskTexture"), (CComponent**)&m_pMaskTextureCom)))
			return E_FAIL;
	}
	/* For.Com_DissolveTexture*/
	
	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, TEXT("Prototype_Component_Texture_Dissolve"), TEXT("Com_DissolveTexture"), (CComponent**)&m_pDissolveTextureCom)))
		return E_FAIL;



	return S_OK;
}

HRESULT CToolEffectMeshObject::SetUp_ConstantTable()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	switch (m_tDesc.tInfo.iMeshKind)
	{
	case (_int)Client::EffectMeshDescription::MESH_KIND::MESH:
	{
		m_pModelCom->SetUp_RawValue("g_matWorld", &m_pTransformCom->Get_WorldMatrix4x4Transpose(), sizeof(_matrix));
		m_pModelCom->SetUp_RawValue("g_matView", &pGameInstance->Get_Transpose(CPipeLine::TRANSFORM::D3DTS_VIEW), sizeof(_matrix));
		m_pModelCom->SetUp_RawValue("g_matProj", &pGameInstance->Get_Transpose(CPipeLine::TRANSFORM::D3DTS_PROJ), sizeof(_matrix));

		//const LIGHTDESC*	pLightDesc = pGameInstance->Get_Light(L"LightDir", 0);

		//m_pModelCom->SetUp_RawValue("g_vLightDir", (void*)&pLightDesc->vDirection, sizeof(_float4));
		//m_pModelCom->SetUp_RawValue("g_vLightDiffuse", (void*)&pLightDesc->vDiffuse, sizeof(_float4));
		//m_pModelCom->SetUp_RawValue("g_vLightSpecular", (void*)&pLightDesc->vSpecular, sizeof(_float4));
		//m_pModelCom->SetUp_RawValue("g_vLightAmbient", (void*)&pLightDesc->vAmbient, sizeof(_float4));
		m_pModelCom->SetUp_RawValue("g_vCamPos", &pGameInstance->Get_CameraPosition(), sizeof(_float4));

		m_pModelCom->SetUp_RawValue("g_fDissolve", &m_tEffectMeshVariables.tShaderVariables.fDissolve, sizeof(_float));
		m_pModelCom->SetUp_Texture("g_texDissolve", m_pDissolveTextureCom->Get_SRV(0));

		break;
	}
	case (_int)Client::EffectMeshDescription::MESH_KIND::IMAGELESS_MESH:
	{
		m_pModelCom->SetUp_RawValue("g_matWorld", &m_pTransformCom->Get_WorldMatrix4x4Transpose(), sizeof(_matrix));
		m_pModelCom->SetUp_RawValue("g_matView", &pGameInstance->Get_Transpose(CPipeLine::TRANSFORM::D3DTS_VIEW), sizeof(_matrix));
		m_pModelCom->SetUp_RawValue("g_matProj", &pGameInstance->Get_Transpose(CPipeLine::TRANSFORM::D3DTS_PROJ), sizeof(_matrix));

		//const LIGHTDESC*	pLightDesc = pGameInstance->Get_Light(L"LightDir", 0);

		//m_pModelCom->SetUp_RawValue("g_vLightDir", (void*)&pLightDesc->vDirection, sizeof(_float4));
		//m_pModelCom->SetUp_RawValue("g_vLightDiffuse", (void*)&pLightDesc->vDiffuse, sizeof(_float4));
		//m_pModelCom->SetUp_RawValue("g_vLightSpecular", (void*)&pLightDesc->vSpecular, sizeof(_float4));
		//m_pModelCom->SetUp_RawValue("g_vLightAmbient", (void*)&pLightDesc->vAmbient, sizeof(_float4));
		m_pModelCom->SetUp_RawValue("g_vCamPos", &pGameInstance->Get_CameraPosition(), sizeof(_float4));

		_float3 fIsTexture = { 1.f, 1.f, 1.f };

		if (m_pDefaultTextureCom && m_tDesc.tTexture.bDefaultTexture)
			m_pModelCom->SetUp_Texture("g_texDefault", m_pDefaultTextureCom->Get_SRV(m_tEffectMeshVariables.tTextureVaribles.iDefaultTextureIndex));
		else fIsTexture.x = 0.f;

		if (m_pAlphaOneTextureCom && m_tDesc.tTexture.bAlphaOneTexture)
			m_pModelCom->SetUp_Texture("g_texAlphaOne", m_pAlphaOneTextureCom->Get_SRV(m_tEffectMeshVariables.tTextureVaribles.iAlphaOneTextureIndex));
		else fIsTexture.y = 0.f;

		if (m_pMaskTextureCom && m_tDesc.tTexture.bMaskTexture)
			m_pModelCom->SetUp_Texture("g_texMask", m_pMaskTextureCom->Get_SRV(m_tEffectMeshVariables.tTextureVaribles.iMaskTextureIndex));
		else fIsTexture.z = 0.f;

		m_pModelCom->SetUp_RawValue("g_fIsTexture", &fIsTexture, sizeof(_float3));

		_int bIsUVAnim = static_cast<_int>(m_tDesc.tUV_Animation.bUVAnimation);
		m_pModelCom->SetUp_RawValue("g_iIsUVAnimation", &bIsUVAnim, sizeof(_int));
		m_pModelCom->SetUp_RawValue("g_vUVAnimation", &m_tEffectMeshVariables.tUVAnimVar, sizeof(_float2));

		_int iBool = m_tDesc.tFade_InOut.bFadeIn;
		m_pModelCom->SetUp_RawValue("g_iIsFadeIn", &iBool, sizeof(_int));
		iBool = m_tDesc.tFade_InOut.bFadeOut;
		m_pModelCom->SetUp_RawValue("g_iIsFadeOut", &iBool, sizeof(_int));
		m_pModelCom->SetUp_RawValue("g_fFadeIn", &m_tEffectMeshVariables.tFadeInVariables.fCurrentFade, sizeof(_float));
		m_pModelCom->SetUp_RawValue("g_fFadeOut", &m_tEffectMeshVariables.tFadeOutVariables.fCurrentFade, sizeof(_float));
	}

		break;
	case (_int)Client::EffectMeshDescription::MESH_KIND::RECT:
	{
		//if (m_tEffectMeshVariables.tTextureVaribles.iDefaultTextureIndex == -1)
		//	break;

		m_pPointbillboardCom->SetUp_RawValue("g_matWorld", &m_pTransformCom->Get_WorldMatrix4x4Transpose(), sizeof(_matrix));
		m_pPointbillboardCom->SetUp_RawValue("g_matView", &pGameInstance->Get_Transpose(CPipeLine::TRANSFORM::D3DTS_VIEW), sizeof(_matrix));
		m_pPointbillboardCom->SetUp_RawValue("g_matProj", &pGameInstance->Get_Transpose(CPipeLine::TRANSFORM::D3DTS_PROJ), sizeof(_matrix));

		//const LIGHTDESC*	pLightDesc = pGameInstance->Get_Light(L"LightDir", 0);

		//m_pPointbillboardCom->SetUp_RawValue("g_vLightDir", (void*)&pLightDesc->vDirection, sizeof(_float4));
		//m_pPointbillboardCom->SetUp_RawValue("g_vLightDiffuse", (void*)&pLightDesc->vDiffuse, sizeof(_float4));
		//m_pPointbillboardCom->SetUp_RawValue("g_vLightSpecular", (void*)&pLightDesc->vSpecular, sizeof(_float4));
		//m_pPointbillboardCom->SetUp_RawValue("g_vLightAmbient", (void*)&pLightDesc->vAmbient, sizeof(_float4));
		m_pPointbillboardCom->SetUp_RawValue("g_vCamPos", &pGameInstance->Get_CameraPosition(), sizeof(_float4));

		_matrix ViewMatrix = pGameInstance->Get_Transform(CPipeLine::TRANSFORM::D3DTS_VIEW);
		_matrix ViewMatrixInverse = XMMatrixInverse(nullptr, ViewMatrix);
		_float4x4 vViewInv4x4 = { };
		XMStoreFloat4x4(&vViewInv4x4, ViewMatrixInverse);
		_vector vCamLook = {};
		memcpy(&vCamLook, &vViewInv4x4.m[2][0], sizeof(_float4));
		XMVectorSetW(vCamLook, 0.f);
		vCamLook = XMVector3Normalize(vCamLook);
		HRESULT res = m_pPointbillboardCom->SetUp_RawValue("g_vCamLook", &vCamLook, sizeof(_float4));
		
		_float3 fIsTexture = { 1.f, 1.f, 1.f };
		HRESULT hRes = S_OK;
		if (m_pDefaultTextureCom && m_tDesc.tTexture.bDefaultTexture)
			hRes = m_pPointbillboardCom->SetUp_Texture("g_texDefault", m_pDefaultTextureCom->Get_SRV(m_tEffectMeshVariables.tTextureVaribles.iDefaultTextureIndex));
		else fIsTexture.x = 0.f;

		if (m_pAlphaOneTextureCom && m_tDesc.tTexture.bAlphaOneTexture)
			m_pPointbillboardCom->SetUp_Texture("g_texAlphaOne", m_pAlphaOneTextureCom->Get_SRV(m_tEffectMeshVariables.tTextureVaribles.iAlphaOneTextureIndex));
		else fIsTexture.y = 0.f;

		if (m_pMaskTextureCom && m_tDesc.tTexture.bMaskTexture)
			m_pPointbillboardCom->SetUp_Texture("g_texMask", m_pMaskTextureCom->Get_SRV(m_tEffectMeshVariables.tTextureVaribles.iMaskTextureIndex));
		else fIsTexture.z = 0.f;

		m_pPointbillboardCom->SetUp_RawValue("g_fIsTexture", &fIsTexture, sizeof(_float3));

		_int iBool = m_tDesc.tFade_InOut.bFadeIn;
		m_pPointbillboardCom->SetUp_RawValue("g_iIsFadeIn", &iBool,  sizeof(_int));
		iBool = m_tDesc.tFade_InOut.bFadeOut;
		m_pPointbillboardCom->SetUp_RawValue("g_iIsFadeOut", &iBool, sizeof(_int));
		m_pPointbillboardCom->SetUp_RawValue("g_fFadeIn", &m_tEffectMeshVariables.tFadeInVariables.fCurrentFade, sizeof(_float));
		m_pPointbillboardCom->SetUp_RawValue("g_fFadeOut", &m_tEffectMeshVariables.tFadeOutVariables.fCurrentFade, sizeof(_float));

		_int bIsUVAnim = static_cast<_int>(m_tDesc.tUV_Animation.bUVAnimation);
		m_pPointbillboardCom->SetUp_RawValue("g_iIsUVAnimation", &bIsUVAnim, sizeof(_int));
		m_pPointbillboardCom->SetUp_RawValue("g_vUVAnimation", &m_tEffectMeshVariables.tUVAnimVar, sizeof(_float2));
	}
		break;
	default:
		break;
	}





	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CToolEffectMeshObject::Update_Desc(const EFFECTMESHDESC & _Desc)
{
	// Texture
	// If there is any change of use or not || Texrture, Update Texture 

	_tchar* szEmpty = L"";
	if ((_tcscmp(_Desc.tTexture.szDefaultTextureName, szEmpty)
		&& _tcscmp(m_tDesc.tTexture.szDefaultTextureName, _Desc.tTexture.szDefaultTextureName))
		|| !m_pDefaultTextureCom)
	{
		Apply_Texture((_int)EffectMeshDescription::TEXTURE_KIND::DEFAULT_TEX, _Desc.tTexture.szDefaultTexturePrototypeName);
	}

	if ((_tcscmp(m_tDesc.tTexture.szAlphaOneTextureName, _Desc.tTexture.szAlphaOneTextureName)
		&& _tcscmp(_Desc.tTexture.szAlphaOneTextureName, szEmpty))
		|| !m_pAlphaOneTextureCom)
	{
		Apply_Texture((_int)EffectMeshDescription::TEXTURE_KIND::ONEALPHA_TEX, _Desc.tTexture.szAlphaOneTexturePrototypeName);
	}
	
	if ((_tcscmp(m_tDesc.tTexture.szMaskTextureName, _Desc.tTexture.szMaskTextureName)
		&& _tcscmp(_Desc.tTexture.szMaskTextureName, szEmpty))
		|| !m_pMaskTextureCom)
	{
		Apply_Texture((_int)EffectMeshDescription::TEXTURE_KIND::MASK_TEX, _Desc.tTexture.szMaskTexturePrototypeName);
	}

	// Update Transform
	m_pTransformCom->Set_State(CTransform::STATE::POSITION, XMVectorSet(_Desc.tInfo.vPosition.x, _Desc.tInfo.vPosition.y, _Desc.tInfo.vPosition.z, 1.f));
	m_pTransformCom->Set_Scale(XMVectorSet(_Desc.tInfo.vScale.x, _Desc.tInfo.vScale.y, _Desc.tInfo.vScale.z, 0.f));
	m_pTransformCom->Rotation_Degree(_Desc.tInfo.vRotation);

	memcpy(&m_tDesc.tInfo, &_Desc.tInfo, sizeof(EFFECTMESHDESC::EFFECTMESHINFO));
	memcpy(&m_tDesc.tFade_InOut, &_Desc.tFade_InOut, sizeof(EFFECTMESHDESC::FADE_INOUT));
	memcpy(&m_tDesc.tPattern, &_Desc.tPattern, sizeof(EFFECTMESHDESC::EFFECTMESHPATTERN));
	memcpy(&m_tDesc.tTexture, &_Desc.tTexture, sizeof(EFFECTMESHDESC::EFFECTMESHTEXTURE));
	memcpy(&m_tDesc.tUV_Animation, &_Desc.tUV_Animation, sizeof(EFFECTMESHDESC::UV_ANIMATION));
	memcpy(&m_tDesc.tUV_Sprite, &_Desc.tUV_Sprite, sizeof(EFFECTMESHDESC::UV_SPRITE));

	for (auto& pIter : m_tDesc.tEffectMeshShader.vecShaderName)
		Safe_Delete(pIter);
	m_tDesc.tEffectMeshShader.vecShaderName.clear();

	for (_int i = 0; i < _Desc.tEffectMeshShader.vecShaderName.size(); ++i)
	{
		SHADERNAME* tShaderName = new SHADERNAME;
		m_tDesc.tEffectMeshShader.vecShaderName.emplace_back(tShaderName);
		_tcscpy_s(tShaderName->szName, _Desc.tEffectMeshShader.vecShaderName[i]->szName);
	}

	//GET_TOOLVIEW->Invalidate();
}

HRESULT CToolEffectMeshObject::Apply_Texture(_int iKind, const _tchar* _szProto)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	switch (iKind)
	{
	case (_int)Client::EffectMeshDescription::TEXTURE_KIND::DEFAULT_TEX:
		if (m_pDefaultTextureCom)
			Safe_Release(m_pDefaultTextureCom);
		//if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, _szProto, TEXT("Com_DefaultTexture"), (CComponent**)&m_pDefaultTextureCom)))
		//	return E_FAIL;

		m_pDefaultTextureCom = static_cast<CTexture*>(pGameInstance->Clone_Component((_uint)LEVEL::STATIC, _szProto));
		if (!m_pDefaultTextureCom) return E_FAIL;
		Safe_AddRef(m_pDefaultTextureCom);

		break;
	case (_int)Client::EffectMeshDescription::TEXTURE_KIND::ONEALPHA_TEX:
		if (m_pAlphaOneTextureCom)
			Safe_Release(m_pAlphaOneTextureCom);
		//if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, _szProto, TEXT("Com_OneAlphaTexture"), (CComponent**)&m_pOneAlphaTextureCom)))
		//	return E_FAIL;
		m_pAlphaOneTextureCom = static_cast<CTexture*>(pGameInstance->Clone_Component((_uint)LEVEL::STATIC, _szProto));
		if (!m_pAlphaOneTextureCom) return E_FAIL;
		Safe_AddRef(m_pAlphaOneTextureCom);

		break;
	case (_int)Client::EffectMeshDescription::TEXTURE_KIND::MASK_TEX:
		if (m_pMaskTextureCom)
			Safe_Release(m_pMaskTextureCom);
		//if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, _szProto, TEXT("Com_MaskTexture"), (CComponent**)&m_pMaskTextureCom)))
		//	return E_FAIL;
		m_pMaskTextureCom = static_cast<CTexture*>(pGameInstance->Clone_Component((_uint)LEVEL::STATIC, _szProto));
		if (!m_pMaskTextureCom) return E_FAIL;
		Safe_AddRef(m_pMaskTextureCom);
		break;
	default:
		break;
	}
	
	return S_OK;
}

//void CToolEffectMeshObject::Ready_Play()
//{
//	// Caclulate Pattern 
//	// Calulate Position Speed
//
//	
//
//
//
//}

void CToolEffectMeshObject::Init_Datas()
{
	//ZeroMemory(&m_tEffectMeshVariables, sizeof(EFFECTMESHVARIABLES));
	Init_PatternData();
	Init_Texture();
	Init_ShaderVariables();
	Init_FadeData();
}

void CToolEffectMeshObject::Init_PatternData()
{
	Init_PositionPatternData();
	Init_RotationPatternData();
	Init_ScalePatternData();
}

void CToolEffectMeshObject::Init_PositionPatternData()
{
	m_tEffectMeshVariables.tPatternVariables.fPattern_Position_LifeTime = m_tEffectMeshVariables.fLifeTime;

	// Position
	//_vector vPosition = XMLoadFloat3(&m_tDesc.tPattern.vPattern_Position_Init);
	_vector vPosition = XMLoadFloat3(&m_tDesc.tInfo.vPosition);
	vPosition = XMVectorSetW(vPosition, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE::POSITION, vPosition);

	_float fPositionTimeLength = m_tDesc.tPattern.vPattern_Position_StartEndTime.y - m_tDesc.tPattern.vPattern_Position_StartEndTime.x;
	if (fPositionTimeLength > 0.f)
	{
		_vector vCurPos = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
		_vector vGoalPos = XMLoadFloat3(&m_tDesc.tPattern.vPattern_Position_Goal);
		XMVectorSetW(vGoalPos, 1.f);

		_float fLength = XMVectorGetX(XMVector3Length(vCurPos - vGoalPos));
		m_tDesc.tPattern.fPositionSpeed = fLength / fPositionTimeLength;
		_vector vDir = vGoalPos - vCurPos;
		XMStoreFloat3(&m_tDesc.tPattern.vPositionDirection, vDir);
	}

	//if (m_tDesc.tPattern.bPattern_Position_Projectile)
	//{
	//	_float4 vStartPos = { m_tDesc.tInfo.vPosition.x, m_tDesc.tInfo.vPosition.y, m_tDesc.tInfo.vPosition.z, 1.f };
	//	_float4 vTargetPos = { m_tDesc.tPattern.vPattern_Position_Goal.x, m_tDesc.tPattern.vPattern_Position_Goal.y, m_tDesc.tPattern.vPattern_Position_Goal.z, 1.f };
	//	m_tDesc.tPattern.vPattern_Position_Projectile_Power = m_pTransformCom->Get_ProjectilePowerToTarget(vStartPos, vTargetPos
	//		, m_tDesc.tPattern.vPattern_Position_StartEndTime.y - m_tDesc.tPattern.vPattern_Position_StartEndTime.x
	//		, m_tDesc.tPattern.fPattern_Position_Projectile_Angle_Result);
	//}
}

void CToolEffectMeshObject::Init_RotationPatternData()
{
	m_tEffectMeshVariables.tPatternVariables.fPattern_Rotation_LifeTime = m_tEffectMeshVariables.fLifeTime;


	// Rotation
	m_pTransformCom->Rotation_Degree(m_tDesc.tInfo.vRotation);

	ZeroMemory(&m_tDesc.tPattern.vRotationSpeed, sizeof(_float3));
	ZeroMemory(&m_tEffectMeshVariables.tPatternVariables.vPattern_Rotation_Current, sizeof(_float3));
	m_tEffectMeshVariables.tPatternVariables.vPattern_Rotation_Current = m_tDesc.tInfo.vRotation;

	_float fRotationTimeLength = m_tDesc.tPattern.vPattern_Rotation_StartEndTime.y - m_tDesc.tPattern.vPattern_Rotation_StartEndTime.x;
	if (fRotationTimeLength > 0.f)
	{
		//_vector vCurRot = XMVectorSet(m_tDesc.tInfo.vRotation.x, m_tDesc.tInfo.vRotation.y, m_tDesc.tInfo.vRotation.z, 0.f);
		//_vector vGoalRot = XMLoadFloat3(&m_tDesc.tPattern.vPattern_Rotation_Goal);
		//XMVectorSetW(vGoalRot, 1.f);

		_float fLengthX = static_cast<_float>((m_tDesc.tPattern.vPattern_Rotation_Goal.x - m_tDesc.tInfo.vRotation.x));
		m_tDesc.tPattern.vRotationSpeed.x = fLengthX / fRotationTimeLength;
		_float fLengthY = static_cast<_float>((m_tDesc.tPattern.vPattern_Rotation_Goal.y - m_tDesc.tInfo.vRotation.y));
		m_tDesc.tPattern.vRotationSpeed.y = fLengthY / fRotationTimeLength;
		_float fLengthZ = static_cast<_float>((m_tDesc.tPattern.vPattern_Rotation_Goal.z - m_tDesc.tInfo.vRotation.z));
		m_tDesc.tPattern.vRotationSpeed.z = fLengthZ / fRotationTimeLength;
	}

}

void CToolEffectMeshObject::Init_ScalePatternData()
{
	m_tEffectMeshVariables.tPatternVariables.fPattern_Scale_LifeTime = m_tEffectMeshVariables.fLifeTime;


	// Scale
	_vector vScale = XMLoadFloat3(&m_tDesc.tInfo.vScale);
	vScale = XMVectorSetW(vScale, 0.f);

	m_pTransformCom->Set_Scale(vScale);

	ZeroMemory(&m_tDesc.tPattern.vScaleSpeed, sizeof(_float3));
	ZeroMemory(&m_tEffectMeshVariables.tPatternVariables.vPattern_Scale_Current, sizeof(_float3));
	m_tEffectMeshVariables.tPatternVariables.vPattern_Scale_Current = m_tDesc.tInfo.vScale;
	_float fScaleTimeLength = m_tDesc.tPattern.vPattern_Scale_StartEndTime.y - m_tDesc.tPattern.vPattern_Scale_StartEndTime.x;
	if (fScaleTimeLength > 0.f)
	{
		_float fLengthX = static_cast<_float>((m_tDesc.tPattern.vPattern_Scale_Goal.x - m_tDesc.tInfo.vScale.x));
		m_tDesc.tPattern.vScaleSpeed.x = fLengthX / fScaleTimeLength;
		_float fLengthY = static_cast<_float>((m_tDesc.tPattern.vPattern_Scale_Goal.y - m_tDesc.tInfo.vScale.y));
		m_tDesc.tPattern.vScaleSpeed.y = fLengthY / fScaleTimeLength;
		_float fLengthZ = static_cast<_float>((m_tDesc.tPattern.vPattern_Scale_Goal.z - m_tDesc.tInfo.vScale.z));
		m_tDesc.tPattern.vScaleSpeed.z = fLengthZ / fScaleTimeLength;
	}
}

void CToolEffectMeshObject::Pattern(_float _fTimeDelta)
{
	if (!m_tEffectMeshVariables.bPlay)
		return;
	
	Pattern_Position(_fTimeDelta);
	Pattern_Rotation(_fTimeDelta);
	Pattern_Scale(_fTimeDelta);

	Update_ShaderVariables(_fTimeDelta);


}

void CToolEffectMeshObject::Pattern_Position(_float _fTimeDelta)
{
	// CHECK time is right
	if (m_tDesc.tPattern.vPattern_Position_StartEndTime.x < m_tDesc.tPattern.vPattern_Position_StartEndTime.y)
	{
		// CHECK it started
		if (m_tDesc.tPattern.vPattern_Position_StartEndTime.x + m_tEffectMeshVariables.tPatternVariables.fPattern_Position_LifeTime < m_tEffectMeshVariables.fLifeTime)
		{
			// CHECK it is NOT end
			if (m_tDesc.tPattern.vPattern_Position_StartEndTime.y + m_tEffectMeshVariables.tPatternVariables.fPattern_Position_LifeTime > m_tEffectMeshVariables.fLifeTime)
			{
				if (m_tDesc.tPattern.bPattern_Position_Projectile)
				{
					_float4 vStartPos = { m_tDesc.tInfo.vPosition.x, m_tDesc.tInfo.vPosition.y, m_tDesc.tInfo.vPosition.z, 1.f };
					m_pTransformCom->Projectile(vStartPos, m_tDesc.tPattern.vPattern_Position_Projectile_Power
						, m_tEffectMeshVariables.fLifeTime - m_tEffectMeshVariables.tPatternVariables.fPattern_Position_LifeTime, m_tDesc.tPattern.fPattern_Position_Projectile_Angle_Result);
				}
				else
				{
					_vector	vPosition = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
					_vector vLook = XMLoadFloat3(&m_tDesc.tPattern.vPositionDirection);
					vPosition += XMVector3Normalize(vLook) * m_tDesc.tPattern.fPositionSpeed * _fTimeDelta;
					m_pTransformCom->Set_State(CTransform::STATE::POSITION, vPosition);
				}




			}
			else if (m_tDesc.tPattern.bPattern_Position)
			{
				Init_PositionPatternData();
			}
		}

	}
}

void CToolEffectMeshObject::Pattern_Rotation(_float _fTimeDelta)
{
	// CHECK time is right
	if (m_tDesc.tPattern.vPattern_Rotation_StartEndTime.x < m_tDesc.tPattern.vPattern_Rotation_StartEndTime.y)
	{
		if (m_tDesc.tPattern.vPattern_Rotation_StartEndTime.x + m_tEffectMeshVariables.tPatternVariables.fPattern_Rotation_LifeTime < m_tEffectMeshVariables.fLifeTime)
		{
			// CHECK it is NOT end
			if (m_tDesc.tPattern.vPattern_Rotation_StartEndTime.y + m_tEffectMeshVariables.tPatternVariables.fPattern_Rotation_LifeTime > m_tEffectMeshVariables.fLifeTime)
			{
				m_tEffectMeshVariables.tPatternVariables.vPattern_Rotation_Current.x += m_tDesc.tPattern.vRotationSpeed.x * _float(_fTimeDelta);
				m_tEffectMeshVariables.tPatternVariables.vPattern_Rotation_Current.y += m_tDesc.tPattern.vRotationSpeed.y * _float(_fTimeDelta);
				m_tEffectMeshVariables.tPatternVariables.vPattern_Rotation_Current.z += m_tDesc.tPattern.vRotationSpeed.z * _float(_fTimeDelta);
				m_pTransformCom->Rotation_Degree(m_tEffectMeshVariables.tPatternVariables.vPattern_Rotation_Current);
			}
			else if (m_tDesc.tPattern.bPattern_Rotation)
			{
				Init_RotationPatternData();
			}
		}




	}
}

void CToolEffectMeshObject::Pattern_Scale(_float _fTimeDelta)
{
	// CHECK time is right
	if (m_tDesc.tPattern.vPattern_Scale_StartEndTime.x < m_tDesc.tPattern.vPattern_Scale_StartEndTime.y)
	{
		// CHECK it started
		if (m_tDesc.tPattern.vPattern_Scale_StartEndTime.x + m_tEffectMeshVariables.tPatternVariables.fPattern_Scale_LifeTime < m_tEffectMeshVariables.fLifeTime)
		{
			// CHECK it is NOT end
			if (m_tDesc.tPattern.vPattern_Scale_StartEndTime.y + m_tEffectMeshVariables.tPatternVariables.fPattern_Scale_LifeTime > m_tEffectMeshVariables.fLifeTime)
			{
				m_tEffectMeshVariables.tPatternVariables.vPattern_Scale_Current.x += m_tDesc.tPattern.vScaleSpeed.x * _float(_fTimeDelta);
				m_tEffectMeshVariables.tPatternVariables.vPattern_Scale_Current.y += m_tDesc.tPattern.vScaleSpeed.y * _float(_fTimeDelta);
				m_tEffectMeshVariables.tPatternVariables.vPattern_Scale_Current.z += m_tDesc.tPattern.vScaleSpeed.z * _float(_fTimeDelta);
				m_pTransformCom->Set_Scale(XMVectorSet(m_tEffectMeshVariables.tPatternVariables.vPattern_Scale_Current.x, m_tEffectMeshVariables.tPatternVariables.vPattern_Scale_Current.y, m_tEffectMeshVariables.tPatternVariables.vPattern_Scale_Current.z, 0.f));
			}
			else if (m_tDesc.tPattern.bPattern_Scale)
			{
				Init_ScalePatternData();
			}
		}
	}
}

void CToolEffectMeshObject::Update_TextureIndex(_float _fTimeDelta)
{
	if (m_tDesc.tTexture.iDefaultTextureNumber > 1
//		&& m_tEffectMeshVariables.tTextureVaribles.iDefaultTextureIndex != -1
		)
	{
		m_tEffectMeshVariables.tTextureVaribles.fDefaultTextureElpasedTime += static_cast<_float>(_fTimeDelta);
		if (m_tEffectMeshVariables.tTextureVaribles.fDefaultTextureElpasedTime > m_tDesc.tTexture.fDefaultTextureInterval)
		{
			m_tEffectMeshVariables.tTextureVaribles.fDefaultTextureElpasedTime = 0.f;
			if (++m_tEffectMeshVariables.tTextureVaribles.iDefaultTextureIndex > m_tDesc.tTexture.iDefaultTextureNumber - 1)
			{
				if (m_tDesc.tTexture.bDefaultTextureSpriteRepeat)
				{
					m_tEffectMeshVariables.tTextureVaribles.iDefaultTextureIndex = 0;
				}
				else
				{
					//m_tEffectMeshVariables.tTextureVaribles.iDefaultTextureIndex = -1;
					--m_tEffectMeshVariables.tTextureVaribles.iDefaultTextureIndex;
				}
			}
		}
	}


	if (m_tDesc.tTexture.iAlphaOneTextureNumber > 1
		)
	{
		m_tEffectMeshVariables.tTextureVaribles.fAlphaOneTextureElpasedTime += static_cast<_float>(_fTimeDelta);
		if (m_tEffectMeshVariables.tTextureVaribles.fAlphaOneTextureElpasedTime > m_tDesc.tTexture.fAlphaOneTextureInterval)
		{
			m_tEffectMeshVariables.tTextureVaribles.fAlphaOneTextureElpasedTime = 0.f;
			if (++m_tEffectMeshVariables.tTextureVaribles.iAlphaOneTextureIndex > m_tDesc.tTexture.iAlphaOneTextureNumber - 1)
			{
				if (m_tDesc.tTexture.bAlphaOneTextureSpriteRepeat)
				{
					m_tEffectMeshVariables.tTextureVaribles.iAlphaOneTextureIndex = 0;
				}
				else
				{
					--m_tEffectMeshVariables.tTextureVaribles.iAlphaOneTextureIndex;
				}
			}
		}
	}

	if (m_tDesc.tTexture.iMaskTextureNumber > 1
		)
	{
		m_tEffectMeshVariables.tTextureVaribles.fMaskTextureElpasedTime += static_cast<_float>(_fTimeDelta);
		if (m_tEffectMeshVariables.tTextureVaribles.fMaskTextureElpasedTime > m_tDesc.tTexture.fAlphaOneTextureInterval)
		{
			m_tEffectMeshVariables.tTextureVaribles.fMaskTextureElpasedTime = 0.f;
			if (++m_tEffectMeshVariables.tTextureVaribles.iMaskTextureIndex > m_tDesc.tTexture.iMaskTextureNumber - 1)
			{
				if (m_tDesc.tTexture.bMaskTextureSpriteRepeat)
				{
					m_tEffectMeshVariables.tTextureVaribles.iMaskTextureIndex = 0;
				}
				else
				{
					--m_tEffectMeshVariables.tTextureVaribles.iMaskTextureIndex;
				}
			}
		}
	}
}

void CToolEffectMeshObject::Init_Texture()
{
	m_tEffectMeshVariables.tTextureVaribles.fDefaultTextureElpasedTime = 0.f;
	m_tEffectMeshVariables.tTextureVaribles.iDefaultTextureIndex = 0;
	m_tEffectMeshVariables.tTextureVaribles.fAlphaOneTextureElpasedTime = 0.f;
	m_tEffectMeshVariables.tTextureVaribles.iAlphaOneTextureIndex = 0;
	m_tEffectMeshVariables.tTextureVaribles.fMaskTextureElpasedTime = 0.f;
	m_tEffectMeshVariables.tTextureVaribles.iMaskTextureIndex = 0;
}

void CToolEffectMeshObject::Update_ShaderVariables(_float _fTimeDelta)
{
	for (size_t i = 0; i < m_tDesc.tEffectMeshShader.vecShaderName.size(); i++)
	{
		if (!_tcscmp(L"Dissolve", m_tDesc.tEffectMeshShader.vecShaderName[i]->szName))
		{
			Update_DissolveShader(_fTimeDelta);
		}
	}
}

void CToolEffectMeshObject::Update_DissolveShader(_float _fTimeDelta)
{
	// When Pattern_Position Ended
	if (m_tDesc.tPattern.vPattern_Position_StartEndTime.y + m_tEffectMeshVariables.tPatternVariables.fPattern_Position_LifeTime + 0.5f < m_tEffectMeshVariables.fLifeTime)
	{
		switch (m_tDesc.tInfo.iMeshKind)
		{
		case (_int)Client::EffectMeshDescription::MESH_KIND::IMAGELESS_MESH:
			break;
		case (_int)Client::EffectMeshDescription::MESH_KIND::MESH:
			m_iModelPass = (_int)MODEL_PASS::IMAGE_DISSOLVE;
			m_tEffectMeshVariables.tShaderVariables.fDissolve += m_tDesc.tEffectMeshShader.fDissolveSpeed * _fTimeDelta;
			break;
		case (_int)Client::EffectMeshDescription::MESH_KIND::RECT:
			break;
		default:
			break;
		}
	}
}

void CToolEffectMeshObject::Init_ShaderVariables()
{
	m_tEffectMeshVariables.tShaderVariables.fDissolve = 0.f;
	
	m_tDesc.tEffectMeshShader.fDissolveSpeed = (m_tEffectMeshVariables.tCreateDeathVariables.vCreateDeathTime.y - m_tDesc.tPattern.vPattern_Position_StartEndTime.y);
	m_tDesc.tEffectMeshShader.fDissolveSpeed = 1.f / m_tDesc.tEffectMeshShader.fDissolveSpeed;

	m_iModelPass = (_int)MODEL_PASS::IMAGE;
	m_iImagelessModelPass = (_int)MODEL_PASS::IMAGELESS;
	m_iPointbillboardPass = (_int)POINTBILLBOARD_PASS::BASE;
}

void CToolEffectMeshObject::Init_FadeData()
{
	_float fFadeInSub = m_tDesc.tFade_InOut.vFadeIn_StartEndTime.y - m_tDesc.tFade_InOut.vFadeIn_StartEndTime.x;
	if (fFadeInSub < 0)
		assert(false);

	m_tEffectMeshVariables.tFadeInVariables.fFadeVal = 1.f / fFadeInSub;
	m_tEffectMeshVariables.tFadeInVariables.fCurrentFade = 0.f;

	_float fFadeOutSub = m_tDesc.tFade_InOut.vFadeOut_StartEndTime.y - m_tDesc.tFade_InOut.vFadeOut_StartEndTime.x;
	if (fFadeOutSub < 0)
		assert(false);

	m_tEffectMeshVariables.tFadeOutVariables.fFadeVal = 1.f / fFadeOutSub;
	m_tEffectMeshVariables.tFadeOutVariables.fCurrentFade = 1.f;

}

void CToolEffectMeshObject::Update_Fade(_float _fTimeDelta)
{
	if (m_tDesc.tFade_InOut.bFadeIn)
		Update_FadeIn(_fTimeDelta);
	if (m_tDesc.tFade_InOut.bFadeOut)
		Update_FadeOut(_fTimeDelta);
}

void CToolEffectMeshObject::Update_FadeIn(_float fTimeDelta)
{
	// if it already ended
	if (m_tEffectMeshVariables.fLifeTime > m_tDesc.tFade_InOut.vFadeIn_StartEndTime.y) return;
	if (m_tEffectMeshVariables.fLifeTime > m_tDesc.tFade_InOut.vFadeIn_StartEndTime.x)
	{
		m_tEffectMeshVariables.tFadeInVariables.fCurrentFade += m_tEffectMeshVariables.tFadeInVariables.fFadeVal * fTimeDelta;
	}
}

void CToolEffectMeshObject::Update_FadeOut(_float fTimeDelta)
{
	// if it already ended
	if (m_tEffectMeshVariables.fLifeTime > m_tDesc.tFade_InOut.vFadeOut_StartEndTime.y) return;
	if (m_tEffectMeshVariables.fLifeTime > m_tDesc.tFade_InOut.vFadeOut_StartEndTime.x)
	{
		m_tEffectMeshVariables.tFadeOutVariables.fCurrentFade -= m_tEffectMeshVariables.tFadeOutVariables.fFadeVal * fTimeDelta;
	}
}

_float4 CToolEffectMeshObject::Get_ProjectilePowerToTarget(_float4 vStartPos, _float4 vTargetPos, _float fProjectileMaxTime, _float fDegree)
{
	return m_pTransformCom->Get_ProjectilePowerToTarget(vStartPos, vTargetPos, fProjectileMaxTime, fDegree);
}

CToolEffectMeshObject * CToolEffectMeshObject::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CToolEffectMeshObject* pGameInstance = new CToolEffectMeshObject(pDevice, pDeviceContext);

	if (FAILED(pGameInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CToolEffectMeshObject");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

CGameObject * CToolEffectMeshObject::Clone(void * pArg)
{
	CToolEffectMeshObject* pGameInstance = new CToolEffectMeshObject(*this);

	if (FAILED(pGameInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Clone CToolEffectMeshObject");
		Safe_Release(pGameInstance);
	}
	
	return pGameInstance;
}

void CToolEffectMeshObject::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);

	Safe_Release(m_pDefaultTextureCom);
	Safe_Release(m_pAlphaOneTextureCom);
	Safe_Release(m_pMaskTextureCom);
	Safe_Release(m_pDissolveTextureCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pPointbillboardCom);

	_int iCount = m_tDesc.tEffectMeshShader.vecShaderName.size();
	for (_int i = 0; i < iCount; ++i) {
		Safe_Delete(m_tDesc.tEffectMeshShader.vecShaderName[i]);
	}
	m_tDesc.tEffectMeshShader.vecShaderName.clear();

}

void CToolEffectMeshObject::Play(_float2 _vCreateDeathTime, _bool _bCreateDeathRepeat)
{
	m_tEffectMeshVariables.bPlay = true;
	m_tEffectMeshVariables.tCreateDeathVariables.vCreateDeathTime = _vCreateDeathTime;
	m_tEffectMeshVariables.tCreateDeathVariables.bCreateDeathRepeat = _bCreateDeathRepeat;

	Init_Datas();
}
