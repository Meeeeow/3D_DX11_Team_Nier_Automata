#include "..\Public\GraphicDebugger.h"

#include <iostream>
#include <string.h>
#include "GameObject.h"
#include "GameObjectManager.h"
#include "ComponentManager.h"
#include "LevelManager.h"
#include "Light.h"

#include "LightManager.h"
#include "RenderTargetManager.h"

#include "GameInstance.h"

IMPLEMENT_SINGLETON(CGraphicDebugger)

CGraphicDebugger::CGraphicDebugger()
{
}

HRESULT CGraphicDebugger::Initialize(HWND hMainWnd)
{
	if (m_bConsole = AllocConsole())
	{
		m_hMainWnd = hMainWnd;
		m_hWnd = GetConsoleWindow();
		SetConsoleTitle(L"GraphicDebugger");
		ConsoleTextColor(CGraphicDebugger::COLOR::WHITE);
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CGraphicDebugger::ConsoleTextColor(COLOR eColor)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (_uint)eColor);
	return S_OK;
}

void CGraphicDebugger::Notice(const char * pNotify, _bool CRLF)
{
	if (m_bConsole == FALSE)
		return;

	FILE* pFile = nullptr;

	freopen_s(&pFile, "CONOUT$", "wb", stdout);

	if (CRLF)
		cout << pNotify << endl;
	else
		cout << pNotify;

	fclose(pFile);
}

void CGraphicDebugger::Notice(const char * pNotify1, COLOR iColor1, const char * pNotify2, COLOR iColor2, const char * pNotify3, COLOR iColor3)
{
	return;

	//if (m_bConsole == FALSE)
	//	return;

	//FILE* pFile = nullptr;
	//freopen_s(&pFile, "CONOUT$", "wb", stdout);

	//ConsoleTextColor(iColor1);

	//cout << pNotify1;
	//ConsoleTextColor(iColor2);
	//cout << pNotify2;;
	//ConsoleTextColor(iColor3);
	//cout << pNotify3 << endl;

	//ConsoleTextColor(COLOR::WHITE);

	//fclose(pFile);
}

void CGraphicDebugger::Request()
{
	if (m_bConsole == FALSE)
		return;

	FILE* pFile = nullptr;
	SetForegroundWindow(m_hWnd);

	freopen_s(&pFile, "CONIN$", "rb", stdin);

	std::string szDebuggingRequest;
	ConsoleTextColor(COLOR::DARKGREEN);
	Notice("====DEBUG====");
	ConsoleTextColor(COLOR::DARKSKY);
	Notice("1. ", false);
	ConsoleTextColor(COLOR::WHITE);
	Notice("Object");
	ConsoleTextColor(COLOR::DARKSKY);
	Notice("2. ", false);
	ConsoleTextColor(COLOR::WHITE);
	Notice("Level");
	ConsoleTextColor(COLOR::DARKSKY);
	Notice("3. ", false);
	ConsoleTextColor(COLOR::WHITE);
	Notice("Light");
	ConsoleTextColor(COLOR::DARKSKY);
	Notice("4. ", false);
	ConsoleTextColor(COLOR::WHITE);
	Notice("RenderTarget");
	ConsoleTextColor(COLOR::DARKSKY);
	Notice("5. ", false);
	ConsoleTextColor(COLOR::WHITE);
	Notice("Bullet Time");
	ConsoleTextColor(COLOR::DARKSKY);
	Notice("6. ", false);
	ConsoleTextColor(COLOR::WHITE);
	Notice("Render Debug Groups");
	ConsoleTextColor(COLOR::DARKSKY);
	Notice("9. ", false);
	ConsoleTextColor(COLOR::WHITE);
	Notice("Exit");
	ConsoleTextColor(COLOR::DARKGREEN);
	Notice("=============");
	ConsoleTextColor(COLOR::WHITE);

	cin >> szDebuggingRequest;
	fclose(pFile);

	DebuggingRequest(szDebuggingRequest.c_str());


	SetForegroundWindow(m_hMainWnd);
}

