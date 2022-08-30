
// ToolView.cpp : CToolView Ŭ������ ����
//

#include "stdafx.h"
// SHARED_HANDLERS�� �̸� ����, ����� �׸� �� �˻� ���� ó���⸦ �����ϴ� ATL ������Ʈ���� ������ �� ������
// �ش� ������Ʈ�� ���� �ڵ带 �����ϵ��� �� �ݴϴ�.
#ifndef SHARED_HANDLERS
#include "Tool.h"
#endif

#include "MainFrm.h"
#include "ToolDoc.h"
#include "ToolView.h"
#include "MainTab.h"
#include "ToolCamera.h"
#include "GameInstance.h"
#include "Terrain.h"
#include "ToolObject.h"
#include "Mouse.h"
#include "ToolParticle.h"
#include "VIBuffer_PointInstance.h"
#include "VIBuffer_Pointbillboard.h"
#include "NavigationTab.h"
#include "ToolEffectMeshObject.h"
#include "Event_Collider.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CToolView

IMPLEMENT_DYNCREATE(CToolView, CView)

BEGIN_MESSAGE_MAP(CToolView, CView)
	// ǥ�� �μ� ����Դϴ�.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_MBUTTONDOWN()
END_MESSAGE_MAP()

// CToolView ����/�Ҹ�

CToolView::CToolView()
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.

}

CToolView::~CToolView()
{
	Safe_Release(m_pCamera);

	Safe_Release(m_pRenderer);

	Safe_Release(m_pGameInstance);
	Safe_Delete(m_pMainTab);

	Safe_Release(m_pGraphicDevice);
	Safe_Release(m_pContextDevice);


	CGameInstance::Release_Engine();
}

BOOL CToolView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	return CView::PreCreateWindow(cs);
}

// CToolView �׸���

void CToolView::OnDraw(CDC* /*pDC*/)
{
	CToolDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: ���⿡ ���� �����Ϳ� ���� �׸��� �ڵ带 �߰��մϴ�.
}


// CToolView �μ�

BOOL CToolView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// �⺻���� �غ�
	return DoPreparePrinting(pInfo);
}

void CToolView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ��ϱ� ���� �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
}

void CToolView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ� �� ���� �۾��� �߰��մϴ�.
}


// CToolView ����

#ifdef _DEBUG
void CToolView::AssertValid() const
{
	CView::AssertValid();
}

void CToolView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CToolDoc* CToolView::GetDocument() const // ����׵��� ���� ������ �ζ������� �����˴ϴ�.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CToolDoc)));
	return (CToolDoc*)m_pDocument;
}
#endif //_DEBUG


// CToolView �޽��� ó����


void CToolView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	CMainFrame*	pMainFrame = dynamic_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd());

	RECT rcMain = {};
	pMainFrame->GetWindowRect(&rcMain);
	SetRect(&rcMain, 0, 0, rcMain.right - rcMain.left, rcMain.bottom - rcMain.top);

	RECT rcView = {};
	GetClientRect(&rcView);

	int iGapX = rcMain.right - rcView.right;
	int iGapY = rcMain.bottom - rcView.bottom;

	pMainFrame->SetWindowPos(nullptr, 0, 0, g_iWinCX + iGapX, g_iWinCY + iGapY, SWP_NOMOVE);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	m_pGameInstance = pGameInstance;
	Safe_AddRef(m_pGameInstance);


	CGraphicDevice::DESC GraphicDesc;

	GraphicDesc.iWinCX = g_iWinCX;
	GraphicDesc.iWinCY = g_iWinCY;

	GraphicDesc.hWnd = m_hWnd;
	GraphicDesc.eWinMode = CGraphicDevice::WINMODE::WIN;

	if (FAILED(pGameInstance->Initialize(AfxGetInstanceHandle(), GraphicDesc,pMainFrame->m_hWnd ,(_uint)LEVEL::COUNT , &m_pGraphicDevice, &m_pContextDevice)))
		return;

	Safe_AddRef(m_pGraphicDevice);
	Safe_AddRef(m_pContextDevice);

	if (FAILED(Ready_Component()))
		return;

	if (FAILED(Ready_Light()))
		return;

	if (FAILED(Ready_Component_ForStatic()))
		return;


	if (FAILED(Ready_Prototype_GameObject_ForStatic()))
		return;

	if (nullptr == m_pMainTab)
	{
		m_pMainTab = new CMainTab;
		m_pMainTab->Create(IDD_MAINTAB);
		m_pMainTab->ShowWindow(SW_SHOW);
	}

	RELEASE_INSTANCE(CGameInstance);


	srand((_uint)time(NULL));
}

