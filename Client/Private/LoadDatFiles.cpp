#include "stdafx.h"
#include "..\Public\LoadDatFiles.h"
#include "GameInstance.h"
#include "Humanoid_Small.h"
#include "Humanoid_Big.h"
#include "Humanoid_Dragoon.h"
#include "Tank.h"
#include "Beauvoir.h"
#include "TreasureBox.h"
#include "ZhuangziDoor.h"
#include "ZhuangziBridge.h"
#include "Level_Stage1.h"
#include "VendingMachine.h"

IMPLEMENT_SINGLETON(CLoadDatFiles)

int CLoadDatFiles::m_iClownCountStatic = 0;

CLoadDatFiles::CLoadDatFiles()
{

}

_uint APIENTRY LOADINFO(LPVOID pArg)
{
	LOADDATA* pData = (LOADDATA*)pArg;

	LAYER eLayer = pData->eLayer;

	EnterCriticalSection(&CLoadDatFiles::Get_Instance()->Get_CS_Scene(eLayer));

	HRESULT hr = 0;
	switch (eLayer)
	{
	case Client::LAYER::BACKGROUND:
		hr = CLoadDatFiles::Get_Instance()->Load_Background(pData);
		break;
	case Client::LAYER::OBSTACLE:
		hr = CLoadDatFiles::Get_Instance()->Load_Obstacle(pData);
		break;
	case Client::LAYER::ENVIRONMENT:
		hr = CLoadDatFiles::Get_Instance()->Load_Environment(pData);
		break;
	case Client::LAYER::MONSTER:
		hr = CLoadDatFiles::Get_Instance()->Load_Monster(pData);
		break;
	case Client::LAYER::BOSS:
		hr = CLoadDatFiles::Get_Instance()->Load_Boss(pData);
		break;
	case Client::LAYER::COUNT:
		break;
	default:
		break;
	}

	LeaveCriticalSection(&CLoadDatFiles::Get_Instance()->Get_CS_Scene(eLayer));
	return 0;
}

_uint APIENTRY CreateDatFile(LPVOID pArg)
{
	LOADMODELDATA* pData = (LOADMODELDATA*)pArg;

	EnterCriticalSection(&CLoadDatFiles::Get_Instance()->Get_CS_Model());

	HRESULT hr = 0;

	hr = CLoadDatFiles::Get_Instance()->Load_Model(pData);
	
	LeaveCriticalSection(&CLoadDatFiles::Get_Instance()->Get_CS_Model());
	return 0;
}

_uint APIENTRY CreateDynamicDatFile(LPVOID pArg)
{
	LOADMODELDATA* pData = (LOADMODELDATA*)pArg;

	EnterCriticalSection(&CLoadDatFiles::Get_Instance()->Get_CS_Model());

	HRESULT hr = 0;

	hr = CLoadDatFiles::Get_Instance()->Load_Dynamic_Model(pData);

	LeaveCriticalSection(&CLoadDatFiles::Get_Instance()->Get_CS_Model());
	return 0;
}

void CLoadDatFiles::Init(_pGraphicDevice pGraphicDeivce, _pContextDevice pContextDevice)
{
	m_pGraphicDevice = pGraphicDeivce;
	Safe_AddRef(m_pGraphicDevice);
	m_pContextDevice = pContextDevice;
	Safe_AddRef(m_pContextDevice);

	LoadStageDatFile();
}

HRESULT CLoadDatFiles::LoadStageDatFile()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	_finddata_t fd;

	__int64 handle = _findfirst("../Bin/Data/SceneData/*.dat", &fd);
	int Check = 0;
	int iResult = 0;
	_uint iIndex = 0;
	char szCurPath[128] = "../Bin/Data/SceneData/";
	char szFullPath[128] = "";

	while (iResult != -1)
	{
		strcpy_s(szFullPath, szCurPath);
		strcat_s(szFullPath, fd.name);

		_tchar	szDataFilePath[MAX_PATH] = TEXT("");
		MultiByteToWideChar(CP_ACP, 0, szFullPath, (int)strlen(szFullPath), szDataFilePath, 260);

		LoadStageInfo(szDataFilePath);
#ifdef _DEBUG
		string strTag(fd.name);
		GRAPHICDEBUGGER()->Notice("Load ", CGraphicDebugger::COLOR::WHITE
			, strTag.c_str(), CGraphicDebugger::COLOR::DARKSKY
			, " Scene Info", CGraphicDebugger::COLOR::WHITE);
#endif // _DEBUG

		iResult = _findnext(handle, &fd);
	}
	_findclose(handle);

	return S_OK;
}

