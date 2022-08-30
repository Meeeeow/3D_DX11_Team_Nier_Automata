#include "stdafx.h"
#include "EffectFactory.h"
#include "Client_Defines.h"
#include "GameInstance.h"
#include "Texture.h"
#include "Model.h"

#include "EffectBullet.h"


IMPLEMENT_SINGLETON(CEffectFactory)



CEffectFactory::CEffectFactory()
	: m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CEffectFactory::Load_Effect_Component_Prototypes()
{
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\EnemyBullet.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\EnemyBullet_Big.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\EnemyBulletExplosion.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\EnemyBulletExplosion_Big.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\Katana_Spark.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\PODBullet.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\PODBullet_Explosion.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\POD_Laser0.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\POD_Laser1.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\POD_Laser_Ready.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\POD_Charging.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\POD_CounterShotGun_Gunfire.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\POD_CounterShotGun_Spark.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\POD_CounterShotGun_Bullet.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\POD_CounterShotGun_Explosion.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\EnergyCharging.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\CounterAttackEffect.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\EnergyCharging_End.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\SpearAttackEffect.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\SpearAttackEffect_Long.dat");


	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\Firework_Explosion_Blue.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\Firework_Explosion_Green.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\Firework_Explosion_Orange.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\Firework_Explosion_Purple.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\Firework_Explosion_Red.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\Firework_Explosion_Yellow.dat");

	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\Firework_Up_Blue.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\Firework_Up_Green.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\Firework_Up_Orange.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\Firework_Up_Purple.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\Firework_Up_Red.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\Firework_Up_Yellow.dat");


	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\Robot_Death_Explosion_Spark4.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\Katana_Decoration_Effect.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\LongKatana_Decoration_Effect.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\Spear_Decoration_Effect.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\Robot_Hit_Electricity.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\Katana_Trail.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\LongKatana_Trail.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\Large_Spark.dat");

	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\Beavoir_Donut_UpDown0.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\Beavoir_Donut_UpDown1.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\Beauvoir_Donut_Up0.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\Beauvoir_Donut_Down0.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\Beauvoir_Donut_Up1.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\Beauvoir_Donut_Down1.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\Beauvoir_Donut_Up2.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\Beauvoir_Donut_Down2.dat");

	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\Beavoir_RingWave.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\Beavoir_Laser0.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\Beavoir_Laser1.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\ElectricDonut.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\ElectricDonut_Explosion.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\MissileExplosion.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\Beauvoir_Jump_Impact.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\Beauvoir_Jump_Smoke.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\Beauvoir_Sonic.dat");



	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\Tank_Laser.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\Tank_Spark.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\Clown_Comfetti.dat");

	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\Zhuangzi_Impact_Plane.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\Zhuangzi_Impact_Smoke.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\Zhuangzi_Impact_Spark.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\Zhuangzi_Ring0.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\Zhuangzi_Shield_Body.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\Zhuangzi_Shield_Leg.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\Zhuangzi_WeaponTrail.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\Zhuangzi_FrontBlade_Spark.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\Zhuangzi_CreateRing_Spark.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\Zhuangzi_Pile.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\Zhuangzi_Sweep_Spark.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\Zhuangzi_Rollin_Spark.dat");

	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\Engles_Clap.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\Engles_Missile_Explosion.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\Engles_Missile_Launch.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\Engles_Spark_Hold.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\Engles_Stamp.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\Engles_WaterSplash.dat");
	//Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\Engels_Hold_Smoke.dat");

	// Tekken
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\PurplishRedFire.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\Tekken_BlueFire.dat");
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\9S_Spiral.dat");

	// 9S Appear
	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\9S_Appear.dat");

	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\Beauvoir_RedFire.dat");


	// Laser Hit
//	Load_Effect_Component_FromData_ToMap(L"..\\Bin\\Data\\EffectSaveData\\Laser_Hit.dat");

	return S_OK;
}