HRESULT CToolView::Ready_Prototype_GameObject_ForStatic()
{
	if (FAILED(m_pGameInstance->Add_GameObject(PROTO_KEY_TERRAIN, CTerrain::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject(PROTO_KEY_TOOLCAMERA, CToolCamera::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject(PROTO_KEY_TOOLOBJECT, CToolObject::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject(PROTO_KEY_MOUSE, CMouse::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_ToolParticle"), CToolParticle::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_ToolEffectMeshObject"), CToolEffectMeshObject::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_EventCollider"), CEvent_Collider::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	return S_OK;
}

HRESULT CToolView::Ready_Component_ForStatic()
{

	return S_OK;
}

HRESULT CToolView::Ready_Component()
{
	if (FAILED(m_pGameInstance->Add_Timer(L"TimerDefault")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Timer(L"Timer60")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Component((_uint)LEVEL::STATIC, PROTO_KEY_RENDERER, m_pRenderer = CRenderer::Create(m_pGraphicDevice, m_pContextDevice,Shader_Viewport, DefaultTechnique))))
		return E_FAIL;
	Safe_AddRef(m_pRenderer);

	if (FAILED(m_pGameInstance->Add_Component((_uint)LEVEL::STATIC, PROTO_KEY_TRANSFORM, CTransform::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	_matrix		PivotMatrix;
	PivotMatrix = XMMatrixIdentity();

	//if (FAILED(m_pGameInstance->Add_Prototype((_uint)LEVEL::STATIC, PROTO_KEY_MODEL_PLAYER, CModel::Create(m_pGraphicDevice(), m_pContextDevice(), CModel::TYPE::ANIM, "../Client/Bin/Resources/Meshes/ema010out/", "ema010out.fbx", Shader_Model, "DefaultTechnique", PivotMatrix))))
	//	return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Component((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_AABB, CCollider::Create(m_pGraphicDevice, m_pContextDevice, CCollider::TYPE::AABB))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Component((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_OBB, CCollider::Create(m_pGraphicDevice, m_pContextDevice, CCollider::TYPE::OBB))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Component((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_SPHERE, CCollider::Create(m_pGraphicDevice, m_pContextDevice, CCollider::TYPE::SPHERE))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Component((_uint)LEVEL::STATIC, L"Debug_NavMeshTexture", CTexture::Create(m_pGraphicDevice, m_pContextDevice, L"../Client/Bin/Resources/Textures/NavMesh/Debug_NavMesh%d.png", 2))))
		return E_FAIL;

	CCamera::DESC tCameraDesc;

	tCameraDesc.fFOV = XMConvertToRadians(60.f);
	tCameraDesc.fAspect = g_iWinCX / (_float)g_iWinCY;
	tCameraDesc.fFar = 500.f;
	tCameraDesc.fNear = 0.2f;
	tCameraDesc.vEye = _float3(0.f, 1.f, -2.f);
	tCameraDesc.vAt = _float3(0.f, 0.f, 0.f);
	tCameraDesc.tTransformDesc.dSpeedPerSec = 10.f;
	tCameraDesc.tTransformDesc.dRotationPerSec = 2.f;
	m_pCamera = CToolCamera::Create(m_pGraphicDevice, m_pContextDevice, tCameraDesc);
	if (m_pCamera == nullptr)
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Component((_uint)LEVEL::STATIC, TEXT("Prototype_Component_VIBuffer_Pointbillboard"), CVIBuffer_Pointbillboard::Create(m_pGraphicDevice, m_pContextDevice, TEXT("../Client/Bin/ShaderFiles/Shader_Pointbillboard.hlsl"), DefaultTechnique))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Component((_uint)LEVEL::STATIC, TEXT("Prototype_Component_VIBuffer_PointInstance"), CVIBuffer_PointInstance::Create(m_pGraphicDevice, m_pContextDevice, TEXT("../Client/Bin/ShaderFiles/Shader_PointInstance.hlsl"), DefaultTechnique))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Component((_uint)LEVEL::STATIC, TEXT("Prototype_Component_Texture_Dissolve"), CTexture::Create(m_pGraphicDevice, m_pContextDevice, L"../Client/Bin/Resources/Effect/ImageEffect/Dissolve/dissolve.png"))))
		return E_FAIL;


	return S_OK;
}

HRESULT CToolView::Ready_Light()
{
	/*LIGHTDESC tLightDesc;

	tLightDesc.eType = LIGHTDESC::TYPE::DIRECTIONAL;
	tLightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	tLightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	tLightDesc.vDirection = _float4(1.f, -1.f, -1.f, 1.f);
	tLightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (m_pGameInstance->Add_Light(m_pGraphicDevice, m_pContextDevice, L"LightDir" ,Shader_Viewport, DefaultTechnique, tLightDesc))
		return E_FAIL;*/


	return S_OK;
}

_int CToolView::Tick(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	m_pCamera->Tick(dTimeDelta);
	m_pMainTab->Tick(dTimeDelta);
	m_pRenderer->Set_DOF_Params(_float4(0.f, 6.f, 150.f, 1.f));
	if (m_pMainTab->IsWindowVisible() == false)
		m_pMainTab->ShowWindow(SW_SHOW);

	if (pGameInstance->Mouse_Down(CInputDevice::DIMB::DIMB_LB))
	{
		if (m_pMainTab->m_pNavigationTab->m_checkPicking.GetCheck() == false)
			return 0;

		Picking(dTimeDelta);
	}

	return _int();
}

_int CToolView::LateTick(_double dTimeDelta)
{
	m_pCamera->LateTick(dTimeDelta);
	m_pMainTab->LateTick(dTimeDelta);
	return _int();
}

HRESULT CToolView::Render()
{
	m_pGameInstance->Clear_BackBufferView(_float4(1.f, 1.f, 1.f, 1.f));
	m_pGameInstance->Clear_DepthStencilView();

	m_pRenderer->Draw_RenderGroup();
	m_pRenderer->Draw_DebugGroup();
	m_pGameInstance->Render_Level();

	m_pGameInstance->Present();

	m_pMainTab->Render();

	return S_OK;
}

_int CToolView::Picking(_double dTimeDelta)
{
	m_pMainTab->Picking(dTimeDelta);
	m_pMainTab->m_pNavigationTab->Picking(dTimeDelta);

	return _int();
}


void CToolView::OnMButtonDown(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	if(GetFocus())
		m_pCamera->Do_Action();
	CView::OnMButtonDown(nFlags, point);
}
