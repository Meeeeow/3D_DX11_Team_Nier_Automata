#pragma once
#ifndef __CLIENT_VENDING_MACHINE_H__
#define __CLIENT_VENDING_MACHINE_H__

#include "GameObjectModel.h"

BEGIN(Client)
class CVendingMachine final : public CGameObjectModel
{
public:
	enum class ANIMSTATE : _uint {
		CLOSE = 0,
		CLOSE2 = 1,
		OPEN = 2,
		INACTIVE = 3
	};

private:
	CVendingMachine(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	explicit CVendingMachine(const CVendingMachine& rhs);
	virtual ~CVendingMachine() DEFAULT;

public:
	virtual	HRESULT					NativeConstruct_Prototype() override;
	virtual HRESULT					NativeConstruct(void* pArg) override;
	virtual _int					Tick(_double dTimeDelta) override;
	virtual _int					LateTick(_double dTimeDelta) override;
	virtual HRESULT					Render() override;

private:
	virtual	HRESULT					SetUp_Components() override;
	
private:
	void							Check_Player(CGameInstance* pGameInstance, _double dTimeDelta);
	void							Check_Key(CGameInstance* pGameInstance);

public:
	void							Set_AlphaTest();

public:
	virtual void					Notify(void* pMessage)			override;

protected:
	virtual _int					VerifyChecksum(void* pMessage)	override;

protected:
	virtual HRESULT					Update_UI(_double dDeltaTime)	override;

private:
	LEVEL							m_eNextLevel = LEVEL::COUNT;
	ANIMSTATE						m_eAnimState = ANIMSTATE::INACTIVE;
	CRenderer::RENDERGROUP			m_eGroup = CRenderer::RENDERGROUP::ALPHABLEND;

private:
	_float							m_fRotation = 0.f;
	_double							m_dDuration = 0.0;
	_bool							m_bCollided = false;
	_bool							m_bActive = false;
	_bool							m_bIsObserverSet = false;

	_bool							m_bIsOpenSound = false;

public:
	static	CVendingMachine*		Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual	CVendingMachine*		Clone(void* pArg) override;
	virtual	void					Free() override;
};
END
#endif // !__CLIENT_VENDING_MACHINE_H__