HRESULT CGraphicDebugger::DebuggingRequest(const char * pNotify)
{
	Notice("");

	if (!strcmp(pNotify, "Object") || !strcmp(pNotify, "1")) {
		Object();
	}
	else if (!strcmp(pNotify, "Level") || !strcmp(pNotify, "2")) {
		Level();
	}
	else if (!strcmp(pNotify, "Light") || !strcmp(pNotify, "3")) {
		Light();
	}
	else if (!strcmp(pNotify, "RenderTarget") || !strcmp(pNotify, "4")) {
		RenderTarget();
	}
	else if (!strcmp(pNotify, "BulletTime") || !strcmp(pNotify, "5")) {
		BulletTime();
	}
	else if (!strcmp(pNotify, "Render Debug Groups") || !strcmp(pNotify, "6")) {
		DebugGroup();
	}



	if (!strcmp(pNotify, "Exit") || !strcmp(pNotify, "9")) {
		return S_OK;
	}

	return S_OK;
}

HRESULT CGraphicDebugger::Object()
{
	FILE* pFile = nullptr;
	freopen_s(&pFile, "CONIN$", "rb", stdin);
	
	size_t iStaticSize = CGameObjectManager::Get_Instance()->Get_LayerSize(0);
	wstring wstrStaticLevel;
	ConsoleTextColor(CGraphicDebugger::COLOR::DARKGREEN);
	Notice("Static Level");
	ConsoleTextColor(CGraphicDebugger::COLOR::WHITE);
	for (size_t i = 0; i < iStaticSize; ++i)
	{
		wstrStaticLevel = CGameObjectManager::Get_Instance()->Get_LayerName(0, (_uint)i);
		string strConverter(wstrStaticLevel.begin(), wstrStaticLevel.end());
		string str = to_string(i) + ". " + strConverter;
		Notice(str.c_str());
	}


	size_t iSize = CGameObjectManager::Get_Instance()->Get_LayerSize(CLevelManager::Get_Instance()->Get_CurrentLevelID());
	wstring wstrCurrentLevel;
	ConsoleTextColor(CGraphicDebugger::COLOR::DARKGREEN);
	Notice("Current Level");
	ConsoleTextColor(CGraphicDebugger::COLOR::WHITE);
	for (size_t i = 0; i < iSize; ++i)
	{
		wstrCurrentLevel = CGameObjectManager::Get_Instance()->Get_LayerName(CLevelManager::Get_Instance()->Get_CurrentLevelID(), (_uint)i);
		string strConverter(wstrCurrentLevel.begin(), wstrCurrentLevel.end());
		string str = to_string(i + iStaticSize) + ". " + strConverter;
		Notice(str.c_str());
	}
	Notice("");

	_uint iIndex = 0;
	ConsoleTextColor(CGraphicDebugger::COLOR::DARKRED);
	Notice("Input Layer Number.");
	ConsoleTextColor(CGraphicDebugger::COLOR::WHITE);
	cin >> iIndex;
	fclose(pFile);
	
	if (iIndex < iStaticSize)
		ObjectSelect(CGameObjectManager::Get_Instance()->Get_ObjectList(0, CGameObjectManager::Get_Instance()->Get_LayerName(0, iIndex)), CGameObjectManager::Get_Instance()->Get_LayerName(0, iIndex));
	else if (iIndex >= iStaticSize)
		ObjectSelect(CGameObjectManager::Get_Instance()->Get_ObjectList(CLevelManager::Get_Instance()->Get_CurrentLevelID(), CGameObjectManager::Get_Instance()->Get_LayerName(CLevelManager::Get_Instance()->Get_CurrentLevelID(), iIndex - (_uint)iStaticSize)), CGameObjectManager::Get_Instance()->Get_LayerName(CLevelManager::Get_Instance()->Get_CurrentLevelID(), iIndex - (_uint)iStaticSize));

	return S_OK;
}

HRESULT CGraphicDebugger::ObjectSelect(list<CGameObject*>* pList, const wchar_t* pNotify)
{
	FILE* pFile = nullptr;
	freopen_s(&pFile, "CONIN$", "rb", stdin);
	wstring wstrTag = pNotify;
	string  strTag(wstrTag.begin(), wstrTag.end());
	ConsoleTextColor(CGraphicDebugger::COLOR::GREEN);
	Notice(""); Notice(strTag.c_str()); Notice("");
	
	ConsoleTextColor(CGraphicDebugger::COLOR::DARKGREEN);
	Notice("====OBJECT====");
	ConsoleTextColor(CGraphicDebugger::COLOR::DARKSKY);
	Notice(" 1. ", false);
	ConsoleTextColor(CGraphicDebugger::COLOR::WHITE);
	Notice("Render");
	ConsoleTextColor(CGraphicDebugger::COLOR::DARKSKY);
	Notice(" 2. ", false);
	ConsoleTextColor(CGraphicDebugger::COLOR::WHITE);
	Notice("State");
	ConsoleTextColor(CGraphicDebugger::COLOR::DARKGREEN);
	Notice("==============");
	ConsoleTextColor(CGraphicDebugger::COLOR::WHITE);
	_uint iSelect = 0;
	cin >> iSelect;
	fclose(pFile);

	if (iSelect == 1)
		ObjectRender(pList);
	else if (iSelect == 2)
		ObjectState(pList);
	else
		return S_OK;

	return S_OK;
}

