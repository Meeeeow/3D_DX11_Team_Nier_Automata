#include "stdafx.h"
#include "..\Public\Loader.h"
#include "LoadDatFiles.h"
#include "ObjectPool.h"

#include "MainCamera.h"
#include "Building.h"
#include "Player.h"
#include "Katana.h"
#include "LongKatana.h"
#include "Spear.h"
#include "Gauntlet.h"
#include "MeshEffect.h"
#include "Humanoid_Small.h"
#include "Android_9S.h"
#include "Humanoid_Big.h"
#include "Humanoid_Bullet.h"
#include "Tank.h"
#include "Camera_Debug.h"
#include "Humanoid_Dragoon.h"
#include "Normal_Attack.h"
#include "Humanoid_Clown.h"
#include "Coaster.h"
#include "Event_Collider.h"
#include "Camera_Cutscene.h"
#include "VendingMachine.h"
#include "Clown_Fly.h"

#include "Beauvoir.h"
#include "Beauvoir_Bullet.h"
#include "Beauvoir_FireMissile.h"
#include "Beauvoir_NaviBullet.h"
#include "Beauvoir_RisingBullet.h"
#include "CrossAndroid.h"
#include "Beauvoir_Skirt.h"
#include "Beauvoir_Hair.h"
#include "BeauvoirCurtain.h"

#include "Pod.h"
#include "Pod9S.h"
#include "PodBullet.h"

#include "Level_Loading.h"

//#include "UI_HUD_Hpbar_Player_Frame.h"
//#include "UI_HUD_Hpbar_Player_Contents.h"
//#include "UI_HUD_Hpbar_Monster_Frame.h"
//#include "UI_HUD_Hpbar_Monster_Contents.h"
//#include "UI_HUD_PodCoolTime.h"
//#include "UI_HUD_Damage.h"
//#include "UI_HUD_Pointer.h"
//#include "UI_HUD_Target.h"
//#include "UI_Core_Background.h"
//#include "UI_Core_Cursor.h"
//#include "UI_Dialogue_EngageMessage.h"
//#include "UI_Dialogue_SystemMessage.h"
//#include "UI_Core_CinematicScreen.h"
//#include "UI_Core_HeadButton.h"
//#include "UI_Core_Status.h"

#include "Sky.h"
#include "TreasureBox.h"

#include "MeshEffect.h"
#include "EffectBullet.h"
#include "AfterImage.h"
#include "Trail.h"
#include "AnimationTrail.h"

#include "Zhuangzi.h"
#include "ZhuangziBridge.h"
#include "ZhuangziDoor.h"
#include "ZhuangziFireMissile.h"
#include "WholeRange.h"
#include "ZhuangziWeaponTrail.h"

#include "Engels.h"
#include "EngelsFireMissile.h"
#include "EngelsHitPart.h"

// MiniGame
#include "Iron_Android9S.h"
#include "Iron_Humanoid_Small.h"
#include "Camera_IronFist.h"
#include "MiniGameBuilding.h"
#include "IronGauntlet.h"

bool CLoader::m_bIsInitialLoading = false;
bool CLoader::m_bIsTestSceneInitialize = false;
bool CLoader::m_bIsRobotMilitaryInitialize = false;
bool CLoader::m_bIsZhuangziInitialize = false;
bool CLoader::m_bIsAmusementParkInitialize = false;
bool CLoader::m_bIsOperaBackStageInitialize = false;

