#pragma once
#ifndef __CLIENT_LOGO_TITLE_H__
#define __CLIENT_LOGO_TITLE_H__

#include "ProjectionBase.h"
BEGIN(Client)
class CLogoTitle final : public CProjectionBase
{
private:
	CLogoTitle(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	explicit CLogoTitle(const CLogoTitle& rhs);
	virtual ~CLogoTitle() DEFAULT;

public:
	virtual	HRESULT			NativeConstruct_Prototype() override;
	virtual HRESULT			NativeConstruct(void* pArg) override;
	virtual _int			Tick(_double dTimeDelta) override;
	virtual _int			LateTick(_double dTimeDelta) override;
	virtual HRESULT			Render() override;


public:
	static	CLogoTitle*		Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual	CLogoTitle*		Clone(void* pArg) override;
	virtual void			Free() override;

};
END

#endif // !__CLIENT_LOGO_TITLE_H__