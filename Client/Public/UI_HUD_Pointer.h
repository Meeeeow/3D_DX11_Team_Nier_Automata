#pragma once
#include "UI.h"

BEGIN(Client)

class CUI_HUD_Pointer final : public CUI
{
public:
	enum class POINTERTYPE
	{
		NPC		= 0,
		VIP		= 1,
		OBJ		= 2,
		NONE	= 3
	};

	typedef struct tagUITargetDesc
	{
		_uint		iObjID;				// 매 틱마다 list를 순회하면서 ObjID가 같은 객체에 대해 패러미터 최신화 수행
		POINTERTYPE	eTargetType;		// 몬스터에따라 Render 방식 다르게할 때 사용
		_float4		vWorldPos;			// UI가 Render될 때 반영할 몬스터의 월드 위치
		_double		dTimeDuration;		// UI 재생시간
	}UIPOINTERDESC;

private:
	CUI_HUD_Pointer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CUI_HUD_Pointer(const CUI_HUD_Pointer& rhs);
	virtual ~CUI_HUD_Pointer() = default;

public:
	static CUI_HUD_Pointer*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
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
	std::list<UIPOINTERDESC*>	m_listRenderPointer;

	_float	m_fFloatingY = 0.f;
	_float	m_fFloatingTimeAcc = 0.f;
};

END