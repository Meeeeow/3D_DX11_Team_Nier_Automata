#include "stdafx.h"
#include "ToolObject.h"
#include "GameInstance.h"
#include "Model.h"

CToolObject::CToolObject(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CToolObject::CToolObject(const CToolObject & rhs)
	: CGameObject(rhs)
{
}


HRESULT CToolObject::NativeConstruct_Prototype()
{
	ZeroMemory(m_szProtoModel, sizeof(MAX_PATH));

	return S_OK;
}

HRESULT CToolObject::NativeConstruct(void * pArg)
{
	if (nullptr != pArg)
		lstrcpy(m_szProtoModel, (const _tchar*)pArg);

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pModelCom->Set_Animation_Tool(m_iAnimationIndex);

	ZeroMemory(m_szCurBoneName, MAX_PATH * sizeof(_tchar));

	return S_OK;
}

_int CToolObject::Tick(_double TimeDelta)
{

	m_pModelCom->Set_Animation_Tool(m_iAnimationIndex);
	m_pModelCom->Synchronize_Animation(m_pTransformCom);

	return _int();
}

_int CToolObject::LateTick(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	if (!m_bStop)
		m_pModelCom->Update_CombinedTransformationMatrixNonContinue_Tool(TimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::ALPHATEST, this);

	if (lstrlen(m_szCurBoneName))
		m_pSphereCom->Update(m_pModelCom->Get_BoneMatrix(m_szCurBoneName) * m_pTransformCom->Get_WorldMatrix());

	if (m_bCollRender)
		m_pRendererCom->Add_DebugGroup(CRenderer::DEBUGGROUP::COLLIDER, m_pSphereCom);

	return _int();
}

HRESULT CToolObject::Render()
{
	if (true == m_bRenderStop)
		return S_OK;

	if (nullptr == m_pModelCom)
		return E_FAIL;


	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	_uint		iNumMeshContainers = m_pModelCom->Get_NumMeshContainers();

	if (FAILED(m_pModelCom->Bind_Buffers()))
		return E_FAIL;

	for (_uint i = 0; i < iNumMeshContainers; ++i)
	{
		m_pModelCom->Set_MaterialTexture(i, aiTextureType_DIFFUSE, "g_texDiffuse");
		m_pModelCom->Render(i,1);
	}

	return S_OK;
}

void CToolObject::Set_RootMotionValue(_double dValue)
{
	if (nullptr == m_pModelCom)
		return;

	m_pModelCom->Set_RootMotionValue(dValue);
}

_int CToolObject::Find_Animation_Initialize(const char* pTag)
{
	if (nullptr == m_pModelCom)
		return -1;

	_int iIndex = m_pModelCom->Find_AnimationIndex(pTag);
	if (-1 == iIndex)
		return -1;

	m_pModelCom->Initialize_Time();

	m_iAnimationIndex = iIndex;
	return m_iAnimationIndex;
}

_int CToolObject::Find_Animation(const char* pTag)
{
	if (nullptr == m_pModelCom)
		return -1;

	_int iIndex = m_pModelCom->Find_AnimationIndex(pTag);
	if (-1 == iIndex)
		return -1;

	m_iAnimationIndex = iIndex;
	return m_iAnimationIndex;
}

void CToolObject::Initialize_Time()
{
	if (nullptr == m_pModelCom)
		return;

	m_pModelCom->Initialize_Time();
}

const _double CToolObject::Get_PlayTime() const
{
	if (nullptr == m_pModelCom)
		return -1.0;

	return m_pModelCom->Get_PlayTime();
}

const _double CToolObject::Get_Duration() const
{
	if (nullptr == m_pModelCom)
		return -1.0;

	return m_pModelCom->Get_Duration();
}

const _double CToolObject::Get_PlaySpeedPerSec() const
{
	if (nullptr == m_pModelCom)
		return -1.0;

	return m_pModelCom->Get_PlaySpeedPerSec();
}

const _double CToolObject::Get_LinkMinTime(_uint iIndex) const
{
	if (nullptr == m_pModelCom)
		return -1.0;

	return m_pModelCom->Get_LinkMinRatio(iIndex);
}

const _double CToolObject::Get_LinkMaxTime(_uint iIndex) const
{
	if (nullptr == m_pModelCom)
		return -1.0;
	
	return m_pModelCom->Get_LinkMaxRatio(iIndex);
}

const _double CToolObject::Get_BlendMaxTime(_uint iIndex) const
{
	if (nullptr == m_pModelCom)
		return -1.0;

	return m_pModelCom->Get_BlendMaxTime(iIndex);
}

const _double CToolObject::Get_BlendLevel(_uint iIndex) const
{
	if (nullptr == m_pModelCom)
		return -1.0;

	return m_pModelCom->Get_BlendLevel(iIndex);
}

const _bool CToolObject::Get_NoneUpdatePos(_uint iIndex) const
{
	if (nullptr == m_pModelCom)
		return false;

	return m_pModelCom->Get_NoneUpdatePos(iIndex);
}

const DIRECTION CToolObject::Get_Direction(_uint iIndex) const
{
	if (nullptr == m_pModelCom)
		return DIRECTION::NONE;

	return m_pModelCom->Get_Direction(iIndex);
}

const _uint CToolObject::Get_RootBone_CurrentKeyFrameIndex(_uint iIndex) const
{
	if (nullptr == m_pModelCom)
		return 0;

	return m_pModelCom->Get_RootBone_CurrentKeyFrameIndex(iIndex);
}

const _float4 CToolObject::Get_BonePosition(_uint iAnimationIndex, const char* pTag) const
{
	if (nullptr == m_pModelCom)
		return _float4(-1.f, -1.f, -1.f, 0.f);

	return m_pModelCom->Get_BonePosition(iAnimationIndex, pTag);
}

const _double CToolObject::Get_RootMotionValue(_uint iAnimationIndex) const
{
	return m_pModelCom->Get_RootMotionValue(iAnimationIndex);
}

const _double CToolObject::Get_RootMotionValue() const
{
	if (nullptr == m_pModelCom)
		return 0.0;

	return m_pModelCom->Get_RootMotionValue();
}

const _double CToolObject::Get_CollisionMinRatio() const
{
	if (nullptr == m_pModelCom)
		return -1.0;

	return m_pModelCom->Get_CollisionMinRatio();
}

const _double CToolObject::Get_CollisionMaxRatio() const
{
	if (nullptr == m_pModelCom)
		return -1.0;

	return m_pModelCom->Get_CollisionMaxRatio();
}

const _double CToolObject::Get_CollisionMinRatio(_uint iIndex) const
{
	if (nullptr == m_pModelCom)
		return -1.0;

	return m_pModelCom->Get_CollisionMinRatio(iIndex);
}

const _double CToolObject::Get_CollisionMaxRatio(_uint iIndex) const
{
	if (nullptr == m_pModelCom)
		return -1.0;

	return m_pModelCom->Get_CollisionMaxRatio(iIndex);
}

vector<CAnimation*>* CToolObject::Get_Animation()
{
	// TODO: ���⿡ ��ȯ ������ �����մϴ�.
	if (nullptr == m_pModelCom)
		return nullptr;

	return m_pModelCom->Get_Animation();
}

vector<CHierarchyNode*>* CToolObject::Get_HierarchyNode()
{
	// TODO: ���⿡ ��ȯ ������ �����մϴ�.
	if (nullptr == m_pModelCom)
		return nullptr;

	return m_pModelCom->Get_HierarchyNode();
}

void CToolObject::Set_PlayTime(_double Time)
{
	if (nullptr == m_pModelCom)
		return;

	return m_pModelCom->Set_PlayTime(Time);
}

void CToolObject::Set_Duration(_double Time)
{
	if (nullptr == m_pModelCom)
		return;

	return m_pModelCom->Set_Duration(Time);
}

void CToolObject::Set_PlaySpeedPerSec(_double Speed)
{
	if (nullptr == m_pModelCom)
		return;

	return m_pModelCom->Set_PlaySpeedPerSec(Speed);
}

void CToolObject::Set_CurBoneName(char * pName)
{
	if (nullptr == pName || nullptr == m_pModelCom)
		return;

	_tchar szName[MAX_PATH];
	ZeroMemory(szName, MAX_PATH * sizeof(_tchar));

	size_t iSize = 0;
	mbstowcs_s(&iSize, m_szCurBoneName, pName, MAX_PATH * sizeof(char));

	m_pModelCom->Release_RefNode(m_szCurBoneName);
	m_pModelCom->Add_RefNode(m_szCurBoneName, pName);
}

void CToolObject::Set_LinkMinRatio(_uint iIndex, _double Ratio)
{
	if (nullptr == m_pModelCom)
		return;

	m_pModelCom->Set_LinkMinRatio(iIndex, Ratio);
}

void CToolObject::Set_LinkMaxRatio(_uint iIndex, _double Ratio)
{
	if (nullptr == m_pModelCom)
		return;

	m_pModelCom->Set_LinkMaxRatio(iIndex, Ratio);
}

void CToolObject::Set_BlendMaxTime(_uint iIndex, _double Time)
{
	if (nullptr == m_pModelCom)
		return;

	m_pModelCom->Set_BlendMaxTime(iIndex, Time);
}

void CToolObject::Set_BlendLevel(_uint iIndex, _uint iLevel)
{
	if (nullptr == m_pModelCom)
		return;

	m_pModelCom->Set_BlendLevel(iIndex, iLevel);
}

void CToolObject::Set_NoneUpdatePos(_uint iIndex, _bool bCheck)
{
	if (nullptr == m_pModelCom)
		return;

	m_pModelCom->Set_NoneUpdatePos(iIndex, bCheck);
}

void CToolObject::Set_Direction(_uint iIndex, DIRECTION eCheck)
{
	if (nullptr == m_pModelCom)
		return;

	m_pModelCom->Set_Direction(iIndex, eCheck);
}

void CToolObject::Set_CollisionMinRatio(_uint iIndex, _double Ratio)
{
	if (nullptr == m_pModelCom)
		return;

	m_pModelCom->Set_CollisionMinRatio(iIndex, Ratio);
}

void CToolObject::Set_CollisionMaxRatio(_uint iIndex, _double Ratio)
{
	if (nullptr == m_pModelCom)
		return;

	m_pModelCom->Set_CollisionMaxRatio(iIndex, Ratio);
}

void CToolObject::Set_CollisionMinRatio(_double Ratio)
{
	if (nullptr == m_pModelCom)
		return;

	m_pModelCom->Set_CollisionMinRatio(Ratio);
}

void CToolObject::Set_CollisionMaxRatio(_double Ratio)
{
	if (nullptr == m_pModelCom)
		return;

	m_pModelCom->Set_CollisionMaxRatio(Ratio);
}


HRESULT CToolObject::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_RENDERER, TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::DESC		TransformDesc;
	TransformDesc.dSpeedPerSec = 5.f;
	TransformDesc.dRotationPerSec = XMConvertToRadians(90.0f);
	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_TRANSFORM, TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, m_szProtoModel, TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	m_pModelCom->Add_RefNode(L"bone-1", "bone-1");

	/* For.Com_SPHERE */
	CCollider::DESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::DESC));

	ColliderDesc.vPivot = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vScale = _float3(0.7f, 0.7f, 0.7f);
	ColliderDesc.vOrientation = _float4(0.0f, 0.0f, 0.0f, 1.f);
	ColliderDesc.fRadius = 0.3f;

	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, TEXT("Prototype_Collider_SPHERE"), COM_KEY_SPHERE, (CComponent**)&m_pSphereCom, &ColliderDesc)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE::POSITION, XMVectorSet(6.f, 0.5f, -0.4f, 1.f));

	return S_OK;
}

