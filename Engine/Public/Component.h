#pragma once
#ifndef __ENGINE_COMPONENT_H__
#define __ENGINE_COMPONENT_H__

#include "Base.h"

BEGIN(Engine)
class ENGINE_DLL CComponent abstract : public CBase
{
protected:
	CComponent(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	explicit CComponent(const CComponent& rhs);
	virtual ~CComponent() DEFAULT;

public:
	virtual	HRESULT			NativeConstruct_Prototype();
	virtual HRESULT			NativeConstruct(void* pArg);

protected:
	_pGraphicDevice			m_pGraphicDevice	= nullptr;
	_pContextDevice			m_pContextDevice	= nullptr;
	_bool					m_bCloned			= false;

public:
	virtual	CComponent*		Clone(void* pArg) PURE;
	virtual void			Free() override;

};
END
#endif // !__ENGINE_COMPONENT_H__