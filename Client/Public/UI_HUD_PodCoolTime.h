#pragma once
#include "UI.h"

BEGIN(Client)

class CUI_HUD_PodCoolTime final : public CUI
{
public:
	typedef struct tagUIPodCoolTimeDesc
	{
		_uint		iObjID;
		_float4		vWorldPos;
		_float		fCrntCoolTimeAcc;
		_float		fMaxCoolTimeAcc;
	}UIPODCOOLTIMEDESC;

private:
	CUI_HUD_PodCoolTime(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CUI_HUD_PodCoolTime(const CUI_HUD_PodCoolTime& rhs);
	virtual ~CUI_HUD_PodCoolTime() = default;

public:
	static CUI_HUD_PodCoolTime*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg)				override;
	virtual void			Free()							override;
	virtual HRESULT			NativeConstruct_Prototype()		override;
	virtual HRESULT			NativeConstruct(void* pArg)		override;

private:
	virtual HRESULT			SetUp_Components()				override;

public:
	virtual _int			Tick(_double TimeDelta)			override;
	virtual _int			LateTick(_double TimeDelta)		override;
	virtual HRESULT			Render()						override;

private:
	virtual HRESULT			Activate(_double dTimeDelta)	override;
	virtual HRESULT			Enable(_double dTimeDelta)		override;
	virtual HRESULT			Inactivate(_double dTimeDelta)	override;
	virtual HRESULT			Disable(_double dTimeDelta)		override;
	virtual HRESULT			Pressed(_double dTimeDelta)		override;
	virtual HRESULT			Released(_double dTimeDelta)	override;

	virtual void			Release_UI()					override;

public:
	virtual void			Notify(void* pMessage)			override;

protected:
	virtual _int			VerifyChecksum(void* pMessage)	override;

private:
	std::list<UIPODCOOLTIMEDESC*>	m_listRenderTarget;

	_float	m_fLetterInterval = 0.f;
	_float	m_fDecimalInterval = 8.f;
	_uint	m_iRenderNumber = 0;
	_uint	m_iBufferNumber = 0;
	_float	m_fPosYOffset = 100.f;

};

END