#pragma once
#include "UI.h"

BEGIN(Client)

class CUI_HUD_Hpbar_MiniGame final : public CUI
{
public:
	enum class TARGET
	{
		PLAYER	= 0,
		ENEMY	= 1,
		NONE	= 2
	};

	typedef struct tagUIHPBarMiniGameDesc 
	{
		_uint		iObjID;				// 매 틱마다 list를 순회하면서 ObjID가 같은 객체에 대해 패러미터 최신화 수행
		TARGET		eTarget = TARGET::NONE;
		_float		fCrntHPUV = 1.0f;
		_float		fPrevHPUV = 1.0f;
	}HPBARDESC;

private:
	CUI_HUD_Hpbar_MiniGame(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CUI_HUD_Hpbar_MiniGame(const CUI_HUD_Hpbar_MiniGame& rhs);
	virtual ~CUI_HUD_Hpbar_MiniGame() DEFAULT;

public:
	static CUI_HUD_Hpbar_MiniGame*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
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
	std::list<HPBARDESC*>	m_listRenderHPBar;
};

END