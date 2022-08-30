#pragma once
#ifndef __CLIENT_LEVEL_STAGE1_H__
#define __CLIENT_LEVEL_STAGE1_H__

#include "Level.h"
#include "Client_Defines.h"

BEGIN(Client)
class CLevel_Stage1 final : public CLevel
{
private:
	CLevel_Stage1(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual ~CLevel_Stage1() DEFAULT;

public:
	virtual	HRESULT					NativeConstruct(_uint iLevelID) override;
	virtual _int					Tick(_double dTimeDelta) override;
	virtual HRESULT					Render() override;

private:
	HRESULT							Add_CollisionTag();
	HRESULT							Load_Light(const _tchar* pLayerTag, CGameInstance* pGameInstance);
	HRESULT							Load_Player(CGameInstance* pGameInstance);
	HRESULT							Active_Monster_Wave1(CGameInstance* pGameInstance);
	HRESULT							Active_Monster_Wave2(CGameInstance* pGameInstance);
	HRESULT							Make_Monster_Wave1(CGameInstance* pGameInstance);
	HRESULT							Make_Monster_Wave2(CGameInstance* pGameInstance);

public:
	 void							Plus_MonsterCount_W0() { ++m_iMonsterCount_W0; }
	 void							Minus_MonsterCount_W0() { --m_iMonsterCount_W0; }

	 void							Plus_MonsterCount_W1() { ++m_iMonsterCount_W1; }
	 void							Minus_MonsterCount_W1() { --m_iMonsterCount_W1; }
	 
	 void							Plus_MonsterCount_W2() { ++m_iMonsterCount_W2; }
	 void							Minus_MonsterCount_W2() { --m_iMonsterCount_W2; }

private:
	_bool							m_bDebug = false;

public:
	static int						m_iMonsterCountStatic;
	int								m_iMonsterCount_W0 = 0;
	_uint							m_iMonsterCount_W1 = 0;
	_uint							m_iMonsterCount_W2 = 0;
	_uint							m_iMonsterWave = 0;
	_bool							m_bOnce = false;
	_bool							m_bWave1 = false;
	_bool							m_bWave2 = false;

	_bool							m_bCallDirectionUI = false;

public:
	static	CLevel_Stage1*			Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, _uint iLevelID);
	virtual void					Free() override;
};
END
#endif // !__CLIENT_LEVEL_STAGE1_H__