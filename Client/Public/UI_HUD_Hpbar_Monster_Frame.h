#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_HUD_Hpbar_Monster_Frame final : public CUI
{
private:
	CUI_HUD_Hpbar_Monster_Frame(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CUI_HUD_Hpbar_Monster_Frame(const CUI_HUD_Hpbar_Monster_Frame& rhs);
	virtual ~CUI_HUD_Hpbar_Monster_Frame() = default;

public:
	static CUI_HUD_Hpbar_Monster_Frame*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*				Clone(void* pArg)				override;
	virtual void						Free()							override;
	virtual HRESULT						NativeConstruct_Prototype()		override;
	virtual HRESULT						NativeConstruct(void* pArg)		override;

private:
	virtual HRESULT						SetUp_Components()				override;

public:
	virtual _int						Tick(_double TimeDelta)			override;
	virtual _int						LateTick(_double TimeDelta)		override;
	virtual HRESULT						Render()						override;

private:
	virtual HRESULT						Activate(_double dTimeDelta)	override;
	virtual HRESULT						Enable(_double dTimeDelta)		override;
	virtual HRESULT						Inactivate(_double dTimeDelta)	override;
	virtual HRESULT						Disable(_double dTimeDelta)		override;
	virtual HRESULT						Pressed(_double dTimeDelta)		override;
	virtual HRESULT						Released(_double dTimeDelta)	override;

	virtual void						Release_UI() override {}
};

END