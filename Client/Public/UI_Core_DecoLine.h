#pragma once
#include "UI.h"

BEGIN(Client)

class CUI_Core_DecoLine final : public CUI
{
public:
	typedef struct UIDecoLineDesc
	{
		_float			fUpperLinePosY;
		_float			fLowerLinePosY;
	}UIDECOLINEDESC;

private:
	CUI_Core_DecoLine(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CUI_Core_DecoLine(const CUI_Core_DecoLine& rhs);
	virtual ~CUI_Core_DecoLine() DEFAULT;

public:
	static CUI_Core_DecoLine*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*		Clone(void* pArg)				override;
	virtual void				Free()							override;
	virtual HRESULT				NativeConstruct_Prototype()		override;
	virtual HRESULT				NativeConstruct(void* pArg)		override;

private:
	virtual HRESULT				SetUp_Components()				override;

public:
	virtual _int				Tick(_double TimeDelta)			override;
	virtual _int				LateTick(_double TimeDelta)		override;
	virtual HRESULT				Render()						override;

private:
	virtual HRESULT				Activate(_double dTimeDelta)	override;
	virtual HRESULT				Enable(_double dTimeDelta)		override;
	virtual HRESULT				Inactivate(_double dTimeDelta)	override;
	virtual HRESULT				Disable(_double dTimeDelta)		override;
	virtual HRESULT				Pressed(_double dTimeDelta)		override;
	virtual HRESULT				Released(_double dTimeDelta)	override;

	virtual void				Release_UI()					override;

public:
	virtual void				Notify(void* pMessage)			override;

protected:
	virtual _int				VerifyChecksum(void* pMessage)	override;

private:
	_float	m_fUVAnimR2LX = 1.f;
	_float	m_fUVAnimL2RX = 0.f;
};

END