HRESULT CGraphicDebugger::ObjectRender(list<CGameObject*>* pList)
{
	FILE* pFile = nullptr;
	freopen_s(&pFile, "CONIN$", "rb", stdin);

	ConsoleTextColor(CGraphicDebugger::COLOR::DARKGREEN);
	Notice("");
	Notice("1. Change Shader Pass Index at Once");
	Notice("2. Change Shader Pass Index Sequentially");
	ConsoleTextColor(CGraphicDebugger::COLOR::WHITE);

	_uint iOption = 0;
	cin >> iOption;

	if (iOption == 1)
	{
		ConsoleTextColor(CGraphicDebugger::COLOR::DARKGREEN);
		_uint iMaxPass = pList->front()->Get_MaxPassIndex();
		auto& iter = pList->begin();
		Notice("");
		for (_uint i = 0; i < iMaxPass; ++i)
		{
			string strPassName = to_string(i) + ". " + (*iter)->Get_PassName(i);
			Notice(strPassName.c_str());
		}
		Notice("");
		ConsoleTextColor(CGraphicDebugger::COLOR::RED);
		Notice("Select Shader Pass Index : ", false);
		ConsoleTextColor(CGraphicDebugger::COLOR::WHITE);

		_uint iPassIndex = 0;
		cin >> iPassIndex;

		for (auto Object : *pList)
			Object->Set_PassIndex(iPassIndex);
	}
	else if (iOption == 2)
	{
		
		Notice("");
		size_t iSize = pList->size();
		auto& iter = pList->begin();
		for (size_t i = 0; i < iSize; ++i)
		{
			string strCurrnetObject = to_string((_uint)i);
			_uint iMaxPass = (*iter)->Get_MaxPassIndex();
			ConsoleTextColor(CGraphicDebugger::COLOR::RED);
			Notice(strCurrnetObject.c_str(), false);
			ConsoleTextColor(CGraphicDebugger::COLOR::WHITE);
			Notice(" Object Shader Pass Index Change : ", false);
			ConsoleTextColor(CGraphicDebugger::COLOR::DARKYELLOW);
			_uint iCurrentPass = (*iter)->Get_PassIndex();
			Notice(to_string(iCurrentPass).c_str());
			ConsoleTextColor(CGraphicDebugger::COLOR::DARKGREEN);
			for (_uint j = 0; j < iMaxPass; ++j)
			{
				string strPassName = to_string(j) + ". " + (*iter)->Get_PassName(j);
				Notice(strPassName.c_str());
			}
			Notice("");
			ConsoleTextColor(CGraphicDebugger::COLOR::RED);
			Notice("Select Shader Pass Index : ", false);
			ConsoleTextColor(CGraphicDebugger::COLOR::WHITE);
			
			_uint iPassIndex = 0;
			cin >> iPassIndex;

			(*iter)->Set_PassIndex(iPassIndex);

			if ((*iter)->Get_BufferContainer())
				EffectObjectRenderDetail((*iter));

			++iter;
		}
	}
	else
	{
		fclose(pFile);
		return S_OK;
	}

	fclose(pFile);
	return S_OK;
}

