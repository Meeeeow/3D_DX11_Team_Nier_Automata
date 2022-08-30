#pragma once
#ifndef __CLIENT_POST_PROCESSOR_H__
#define __CLIENT_POST_PROCESSOR_H__


#include "Base.h"

BEGIN(Client)
class CPostProcessor : public CBase
{
	DECLARE_SINGLETON(CPostProcessor)

private:
	explicit CPostProcessor();
	virtual ~CPostProcessor() DEFAULT;

private:
	class CRenderer*				m_pRenderer = nullptr;

private:
	_pGraphicDevice					m_pGraphicDevice = nullptr;
	_pContextDevice					m_pContextDevice = nullptr;


////////////
private:
	unordered_map<const _tchar*, class CTexture*>	m_mapTexture;
	vector<MYSRV*>									m_vecDeliveredTexture;

public:
	void Set_PP_BeforeUI_SetTexture();
	void Set_PP_AfterUI_SetTexture();


public:
	virtual	void					Free() override;
	HRESULT							NativeConstruct(_pGraphicDevice m_pGraphicDevice, _pContextDevice m_pContextDevice);
};
END

#endif // !__CLIENT_POST_PROCESSOR_H__