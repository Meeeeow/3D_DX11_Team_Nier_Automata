#pragma once
#ifndef __CLIENT_LOGO_ANY_KEY_H__
#define __CLIENT_LOGO_ANY_KEY_H__


#include "ProjectionBase.h"
BEGIN(Client)
class CLogoAnyKey final : public CProjectionBase
{
private:
	CLogoAnyKey(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	explicit CLogoAnyKey(const CLogoAnyKey& rhs);
	virtual ~CLogoAnyKey() DEFAULT;

public:
	virtual HRESULT				NativeConstruct_Prototype() override;
	virtual HRESULT				NativeConstruct(void* pArg) override;
	virtual _int				Tick(_double dTimeDelta) override;
	virtual _int				LateTick(_double dTimeDelta) override;
	virtual HRESULT				Render() override;

private:
	int							m_iMulti = 1;

public:
	static	CLogoAnyKey*		Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual CLogoAnyKey*		Clone(void* pArg) override;
	virtual void				Free() override;

};
END

#endif // !__CLIENT_LOGO_ANY_KEY_H__