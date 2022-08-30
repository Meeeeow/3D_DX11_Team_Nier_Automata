#pragma once
#ifndef __CLIENT_LEVEL_OPERA_BACK_STAGE_H__
#define __CLIENT_LEVEL_OPERA_BACK_STAGE_H__

#include "Client_Defines.h"
#include "Level.h"
#include "MinigameManager.h"

BEGIN(Client)
class CLevel_OperaBackStage final : public CLevel
{
private:
	CLevel_OperaBackStage(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual ~CLevel_OperaBackStage() DEFAULT;

public:
	virtual	HRESULT						NativeConstruct(_uint iLevelID) override;
	virtual	_int						Tick(_double dTimeDelta) override;
	virtual HRESULT						Render() override;

private:
	HRESULT								Add_CollisionTag();
	HRESULT								Set_Light(CGameInstance* pGameInstance);
	HRESULT								Set_Player(CGameInstance* pGameInstance);

private:
	void								StageLighting_Director(_double dTimeDelta);
	void								Light_To_Villain(CGameInstance* pGameInstance, _double dTimeDelta);
	void								Light_To_Heroine(CGameInstance* pGameInstance, _double dTimeDelta);
	HRESULT								Load_EventCollider(CGameInstance* pGameInstance, const _tchar* pEventColliderDataFilePath);

public:
	static	CLevel_OperaBackStage*		Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, _uint iLevelID);
	virtual	void						Free() override;

private:
	_double		m_dEngageScreamTimeAcc = 0.0;
	_double		m_dEngageScreamTimeMax = 5.0;
	_double		m_dEngageWarningTimeMax = 7.0;	

	_float		m_fVolumeForTrigger = 0.f;
	_bool		m_bShowOnce = false;

	static	_bool	m_bCallScream;
	static	_bool	m_bCallWarning;
	static	_bool	m_bInitialEngage;

	CMinigameManager*	m_pMiniGameManager = nullptr;

public:
	static	_bool	m_bStart;
	static	_bool	m_bEngage_UI;

};
END
#endif // !__CLIENT_LEVEL_OPERA_BACK_STAGE_H__