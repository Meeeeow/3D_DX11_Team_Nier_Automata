#pragma once
#ifndef __CLIENT_MAIN_APPLICATION_H__
#define __CLIENT_MAIN_APPLICATION_H__

#include "Client_Defines.h"
#include "Base.h"
#include "Atlas_Manager.h"
#include "CameraMgr.h"

BEGIN(Engine)
class CRenderer;
class CGameInstance;
END

BEGIN(Client)
class CMainApplication final : public CBase
{
private:
	CMainApplication();
	virtual ~CMainApplication() DEFAULT;

public:
	HRESULT							NativeConstruct();
	_int							Tick(_double dTimeDelta);
	 void							Update_FPS(_double dTimeDelta);
	HRESULT							Render();

public:
	HRESULT							Open_Level(LEVEL eLevel);
	HRESULT							Ready_Component_ForStatic();


private:
	CGameInstance*					m_pGameInstance = nullptr;
	CRenderer*						m_pRenderer = nullptr;
	CAtlas_Manager*					m_pAtlasManager = nullptr;
	class CPostProcessor*			m_pPostProcessor = nullptr;
	CCameraMgr*						m_pCameraManager = nullptr;

private:
	_pGraphicDevice					m_pGraphicDevice = nullptr;
	_pContextDevice					m_pContextDevice = nullptr;

private:
	_uint							m_iFPS = 0;
	_double							m_dTimeDelta = 0.0;

public:
	static	CMainApplication*		Create();
	virtual void					Free() override;

};
END
#endif // !__CLIENT_MAIN_APPLICATION_H__