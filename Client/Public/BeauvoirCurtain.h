#pragma once
#ifndef __CLIENT_BEAUVOIR_CURTAIN_H__
#define __CLIENT_BEAUVOIR_CURTAIN_H__

#include "GameObjectModel.h"

BEGIN(Client)
class CBeauvoirCurtain final : public CGameObjectModel
{
private:
	CBeauvoirCurtain(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	explicit CBeauvoirCurtain(const CBeauvoirCurtain& rhs);
	virtual ~CBeauvoirCurtain() DEFAULT;

public:
	enum class PATTERN : _uint {
		CLOSING			= 0,
		OPENING			= 1,
		OPEND			= 2,
		CLOSED			= 3,
		COUNT			= 4
	};

public:
	virtual	HRESULT					NativeConstruct_Prototype() override;
	virtual HRESULT					NativeConstruct(void* pArg) override;
	virtual _int					Tick(_double dTimeDelta) override;
	virtual _int					LateTick(_double dTimeDelta) override;
	virtual HRESULT					Render() override;

public:
	void							Set_Animation(PATTERN eAnimation);

private:
	virtual HRESULT					SetUp_Components() override;

	_bool							m_bClosed = false;
	_bool							m_bOpend = false;

	PATTERN							m_ePattern = PATTERN::CLOSED;

public:
	static	CBeauvoirCurtain*		Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual	CGameObject*			Clone(void* pArg) override;
	virtual void					Free() override;

};
END
#endif // !__CLIENT_BEAUVOIR_CURTAIN_H__