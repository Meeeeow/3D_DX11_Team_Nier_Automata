#pragma once
#ifndef __CLIENT_LOADING_SCREEN_H__
#define __CLIENT_LOADING_SCREEN_H__

#include "ProjectionBase.h"
BEGIN(Client)
class CLoadingScreen final : public CProjectionBase
{
private:
	CLoadingScreen(_pGraphicDevice pGraphicDevice,_pContextDevice pContextDevice);
	explicit CLoadingScreen(const CLoadingScreen& rhs);
	virtual ~CLoadingScreen() DEFAULT;

public:
	virtual	HRESULT					NativeConstruct_Prototype() override;
	virtual HRESULT					NativeConstruct(void* pArg) override;
	virtual _int					Tick(_double dTimeDelta) override;
	virtual _int					LateTick(_double dTimeDelta) override;
	virtual HRESULT					Render() override;

public:
	static	CLoadingScreen*			Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual CLoadingScreen*			Clone(void* pArg) override;
	virtual void					Free() override;
	
};

END
#endif // !__CLIENT_LOADING_SCREEN_H__