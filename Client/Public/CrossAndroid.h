#pragma once

#ifndef __CLIENT_CROSSANDROID_H__
#define __CLINENT_CROSSANDROID_H__

#include "GameObjectModel.h"

BEGIN(Client)

class CCrossAndroid final : public CGameObjectModel
{
private:
	explicit CCrossAndroid(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	explicit CCrossAndroid(const CCrossAndroid& rhs);
	virtual ~CCrossAndroid() DEFAULT;

public:
	virtual	HRESULT					NativeConstruct_Prototype() override;
	virtual HRESULT					NativeConstruct(void* pArg) override;
	virtual _int					Tick(_double dTimeDelta);
	virtual _int					LateTick(_double dTimeDelta);
	virtual HRESULT					Render();

public:
	void							Check_Create(_double TimeDelta);
	void							Check_Landing();

public:
	void							Set_Start(_bool bCheck);

private:
	HRESULT							SetUp_Components();
	HRESULT							SetUp_ConstantTable();

public:
	virtual _bool					Check_Collision(CGameObject* pGameObject, _uint iColliderType) override;
	virtual void					Collision(CGameObject* pGameObject) override;

public:
	void							Check_Beauvoir();

public:
	void							Look_Target();

public:
	virtual void					Notify(void* pMessage)			override;

protected:
	virtual _int					VerifyChecksum(void* pMessage)	override;

protected:
	virtual HRESULT					Update_UI(_double dDeltaTime)	override;

private:
	HRESULT							Update_Effect(_float fTimeDelta);

private:
	_bool							m_bCallHPBarUI = false;
	_double							m_dCallHPBarUITimeDuration = 3.0;

private:
	_double							m_CreateTime = 0.0;
	_double							m_CreateDelayTime = 0.0;

private:
	_double							m_dAppear = 2.5;
	_bool							m_bAppear = false;


private:
	_float							m_fGapAngle = 0.0f;
	_float							m_fAngleRight = 0.0f;

private:
	_uint							m_iAnimation = 0;
	_bool							m_bContinue = false;
	_bool							m_bStart = false;

public:
	static	CCrossAndroid*			Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual	CGameObject*			Clone(void* pArg) override;
	virtual void					Free() override;
};

END

#endif



