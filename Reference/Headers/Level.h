#pragma once
#ifndef __ENGINE_LEVEL_H__
#define __ENGINE_LEVEL_H__

#include "Base.h"
BEGIN(Engine)
class ENGINE_DLL CLevel abstract : public CBase
{
public:
	CLevel(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual ~CLevel() DEFAULT;

public:
	virtual	HRESULT			NativeConstruct(_uint iLevelID);
	virtual _int			Tick(_double dTimeDelta);
	virtual _int			LateTick(_double dTimeDelta);
	virtual HRESULT			Render();

public:
	_uint					Get_CurrentLevelID() const {
		return m_iCurrentLevelID;
	}

public:
	CLevel*					Get_NextLevel();

public:
	void					Set_NextLevel(CLevel* pNextLevel);

protected:
	_pGraphicDevice			m_pGraphicDevice = nullptr;
	_pContextDevice			m_pContextDevice = nullptr;
	_uint					m_iCurrentLevelID;
	CLevel*					m_pNextLevel = nullptr;
public:
	virtual	void			Free() override;

};
END
#endif // !__ENGINE_LEVEL_H__