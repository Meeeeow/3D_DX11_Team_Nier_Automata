#pragma once
#ifndef __CLIENT_LOADER_H__
#define __CLIENT_LOADER_H__

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)

class CLoader final : public CBase
{
private:
	CLoader(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual ~CLoader() DEFAULT;

public:
	CRITICAL_SECTION		Get_CS() { return m_CS; }
	LEVEL					Get_NextLevel() { return m_eNextLevel; }
	_bool					Get_Finished() { return m_bFinished; }

private:
	HRESULT					NativeConstruct(LEVEL eNextLevel);

public:
	HRESULT					Load_Stage1();
	HRESULT					Load_Stage2();
	HRESULT					Load_Stage3();
	HRESULT					Load_ZhuangziStage();
	HRESULT					Load_AmusementPark();
	HRESULT					Load_OperaBackStage();
	HRESULT					Load_ROBOTGENERAL();

public:
	HRESULT					Initialize_Static_Component();
	HRESULT					Initialize_Static_Object();

private:
	_pGraphicDevice			m_pGraphicDevice = nullptr;
	_pContextDevice			m_pContextDevice = nullptr;

	LEVEL					m_eNextLevel = LEVEL::COUNT;
	HANDLE					m_hThread = 0;
	CRITICAL_SECTION		m_CS;

	_bool					m_bFinished = false;

public:
	#pragma region INITIAILIZE
	static	bool			m_bIsInitialLoading;
	// Scene Test
	static	bool			m_bIsTestSceneInitialize;

	// Scene Robot
	static	bool			m_bIsRobotMilitaryInitialize;
	static	bool			m_bIsZhuangziInitialize;

	// Scene Opera
	static	bool			m_bIsAmusementParkInitialize;
	static	bool			m_bIsOperaBackStageInitialize;
	#pragma endregion

public:
	static	CLoader*		Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, LEVEL eNextLevel);
	virtual void			Free() override;
};

END
#endif // !__CLIENT_LOADER_H__