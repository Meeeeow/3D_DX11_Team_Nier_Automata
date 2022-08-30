#pragma once
#ifndef __CLIENT_LOGO_GEAR_H__
#define __CLIENT_LOGO_GEAR_H__

#include "ProjectionBase.h"
BEGIN(Client)
class CLogoGear final : public CProjectionBase
{
private:
	CLogoGear(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	explicit CLogoGear(const CLogoGear& rhs);
	virtual ~CLogoGear() DEFAULT;

public:
	virtual HRESULT			NativeConstruct_Prototype() override;
	virtual HRESULT			NativeConstruct(void* pArg) override;
	virtual _int			Tick(_double dTimeDelta) override;
	virtual _int			LateTick(_double dTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	static	CLogoGear*		Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual CLogoGear*		Clone(void* pArg) override;
	virtual void			Free() override;

};
END

#endif // !__CLIENT_LOGO_GEAR_H__