HRESULT CToolObject::SetUp_ConstantTable()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	_float fFar = pGameInstance->Get_Far();

	m_pModelCom->SetUp_RawValue("g_matWorld", &m_pTransformCom->Get_WorldMatrix4x4Transpose(), sizeof(_matrix));
	m_pModelCom->SetUp_RawValue("g_matView", &pGameInstance->Get_Transpose(CPipeLine::TRANSFORM::D3DTS_VIEW), sizeof(_matrix));
	m_pModelCom->SetUp_RawValue("g_matProj", &pGameInstance->Get_Transpose(CPipeLine::TRANSFORM::D3DTS_PROJ), sizeof(_matrix));
	m_pModelCom->SetUp_RawValue("vDofParams", m_pRendererCom->Get_DOF_Parmas(), sizeof(_float4));
	m_pModelCom->SetUp_RawValue("g_vCamPos", &pGameInstance->Get_CameraPosition(), sizeof(_float4));
	m_pModelCom->SetUp_RawValue("g_fFar", &fFar, sizeof(_float));

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CToolObject * CToolObject::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CToolObject* pGameInstance = new CToolObject(pDevice, pDeviceContext);

	if (FAILED(pGameInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CToolObject");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

CGameObject * CToolObject::Clone(void * pArg)
{
	CToolObject* pGameInstance = new CToolObject(*this);

	if (FAILED(pGameInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CToolObject");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CToolObject::Free()
{
	__super::Free();

	Safe_Release(m_pSphereCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

}