HRESULT CGraphicDebugger::EffectObjectRenderDetail(CGameObject * pEffectObject)
{
	FILE* pFile = nullptr;
	freopen_s(&pFile, "CONIN$", "rb", stdin);
	size_t iEffectSize = pEffectObject->Get_BufferContainerSize();
	for (size_t i = 0; i < iEffectSize; ++i)
	{
		ConsoleTextColor(CGraphicDebugger::COLOR::WHITE);
		Notice("> ", false);
		ConsoleTextColor(CGraphicDebugger::COLOR::RED);
		Notice(to_string((_uint)i).c_str(), false);
		ConsoleTextColor(CGraphicDebugger::COLOR::WHITE);
		Notice(" Effect's Shader Pass Index Change : ", false);
		_uint iCurrentPassIndex = pEffectObject->Get_BufferContainerPassIndex((_uint)i);
		ConsoleTextColor(CGraphicDebugger::COLOR::DARKYELLOW);
		Notice(to_string(iCurrentPassIndex).c_str());
		ConsoleTextColor(CGraphicDebugger::COLOR::DARKGREEN);
		_uint MaxPass = pEffectObject->Get_BufferContainerMaxPassIndex((_uint)i);
		for (_uint j = 0; j < MaxPass; ++j)
		{
			string strPassName = to_string(j) + ". " + pEffectObject->Get_BufferContainerPassName((_uint)i, j);
			Notice(strPassName.c_str());
		}
		Notice("");
		ConsoleTextColor(CGraphicDebugger::COLOR::RED);
		Notice("Select Shader Pass Index : ", false);
		ConsoleTextColor(CGraphicDebugger::COLOR::WHITE);

		_uint iPassIndex = 0;
		cin >> iPassIndex;
		
		pEffectObject->Set_BufferContainerPassIndex((_uint)i, iPassIndex);
	}
	fclose(pFile);
	return S_OK;
}

HRESULT CGraphicDebugger::ObjectState(list<CGameObject*>* pList)
{
	FILE* pFile = nullptr;
	freopen_s(&pFile, "CONIN$", "rb", stdin);

	ConsoleTextColor(CGraphicDebugger::COLOR::DARKGREEN);
	Notice("");
	Notice("1. ", false); 
	ConsoleTextColor(CGraphicDebugger::COLOR::WHITE);
	Notice("Change Object State Setting at Once");
	ConsoleTextColor(CGraphicDebugger::COLOR::DARKGREEN);
	Notice("2. ", false); 
	ConsoleTextColor(CGraphicDebugger::COLOR::WHITE);
	Notice("Change Object State Setting Sequentially");
	


	_uint iOption = 0;
	cin >> iOption;

	if (iOption == 1)
	{
		ConsoleTextColor(CGraphicDebugger::COLOR::DARKYELLOW);
		Notice("1. ", false);
		ConsoleTextColor(CGraphicDebugger::COLOR::DARKRED);
		Notice("Enable");
		ConsoleTextColor(CGraphicDebugger::COLOR::DARKYELLOW);
		Notice("2. ", false);
		ConsoleTextColor(CGraphicDebugger::COLOR::DARKRED);
		Notice("Disable");
		ConsoleTextColor(CGraphicDebugger::COLOR::DARKYELLOW);
		Notice("3. ", false);
		ConsoleTextColor(CGraphicDebugger::COLOR::DARKRED);
		Notice("Dead");

		Notice("");
		ConsoleTextColor(CGraphicDebugger::COLOR::RED);
		Notice("Select Shader Pass Index : ", false);
		ConsoleTextColor(CGraphicDebugger::COLOR::WHITE);

		_uint iStateIndex = 0;
		cin >> iStateIndex;

		if (iStateIndex == 1)
		{
			for (auto iter : *pList)
				iter->Set_State(OBJSTATE::ENABLE);
		}
		else if (iStateIndex == 2)
		{
			for (auto iter : *pList)
				iter->Set_State(OBJSTATE::DISABLE);
		}
		else if (iStateIndex == 3)
		{
			for (auto iter : *pList)
				iter->Set_State(OBJSTATE::DEAD);
		}
		else
		{
			fclose(pFile);
			return S_OK;
		}

	}
	else if (iOption == 2)
	{
		size_t iSize = pList->size();
		auto& iter = pList->begin();
		for (size_t i = 0; i < iSize; ++i)
		{
			ConsoleTextColor(CGraphicDebugger::COLOR::DARKYELLOW);
			Notice("1. ", false);
			ConsoleTextColor(CGraphicDebugger::COLOR::DARKRED);
			Notice("Enable");
			ConsoleTextColor(CGraphicDebugger::COLOR::DARKYELLOW);
			Notice("2. ", false);
			ConsoleTextColor(CGraphicDebugger::COLOR::DARKRED);
			Notice("Disable");
			ConsoleTextColor(CGraphicDebugger::COLOR::DARKYELLOW);
			Notice("3. ", false);
			ConsoleTextColor(CGraphicDebugger::COLOR::DARKRED);
			Notice("Dead");

			string numberstr = to_string(i);
			ConsoleTextColor(CGraphicDebugger::COLOR::DARKRED);
			Notice(numberstr.c_str(), false);
			ConsoleTextColor(CGraphicDebugger::COLOR::WHITE);
			Notice(" Object State : ", false);
			ConsoleTextColor(CGraphicDebugger::COLOR::DARKGREEN);
			OBJSTATE eState = (*iter)->Get_State();
			if (eState == OBJSTATE::ENABLE)
			{
				Notice("ENABLE");
			}
			else if (eState == OBJSTATE::DISABLE)
			{
				Notice("DISABLE");
			}
			else if (eState == OBJSTATE::DEAD)
			{
				Notice("DEAD");
			}

			Notice("");
			ConsoleTextColor(CGraphicDebugger::COLOR::RED);
			Notice("Select Shader Pass Index : ", false);
			ConsoleTextColor(CGraphicDebugger::COLOR::WHITE);

			_uint iStateIndex = 0;
			cin >> iStateIndex;

			if (iStateIndex == 1)
				(*iter)->Set_State(OBJSTATE::ENABLE);
			else if (iStateIndex == 2)
				(*iter)->Set_State(OBJSTATE::DISABLE);
			else if (iStateIndex == 3)
				(*iter)->Set_State(OBJSTATE::DEAD);
			else
			{
				fclose(pFile);
				return S_OK;
			}
			++iter;
		}
	}
	else
	{
		fclose(pFile);
		return S_OK;
	}
	fclose(pFile);
	return S_OK;
}

