#pragma once
#include "UI.h"

BEGIN(Client)

class CUI_HUD_Damage final : public CUI
{
public:
	enum class DAMAGETYPE
	{
		PLAYER	= 0,
		MONSTER	= 1,
		MELEE	= 2,
		RANGE	= 3,
		NONE	= 4
	};

	typedef struct tagUIDamageDesc
	{
		DAMAGETYPE	eType;
		_float4		vWorldPos;
		_uint		iDamageAmount;
		_double		dTimeAcc = 0.0;
		_float		fAlphaOffset = 0.0f;
	}UIDAMAGEDESC;

private:
	CUI_HUD_Damage(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CUI_HUD_Damage(const CUI_HUD_Damage& rhs);
	virtual ~CUI_HUD_Damage() DEFAULT;

public:
	static CUI_HUD_Damage*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
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
	std::list<UIDAMAGEDESC*>	m_listRenderDamage;

	_float	m_fLetterInterval;
	_float	m_fAlphaOutSpeed;
	_double	m_dTimeOutSpeed;
	_uint	m_iRenderNumber = 0;
	_uint	m_iBufferNumber = 0;
};

END