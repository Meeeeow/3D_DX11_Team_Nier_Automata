#pragma once
#include "ToolObjectBase.h"

class CEvent_Collider final : public CToolObjectBase
{
public:
	CEvent_Collider(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual ~CEvent_Collider() DEFAULT;

public:
	virtual HRESULT			NativeConstruct_Prototype() override;
	virtual HRESULT			NativeConstruct(void* pArg) override;
	virtual _int			Tick(_double dTimeDelta) override;
	virtual _int			LateTick(_double dTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	void					Set_Name(CString strName) { m_StrName = strName; }
	CString					Get_Name() { return m_StrName; }

public:
	static	CEvent_Collider*		Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, void* pArg = nullptr);
	virtual CGameObject*			Clone(void* pArg);
	virtual void					Free();

	CString							m_StrName;
};