HRESULT CGraphicDebugger::Level()
{

	return S_OK;
}

HRESULT CGraphicDebugger::Light()
{
	FILE* pFile = nullptr;
	freopen_s(&pFile, "CONIN$", "rb", stdin);

	size_t iSize = CLightManager::Get_Instance()->Get_LightMapSize();

	for (size_t i = 0; i < iSize; ++i)
	{
		ConsoleTextColor(COLOR::DARKGREEN);
		Notice(to_string((_uint)i+1).c_str(), false);
		ConsoleTextColor(COLOR::YELLOW);
		wstring wstr = CLightManager::Get_Instance()->Get_LightTag((_uint)i);
		string strTag(wstr.begin(), wstr.end());
		Notice(". ", false); Notice(strTag.c_str(), false);
		ConsoleTextColor(COLOR::WHITE);
		Notice(" : ", false);
		size_t iLightSize = CLightManager::Get_Instance()->Get_LightSize(wstr.c_str());
		ConsoleTextColor(COLOR::YELLOW);
		Notice(to_string(iLightSize).c_str());
		ConsoleTextColor(COLOR::WHITE);
	}
	ConsoleTextColor(COLOR::DARKRED);
	Notice("Select Light Layer : ", false);
	ConsoleTextColor(COLOR::WHITE);
	_uint iSelect = 0;
	cin >> iSelect;
	fclose(pFile);

	LightSelect(CLightManager::Get_Instance()->Find_LightMap(CLightManager::Get_Instance()->Get_LightTag(iSelect - 1)), CLightManager::Get_Instance()->Get_LightTag(iSelect - 1));

	return S_OK;
}

HRESULT CGraphicDebugger::LightSelect(list<class CLight*>* pList, const wchar_t* pNotify)
{
	FILE* pFile = nullptr;
	freopen_s(&pFile, "CONIN$", "rb", stdin);
	wstring wstrTag = pNotify;
	string  strTag(wstrTag.begin(), wstrTag.end());
	ConsoleTextColor(CGraphicDebugger::COLOR::GREEN);
	Notice(""); Notice(strTag.c_str()); Notice("");


	ConsoleTextColor(CGraphicDebugger::COLOR::DARKGREEN);
	Notice("====LIGHT====");
	ConsoleTextColor(CGraphicDebugger::COLOR::DARKSKY);
	Notice(" 1. ", false);
	ConsoleTextColor(CGraphicDebugger::COLOR::WHITE);
	Notice(" Active");
	ConsoleTextColor(CGraphicDebugger::COLOR::DARKSKY);
	Notice(" 2. ", false);
	ConsoleTextColor(CGraphicDebugger::COLOR::WHITE);
	Notice(" DeActive");
	ConsoleTextColor(CGraphicDebugger::COLOR::DARKSKY);
	Notice(" 3. ", false);
	ConsoleTextColor(CGraphicDebugger::COLOR::WHITE);
	Notice(" Delete");
	ConsoleTextColor(CGraphicDebugger::COLOR::DARKGREEN);
	Notice("=============");
	ConsoleTextColor(CGraphicDebugger::COLOR::WHITE);

	ConsoleTextColor(COLOR::DARKRED);
	Notice("Select Light Action : ", false);
	ConsoleTextColor(COLOR::WHITE);
	_uint iSelect = 0;
	cin >> iSelect;
	fclose(pFile);

	if (iSelect == 1)
		LightActive(pList);
	else if (iSelect == 2)
		LightDeActive(pList);
	else if (iSelect == 3)
		LightDelete(pList);
	else
		return S_OK;

	return S_OK;
}

