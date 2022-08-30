#pragma once
#ifndef __CLIENT_LEVEL_LOADING_H__
#define __CLIENT_LEVEL_LOADING_H__

#include "Client_Defines.h"
#include "Level.h"
#include "Loader.h"

BEGIN(Client)
class CLevel_Loading final : public CLevel
{
private:
	CLevel_Loading(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual ~CLevel_Loading() DEFAULT;

public:
	virtual	HRESULT					NativeConstruct(LEVEL eNextLevel, _uint iLevelID);
	virtual _int					Tick(_double dTimeDelta) override;
	virtual _int					LateTick(_double dTimeDelta) override;
	virtual HRESULT					Render() override;

public:
	void							Increase_Percent();

private:
	LEVEL							m_eNextLevel = LEVEL::COUNT;
	CLoader*						m_pLoader = nullptr;

	class CLoadingScreen*			m_pLoadingScreen = nullptr;
	class CLoadingPercent*			m_pLoadingPercent = nullptr;
	class CLoadingEmil*				m_pLoadingEmil = nullptr;

public:
	static	CLevel_Loading*			Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, LEVEL eNextLevel, _uint iLevelIndex);
	virtual void					Free() override;

};
END
#endif // !__CLIENT_LEVEL_LOADING_H__