CLoader::CLoader(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: m_pGraphicDevice(pGraphicDevice), m_pContextDevice(pContextDevice)
{
	Safe_AddRef(pGraphicDevice);
	Safe_AddRef(pContextDevice);
}

_uint APIENTRY LoadingThread(LPVOID pArg)
{
	CLoader*	pLoader = (CLoader*)pArg;
	EnterCriticalSection(&pLoader->Get_CS());

	if (false == CLoader::m_bIsInitialLoading)
	{
		pLoader->Initialize_Static_Component();
		pLoader->Initialize_Static_Object();
		CLoader::m_bIsInitialLoading = true;
	}

	HRESULT		hr = 0;
	switch (pLoader->Get_NextLevel())
	{
	case LEVEL::STAGE1:
		hr = pLoader->Load_Stage1();
		break;
	case LEVEL::ROBOTMILITARY:
		hr = pLoader->Load_Stage2();
		break;
	case LEVEL::ZHUANGZISTAGE:
		hr = pLoader->Load_ZhuangziStage();
		break;
	case LEVEL::ROBOTGENERAL:
		hr = pLoader->Load_ROBOTGENERAL();
		break;
	case LEVEL::AMUSEMENTPARK:
		hr = pLoader->Load_AmusementPark();
		break;
	case LEVEL::OPERABACKSTAGE:
		hr = pLoader->Load_OperaBackStage();
		break;
	default:
		break;
	}

	if (FAILED(hr))
		return E_FAIL;

	LeaveCriticalSection(&pLoader->Get_CS());
	return S_OK;
}

HRESULT CLoader::NativeConstruct(LEVEL eNextLevel)
{
	InitializeCriticalSection(&m_CS);

	m_eNextLevel = eNextLevel;

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingThread, this, 0, nullptr);
	if (m_hThread == 0)
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Load_Stage1()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	
	CLoadDatFiles::Get_Instance()->Load_Dynamic_ModelDat_File(LEVEL::STAGE1, "../Bin/Data/PrototypeData/Stage1/*.dat");

	if (dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel()) != nullptr)
		dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel())->Increase_Percent();

	m_bFinished = true;
	return S_OK;
}

HRESULT CLoader::Load_Stage2()
{

	m_bFinished = true;
	return S_OK;
}

HRESULT CLoader::Load_Stage3()
{

	m_bFinished = true;
	return S_OK;
}