HRESULT CGraphicDebugger::LightActive(list<class CLight*>* pList)
{
	FILE* pFile = nullptr;
	freopen_s(&pFile, "CONIN$", "rb", stdin);

	ConsoleTextColor(COLOR::DARKYELLOW);
	Notice("\nLight Active\n");
	ConsoleTextColor(COLOR::DARKGREEN);
	Notice("1. ", false); 
	ConsoleTextColor(COLOR::WHITE);
	Notice("Active Light at Once");
	ConsoleTextColor(COLOR::DARKGREEN);
	Notice("2. ", false);
	ConsoleTextColor(COLOR::WHITE);
	Notice("Active Light at Sequentially");

	_uint iSelect = 0;
	cin >> iSelect;
	if (iSelect == 1)
	{
		for (auto Light : *pList)
			Light->Active();
	}
	else if (iSelect == 2)
	{
		size_t iSize = pList->size();
		auto& begin = pList->begin();
		for (size_t i = 0; i < iSize; ++i)
		{
			ConsoleTextColor(COLOR::DARKGREEN);
			Notice(to_string((_uint)i).c_str(), false);
			ConsoleTextColor(COLOR::WHITE);
			Notice(" Light Active : ", false);
			_bool bState= (*begin)->Get_LightState();
			ConsoleTextColor(COLOR::DARKYELLOW);
			if (bState)
				Notice(" TRUE ");
			else
				Notice(" FALSE ");

			ConsoleTextColor(COLOR::DARKGREEN);
			Notice("1. ", false);
			ConsoleTextColor(COLOR::WHITE);
			Notice(" Active");

			ConsoleTextColor(COLOR::DARKGREEN);
			Notice("2. ", false);
			ConsoleTextColor(COLOR::WHITE);
			Notice(" DeActive");


			ConsoleTextColor(COLOR::DARKRED);
			Notice("Select Light State : ", false);
			ConsoleTextColor(COLOR::WHITE);
			_uint iSequence = 0;
			cin >> iSequence;


			if (iSequence == 1)
				(*begin)->Active();
			else if (iSequence == 2)
				(*begin)->DeActive();
			else
				(*begin)->Active();

			++begin;
		}
	}
	else
	{
		fclose(pFile);
		return S_OK;
	}

	fclose(pFile);
	return S_OK;
}

HRESULT CGraphicDebugger::LightDeActive(list<class CLight*>* pList)
{
	FILE* pFile = nullptr;
	freopen_s(&pFile, "CONIN$", "rb", stdin);

	ConsoleTextColor(COLOR::DARKYELLOW);
	Notice("\nLight DeActive\n");
	ConsoleTextColor(COLOR::DARKGREEN);
	Notice("1. ", false);
	ConsoleTextColor(COLOR::WHITE);
	Notice("DeActive Light at Once");
	ConsoleTextColor(COLOR::DARKGREEN);
	Notice("2. ", false);
	ConsoleTextColor(COLOR::WHITE);
	Notice("DeActive Light at Sequentially");

	_uint iSelect = 0;
	cin >> iSelect;
	if (iSelect == 1)
	{
		for (auto Light : *pList)
			Light->DeActive();
	}
	else if (iSelect == 2)
	{
		size_t iSize = pList->size();
		auto& begin = pList->begin();
		for (size_t i = 0; i < iSize; ++i)
		{
			ConsoleTextColor(COLOR::DARKGREEN);
			Notice(to_string((_uint)i).c_str(), false);
			ConsoleTextColor(COLOR::WHITE);
			Notice(" Light Active : ", false);
			_bool bState = (*begin)->Get_LightState();
			ConsoleTextColor(COLOR::DARKYELLOW);
			if (bState)
				Notice(" TRUE ");
			else
				Notice(" FALSE ");

			ConsoleTextColor(COLOR::DARKGREEN);
			Notice("1. ", false);
			ConsoleTextColor(COLOR::WHITE);
			Notice(" Active");

			ConsoleTextColor(COLOR::DARKGREEN);
			Notice("2. ", false);
			ConsoleTextColor(COLOR::WHITE);
			Notice(" DeActive");


			ConsoleTextColor(COLOR::DARKRED);
			Notice("Select Light State : ", false);
			ConsoleTextColor(COLOR::WHITE);
			_uint iSequence = 0;
			cin >> iSequence;


			if (iSequence == 1)
				(*begin)->Active();
			else if (iSequence == 2)
				(*begin)->DeActive();
			else
				(*begin)->DeActive();

			++begin;
		}
	}
	else
	{
		fclose(pFile);
		return S_OK;
	}

	fclose(pFile);
	return S_OK;
}