HRESULT CLoadDatFiles::LoadStageInfo(const _tchar * pDatFilePath)
{
	DWORD dwByte = 0;
	HANDLE hFile = CreateFile(pDatFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (hFile == INVALID_HANDLE_VALUE)
		FAILMSG("Failed to Read Stage Data");

	SCENE_INFO		tRootInfo;
	_uint			iLayerSize = 0;

	ReadFile(hFile, &tRootInfo, sizeof(SCENE_INFO), &dwByte, nullptr);
	ReadFile(hFile, &iLayerSize, sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < iLayerSize; ++i)
	{
		SCENE_INFO	tLayerInfo;
		_uint		iObjectSize;

		ReadFile(hFile, &tLayerInfo, sizeof(SCENE_INFO), &dwByte, nullptr);
		ReadFile(hFile, &iObjectSize, sizeof(_uint), &dwByte, nullptr);

		vector<MO_INFO*>	vectorInfoList;
		for (_uint j = 0; j < iObjectSize; ++j)
		{
			MO_INFO* tObjectInfo = new MO_INFO;
			ReadFile(hFile, tObjectInfo, sizeof(MO_INFO), &dwByte, nullptr);

			vectorInfoList.push_back(tObjectInfo);
		}
		wstring wstr = tLayerInfo.szSceneName;
		m_strStageName.push_back(wstr);
		auto iter = find_if(m_mapWorld.begin(), m_mapWorld.end(), CStringFinder(wstring(tRootInfo.szSceneName)));

		if (iter == m_mapWorld.end())
		{
			OBJECTSINFO		mapObjectsInfo;
			mapObjectsInfo.emplace(wstr, vectorInfoList);
			m_mapWorld.emplace(wstring(tRootInfo.szSceneName), mapObjectsInfo);
		}
		else
		{
			iter->second.emplace(wstr, vectorInfoList);
		}
	}


	return S_OK;
}

HRESULT CLoadDatFiles::Load_Object_Info(LEVEL eLevel, wstring pStageTag)
{
	if (m_pSceneLoad != nullptr)
	{
		for (_uint i = 0; i < (_uint)LAYER::COUNT; ++i)
			Safe_Delete(m_pSceneLoad[i]);
	}

	map<wstring, vector<MO_INFO*>>* pList = Get_StageInfo(pStageTag);
	if (pList == nullptr)
		return E_FAIL;

	m_bSceneLoad = false;

	for (_uint i = 0; i < (_uint)LAYER::COUNT; ++i)
		m_pSceneLoad[i] = new LOADDATA;

	for (_uint i = 0; i < (_uint)LAYER::COUNT; ++i)
		InitializeCriticalSection(&m_CS_Scene[i]);

	m_pSceneLoad[(_uint)LAYER::BACKGROUND]->eLayer = LAYER::BACKGROUND;
	m_pSceneLoad[(_uint)LAYER::OBSTACLE]->eLayer = LAYER::OBSTACLE;
	m_pSceneLoad[(_uint)LAYER::ENVIRONMENT]->eLayer = LAYER::ENVIRONMENT;
	m_pSceneLoad[(_uint)LAYER::MONSTER]->eLayer = LAYER::MONSTER;
	m_pSceneLoad[(_uint)LAYER::BOSS]->eLayer = LAYER::BOSS;
	
	auto mapBackground = find_if((*pList).begin(), (*pList).end(), CStringFinder(wstring(L"Background")));
	if (mapBackground != (*pList).end())
	{
		m_pSceneLoad[(_uint)LAYER::BACKGROUND]->eLevel = eLevel;
		m_pSceneLoad[(_uint)LAYER::BACKGROUND]->iSize = mapBackground->second.size();
		m_pSceneLoad[(_uint)LAYER::BACKGROUND]->vecInfo.reserve(m_pSceneLoad[(_uint)LAYER::BACKGROUND]->iSize);
		for (auto& vector : mapBackground->second)
			m_pSceneLoad[(_uint)LAYER::BACKGROUND]->vecInfo.push_back(vector);
	}

	auto mapObstacle = find_if((*pList).begin(), (*pList).end(), CStringFinder(wstring(L"Obstacle")));
	if (mapObstacle != (*pList).end())
	{
		m_pSceneLoad[(_uint)LAYER::OBSTACLE]->eLevel = eLevel;
		m_pSceneLoad[(_uint)LAYER::OBSTACLE]->iSize = mapObstacle->second.size();
		m_pSceneLoad[(_uint)LAYER::OBSTACLE]->vecInfo.reserve(m_pSceneLoad[(_uint)LAYER::OBSTACLE]->iSize);
		for (auto& vector : mapObstacle->second)
			m_pSceneLoad[(_uint)LAYER::OBSTACLE]->vecInfo.push_back(vector);
	}

	auto mapEnvironment = find_if((*pList).begin(), (*pList).end(), CStringFinder(wstring(L"Environment")));
	if (mapEnvironment != (*pList).end())
	{
		m_pSceneLoad[(_uint)LAYER::ENVIRONMENT]->eLevel = eLevel;
		m_pSceneLoad[(_uint)LAYER::ENVIRONMENT]->iSize = mapEnvironment->second.size();
		m_pSceneLoad[(_uint)LAYER::ENVIRONMENT]->vecInfo.reserve(m_pSceneLoad[(_uint)LAYER::ENVIRONMENT]->iSize);
		for (auto& vector : mapEnvironment->second)
			m_pSceneLoad[(_uint)LAYER::ENVIRONMENT]->vecInfo.push_back(vector);
	}

	auto mapMonster = find_if((*pList).begin(), (*pList).end(), CStringFinder(wstring(L"Monster")));
	if (mapMonster != (*pList).end())
	{
		m_pSceneLoad[(_uint)LAYER::MONSTER]->eLevel = eLevel;
		m_pSceneLoad[(_uint)LAYER::MONSTER]->iSize = mapMonster->second.size();
		m_pSceneLoad[(_uint)LAYER::MONSTER]->vecInfo.reserve(m_pSceneLoad[(_uint)LAYER::MONSTER]->iSize);
		for (auto& vector : mapMonster->second)
			m_pSceneLoad[(_uint)LAYER::MONSTER]->vecInfo.push_back(vector);
	}

	auto mapBoss = find_if((*pList).begin(), (*pList).end(), CStringFinder(wstring(L"Boss")));
	if (mapBoss != (*pList).end())
	{
		m_pSceneLoad[(_uint)LAYER::BOSS]->eLevel = eLevel;
		m_pSceneLoad[(_uint)LAYER::BOSS]->iSize = mapBoss->second.size();
		m_pSceneLoad[(_uint)LAYER::BOSS]->vecInfo.reserve(m_pSceneLoad[(_uint)LAYER::BOSS]->iSize);
		for (auto& vector : mapBoss->second)
			m_pSceneLoad[(_uint)LAYER::BOSS]->vecInfo.push_back(vector);
	}

	m_hThread_Scene[(_uint)LAYER::BACKGROUND] = (HANDLE)_beginthreadex(nullptr, 0, LOADINFO, m_pSceneLoad[(_uint)LAYER::BACKGROUND], 0, nullptr);
	m_hThread_Scene[(_uint)LAYER::OBSTACLE] = (HANDLE)_beginthreadex(nullptr, 0, LOADINFO, m_pSceneLoad[(_uint)LAYER::OBSTACLE], 0, nullptr);
	m_hThread_Scene[(_uint)LAYER::ENVIRONMENT] = (HANDLE)_beginthreadex(nullptr, 0, LOADINFO, m_pSceneLoad[(_uint)LAYER::ENVIRONMENT], 0, nullptr);
	m_hThread_Scene[(_uint)LAYER::MONSTER] = (HANDLE)_beginthreadex(nullptr, 0, LOADINFO, m_pSceneLoad[(_uint)LAYER::MONSTER], 0, nullptr);
	m_hThread_Scene[(_uint)LAYER::BOSS] = (HANDLE)_beginthreadex(nullptr, 0, LOADINFO, m_pSceneLoad[(_uint)LAYER::BOSS], 0, nullptr);

	WaitForMultipleObjects((_uint)LAYER::COUNT, m_hThread_Scene, TRUE, INFINITE);

	for (_uint i = 0; i < (_uint)LAYER::COUNT; ++i)
		DeleteCriticalSection(&m_CS_Scene[i]);

	for (_uint i = 0; i < (_uint)LAYER::COUNT; ++i)
	{
		DeleteObject(m_hThread_Scene[i]);
		CloseHandle(m_hThread_Scene[i]);
	}

	return S_OK;
}

HRESULT CLoadDatFiles::Load_ModelDat_File(LEVEL eLevel, const char * pDatFilePath)
{
	if (m_pModelLoad != nullptr)
		Safe_Delete(m_pModelLoad);
	InitializeCriticalSection(&m_CS_Model);

	m_pModelLoad = new LOADMODELDATA;

	m_pModelLoad->eLevel = eLevel;
	m_pModelLoad->pTag = pDatFilePath;

	m_hThread_Model = (HANDLE)_beginthreadex(nullptr, 0, CreateDatFile, m_pModelLoad, 0, nullptr);

	WaitForSingleObject(m_hThread_Model, INFINITE);

	DeleteCriticalSection(&m_CS_Model);
	DeleteObject(m_hThread_Model);
	CloseHandle(m_hThread_Model);
	return S_OK;
}

HRESULT CLoadDatFiles::Load_Dynamic_ModelDat_File(LEVEL eLevel, const char * pDatFilePath)
{
	if (m_pModelLoad != nullptr)
		Safe_Delete(m_pModelLoad);
	InitializeCriticalSection(&m_CS_Model);

	m_pModelLoad = new LOADMODELDATA;

	m_pModelLoad->eLevel = eLevel;
	m_pModelLoad->pTag = pDatFilePath;

	m_hThread_Model = (HANDLE)_beginthreadex(nullptr, 0, CreateDynamicDatFile, m_pModelLoad, 0, nullptr);

	WaitForSingleObject(m_hThread_Model, INFINITE);

	DeleteCriticalSection(&m_CS_Model);
	DeleteObject(m_hThread_Model);
	CloseHandle(m_hThread_Model);
	return S_OK;
}

HRESULT CLoadDatFiles::Load_Background(LOADDATA* pData)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	for (size_t i = 0; i < pData->iSize; ++i)
	{
		auto& iter = pData->vecInfo.begin() + i;
		if (FAILED(pGameInstance->Add_GameObjectToLayer((_uint)pData->eLevel, PROTO_KEY_BUILDING, L"Background", *iter)))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CLoadDatFiles::Load_Obstacle(LOADDATA * pData)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	_uint iBridgeCount = 0, iDoorCount = 0;
	for (size_t i = 0; i < pData->iSize; ++i)
	{
		auto iter = pData->vecInfo.begin() + i;

		if (lstrcmp((*iter)->szModelObjectName, L"TreasureBox") == 0)
		{
			auto& tier = pData->vecInfo.begin() + i;
			if (FAILED(pGameInstance->Add_GameObjectToLayer((_uint)pData->eLevel, L"Prototype_GameObject_TreasureBox", L"Obstacle", *iter)))
				return E_FAIL;
		}

		if (lstrcmp((*iter)->szModelObjectName, L"ZhuangziBridge") == 0)
		{
			auto& tier = pData->vecInfo.begin() + i;

			CZhuangziBridge* pObject = nullptr;
			if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr((_uint)pData->eLevel, PROTO_KEY_ZHUANGZIBRIDGE, L"ZhuangziBridge", (CGameObject**)&pObject, *iter)))
				return E_FAIL;

			if (nullptr == pObject)
				return E_FAIL;

			if (EXITBRIDGE_LAYERINDEX == iBridgeCount)
				pObject->Set_Exit(true);

			++iBridgeCount;
		}

		if (lstrcmp((*iter)->szModelObjectName, L"ZhuangziDoor") == 0)
		{
			auto& tier = pData->vecInfo.begin() + i;

			CZhuangziDoor* pObject = nullptr;
			if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr((_uint)pData->eLevel, PROTO_KEY_ZHUANGZIDOOR, L"ZhuangziDoor", (CGameObject**)&pObject, *iter)))
				return E_FAIL;

			if (nullptr == pObject)
				return E_FAIL;

			if (EXITDOOR_LAYERINDEX == iDoorCount)
				pObject->Set_Exit(true);

			++iDoorCount;
		}

		if (lstrcmp((*iter)->szModelObjectName, L"Coaster") == 0)
		{
			auto& tier = pData->vecInfo.begin() + i;
			if (FAILED(pGameInstance->Add_GameObjectToLayer((_uint)pData->eLevel, L"Coaster", L"Coasters", *iter)))
				return E_FAIL;
		}

		if (lstrcmp((*iter)->szModelObjectName, L"Curtain") == 0)
		{
			auto& iter = pData->vecInfo.begin() + i;
			if (FAILED(pGameInstance->Add_GameObjectToLayer((_uint)pData->eLevel, PROTO_KEY_BEAUVOIR_CURATIN, L"Obstacle", *iter)))
				return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CLoadDatFiles::Load_Environment(LOADDATA * pData)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	for (size_t i = 0; i < pData->iSize; ++i)
	{
		auto iter = pData->vecInfo.begin() + i;
		if (lstrcmp((*iter)->szModelObjectName, L"VendingMachine") == 0)
		{
			auto& iter = pData->vecInfo.begin() + i;
			if (FAILED(pGameInstance->Add_GameObjectToLayer((_uint)pData->eLevel, PROTO_KEY_VENDING_MACHINE, L"Environment", *iter)))
				return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CLoadDatFiles::Load_Monster(LOADDATA * pData)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	for (size_t i = 0; i < pData->iSize; ++i)
	{
		auto& iter = pData->vecInfo.begin() + i;

		if (lstrcmp((*iter)->szModelObjectName, L"Humanoid_Small") == 0)
		{
			if (FAILED(pGameInstance->Add_GameObjectToLayer((_uint)pData->eLevel, PROTO_KEY_HUMANOID_SMALL, L"Monster", *iter)))
				return E_FAIL;

			if (pData->eLevel == LEVEL::STAGE1)
			{
				++CLevel_Stage1::m_iMonsterCountStatic;
			}

		}
		if (lstrcmp((*iter)->szModelObjectName, L"Humanoid_Big") == 0)
		{
			if (FAILED(pGameInstance->Add_GameObjectToLayer((_uint)pData->eLevel, PROTO_KEY_HUMANOID_BIG, L"Monster", *iter)))
				return E_FAIL;

			if (pData->eLevel == LEVEL::STAGE1)
			{
				++CLevel_Stage1::m_iMonsterCountStatic;
			}

		}

		if (lstrcmp((*iter)->szModelObjectName, L"Humanoid_Clown") == 0)
		{
			if (FAILED(pGameInstance->Add_GameObjectToLayer((_uint)pData->eLevel, L"Humanoid_Clown", L"Monster", *iter)))
				return E_FAIL;

			++m_iClownCountStatic;
		}
	}

	return S_OK;
}

HRESULT CLoadDatFiles::Load_Boss(LOADDATA * pData)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	for (size_t i = 0; i < pData->iSize; ++i)
	{
		auto iter = pData->vecInfo.begin() + i;

		if (lstrcmp((*iter)->szModelObjectName, L"Tank") == 0)
		{
			if (FAILED(pGameInstance->Add_GameObjectToLayer((_uint)pData->eLevel, PROTO_KEY_TANK, L"Boss", *iter)))
				return E_FAIL;
		}

		if (lstrcmp((*iter)->szModelObjectName, L"Beauvoir") == 0)
		{
			if (FAILED(pGameInstance->Add_GameObjectToLayer((_uint)pData->eLevel, PROTO_KEY_BEAUVOIR, L"Boss", *iter)))
				return E_FAIL;
		}

		if (lstrcmp((*iter)->szModelObjectName, L"Zhuangzi") == 0)
		{
			if (FAILED(pGameInstance->Add_GameObjectToLayer((_uint)pData->eLevel, PROTO_KEY_ZHUANGZI, L"Boss", *iter)))
				return E_FAIL;
		}

		if (lstrcmp((*iter)->szModelObjectName, L"Engels") == 0)
		{
			if (FAILED(pGameInstance->Add_GameObjectToLayer((_uint)pData->eLevel, PROTO_KEY_ENGELS, L"Boss", *iter)))
				return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CLoadDatFiles::Load_Model(LOADMODELDATA * pData)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	_finddata_t fd;

	const char* pTag = pData->pTag;
	

	__int64 handle = _findfirst(pTag, &fd);
	int Check = 0;
	int iResult = 0;

	char szCurPath[128] = "";
	strcpy_s(szCurPath, pTag);

	char* pCurPath;
	strtok_s(szCurPath, "*", &pCurPath);
	char szFullPath[128] = "";
	while (iResult != -1)
	{
		strcpy_s(szFullPath, szCurPath);
		strcat_s(szFullPath, fd.name);

		_tchar	szMaterialFullPath[MAX_PATH] = TEXT("");
		MultiByteToWideChar(CP_ACP, 0, szFullPath, (int)strlen(szFullPath), szMaterialFullPath, 260);
		wstring wstr(fd.name, &fd.name[260]);
		wstring::size_type wstrRemover;
		wstrRemover = wstr.find(L".dat", 0);
		wstr.erase(wstrRemover, 4);

		m_strComponentName.push_back(wstr);

		auto& iter = m_strComponentName.begin() + m_iLoadIndex;
		if (FAILED(pGameInstance->Add_Component((_uint)pData->eLevel , iter->c_str(), CModel::Create(m_pGraphicDevice, m_pContextDevice, szMaterialFullPath, Shader_Model, DefaultTechnique))))
			return E_FAIL;

		iResult = _findnext(handle, &fd);

		++m_iLoadIndex;
	}
	_findclose(handle);

	return S_OK;
}

HRESULT CLoadDatFiles::Load_Dynamic_Model(LOADMODELDATA * pData)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	_finddata_t fd;

	__int64 handle = _findfirst(pData->pTag, &fd);
	int Check = 0;
	int iResult = 0;

	char szCurPath[128] = "";
	strcpy_s(szCurPath, pData->pTag);

	char* pCurPath;
	strtok_s(szCurPath, "*", &pCurPath);
	char szFullPath[128] = "";
	while (iResult != -1)
	{
		strcpy_s(szFullPath, szCurPath);
		strcat_s(szFullPath, fd.name);

		_tchar	szMaterialFullPath[MAX_PATH] = TEXT("");
		MultiByteToWideChar(CP_ACP, 0, szFullPath, (int)strlen(szFullPath), szMaterialFullPath, 260);
		wstring wstr(fd.name, &fd.name[260]);
		wstring::size_type wstrRemover;
		wstrRemover = wstr.find(L".dat", 0);
		wstr.erase(wstrRemover, 4);

		m_strComponentName.push_back(wstr);
		// Create -> Dynamic Create
		auto& iter = m_strComponentName.begin() + m_iLoadIndex;
		if (FAILED(pGameInstance->Add_Component((_uint)pData->eLevel, iter->c_str(), COctreeModel::Create(m_pGraphicDevice, m_pContextDevice, szMaterialFullPath, Shader_Model, DefaultTechnique))))
			return E_FAIL;

		iResult = _findnext(handle, &fd);

		++m_iLoadIndex;
	}
	_findclose(handle);

	return S_OK;
}

map<wstring, vector<MO_INFO*>>* CLoadDatFiles::Get_StageInfo(wstring pStageTag)
{
	auto iter = find_if(m_mapWorld.begin(), m_mapWorld.end(), CStringFinder(pStageTag));
	if (iter == m_mapWorld.end())
		return nullptr;

	return &(iter->second);
}

void CLoadDatFiles::Release_Device()
{
	unsigned long dwRefCnt = 0;

	dwRefCnt = Safe_Release(m_pGraphicDevice);
	dwRefCnt = Safe_Release(m_pContextDevice);
}

void CLoadDatFiles::Free()
{
	unsigned long dwRefCnt = 0;
	
	Safe_Delete(m_pModelLoad);
	LeaveCriticalSection(&m_CS_Model);
	DeleteCriticalSection(&m_CS_Model);
	for (_uint i = 0; i < (_uint)LAYER::COUNT; ++i)
	{
		Safe_Delete(m_pSceneLoad[i]);
		LeaveCriticalSection(&m_CS_Scene[i]);
		DeleteCriticalSection(&m_CS_Scene[i]);
	}
	
	for (auto map : m_mapWorld)
	{
		for (auto vector : map.second)
		{
			for (auto Data : vector.second)
				Safe_Delete(Data);
		}
		map.second.clear();
	}
	m_mapWorld.clear();
}
