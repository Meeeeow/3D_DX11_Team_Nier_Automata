#pragma once
#ifndef __ENGINE_LEVEL_MANAGER_H__
#define __ENGINE_LEVEL_MANAGER_H__

#include "Level.h"

BEGIN(Engine)
class CLevelManager final : public CBase
{
	DECLARE_SINGLETON(CLevelManager)

private:
	explicit CLevelManager();
	virtual ~CLevelManager() DEFAULT;

public:
	HRESULT					Open_Level(CLevel* pLevel);
	_int					Tick(_double dTimeDelta);
	_int					LateTick(_double dTimeDelta);
	HRESULT					Render();
	CLevel*					Get_CurrentLevel() { return m_pCurrentLevel; }
	_uint					Get_CurrentLevelID() const { return m_pCurrentLevel->Get_CurrentLevelID(); }
	const _uint&			Get_NextLevelID() const { return m_iNextLevel; }

public:
	void					Set_NextLevelID(const _uint& iLevelID) { m_iNextLevel = iLevelID; }
	void					Set_NextLevel(CLevel* pNextLevel);

private:
	CLevel*					m_pCurrentLevel = nullptr;
	_uint					m_iNextLevel = 0;


public:
	virtual void			Free() override;

};
END
#endif // !__ENGINE_LEVEL_MANAGER_H__