HRESULT CGraphicDebugger::LightDelete(list<class CLight*>* pList)
{
	FILE* pFile = nullptr;
	freopen_s(&pFile, "CONIN$", "rb", stdin);

	ConsoleTextColor(COLOR::DARKYELLOW);
	Notice("\nLight Delete\n");
	ConsoleTextColor(COLOR::DARKGREEN);
	Notice("1. ", false);
	ConsoleTextColor(COLOR::WHITE);
	Notice("Delete Light at Once");
	ConsoleTextColor(COLOR::DARKGREEN);
	Notice("2. ", false);
	ConsoleTextColor(COLOR::WHITE);
	Notice("Delete Light at Sequentially");

	_uint iSelect = 0;
	cin >> iSelect;
	if (iSelect == 1)
	{
		auto& begin = pList->begin();
		auto& end = pList->end();

		for (; begin != end;)
		{
			Safe_Release(*begin);
			begin = pList->erase(begin);
		}

	}
	else if (iSelect == 2)
	{
		_uint i = 0;
		auto& begin = pList->begin();
		auto& end = pList->end();
		for (; begin != end;)
		{
			ConsoleTextColor(COLOR::DARKGREEN);
			Notice(to_string((_uint)i++).c_str(), false);
			ConsoleTextColor(COLOR::WHITE);
			Notice(" Light Active : ", false);
			_bool bState = (*begin)->Get_LightState();
			ConsoleTextColor(COLOR::DARKYELLOW);
			if (bState)
				Notice(" TRUE ");
			else
				Notice(" FALSE ");

			ConsoleTextColor(COLOR::DARKGREEN);
			Notice("1. ", false);
			ConsoleTextColor(COLOR::WHITE);
			Notice(" Delete");

			ConsoleTextColor(COLOR::DARKGREEN);
			Notice("2. ", false);
			ConsoleTextColor(COLOR::WHITE);
			Notice(" Pass");


			ConsoleTextColor(COLOR::DARKRED);
			Notice("Select Light State : ", false);
			ConsoleTextColor(COLOR::WHITE);
			_uint iSequence = 0;
			cin >> iSequence;


			if (iSequence == 1)
			{
				Safe_Release(*begin);
				begin = pList->erase(begin);
			}
			else if (iSequence == 2)
			{
				++begin;
			}
		}
	}
	else
	{
		fclose(pFile);
		return S_OK;
	}
	fclose(pFile);
	return S_OK;
}

HRESULT CGraphicDebugger::RenderTarget()
{
	FILE* pFile = nullptr;
	freopen_s(&pFile, "CONIN$", "rb", stdin);
	ConsoleTextColor(COLOR::DARKYELLOW);
	Notice("\nRender Target\n");
	ConsoleTextColor(COLOR::DARKGREEN);
	Notice("1. ", false);
	ConsoleTextColor(COLOR::WHITE);
	Notice("Render On");
	ConsoleTextColor(COLOR::DARKGREEN);
	Notice("2. ", false);
	ConsoleTextColor(COLOR::WHITE);
	Notice("Render Off");

	ConsoleTextColor(COLOR::DARKRED);
	Notice("Select Render Target : ", false);
	ConsoleTextColor(COLOR::WHITE);
	_uint iRenderState = 0;
	cin >> iRenderState;
	fclose(pFile);
	if (iRenderState == 1)
	{
		CRenderTargetManager::Get_Instance()->IsRender(true);
	}
	else if (iRenderState == 2)
	{
		CRenderTargetManager::Get_Instance()->IsRender(false);
	}
	else
		return S_OK;

	return S_OK;
}