HRESULT CLoader::Load_ZhuangziStage()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	
	CLoadDatFiles::Get_Instance()->Load_Dynamic_ModelDat_File(LEVEL::ZHUANGZISTAGE, "../Bin/Data/PrototypeData/ZhuangziStage/*.dat");

	if (dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel()) != nullptr)
		dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel())->Increase_Percent();

	if (dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel()) != nullptr)
		dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel())->Increase_Percent();

	if (FAILED(pGameInstance->Add_Component((_uint)LEVEL::ZHUANGZISTAGE, PROTO_KEY_MODEL_ZHUANGZIBRIDGE
		, CModel::Create(m_pGraphicDevice, m_pContextDevice, TEXT("../Bin/Data/PrototypeData/ZhuangziStage/Animation/Bridge.dat")
			, Shader_Model, DefaultTechnique, false, TEXT("../Bin/Data/Animation/Bridge.ANIMATION")))))
		return E_FAIL;

	if (dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel()) != nullptr)
		dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel())->Increase_Percent();

	if (FAILED(pGameInstance->Add_Component((_uint)LEVEL::ZHUANGZISTAGE, PROTO_KEY_MODEL_ZHUANGZI
		, CModel::Create(m_pGraphicDevice, m_pContextDevice, TEXT("../Bin/Data/PrototypeData/ZhuangziStage/Animation/Zhuangzi.dat")
			, Shader_Model, DefaultTechnique, false, TEXT("../Bin/Data/Animation/Zhuangzi.ANIMATION")))))
		return E_FAIL;

	if (dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel()) != nullptr)
		dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel())->Increase_Percent();

	if (FAILED(pGameInstance->Add_Component((_uint)LEVEL::ZHUANGZISTAGE, PROTO_KEY_MODEL_ZHUANGZIDOOR
		, CModel::Create(m_pGraphicDevice, m_pContextDevice, TEXT("../Bin/Data/PrototypeData/ZhuangziStage/Animation/ZhuangziDoor.dat")
			, Shader_Model, DefaultTechnique, false, TEXT("../Bin/Data/Animation/ZhuangziDoor.ANIMATION")))))
		return E_FAIL;

	if (dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel()) != nullptr)
		dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel())->Increase_Percent();

	if (m_bIsZhuangziInitialize)
	{
		m_bFinished = true;
		return S_OK;
	}

	if (FAILED(pGameInstance->Add_GameObject(PROTO_KEY_ZHUANGZIBRIDGE, CZhuangziBridge::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(PROTO_KEY_ZHUANGZIDOOR, CZhuangziDoor::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(PROTO_KEY_ZHUANGZI, CZhuangzi::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(PROTO_KEY_WHOLERANGE, CWholeRange::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(L"Prototype_Zhuangzi_FireMissile", CZhuangziFireMissile::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(L"Prototype_Zhuangzi_WeaponTrail", CZhuangziWeaponTrail::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel()) != nullptr)
		dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel())->Increase_Percent();

	m_bFinished = true;
	m_bIsZhuangziInitialize = true;

	return S_OK;
}

HRESULT CLoader::Load_AmusementPark()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	CLoadDatFiles::Get_Instance()->Load_Dynamic_ModelDat_File(LEVEL::AMUSEMENTPARK, "../Bin/Data/PrototypeData/AmusementPark/Dynamic/*.dat");

	if (dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel()) != nullptr)
		dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel())->Increase_Percent();

	if (FAILED(pGameInstance->Add_Component((_uint)LEVEL::AMUSEMENTPARK, L"Humanoid_Clown"
		, CModel::Create(m_pGraphicDevice, m_pContextDevice, TEXT("../Bin/Data/PrototypeData/AmusementPark/Animation/Humanoid_Clown.dat")
			, Shader_Model, DefaultTechnique, false, TEXT("../Bin/Data/Animation/Humanoid_Clown.ANIMATION")))))
		return E_FAIL;

	if (dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel()) != nullptr)
		dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel())->Increase_Percent();

	if (FAILED(pGameInstance->Add_Component((_uint)LEVEL::AMUSEMENTPARK, L"Tank"
		, CModel::Create(m_pGraphicDevice, m_pContextDevice, TEXT("../Bin/Data/PrototypeData/AmusementPark/Animation/Tank_Ap.dat")
			, Shader_Model, DefaultTechnique, false, TEXT("../Bin/Data/Animation/Tank_AP.ANIMATION")))))
		return E_FAIL;

	if (dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel()) != nullptr)
		dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel())->Increase_Percent();

	if (FAILED(pGameInstance->Add_Component((_uint)LEVEL::AMUSEMENTPARK, L"Coaster"
		, CModel::Create(m_pGraphicDevice, m_pContextDevice, TEXT("../Bin/Data/PrototypeData/AmusementPark/Animation/Coaster.dat")
			, Shader_Model, DefaultTechnique, false, TEXT("../Bin/Data/Animation/Coaster.ANIMATION")))))
		return E_FAIL;

	if (dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel()) != nullptr)
		dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel())->Increase_Percent();

	if (FAILED(pGameInstance->Add_Component((_uint)LEVEL::AMUSEMENTPARK, L"Clown_Fly"
		, CModel::Create(m_pGraphicDevice, m_pContextDevice, TEXT("../Bin/Data/PrototypeData/AmusementPark/Animation/Clown_Fly.dat")
			, Shader_Model, DefaultTechnique, false, TEXT("../Bin/Data/Animation/Clown_Fly.ANIMATION")))))
		return E_FAIL;

	if (m_bIsAmusementParkInitialize)
	{
		m_bFinished = true;
		return S_OK;
	}

	if (FAILED(pGameInstance->Add_GameObject(L"Humanoid_Clown", CHumanoid_Clown::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(PROTO_KEY_TANK, CTank::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel()) != nullptr)
		dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel())->Increase_Percent();

	if (FAILED(pGameInstance->Add_GameObject(L"Coaster", CCoaster::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(L"Clown_Fly", CClown_Fly::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel()) != nullptr)
		dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel())->Increase_Percent();

	m_bFinished = true;
	m_bIsAmusementParkInitialize = true;

	return S_OK;
}

HRESULT CLoader::Load_OperaBackStage()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();


	if (FAILED(pGameInstance->Add_Component((_uint)LEVEL::OPERABACKSTAGE, PROTO_KEY_MODEL_BEAUVOIR
		, CModel::Create(m_pGraphicDevice, m_pContextDevice, TEXT("../Bin/Data/PrototypeData/OperaBackStage/Animation/Beauvoir.dat")
			, Shader_Model, DefaultTechnique, false, TEXT("../Bin/Data/Animation/Beauvoir.ANIMATION")))))
		return E_FAIL;

	if (dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel()) != nullptr)
		dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel())->Increase_Percent();

	if (FAILED(pGameInstance->Add_Component((_uint)LEVEL::OPERABACKSTAGE, PROTO_KEY_MODEL_CROSSANDROID
		, CModel::Create(m_pGraphicDevice, m_pContextDevice, TEXT("../Bin/Data/PrototypeData/OperaBackStage/Animation/CrossAndroid.dat")
			, Shader_Model, DefaultTechnique, false))))
		return E_FAIL;

	if (dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel()) != nullptr)
		dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel())->Increase_Percent();

	CLoadDatFiles::Get_Instance()->Load_Dynamic_ModelDat_File(LEVEL::OPERABACKSTAGE, "../Bin/Data/PrototypeData/OperaBackStage/Dynamic/*.dat");
	CLoadDatFiles::Get_Instance()->Load_ModelDat_File(LEVEL::OPERABACKSTAGE, "../Bin/Data/PrototypeData/OperaBackStage/NonAnim/*.dat");

	if (dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel()) != nullptr)
		dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel())->Increase_Percent();

	CLoadDatFiles::Get_Instance()->Load_ModelDat_File(LEVEL::OPERABACKSTAGE, "../Bin/Data/PrototypeData/OperaBackStage/*.dat");

	if (dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel()) != nullptr)
		dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel())->Increase_Percent();

	if (m_bIsOperaBackStageInitialize)
	{
		m_bFinished = true;
		return S_OK;
	}

	if (FAILED(pGameInstance->Add_GameObject(PROTO_KEY_BEAUVOIR, CBeauvoir::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel()) != nullptr)
		dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel())->Increase_Percent();

	if (FAILED(pGameInstance->Add_GameObject(PROTO_KEY_BEAUVOIR_SKIRT, CBeauvoir_Skirt::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(PROTO_KEY_CROSSANDROID, CCrossAndroid::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(PROTO_KEY_BEAUVOIR_CURATIN, CBeauvoirCurtain::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(L"Prototype_Beauvoir_FireMissile", CBeauvoir_FireMissile::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel()) != nullptr)
		dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel())->Increase_Percent();

	m_bFinished = true;
	m_bIsOperaBackStageInitialize = true;

	return S_OK;
}

HRESULT CLoader::Load_ROBOTGENERAL()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();


	CLoadDatFiles::Get_Instance()->Load_Dynamic_ModelDat_File(LEVEL::ROBOTGENERAL, "../Bin/Data/PrototypeData/EngelsStage/*.dat");

	if (dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel()) != nullptr)
		dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel())->Increase_Percent();

	if (FAILED(pGameInstance->Add_Component((_uint)LEVEL::ROBOTGENERAL, PROTO_KEY_MODEL_ENGELS
		, CModel::Create(m_pGraphicDevice, m_pContextDevice, TEXT("../Bin/Data/PrototypeData/EngelsStage/Animation/Engels.dat")
			, Shader_Model, DefaultTechnique, false, TEXT("../Bin/Data/Animation/Engels.ANIMATION")))))
		return E_FAIL;

	if (dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel()) != nullptr)
		dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel())->Increase_Percent();


	if (dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel()) != nullptr)
		dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel())->Increase_Percent();

	if (FAILED(pGameInstance->Add_GameObject(PROTO_KEY_ENGELS, CEngels::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel()) != nullptr)
		dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel())->Increase_Percent();

	if (FAILED(pGameInstance->Add_GameObject(L"Prototype_Engels_FireMissile", CEngelsFireMissile::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel()) != nullptr)
		dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel())->Increase_Percent();

	if (FAILED(pGameInstance->Add_GameObject(PROTO_KEY_ENGELS_HITPART, CEngelsHitPart::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Component((_uint)LEVEL::ROBOTGENERAL, L"Prototype_Compoent_Texture_EngelsHit", CTexture::Create(m_pGraphicDevice, m_pContextDevice, TEXT("../Bin/Resources/Effect/ImageEffect/Engels/EngelsHit_%d.dds"), 4))))
		return E_FAIL;


	if (dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel()) != nullptr)
		dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel())->Increase_Percent();

	if (dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel()) != nullptr)
		dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel())->Increase_Percent();

	if (dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel()) != nullptr)
		dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel())->Increase_Percent();

	m_bFinished = true;

	return S_OK;
}

