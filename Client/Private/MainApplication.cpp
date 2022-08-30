#include "stdafx.h"
#include "..\Public\MainApplication.h"
#include "SoundMgr.h"
#include "Level_Logo.h"
#include "LoadDatFiles.h"
#include "Collider.h"
#include "VIBuffer_Pointbillboard.h"
#include "VIBuffer_PointInstance.h"
#include "Inventory.h"
#include "LogoScreen.h"
#include "LogoTitle.h"
#include "LogoGear.h"
#include "LogoAnyKey.h"
#include "LogoCopyRight.h"
#include "PostProcessor.h"
#include "ObjectPool.h"
#include "EffectFactory.h"
#include "EffectBullet.h"
#include "MeshEffect.h"
#include "CameraMgr.h"
#include "LoadingScreen.h"
#include "LoadingPercent.h"
#include "LoadingEmil.h"
#include "DecalCube.h"

CMainApplication::CMainApplication()
	: m_pGameInstance(CGameInstance::Get_Instance())
	, m_pAtlasManager(CAtlas_Manager::Get_Instance())
	, m_pPostProcessor(CPostProcessor::Get_Instance())
	, m_pCameraManager(CCameraMgr::Get_Instance())
{
	int iRefCount = 0;
	
	iRefCount = Safe_AddRef(m_pGameInstance);
	iRefCount = Safe_AddRef(m_pAtlasManager);
	iRefCount = Safe_AddRef(m_pPostProcessor);
	iRefCount = Safe_AddRef(m_pCameraManager);
}

HRESULT CMainApplication::NativeConstruct()
{
	CSoundMgr::Get_Instance()->Initialize();


	CAtlas_Manager::Get_Instance()->NativeConstruct();

	if (m_pGameInstance == nullptr)
		return E_FAIL;

	CGraphicDevice::DESC		tGraphicDeviceDesc;

	tGraphicDeviceDesc.hWnd = g_hWnd;
	tGraphicDeviceDesc.iWinCX = g_iWinCX;
	tGraphicDeviceDesc.iWinCY = g_iWinCY;
	tGraphicDeviceDesc.eWinMode = CGraphicDevice::WINMODE::WIN;

	if (FAILED(m_pGameInstance->Initialize(g_hInst, tGraphicDeviceDesc, (_uint)LEVEL::COUNT, &m_pGraphicDevice, &m_pContextDevice)))
		return E_FAIL;

	Safe_AddRef(m_pGraphicDevice);
	Safe_AddRef(m_pContextDevice);

	CLoadDatFiles::Get_Instance()->Init(m_pGraphicDevice, m_pContextDevice);

	CEffectFactory::Get_Instance()->NativeConstruct(m_pGraphicDevice, m_pContextDevice);
	CEffectFactory::Get_Instance()->Load_Effect_Component_Prototypes();

	CObjectPool::Get_Instance()->NativeConstruct(/*m_pGraphicDevice, m_pContextDevice*/);

	if (FAILED(Ready_Component_ForStatic()))
		return E_FAIL;

	if (FAILED(Open_Level(LEVEL::LOGO)))
		return E_FAIL;

	srand((unsigned int)time(NULL));

	m_pPostProcessor->NativeConstruct(m_pGraphicDevice, m_pContextDevice);

	return S_OK;
}

_int CMainApplication::Tick(_double dTimeDelta)
{
	//if (m_pPostProcessor)
	//{
	//	m_pPostProcessor->Set_PP_BeforeUI_SetTexture();
	//	m_pPostProcessor->Set_PP_AfterUI_SetTexture();
	//}


	return m_pGameInstance->Tick(dTimeDelta);
}

void CMainApplication::Update_FPS(_double dTimeDelta)
{
	++m_iFPS;
	m_dTimeDelta += dTimeDelta;
}