HRESULT CGraphicDebugger::BulletTime()
{
	FILE* pFile = nullptr;
	freopen_s(&pFile, "CONIN$", "rb", stdin);
	ConsoleTextColor(COLOR::DARKYELLOW);
	Notice("\nBullet Time ", false);
	ConsoleTextColor(COLOR::WHITE);
	Notice(" : ", false);
	ConsoleTextColor(COLOR::DARKRED);

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	 _bool bBulletTime = pGameInstance->Get_BulletTime();

	if (bBulletTime)
		Notice("TRUE");
	else
		Notice("FALSE");

	ConsoleTextColor(COLOR::WHITE);
	Notice("Time Remaining : ", false);
	ConsoleTextColor(COLOR::DARKGREEN);
	Notice(to_string(pGameInstance->Get_BulletRemaining()).c_str());

	ConsoleTextColor(COLOR::WHITE);
	Notice("Bullet Time Level : ", false);
	ConsoleTextColor(COLOR::DARKGREEN);
	Notice(to_string(pGameInstance->Get_BulletLevel()).c_str());

	ConsoleTextColor(COLOR::DARKGREEN);
	Notice("1. ", false);
	ConsoleTextColor(COLOR::WHITE);
	Notice("BulletTime On");
	ConsoleTextColor(COLOR::DARKGREEN);
	Notice("2. ", false);
	ConsoleTextColor(COLOR::WHITE);
	Notice("BulletTime Off");

	ConsoleTextColor(COLOR::DARKRED);
	Notice("Select BulletTime : ", false);
	ConsoleTextColor(COLOR::WHITE);

	_uint iBulletTime = 0;
	cin >> iBulletTime;
	if (iBulletTime == 1)
	{
		pGameInstance->Set_BulletTime(true);

		ConsoleTextColor(COLOR::DARKGREEN);
		Notice("\nBulletTime Duration : ", false);
		ConsoleTextColor(COLOR::WHITE);
		_double dTimeDelta;
		cin >> dTimeDelta;

		pGameInstance->Set_BulletRemaining(dTimeDelta);

		ConsoleTextColor(COLOR::DARKGREEN);
		Notice("\nBulletTime Level : ", false);
		ConsoleTextColor(COLOR::WHITE);
		_double dBulletLevel;
		cin >> dBulletLevel;
		fclose(pFile);
		pGameInstance->Set_BulletLevel(dBulletLevel);
	}
	else if (iBulletTime == 2)
	{
		fclose(pFile);
		pGameInstance->Set_BulletTime(false);
	}
	else
	{
		fclose(pFile);
		return S_OK;
	}

	return S_OK;
}

HRESULT CGraphicDebugger::DebugGroup()
{
	FILE* pFile = nullptr;
	freopen_s(&pFile, "CONIN$", "rb", stdin);
	ConsoleTextColor(COLOR::DARKYELLOW);
	Notice("\nDebug Draw : ", false);
	ConsoleTextColor(COLOR::DARKRED);
	_bool bDraw = CGameInstance::Get_Instance()->Get_DebugDraw();
	if (bDraw)
		Notice("TRUE");
	else
		Notice("FALSE");

	ConsoleTextColor(COLOR::DARKGREEN);
	Notice("1. ", false);
	ConsoleTextColor(COLOR::WHITE);
	Notice("Draw On");
	ConsoleTextColor(COLOR::DARKGREEN);
	Notice("2. ", false);
	ConsoleTextColor(COLOR::WHITE);
	Notice("Draw Off");

	ConsoleTextColor(COLOR::DARKRED);
	Notice("Select Debug Draw : ", false);
	ConsoleTextColor(COLOR::WHITE);
	_uint iRenderState = 0;
	cin >> iRenderState;
	fclose(pFile);
	if (iRenderState == 1)
	{
		CGameInstance::Get_Instance()->Set_DebugDraw(true);
	}
	else if (iRenderState == 2)
	{
		CGameInstance::Get_Instance()->Set_DebugDraw(false);
	}
	else
		return S_OK;

	return S_OK;
}

void CGraphicDebugger::Free()
{
	FreeConsole();
}
