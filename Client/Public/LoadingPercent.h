#pragma once
#ifndef __CLIENT_LOADING_PERCENT_H__
#define __CLIENT_LOADING_PERCENT_H__

#include "ProjectionBase.h"
BEGIN(Client)
class CLoadingPercent final : public CProjectionBase
{
private:
	CLoadingPercent(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	explicit CLoadingPercent(const CLoadingPercent& rhs);
	virtual ~CLoadingPercent() DEFAULT;

public:
	virtual	HRESULT					NativeConstruct_Prototype() override;
	virtual HRESULT					NativeConstruct(void* pArg) override;
	virtual _int					Tick(_double dTimeDelta) override;
	virtual _int					LateTick(_double dTimedelta) override;
	virtual HRESULT					Render() override;

public:
	void							Increase_Percent();

private:
	_uint							m_iIndex = 0;
	_int							m_iMulti = -1;

public:
	static	CLoadingPercent*		Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual	CLoadingPercent*		Clone(void* pArg) override;
	virtual void					Free() override;

};
END

#endif // !__CLIENT_LOADING_PERCENT_H__