#pragma once
#ifndef __CLIENT_LOGO_COPY_LIGHTS_H__
#define __CLIENT_LOGO_COPY_LIGHTS_H__

#include "ProjectionBase.h"
BEGIN(Client)
class CLogoCopyRight final : public CProjectionBase
{
private:
	CLogoCopyRight(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	explicit CLogoCopyRight(const CLogoCopyRight& rhs);
	virtual ~CLogoCopyRight() DEFAULT;

public:
	virtual	HRESULT				NativeConstruct_Prototype() override;
	virtual HRESULT				NativeConstruct(void* pArg) override;
	virtual _int				Tick(_double dTimeDelta) override;
	virtual _int				LateTick(_double dTimeDelta) override;
	virtual HRESULT				Render() override;
		 
public:
	static	CLogoCopyRight*		Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual CLogoCopyRight*		Clone(void* pArg) override;
	virtual void				Free() override;

};
END

#endif // !__CLIENT_LOGO_COPY_LIGHTS_H__