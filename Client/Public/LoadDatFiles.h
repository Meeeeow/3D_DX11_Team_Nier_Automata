#pragma once
#ifndef __CLIENT_LOAD_DAT_FILES_H__
#define __CLIENT_LOAD_DAT_FILES_H__

#include "Base.h"
#include "Client_Defines.h"
#include <io.h>


BEGIN(Client)
class CLoadDatFiles final : public CBase
{
	DECLARE_SINGLETON(CLoadDatFiles)
private:
	explicit CLoadDatFiles();
	virtual ~CLoadDatFiles() DEFAULT;

public:
	// Key - Layer 이름
	// Value - 해당 Layer안에 있는 Object 정보를 담은 vector
	typedef map<wstring, vector<MO_INFO*>>		OBJECTSINFO;
	// Key - Stage 이름
	// Value - OBJECTSINFO
	typedef map<wstring, OBJECTSINFO>			WORLDMAP;

public:
	void							Init(_pGraphicDevice pGraphicDeivce, _pContextDevice pContextDevice);
	HRESULT							LoadStageDatFile();
	HRESULT							LoadStageInfo(const _tchar* pDatFilePath);

public:
	// 스레드 시작 함수 
	HRESULT							Load_Object_Info(LEVEL eLevel, wstring pStageTag);
	HRESULT							Load_ModelDat_File(LEVEL eLevel, const char* pDatFilePath);
	HRESULT							Load_Dynamic_ModelDat_File(LEVEL eLevel, const char* pDatFilePath);

public:
	// Thread Function
	HRESULT							Load_Background(LOADDATA* pData);
	HRESULT							Load_Obstacle(LOADDATA* pData);
	HRESULT							Load_Environment(LOADDATA* pData);
	HRESULT							Load_Monster(LOADDATA* pData);
	HRESULT							Load_Boss(LOADDATA* pData);

public:
	HRESULT							Load_Model(LOADMODELDATA* pData);
	HRESULT							Load_Dynamic_Model(LOADMODELDATA* pData);

public:
	map<wstring, vector<MO_INFO*>>*	Get_StageInfo(wstring pStageTag);
	CRITICAL_SECTION				Get_CS_Scene(LAYER eLayer) { return m_CS_Scene[(_uint)eLayer]; }
	CRITICAL_SECTION				Get_CS_Model() { return m_CS_Model; }

private:
	_pGraphicDevice					m_pGraphicDevice = nullptr;
	_pContextDevice					m_pContextDevice = nullptr;

private:
	_bool							m_bSceneLoad = false;
	LOADDATA*						m_pSceneLoad[(_uint)LAYER::COUNT] = { nullptr, nullptr, nullptr, nullptr, nullptr };
	LOADMODELDATA*					m_pModelLoad = nullptr;
	_uint							m_iLoadIndex = 0;

private:
	WORLDMAP						m_mapWorld;
	vector<wstring>					m_strComponentName;
	vector<wstring>					m_strStageName;

private:
	CRITICAL_SECTION				m_CS_Scene[(_uint)LAYER::COUNT];
	HANDLE							m_hThread_Scene[(_uint)LAYER::COUNT] = { NULL, NULL, NULL, NULL, NULL };

	CRITICAL_SECTION				m_CS_Model;
	HANDLE							m_hThread_Model = NULL;

public:
	void							Release_Device();
	virtual	void					Free() override;
	static int						m_iClownCountStatic;

};
END
#endif // !__CLIENT_LOAD_DAT_FILES_H__