void CEffectFactory::Load_Effect_FromData(const _tchar * szPath, EffectParticleGroupDescription& tGroupDesc)
{
	HANDLE hFile = CreateFile(szPath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (INVALID_HANDLE_VALUE == hFile)
		return;

	EffectMeshDescription tMeshDesc;

	ZeroMemory(&(tGroupDesc.tEffectMeshGroupDesc.szGroupName), MAX_PATH * sizeof(_tchar));

	_ulong	dwByte = 0;

	_int iSize = 0;
	ReadFile(hFile, &iSize, sizeof(_int), &dwByte, nullptr);

	for (_int i = 0; i < iSize; ++i)
	{
		TEXNAMEPROTOPATH tTex;

		ReadFile(hFile, tTex.szName, sizeof(TCHAR) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, tTex.szPath, sizeof(TCHAR) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, tTex.szProto, sizeof(TCHAR) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, &tTex.iTextureNumber, sizeof(_int), &dwByte, nullptr);
	}

	ReadFile(hFile, &iSize, sizeof(_int), &dwByte, nullptr);
	for (_int i = 0; i < iSize; ++i)
	{
		MODELPATHNAME tModel;
		ReadFile(hFile, tModel.szName, sizeof(TCHAR) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, tModel.szPath, sizeof(TCHAR) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, tModel.szProto, sizeof(TCHAR) * MAX_PATH, &dwByte, nullptr);
	}

	tGroupDesc.tEffectMeshGroupDesc.szGroupName;
	ReadFile(hFile, &tGroupDesc.tEffectMeshGroupDesc.szGroupName, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);

	iSize = 0;
	ReadFile(hFile, &iSize, sizeof(_int), &dwByte, nullptr);

	_int _iShaderCount = 0;
	EFFECTMESHDESC* pDesc = nullptr;
	for (size_t i = 0; i < iSize; i++)
	{
		pDesc = new EFFECTMESHDESC;


		ReadFile(hFile, &pDesc->tInfo, sizeof(EFFECTMESHDESC::EffectMeshInfo), &dwByte, nullptr);
		ReadFile(hFile, &pDesc->tTexture, sizeof(EFFECTMESHDESC::EFFECTMESHTEXTURE), &dwByte, nullptr);
		ReadFile(hFile, &pDesc->tPattern, sizeof(EFFECTMESHDESC::EffectMeshPattern), &dwByte, nullptr);

		ReadFile(hFile, &pDesc->tFade_InOut, sizeof(EFFECTMESHDESC::Fade_InOut), &dwByte, nullptr);

		ReadFile(hFile, &pDesc->tUV_Sprite, sizeof(EFFECTMESHDESC::UV_SPRITE), &dwByte, nullptr);
		ReadFile(hFile, &pDesc->tUV_Animation, sizeof(EFFECTMESHDESC::UV_ANIMATION), &dwByte, nullptr);

		_int iVecSize = 0;
		ReadFile(hFile, &iVecSize, sizeof(_int), &dwByte, nullptr);
		for (_int i = 0; i < iVecSize; ++i)
		{
			SHADERNAME* tShaderName = new SHADERNAME;
			ZeroMemory(tShaderName, sizeof(SHADERNAME));
			TCHAR szTemp[MAX_PATH] = L"";

			ReadFile(hFile, &szTemp, sizeof(TCHAR) * MAX_PATH, &dwByte, nullptr);
			_tcscpy_s(tShaderName->szName, szTemp);

			pDesc->tEffectMeshShader.vecShaderName.emplace_back(tShaderName);
		}
		ReadFile(hFile, &pDesc->tEffectMeshShader.fDissolveSpeed, sizeof(_float), &dwByte, nullptr);
		tGroupDesc.tEffectMeshGroupDesc.listEffectMeshDesc.emplace_back(pDesc);
	}

	ReadFile(hFile, &tGroupDesc.tEffectMeshGroupDesc.bCreateDeathRepeat, sizeof(_bool), &dwByte, nullptr);
	ReadFile(hFile, &tGroupDesc.tEffectMeshGroupDesc.vCreateDeathTime, sizeof(_float2), &dwByte, nullptr);

	// Load Particle
	_int iParticleSize = 0;
	ReadFile(hFile, &iParticleSize, sizeof(_int), &dwByte, nullptr);
	for (_int i = 0; i < iParticleSize; i++)
	{
		PARTICLEDESC* pParticleDesc = new PARTICLEDESC;
		TCHAR szTemp[MAX_PATH] = L"";
		ReadFile(hFile, &szTemp, sizeof(TCHAR) * MAX_PATH, &dwByte, nullptr);
		_tcscpy_s(pParticleDesc->szParticleName, szTemp);
		ReadFile(hFile, &pParticleDesc->vPosition, sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFile, &pParticleDesc->vScale, sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFile, &pParticleDesc->vRotation, sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFile, &pParticleDesc->iTextureNumber, sizeof(_int), &dwByte, nullptr);
		ReadFile(hFile, &szTemp, sizeof(TCHAR) * MAX_PATH, &dwByte, nullptr);
		_tcscpy_s(pParticleDesc->szTextureName, szTemp);
		ReadFile(hFile, &szTemp, sizeof(TCHAR) * MAX_PATH, &dwByte, nullptr);
		_tcscpy_s(pParticleDesc->szTextureProto, szTemp);
		ReadFile(hFile, &szTemp, sizeof(TCHAR) * MAX_PATH, &dwByte, nullptr);
		_tcscpy_s(pParticleDesc->szTexturePath, szTemp);
		ReadFile(hFile, &pParticleDesc->fMultiTextureInterval, sizeof(_float), &dwByte, nullptr);
		ReadFile(hFile, &pParticleDesc->bMultiTextureRepeat, sizeof(_bool), &dwByte, nullptr);

		_int iCount = 0;
		ReadFile(hFile, &iCount, sizeof(_int), &dwByte, nullptr);

		for (_int i = 0; i < iCount; ++i)
		{
			SHADERNAME* tShaderName = new SHADERNAME;
			ZeroMemory(tShaderName, sizeof(SHADERNAME));

			ReadFile(hFile, &szTemp, sizeof(TCHAR) * MAX_PATH, &dwByte, nullptr);
			_tcscpy_s(tShaderName->szName, szTemp);
			pParticleDesc->tParticleShaderDesc.vecShaderName.push_back(tShaderName);
		}

		ReadFile(hFile, &pParticleDesc->tPointInst_Desc, sizeof(POINTINST_DESC), &dwByte, nullptr);
		tGroupDesc.tParticleGroupDesc.listParticleDesc.emplace_back(pParticleDesc);
	}



	CloseHandle(hFile);
}

void CEffectFactory::Load_Effect_FromMap(const _tchar * szPath, EffectParticleGroupDescription & tGroupDesc)
{
	m_mapLoadedEffectInfo[szPath]->vecTextureName;

	_tcscpy_s(tGroupDesc.tEffectMeshGroupDesc.szGroupName, m_mapLoadedEffectInfo[szPath]->tEffectGroupDesc.tEffectMeshGroupDesc.szGroupName);

	_int _iShaderCount = 0;
	EFFECTMESHDESC* pDesc = nullptr;

	for (auto& pIter : m_mapLoadedEffectInfo[szPath]->tEffectGroupDesc.tEffectMeshGroupDesc.listEffectMeshDesc)
	{
		pDesc = new EFFECTMESHDESC;
		pDesc->tInfo = pIter->tInfo;
		pDesc->tTexture = pIter->tTexture;
		pDesc->tPattern = pIter->tPattern;
		pDesc->tFade_InOut = pIter->tFade_InOut;
		pDesc->tUV_Sprite = pIter->tUV_Sprite;
		pDesc->tUV_Animation = pIter->tUV_Animation;

		for (auto& pShaderNameIter : pIter->tEffectMeshShader.vecShaderName)
		{
			SHADERNAME* tShaderName = new SHADERNAME;
			_tcscpy_s(tShaderName->szName, pShaderNameIter->szName);
			pDesc->tEffectMeshShader.vecShaderName.emplace_back(tShaderName);
		}
		pDesc->tEffectMeshShader.fDissolveSpeed = pIter->tEffectMeshShader.fDissolveSpeed;
		tGroupDesc.tEffectMeshGroupDesc.listEffectMeshDesc.emplace_back(pDesc);
	}
	tGroupDesc.tEffectMeshGroupDesc.bCreateDeathRepeat = m_mapLoadedEffectInfo[szPath]->tEffectGroupDesc.tEffectMeshGroupDesc.bCreateDeathRepeat;
	tGroupDesc.tEffectMeshGroupDesc.vCreateDeathTime = m_mapLoadedEffectInfo[szPath]->tEffectGroupDesc.tEffectMeshGroupDesc.vCreateDeathTime;
	


	// Particle

	for (auto& pIter : m_mapLoadedEffectInfo[szPath]->tEffectGroupDesc.tParticleGroupDesc.listParticleDesc)
	{
		PARTICLEDESC* pParticleDesc = new PARTICLEDESC;
		_tcscpy_s(pParticleDesc->szParticleName, pIter->szParticleName);
		pParticleDesc->vPosition = pIter->vPosition;
		pParticleDesc->vScale = pIter->vScale;
		pParticleDesc->vRotation = pIter->vRotation;
		pParticleDesc->iTextureNumber = pIter->iTextureNumber;
		pParticleDesc->fMultiTextureInterval = pIter->fMultiTextureInterval;
		pParticleDesc->bMultiTextureRepeat = pIter->bMultiTextureRepeat;
		_tcscpy_s(pParticleDesc->szTextureName, pIter->szTextureName);
		_tcscpy_s(pParticleDesc->szTextureProto, pIter->szTextureProto);
		_tcscpy_s(pParticleDesc->szTexturePath, pIter->szTexturePath);


		for (auto& pShaderIter : pIter->tParticleShaderDesc.vecShaderName)
		{
			SHADERNAME* tShaderName = new SHADERNAME;
			_tcscpy_s(tShaderName->szName, pShaderIter->szName);
			pParticleDesc->tParticleShaderDesc.vecShaderName.push_back(tShaderName);
		}
		
		pParticleDesc->tPointInst_Desc = pIter->tPointInst_Desc;
		tGroupDesc.tParticleGroupDesc.listParticleDesc.emplace_back(pParticleDesc);
	}
}

HRESULT CEffectFactory::NativeConstruct(_pGraphicDevice _pGraphicDevice, _pContextDevice _pContextDevice)
{
	m_pGraphicDevice = _pGraphicDevice; 
	Safe_AddRef(m_pGraphicDevice);
	m_pContextDevice = _pContextDevice; 
	Safe_AddRef(m_pContextDevice);



	return S_OK;
}


CGameObject* CEffectFactory::Load_Effect(const _tchar * szPath, EFFECT_PACKET* pPacket, const _tchar* szLayer)
{
	EFFECT_DESC tDesc;
	Load_Effect_FromMap(szPath, tDesc.tGroupDesc);

	tDesc.tEffectPacket = *pPacket;

	CGameObject* pOut = nullptr;

	m_pGameInstance->Add_GameObjectToLayerWithPtr((_uint)pPacket->eLevel, L"Prototype_GameObject_MeshEffect", szLayer, &pOut, &tDesc);
	if (!pOut)
		assert(FALSE);

 	for (auto& pIter : tDesc.tGroupDesc.tEffectMeshGroupDesc.listEffectMeshDesc)
	{
		_int iShaderCount = (_int)pIter->tEffectMeshShader.vecShaderName.size();
		for (_int j = 0; j < iShaderCount; ++j)
			Safe_Delete(pIter->tEffectMeshShader.vecShaderName[j]);
		pIter->tEffectMeshShader.vecShaderName.clear();
		Safe_Delete(pIter);
	}
	tDesc.tGroupDesc.tEffectMeshGroupDesc.listEffectMeshDesc.clear();


	for (auto& pIter : tDesc.tGroupDesc.tParticleGroupDesc.listParticleDesc)
	{
		for (_int i = 0; i < pIter->tParticleShaderDesc.vecShaderName.size(); ++i)
		{
			Safe_Delete(pIter->tParticleShaderDesc.vecShaderName[i]);
		}
		pIter->tParticleShaderDesc.vecShaderName.clear();
		Safe_Delete(pIter);
	}
	tDesc.tGroupDesc.tParticleGroupDesc.listParticleDesc.clear();


	return pOut;
}

CGameObject * CEffectFactory::Load_EffectBullet(const _tchar * szPath, EFFECT_PACKET* pPacket, const _tchar* pTag)
{
	EFFECT_DESC	tEffectDesc;
	tEffectDesc.tGroupDesc;

	Load_Effect_FromMap(szPath, tEffectDesc.tGroupDesc);

	tEffectDesc.tEffectPacket.ulID = pPacket->ulID;
	tEffectDesc.tEffectPacket.tEffectMovement = pPacket->tEffectMovement;
	tEffectDesc.tEffectPacket.eLevel= pPacket->eLevel;
	tEffectDesc.tEffectPacket.fMaxLifeTime = pPacket->fMaxLifeTime;
	tEffectDesc.tEffectPacket.iAtt = pPacket->iAtt;
	tEffectDesc.tEffectPacket.iNavIndex = pPacket->iNavIndex;
	tEffectDesc.tEffectPacket.fColliderRadius = pPacket->fColliderRadius;
	tEffectDesc.tEffectPacket.bForMiniGame = pPacket->bForMiniGame;

	CGameObject* pOut = nullptr;
	m_pGameInstance->Add_GameObjectToLayerWithPtr((_uint)pPacket->eLevel, L"Prototype_GameObject_EffectBullet", pTag, &pOut, &tEffectDesc);
	if (!pOut)
		assert(FALSE);

	dynamic_cast<CEffectBullet*>(pOut)->Set_Movement(pPacket->tEffectMovement);



	for (auto& pIter : tEffectDesc.tGroupDesc.tEffectMeshGroupDesc.listEffectMeshDesc)
	{
		_int iShaderCount = (_int)pIter->tEffectMeshShader.vecShaderName.size();
		for (_int j = 0; j < iShaderCount; ++j)
			Safe_Delete(pIter->tEffectMeshShader.vecShaderName[j]);
		pIter->tEffectMeshShader.vecShaderName.clear();
		Safe_Delete(pIter);
	}
	tEffectDesc.tGroupDesc.tEffectMeshGroupDesc.listEffectMeshDesc.clear();


	for (auto& pIter : tEffectDesc.tGroupDesc.tParticleGroupDesc.listParticleDesc)
		Safe_Delete(pIter);
	tEffectDesc.tGroupDesc.tParticleGroupDesc.listParticleDesc.clear();


	return pOut;
}

CGameObject * CEffectFactory::Load_EffectProto(const _tchar * szPath, EFFECT_PACKET * pPacket, const _tchar * szProto, const _tchar * szLayer)
{
	EFFECT_DESC tDesc;
	Load_Effect_FromMap(szPath, tDesc.tGroupDesc);

	tDesc.tEffectPacket = *pPacket;

	CGameObject* pOut = nullptr;

	m_pGameInstance->Add_GameObjectToLayerWithPtr((_uint)pPacket->eLevel, szProto, szLayer, &pOut, &tDesc);

	if (!pOut)
		assert(FALSE);

	for (auto& pIter : tDesc.tGroupDesc.tEffectMeshGroupDesc.listEffectMeshDesc)
	{
		_int iShaderCount = (_int)pIter->tEffectMeshShader.vecShaderName.size();
		for (_int j = 0; j < iShaderCount; ++j)
			Safe_Delete(pIter->tEffectMeshShader.vecShaderName[j]);
		pIter->tEffectMeshShader.vecShaderName.clear();
		Safe_Delete(pIter);
	}
	tDesc.tGroupDesc.tEffectMeshGroupDesc.listEffectMeshDesc.clear();


	for (auto& pIter : tDesc.tGroupDesc.tParticleGroupDesc.listParticleDesc)
	{
		for (_int i = 0; i < pIter->tParticleShaderDesc.vecShaderName.size(); ++i)
		{
			Safe_Delete(pIter->tParticleShaderDesc.vecShaderName[i]);
		}
		pIter->tParticleShaderDesc.vecShaderName.clear();
		Safe_Delete(pIter);
	}
	tDesc.tGroupDesc.tParticleGroupDesc.listParticleDesc.clear();


	return pOut;
}

void CEffectFactory::Load_Effect_Component_FromData(const _tchar * szPath)
{
	HANDLE hFile = CreateFile(szPath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (INVALID_HANDLE_VALUE == hFile)
		return;

	EffectParticleGroupDescription tGroupDesc;
	EffectMeshDescription tMeshDesc;

	_ulong	dwByte = 0;

	_int iSize = 0;
	ReadFile(hFile, &iSize, sizeof(_int), &dwByte, nullptr);

	for (_int i = 0; i < iSize; ++i)
	{
		TEXNAMEPROTOPATH* pTex = new TEXNAMEPROTOPATH;

		ReadFile(hFile, pTex->szName, sizeof(TCHAR) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, pTex->szPath, sizeof(TCHAR) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, pTex->szProto, sizeof(TCHAR) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, &pTex->iTextureNumber, sizeof(_int), &dwByte, nullptr);

		_bool bOverlap = false;
		for (auto& pIter : m_vecTexProtoPath)
		{
			if (!_tcscmp(pIter->szName, pTex->szName))
			{
				bOverlap = true;
				break;
			}
		}
		if (bOverlap)
		{
			Safe_Delete(pTex);
			continue;
		}
		m_vecTexProtoPath.push_back(pTex);

		// Change Path to Client From Tool
		wstring wstr(dynamic_cast<TEXNAMEPROTOPATH*>(pTex)->szPath);
		wstring::size_type wstrRemover;
		wstrRemover = wstr.find(L"Client\\", 0);
		wstr.erase(wstrRemover, 7);
		lstrcpy(dynamic_cast<TEXNAMEPROTOPATH*>(pTex)->szPath, wstr.c_str());

		m_pGameInstance->Add_Component((_uint)LEVEL::STATIC, pTex->szProto, CTexture::Create(m_pGraphicDevice, m_pContextDevice, pTex->szPath, pTex->iTextureNumber));
		// Load Texture
		//if (FAILED())
		//{
		//	int a = 0;
		//}   

	}

	ReadFile(hFile, &iSize, sizeof(_int), &dwByte, nullptr);
	for (_int i = 0; i < iSize; ++i)
	{
		MODELPATHNAME* pModel = new MODELPATHNAME;
		ReadFile(hFile, dynamic_cast<MODELPATHNAME*>(pModel)->szName, sizeof(TCHAR) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, dynamic_cast<MODELPATHNAME*>(pModel)->szPath, sizeof(TCHAR) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, dynamic_cast<MODELPATHNAME*>(pModel)->szProto, sizeof(TCHAR) * MAX_PATH, &dwByte, nullptr);


		_bool bOverlap = false;
		for (auto& pIter : m_vecModelPathName)
		{
			if (!_tcscmp(pIter->szName, pModel->szName))
			{
				bOverlap = true;
				break;
			}
		}
		if (bOverlap)
		{
			Safe_Delete(pModel);
			continue;
		}

		m_vecModelPathName.push_back(pModel);

		// Change Path to Client From Tool
		wstring wstr(dynamic_cast<MODELPATHNAME*>(pModel)->szPath);
		wstring::size_type wstrRemover;
		wstrRemover = wstr.find(L"Client\\", 0);
		wstr.erase(wstrRemover, 7);
		lstrcpy(dynamic_cast<MODELPATHNAME*>(pModel)->szPath, wstr.c_str());

		if (FAILED(m_pGameInstance->Add_Component((_uint)LEVEL::STATIC, pModel->szProto,
			CModel::Create(m_pGraphicDevice, m_pContextDevice, pModel->szPath,
				TEXT("../Bin/ShaderFiles/Shader_EffectMesh.hlsl"), "DefaultTechnique"))))
		{
			int a = 0;
		}
	}

	CloseHandle(hFile);
}

void CEffectFactory::Load_Effect_Component_FromData_ToMap(const _tchar * szPath)
{
	static _int iCount = 0;
	++iCount;


	HANDLE hFile = CreateFile(szPath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		assert(false);
		return;
	}

	//EffectMeshDescription tMeshDesc;

	EFFECT_LOAD_INFO* pEffectLoadInfo = new EFFECT_LOAD_INFO;
	m_mapLoadedEffectInfo[szPath] = pEffectLoadInfo;

	
	_ulong	dwByte = 0;

	_int iSize = 0;
	ReadFile(hFile, &iSize, sizeof(_int), &dwByte, nullptr);

	for (_int i = 0; i < iSize; ++i)
	{
		TEXNAMEPROTOPATH* pTex = new TEXNAMEPROTOPATH;

		ReadFile(hFile, pTex->szName, sizeof(TCHAR) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, pTex->szPath, sizeof(TCHAR) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, pTex->szProto, sizeof(TCHAR) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, &pTex->iTextureNumber, sizeof(_int), &dwByte, nullptr);

		m_mapLoadedEffectInfo[szPath]->vecTextureName.emplace_back(pTex->szName);

		_bool bOverlap = false;
		for (auto& pIter : m_vecTexProtoPath)
		{
			if (!_tcscmp(pIter->szName, pTex->szName))
			{
				bOverlap = true;
				break;
			}
		}
		if (bOverlap)
		{
			Safe_Delete(pTex);
			continue;
		}
		m_vecTexProtoPath.push_back(pTex);



		// Change Path to Client From Tool
		wstring wstr(dynamic_cast<TEXNAMEPROTOPATH*>(pTex)->szPath);
		wstring::size_type wstrRemover;
		wstrRemover = wstr.find(L"Client\\", 0);
		wstr.erase(wstrRemover, 7);
		lstrcpy(dynamic_cast<TEXNAMEPROTOPATH*>(pTex)->szPath, wstr.c_str());

		m_pGameInstance->Add_Component((_uint)LEVEL::STATIC, pTex->szProto, CTexture::Create(m_pGraphicDevice, m_pContextDevice, pTex->szPath, pTex->iTextureNumber));


	}

	ReadFile(hFile, &iSize, sizeof(_int), &dwByte, nullptr);
	for (_int i = 0; i < iSize; ++i)
	{
		MODELPATHNAME* pModel = new MODELPATHNAME;
		ReadFile(hFile, dynamic_cast<MODELPATHNAME*>(pModel)->szName, sizeof(TCHAR) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, dynamic_cast<MODELPATHNAME*>(pModel)->szPath, sizeof(TCHAR) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, dynamic_cast<MODELPATHNAME*>(pModel)->szProto, sizeof(TCHAR) * MAX_PATH, &dwByte, nullptr);

		m_mapLoadedEffectInfo[szPath]->vecModelName.emplace_back(pModel->szName);

		_bool bOverlap = false;
		for (auto& pIter : m_vecModelPathName)
		{
			if (!_tcscmp(pIter->szName, pModel->szName))
			{
				bOverlap = true;
				break;
			}
		}
		if (bOverlap)
		{
			Safe_Delete(pModel);
			continue;
		}

		m_vecModelPathName.push_back(pModel);

		// Change Path to Client From Tool
		wstring wstr(dynamic_cast<MODELPATHNAME*>(pModel)->szPath);
		wstring::size_type wstrRemover;
		wstrRemover = wstr.find(L"Client\\", 0);
		wstr.erase(wstrRemover, 7);
		lstrcpy(dynamic_cast<MODELPATHNAME*>(pModel)->szPath, wstr.c_str());

		if (FAILED(m_pGameInstance->Add_Component((_uint)LEVEL::STATIC, pModel->szProto,
			CModel::Create(m_pGraphicDevice, m_pContextDevice, pModel->szPath,
				TEXT("../Bin/ShaderFiles/Shader_EffectMesh.hlsl"), "DefaultTechnique"))))
		{
			int a = 0;
		}
	}





	////////////////////////////////////////////////////////////////////////////////////////////




	ZeroMemory(&(m_mapLoadedEffectInfo[szPath]->tEffectGroupDesc.tEffectMeshGroupDesc.szGroupName), MAX_PATH * sizeof(_tchar));
	ReadFile(hFile, &m_mapLoadedEffectInfo[szPath]->tEffectGroupDesc.tEffectMeshGroupDesc.szGroupName, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	
	iSize = 0;
	ReadFile(hFile, &iSize, sizeof(_int), &dwByte, nullptr);

	_int _iShaderCount = 0;
	EFFECTMESHDESC* pDesc = nullptr;
	for (size_t i = 0; i < iSize; i++)
	{
		pDesc = new EFFECTMESHDESC;


		ReadFile(hFile, &pDesc->tInfo, sizeof(EFFECTMESHDESC::EffectMeshInfo), &dwByte, nullptr);
		ReadFile(hFile, &pDesc->tTexture, sizeof(EFFECTMESHDESC::EFFECTMESHTEXTURE), &dwByte, nullptr);
		ReadFile(hFile, &pDesc->tPattern, sizeof(EFFECTMESHDESC::EffectMeshPattern), &dwByte, nullptr);

		ReadFile(hFile, &pDesc->tFade_InOut, sizeof(EFFECTMESHDESC::Fade_InOut), &dwByte, nullptr);

		ReadFile(hFile, &pDesc->tUV_Sprite, sizeof(EFFECTMESHDESC::UV_SPRITE), &dwByte, nullptr);
		ReadFile(hFile, &pDesc->tUV_Animation, sizeof(EFFECTMESHDESC::UV_ANIMATION), &dwByte, nullptr);

		_int iVecSize = 0;
		ReadFile(hFile, &iVecSize, sizeof(_int), &dwByte, nullptr);
		for (_int i = 0; i < iVecSize; ++i)
		{
			SHADERNAME* tShaderName = new SHADERNAME;
			ZeroMemory(tShaderName, sizeof(SHADERNAME));
			TCHAR szTemp[MAX_PATH] = L"";

			ReadFile(hFile, &szTemp, sizeof(TCHAR) * MAX_PATH, &dwByte, nullptr);
			_tcscpy_s(tShaderName->szName, szTemp);

			pDesc->tEffectMeshShader.vecShaderName.emplace_back(tShaderName);
		}
		ReadFile(hFile, &pDesc->tEffectMeshShader.fDissolveSpeed, sizeof(_float), &dwByte, nullptr);
		m_mapLoadedEffectInfo[szPath]->tEffectGroupDesc.tEffectMeshGroupDesc.listEffectMeshDesc.emplace_back(pDesc);
	}

	ReadFile(hFile, &m_mapLoadedEffectInfo[szPath]->tEffectGroupDesc.tEffectMeshGroupDesc.bCreateDeathRepeat, sizeof(_bool), &dwByte, nullptr);
	ReadFile(hFile, &m_mapLoadedEffectInfo[szPath]->tEffectGroupDesc.tEffectMeshGroupDesc.vCreateDeathTime, sizeof(_float2), &dwByte, nullptr);

	// Load Particle
	_int iParticleSize = 0;
	ReadFile(hFile, &iParticleSize, sizeof(_int), &dwByte, nullptr);
	for (_int i = 0; i < iParticleSize; i++)
	{
		PARTICLEDESC* pParticleDesc = new PARTICLEDESC;
		TCHAR szTemp[MAX_PATH] = L"";
		ReadFile(hFile, &szTemp, sizeof(TCHAR) * MAX_PATH, &dwByte, nullptr);
		_tcscpy_s(pParticleDesc->szParticleName, szTemp);
		ReadFile(hFile, &pParticleDesc->vPosition, sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFile, &pParticleDesc->vScale, sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFile, &pParticleDesc->vRotation, sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFile, &pParticleDesc->iTextureNumber, sizeof(_int), &dwByte, nullptr);
		ReadFile(hFile, &szTemp, sizeof(TCHAR) * MAX_PATH, &dwByte, nullptr);
		_tcscpy_s(pParticleDesc->szTextureName, szTemp);
		ReadFile(hFile, &szTemp, sizeof(TCHAR) * MAX_PATH, &dwByte, nullptr);
		_tcscpy_s(pParticleDesc->szTextureProto, szTemp);
		ReadFile(hFile, &szTemp, sizeof(TCHAR) * MAX_PATH, &dwByte, nullptr);
		_tcscpy_s(pParticleDesc->szTexturePath, szTemp);
		ReadFile(hFile, &pParticleDesc->fMultiTextureInterval, sizeof(_float), &dwByte, nullptr);
		ReadFile(hFile, &pParticleDesc->bMultiTextureRepeat, sizeof(_bool), &dwByte, nullptr);

		_int iCount = 0;
		ReadFile(hFile, &iCount, sizeof(_int), &dwByte, nullptr);

		for (_int i = 0; i < iCount; ++i)
		{
			SHADERNAME* tShaderName = new SHADERNAME;
			ZeroMemory(tShaderName, sizeof(SHADERNAME));

			ReadFile(hFile, &szTemp, sizeof(TCHAR) * MAX_PATH, &dwByte, nullptr);
			_tcscpy_s(tShaderName->szName, szTemp);
			pParticleDesc->tParticleShaderDesc.vecShaderName.push_back(tShaderName);
		}

		ReadFile(hFile, &pParticleDesc->tPointInst_Desc, sizeof(POINTINST_DESC), &dwByte, nullptr);
		m_mapLoadedEffectInfo[szPath]->tEffectGroupDesc.tParticleGroupDesc.listParticleDesc.emplace_back(pParticleDesc);
	}



	CloseHandle(hFile);
}

void CEffectFactory::Free()
{
	for (auto& pIter : m_mapLoadedEffectInfo)
	{
		pIter.second->vecModelName.clear();
		pIter.second->vecTextureName.clear();
		for (auto& pEffectMeshIter : pIter.second->tEffectGroupDesc.tEffectMeshGroupDesc.listEffectMeshDesc)
		{
			_int iShaderCount = static_cast<_int>(pEffectMeshIter->tEffectMeshShader.vecShaderName.size());
			for (_int i = 0; i < iShaderCount; ++i) 
			{
				Safe_Delete(pEffectMeshIter->tEffectMeshShader.vecShaderName[i]);
			}
			pEffectMeshIter->tEffectMeshShader.vecShaderName.clear();
			Safe_Delete(pEffectMeshIter);
		}
		pIter.second->tEffectGroupDesc.tEffectMeshGroupDesc.listEffectMeshDesc.clear();

		for (auto& pParticleIter : pIter.second->tEffectGroupDesc.tParticleGroupDesc.listParticleDesc)
		{
			for (auto& pShaderNameIter : pParticleIter->tParticleShaderDesc.vecShaderName)
			{
				Safe_Delete(pShaderNameIter);
			}
			_int iShaderCount = static_cast<_int>(pParticleIter->tParticleShaderDesc.vecShaderName.size());
			for (_int i = 0; i < iShaderCount; ++i)
			{
				Safe_Delete(pParticleIter->tParticleShaderDesc.vecShaderName[i]);
			}
			pParticleIter->tParticleShaderDesc.vecShaderName.clear();
			Safe_Delete(pParticleIter);
		}
		pIter.second->tEffectGroupDesc.tParticleGroupDesc.listParticleDesc.clear();
		Safe_Delete(pIter.second);
	}
	m_mapLoadedEffectInfo.clear();



	for (auto& pIter : m_vecTexProtoPath)
		Safe_Delete(pIter);
	m_vecTexProtoPath.clear();

	for (auto& pIter : m_vecModelPathName)
		Safe_Delete(pIter);
	m_vecModelPathName.clear();

	Safe_Release(m_pGameInstance);

	Safe_Release(m_pGraphicDevice);
	Safe_Release(m_pContextDevice);


}