HRESULT CMainApplication::Render()
{
	if (FAILED(m_pGameInstance->Clear_BackBufferView(_float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Clear_DepthStencilView()))
		return E_FAIL;

	if (FAILED(m_pRenderer->Draw_RenderGroup()))
		return E_FAIL;

	if (FAILED(m_pRenderer->Draw_DebugGroup()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Render_Level()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Present()))
		return E_FAIL;

	if (m_dTimeDelta >= 1.0)
	{
#ifdef _DEBUG
		TCHAR		szFPS[16] = L"";
		swprintf_s(szFPS, L"FPS : %d", m_iFPS);
		SetWindowText(g_hWnd, szFPS);
#else
		TCHAR		szFPS[64] = L"";
		//m_iFPS += 15;
		if (m_iFPS >= 60)
			m_iFPS = 60;
		swprintf_s(szFPS, L"Nier Automata, FPS : %d", m_iFPS);
		SetWindowText(g_hWnd, szFPS);
#endif // _DEBUG

		m_iFPS = 0;
		m_dTimeDelta = 0.0;
	}

	return S_OK;
}

HRESULT CMainApplication::Open_Level(LEVEL eLevel)
{
	if (eLevel == LEVEL::LOGO)
	{
		if (FAILED(m_pGameInstance->Open_Level(CLevel_Logo::Create(m_pGraphicDevice, m_pContextDevice, (_uint)LEVEL::LOGO))))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CMainApplication::Ready_Component_ForStatic()
{
	if (FAILED(m_pGameInstance->Add_Component((_uint)LEVEL::STATIC, PROTO_KEY_RENDERER
		, m_pRenderer = CRenderer::Create(m_pGraphicDevice, m_pContextDevice, Shader_Viewport, DefaultTechnique))))
		return E_FAIL;
	Safe_AddRef(m_pRenderer);

	if (FAILED(m_pGameInstance->Add_Component((_uint)LEVEL::STATIC, PROTO_KEY_TRANSFORM
		, CTransform::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Component((_uint)LEVEL::STATIC, PROTO_KEY_VIBUFFER_RECT
		, CVIBuffer_Rect::Create(m_pGraphicDevice, m_pContextDevice, Shader_Rect, DefaultTechnique))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Component((_uint)LEVEL::STATIC, PROTO_KEY_VIBUFFER_CUBE
		, CVIBuffer_Cube::Create(m_pGraphicDevice, m_pContextDevice
		, TEXT("../Bin/ShaderFiles/Shader_Cube.hlsl"), DefaultTechnique))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Component((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_AABB
		, CCollider::Create(m_pGraphicDevice, m_pContextDevice, CCollider::TYPE::AABB))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Component((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_OBB
		, CCollider::Create(m_pGraphicDevice, m_pContextDevice, CCollider::TYPE::OBB))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Component((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_SPHERE
		, CCollider::Create(m_pGraphicDevice, m_pContextDevice, CCollider::TYPE::SPHERE))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Component((_uint)LEVEL::STATIC, PROTO_KEY_INVENTORY
		, CInventory::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Component((_uint)LEVEL::STATIC, PROTO_KEY_NAVIGATION_IRONFIST
		, CNavigation::Create(m_pGraphicDevice, m_pContextDevice, TEXT("../Bin/Data/NavigationData/IronFistNavi.dat")))))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_Component((_uint)LEVEL::STATIC, L"Dragoon"
	//	, CModel::Create(m_pGraphicDevice, m_pContextDevice, TEXT("../Bin/Data/PrototypeData/Animation/Dragoon.dat")
	//		, Shader_Model, DefaultTechnique, false, TEXT("../Bin/Data/Animation/Dragoon.ANIMATION")))))
	//	return E_FAIL;



	if (FAILED(m_pGameInstance->Add_GameObject(L"Logo", CLogoScreen::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject(L"LogoTitle", CLogoTitle::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject(L"LogoGear", CLogoGear::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject(L"LogoAnyKey", CLogoAnyKey::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject(L"LogoCopyRight", CLogoCopyRight::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject(L"LoadingScreen", CLoadingScreen::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject(L"LoadingPercent", CLoadingPercent::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject(L"LoadingEmil", CLoadingEmil::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject(L"DecalCube", CDecalCube::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Component((_uint)LEVEL::STATIC, PROTO_KEY_TEXTURE_ATLAS, CTexture::Create(m_pGraphicDevice, m_pContextDevice, TEXT("../Bin/Resources/Textures/UI/Atlas%d.png"), (_uint)CAtlas_Manager::CATEGORY::COUNT))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Component((_uint)LEVEL::LOGO, PROTO_KEY_LOGO_SCREEN, CTexture::Create(m_pGraphicDevice, m_pContextDevice, L"../Bin/Resources/Textures/LogoScreen/NierAutomata_Logo.png"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Component((_uint)LEVEL::LOGO, PROTO_KEY_LOGO_TITLE, CTexture::Create(m_pGraphicDevice, m_pContextDevice, L"../Bin/Resources/Textures/LogoScreen/NierAutomata_Title.png"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Component((_uint)LEVEL::LOGO, PROTO_KEY_LOGO_GEAR, CTexture::Create(m_pGraphicDevice, m_pContextDevice, L"../Bin/Resources/Textures/LogoScreen/NierAutomata_Gear.png"))))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Add_Component((_uint)LEVEL::LOGO, PROTO_KEY_LOGO_ANYKEY, CTexture::Create(m_pGraphicDevice, m_pContextDevice, L"../Bin/Resources/Textures/LogoScreen/NierAutomata_AnyKey.png"))))
		return E_FAIL;
	 
	if (FAILED(m_pGameInstance->Add_Component((_uint)LEVEL::LOGO, PROTO_KEY_LOGO_COPYRIGHT, CTexture::Create(m_pGraphicDevice, m_pContextDevice, L"../Bin/Resources/Textures/LogoScreen/NierAutomata_CopyRight.png"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Component((_uint)LEVEL::STATIC, PROTO_KEY_LOADING_SCREEN, CTexture::Create(m_pGraphicDevice, m_pContextDevice, L"../Bin/Resources/Textures/LoadingScreen/Loading_Screen.png"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Component((_uint)LEVEL::STATIC, PROTO_KEY_LOADING_PERCENT, CTexture::Create(m_pGraphicDevice, m_pContextDevice, L"../Bin/Resources/Textures/LoadingScreen/367_%d.dds", 7))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Component((_uint)LEVEL::STATIC, PROTO_KEY_LOADING_EMIL, CTexture::Create(m_pGraphicDevice, m_pContextDevice, L"../Bin/Resources/Textures/LoadingScreen/NierAutomata_Ball.png"))))
		return E_FAIL;

	

	if (FAILED(m_pGameInstance->Add_Component((_uint)LEVEL::STATIC, TEXT("Prototype_Component_VIBuffer_Pointbillboard"), CVIBuffer_Pointbillboard::Create(m_pGraphicDevice, m_pContextDevice, TEXT("../Bin/ShaderFiles/Shader_Pointbillboard.hlsl"), DefaultTechnique))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Component((_uint)LEVEL::STATIC, TEXT("Prototype_Component_VIBuffer_PointInstance"), CVIBuffer_PointInstance::Create(m_pGraphicDevice, m_pContextDevice, TEXT("../Bin/ShaderFiles/Shader_PointInstance.hlsl"), DefaultTechnique))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Component((_uint)LEVEL::STATIC, TEXT("Prototype_Component_VIBuffer_Trail"), CVIBuffer_Trail::Create(m_pGraphicDevice, m_pContextDevice, TEXT("../Bin/ShaderFiles/Shader_Trail.hlsl"), DefaultTechnique))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Component((_uint)LEVEL::STATIC, TEXT("Prototype_Component_VIBuffer_AnimationTrail"), CVIBuffer_AnimationTrail::Create(m_pGraphicDevice, m_pContextDevice, TEXT("../Bin/ShaderFiles/Shader_Trail.hlsl"), DefaultTechnique))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Component((_uint)LEVEL::STATIC, TEXT("Prototype_Component_Texture_Dissolve"), CTexture::Create(m_pGraphicDevice, m_pContextDevice, TEXT("../Bin/Resources/Effect/ImageEffect/Dissolve/dissolve.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Component((_uint)LEVEL::STATIC, TEXT("Prototype_Component_Texture_AfterImage"), CTexture::Create(m_pGraphicDevice, m_pContextDevice, TEXT("../Bin/Resources/Effect/ImageEffect/AfterImage/AfterImage_%d.dds"), 2))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Component((_uint)LEVEL::STATIC, TEXT("Prototype_Component_Texture_ElectricityEffect"), CTexture::Create(m_pGraphicDevice, m_pContextDevice, TEXT("../Bin/Resources/Effect/ImageEffect/Electricity4/Spark4_%d.dds"), 59))))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_Component((_uint)LEVEL::STATIC, TEXT("Prototype_Component_Model_Cirtain"), CModel::Create(m_pGraphicDevice, m_pContextDevice, TEXT("../Bin/Resources/Effect/ImageEffect/Electricity4/Spark4_%d.dds"), 59))))
	//	return E_FAIL;



	if (FAILED(m_pGameInstance->Add_Component((_uint)LEVEL::STATIC, TEXT("Prototype_Component_Model_Missile")
		, CModel::Create(m_pGraphicDevice, m_pContextDevice, TEXT("../Bin/Data/PrototypeData/Missile/Missile.dat")
			, Shader_Model, DefaultTechnique, false))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Component((_uint)LEVEL::STATIC, L"Prototype_Texture_BlackToWhite", CTexture::Create(m_pGraphicDevice, m_pContextDevice, TEXT("../Bin/Resources/Effect/Gradation/BlackToWhite.dds")))))
		return E_FAIL;

	return S_OK;
}

CMainApplication * CMainApplication::Create()
{
	CMainApplication* pInstance = new CMainApplication();
	if (FAILED(pInstance->NativeConstruct()))
		Safe_Release(pInstance);

	return pInstance;
}

void CMainApplication::Free()
{
	unsigned long dwRefCnt = 0;

	dwRefCnt = Safe_Release(m_pContextDevice);
	dwRefCnt = Safe_Release(m_pGraphicDevice);
	dwRefCnt = Safe_Release(m_pAtlasManager);
	dwRefCnt = Safe_Release(m_pPostProcessor);
	dwRefCnt = Safe_Release(m_pCameraManager);

	dwRefCnt = Safe_Release(m_pRenderer);

	CEffectFactory::Destroy_Instance();
	CAtlas_Manager::Destroy_Instance();
	CPostProcessor::Destroy_Instance();
	CCameraMgr::Destroy_Instance();

	CLoadDatFiles::Get_Instance()->Release_Device();
	dwRefCnt = CLoadDatFiles::Destroy_Instance();

	CObjectPool::Destroy_Instance();
	CSoundMgr::Destroy_Instance();

	CGameInstance::Release_Engine();
}

