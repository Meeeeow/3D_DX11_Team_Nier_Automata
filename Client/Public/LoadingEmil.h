#pragma once
#ifndef __CLIENT_LOADING_EMIL_H__
#define __CLIENT_LOADING_EMIL_H__

#include "ProjectionBase.h"
BEGIN(Client)
class CLoadingEmil final : public CProjectionBase
{
private:
	CLoadingEmil(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	explicit CLoadingEmil(const CLoadingEmil& rhs);
	virtual ~CLoadingEmil() DEFAULT;

public:
	virtual HRESULT			NativeConstruct_Prototype() override;
	virtual HRESULT			NativeConstruct(void* pArg) override;
	virtual _int			Tick(_double dTimeDelta) override;
	virtual _int			LateTick(_double dTimeDelta) override;
	virtual HRESULT			Render() override;


	void					Jump(_double dTimeDelta);
	void					Move(_double dTimeDelta);

private:
	_bool					m_bJump = false;
	_float					m_fDirectionX = 1;
	_float					m_fMinimumX = -608.f;
	_float					m_fMaximumX = 416.f;

public:
	static	CLoadingEmil*	Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual	CLoadingEmil*	Clone(void* pArg) override;
	virtual void			Free() override;

};
END

#endif // !__CLIENT_LOADING_EMIL_H__