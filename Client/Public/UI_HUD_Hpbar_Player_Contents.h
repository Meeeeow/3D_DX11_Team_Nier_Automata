#pragma once
#include "UI.h"

BEGIN(Client)

class CUI_HUD_Hpbar_Player_Contents final : public CUI
{
public:
	enum class PLAYERSTATE
	{
		IDLE		= 0,
		HEALING		= 1,
		DAMAGED		= 2,
		NONE		= 3
	};

	typedef struct tagUIHPBarPlayerDesc 
	{
		_uint		iObjID;				// 매 틱마다 list를 순회하면서 ObjID가 같은 객체에 대해 패러미터 최신화 수행
		PLAYERSTATE	ePlayerState;		// 플레이어 상태에 따라 Render 방식 다르게할 때 사용
		_double		dTimeDuration;		// UI 재생시간
		_float		fCrntHPUV = 1.0f;
		_float		fPrevHPUV = 1.0f;
	}UIHPBARPLAYERDESC;

private:
	CUI_HUD_Hpbar_Player_Contents(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CUI_HUD_Hpbar_Player_Contents(const CUI_HUD_Hpbar_Player_Contents& rhs);
	virtual ~CUI_HUD_Hpbar_Player_Contents() DEFAULT;

public:
	static CUI_HUD_Hpbar_Player_Contents*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*					Clone(void* pArg)				override;
	virtual void							Free()							override;
	virtual HRESULT							NativeConstruct_Prototype()		override;
	virtual HRESULT							NativeConstruct(void* pArg)		override;

private:
	virtual HRESULT							SetUp_Components()				override;

public:
	virtual _int							Tick(_double TimeDelta)			override;
	virtual _int							LateTick(_double TimeDelta)		override;
	virtual HRESULT							Render()						override;

private:
	virtual HRESULT							Activate(_double dTimeDelta)	override;
	virtual HRESULT							Enable(_double dTimeDelta)		override;
	virtual HRESULT							Inactivate(_double dTimeDelta)	override;
	virtual HRESULT							Disable(_double dTimeDelta)		override;
	virtual HRESULT							Pressed(_double dTimeDelta)		override;
	virtual HRESULT							Released(_double dTimeDelta)	override;

	virtual void							Release_UI() override {}

public:
	virtual void							Notify(void* pMessage)			override;

protected:
	virtual _int							VerifyChecksum(void* pMessage)	override;

private:
	std::list<UIHPBARPLAYERDESC*>	m_listRenderHPBar;
};

END