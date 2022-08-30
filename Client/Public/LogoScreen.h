#pragma once
#ifndef __CLIENT_LOGO_SCREEN_H__
#define __CLIENT_LOGO_SCREEN_H__

#include "ProjectionBase.h"
BEGIN(Client)
class CLogoScreen final : public CProjectionBase
{
private:
	CLogoScreen(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	explicit CLogoScreen(const CLogoScreen& rhs);
	virtual ~CLogoScreen() DEFAULT;

public:
	virtual	HRESULT					NativeConstruct_Prototype() override;
	virtual HRESULT					NativeConstruct(void* pArg) override;
	virtual _int					Tick(_double dTimeDelta) override;
	virtual _int					LateTick(_double dTimeDelta) override;
	virtual HRESULT					Render() override;

public:
	static	CLogoScreen*			Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual	CLogoScreen*			Clone(void* pArg) override;
	virtual void					Free() override;

};
END
#endif // !__CLIENT_LOGO_SCREEN_H__