HRESULT CLoader::Initialize_Static_Component()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	if (FAILED(pGameInstance->Add_Component((_uint)LEVEL::STATIC, PROTO_KEY_MODEL_PLAYER
		, CModel::Create(m_pGraphicDevice, m_pContextDevice, TEXT("../Bin/Data/PrototypeData/Static/Animation/2B.dat")
			, Shader_Model, DefaultTechnique, false, TEXT("../Bin/Data/Animation/2B.ANIMATION")))))
		return E_FAIL;

	if (dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel()) != nullptr)
		dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel())->Increase_Percent();

	if (FAILED(pGameInstance->Add_Component((_uint)LEVEL::STATIC, PROTO_KEY_MODEL_9S
		, CModel::Create(m_pGraphicDevice, m_pContextDevice, TEXT("../Bin/Data/PrototypeData/Static/Animation/9S.dat")
			, Shader_Model, DefaultTechnique, false, TEXT("../Bin/Data/Animation/9S.ANIMATION")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Component((_uint)LEVEL::STATIC, PROTO_KEY_MODEL_IRON9S
		, CModel::Create(m_pGraphicDevice, m_pContextDevice, TEXT("../Bin/Data/PrototypeData/Static/Animation/Iron9S.dat")
			, Shader_Model, DefaultTechnique, false, TEXT("../Bin/Data/Animation/Iron9S.ANIMATION")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Component((_uint)LEVEL::STATIC, PROTO_KEY_MODEL_IRONFIST_BACKGROUND
		, CModel::Create(m_pGraphicDevice, m_pContextDevice, TEXT("../Bin/Data/PrototypeData/Static/bg008a.dat")
			, Shader_Model, DefaultTechnique, false))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Component((_uint)LEVEL::STATIC, PROTO_KEY_MODEL_KATANA
		, CModel::Create(m_pGraphicDevice, m_pContextDevice, TEXT("../Bin/Data/PrototypeData/Static/Animation/Katana.dat")
			, Shader_Model, DefaultTechnique, false, TEXT("../Bin/Data/Animation/Katana.ANIMATION")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Component((_uint)LEVEL::STATIC, PROTO_KEY_MODEL_LONGKATANA
		, CModel::Create(m_pGraphicDevice, m_pContextDevice, TEXT("../Bin/Data/PrototypeData/Static/Animation/LongKatana.dat")
			, Shader_Model, DefaultTechnique, false, TEXT("../Bin/Data/Animation/LongKatana.ANIMATION")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Component((_uint)LEVEL::STATIC, PROTO_KEY_MODEL_SPEAR
		, CModel::Create(m_pGraphicDevice, m_pContextDevice, TEXT("../Bin/Data/PrototypeData/Static/Animation/Spear.dat")
			, Shader_Model, DefaultTechnique, false, TEXT("../Bin/Data/Animation/Spear.ANIMATION")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Component((_uint)LEVEL::STATIC, PROTO_KEY_MODEL_GAUNTLET
		, CModel::Create(m_pGraphicDevice, m_pContextDevice, TEXT("../Bin/Data/PrototypeData/Static/Animation/Gauntlet.dat")
			, Shader_Model, DefaultTechnique, false, TEXT("../Bin/Data/Animation/Gauntlet.ANIMATION")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Component((_uint)LEVEL::STATIC, PROTO_KEY_MODEL_POD
		, CModel::Create(m_pGraphicDevice, m_pContextDevice, TEXT("../Bin/Data/PrototypeData/Static/Animation/POD.dat")
			, Shader_Model, DefaultTechnique, false, TEXT("../Bin/Data/Animation/POD.ANIMATION")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Component((_uint)LEVEL::STATIC, PROTO_KEY_MODEL_POD9S
		, CModel::Create(m_pGraphicDevice, m_pContextDevice, TEXT("../Bin/Data/PrototypeData/Static/Animation/POD9S.dat")
			, Shader_Model, DefaultTechnique, false))))
		return E_FAIL;
	if (dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel()) != nullptr)
		dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel())->Increase_Percent();

	if (FAILED(pGameInstance->Add_Component((_uint)LEVEL::STATIC, L"TreasureBox"
		, CModel::Create(m_pGraphicDevice, m_pContextDevice, TEXT("../Bin/Data/PrototypeData/Static/Animation/TreasureBox.dat")
			, Shader_Model, DefaultTechnique, false, TEXT("../Bin/Data/Animation/TreasureBox.ANIMATION")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Component((_uint)LEVEL::STATIC, L"Humanoid_Small"
		, CModel::Create(m_pGraphicDevice, m_pContextDevice, TEXT("../Bin/Data/PrototypeData/Static/Animation/Humanoid_Small.dat")
			, Shader_Model, DefaultTechnique, false, TEXT("../Bin/Data/Animation/Humanoid_Small.ANIMATION")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Component((_uint)LEVEL::STATIC, PROTO_KEY_MODEL_IRON_HUMANOID_SMALL
		, CModel::Create(m_pGraphicDevice, m_pContextDevice, TEXT("../Bin/Data/PrototypeData/Static/Animation/Humanoid_Small.dat")
			, Shader_Model, DefaultTechnique, false, TEXT("../Bin/Data/Animation/IronHumanoid_Small.ANIMATION")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Component((_uint)LEVEL::STATIC, L"Humanoid_Big"
		, CModel::Create(m_pGraphicDevice, m_pContextDevice, TEXT("../Bin/Data/PrototypeData/Static/Animation/Humanoid_Big.dat")
			, Shader_Model, DefaultTechnique, false, TEXT("../Bin/Data/Animation/Humanoid_Big.ANIMATION")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Component((_uint)LEVEL::STATIC, PROTO_KEY_MODEL_VENDING_MACHINE
		, CModel::Create(m_pGraphicDevice, m_pContextDevice, TEXT("../Bin/Data/PrototypeData/Static/Animation/VendingMachine.dat")
			, Shader_Model, DefaultTechnique, false))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Component((_uint)LEVEL::STATIC, PROTO_KEY_MODEL_DEAD_NIER
		, CModel::Create(m_pGraphicDevice, m_pContextDevice, TEXT("../Bin/Data/PrototypeData/Static/DeadNier.dat")
			, Shader_Model, DefaultTechnique, false))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Component((_uint)LEVEL::STATIC, PROTO_KEY_MODEL_SKY
		, CModel::Create(m_pGraphicDevice, m_pContextDevice, TEXT("../Bin/Data/PrototypeData/Static/bga013.dat")
			, Shader_Model, DefaultTechnique, false))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Component((_uint)LEVEL::STATIC, PROTO_KEY_MODEL_SKY_MORNING
		, CModel::Create(m_pGraphicDevice, m_pContextDevice, TEXT("../Bin/Data/PrototypeData/Static/World0.dat")
			, Shader_Model, DefaultTechnique, false))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Component((_uint)LEVEL::STATIC, PROTO_KEY_MODEL_SKY_AFTERNOON
		, CModel::Create(m_pGraphicDevice, m_pContextDevice, TEXT("../Bin/Data/PrototypeData/Static/World1.dat")
			, Shader_Model, DefaultTechnique, false))))
		return E_FAIL;

	if (dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel()) != nullptr)
		dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel())->Increase_Percent();

	return S_OK;
}

HRESULT CLoader::Initialize_Static_Object()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	if (dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel()) != nullptr)
		dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel())->Increase_Percent();

	if (FAILED(pGameInstance->Add_GameObject(PROTO_KEY_BUILDING, CBuilding::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(PROTO_KEY_CAMERA, CMainCamera::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_Camera_Debug"), CCamera_Debug::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_Camera_Cutscene"), CCamera_Cutscene::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(PROTO_KEY_IRONFIST_CAMERA, CCamera_IronFist::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(PROTO_KEY_PLAYER, CPlayer::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(PROTO_KEY_POD, CPod::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(PROTO_KEY_9S, CAndroid_9S::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(PROTO_KEY_IRON9S, CIron_Android9S::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(PROTO_KEY_IRONFIST_BACKGROUND, CMiniGameBuilding::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(PROTO_KEY_IRONHUMANOID_SMALL, CIron_Humanoid_Small::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(PROTO_KEY_POD9S, CPod9S::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(PROTO_KEY_KATANA, CKatana::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(PROTO_KEY_LONGKATANA, CLongKatana::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(PROTO_KEY_SPEAR, CSpear::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(PROTO_KEY_GAUNTLET, CGauntlet::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(PROTO_KEY_IRONGAUNTLET, CIronGauntlet::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(PROTO_KEY_HUMANOID_SMALL, CHumanoid_Small::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(PROTO_KEY_HUMANOID_BIG, CHumanoid_Big::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(L"Prototype_GameObject_EventCollider", CEvent_Collider::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;
	
	if (FAILED(pGameInstance->Add_GameObject(PROTO_KEY_VENDING_MACHINE, CVendingMachine::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	// Normal Attack
	if (FAILED(pGameInstance->Add_GameObject(L"Prototype_Normal_Attack", CNormal_Attack::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	// Effect
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_PodBullet"), CPodBullet::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_MeshEffect"), CMeshEffect::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_EffectBullet"), CEffectBullet::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_AfterImage"), CAfterImage::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Sky"), CSky::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_TreasureBox"), CTreasureBox::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_EffectMesh"), CMeshEffect::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Trail"), CTrail::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_AnimationTrail"), CAnimationTrail::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel()) != nullptr)
		dynamic_cast<CLevel_Loading*>(pGameInstance->Get_CurrentLevel())->Increase_Percent();

	return S_OK;
}


CLoader * CLoader::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, LEVEL eNextLevel)
{
	CLoader* pInstance = new CLoader(pGraphicDevice, pContextDevice);
	if (FAILED(pInstance->NativeConstruct(eNextLevel)))
		Safe_Release(pInstance);

	return pInstance;
}

void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);

	DeleteObject(m_hThread);
	CloseHandle(m_hThread);
	DeleteCriticalSection(&m_CS);

	Safe_Release(m_pContextDevice);
	Safe_Release(m_pGraphicDevice);
}
