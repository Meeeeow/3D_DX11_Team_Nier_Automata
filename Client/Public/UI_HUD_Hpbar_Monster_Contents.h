#pragma once
#include "UI.h"
#include "UI_Dialogue_EngageMessage.h"

BEGIN(Client)

class CUI_HUD_Hpbar_Monster_Contents final : public CUI
{
public:
	enum class MONSTERTYPE : _uint
	{
		EMPTY	= 0,
		COMMON	= 1,
		BOSS	= 2,
		NONE	= 3
	};

	typedef struct tagUIHPBarMonsterDesc
	{
		_uint		iObjID;				// 매 틱마다 list를 순회하면서 ObjID가 같은 객체에 대해 패러미터 최신화 수행
		MONSTERTYPE	eMonsterType;		// 몬스터에따라 Render 방식 다르게할 때 사용
		
		_float		fPosOffset = 0.5f;
		_float4		vWorldPos;			// UI가 Render될 때 반영할 몬스터의 월드 위치
		_double		dTimeDuration;		// UI 재생시간
		_float		fCrntHPUV = 1.0f;
		_float		fPrevHPUV = 1.0f;

		CUI_Dialogue_EngageMessage::BOSSID	eID = CUI_Dialogue_EngageMessage::BOSSID::NONE;
	}UIHPBARMONDESC;

private:
	CUI_HUD_Hpbar_Monster_Contents(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CUI_HUD_Hpbar_Monster_Contents(const CUI_HUD_Hpbar_Monster_Contents& rhs);
	virtual ~CUI_HUD_Hpbar_Monster_Contents() DEFAULT;

public:
	static CUI_HUD_Hpbar_Monster_Contents*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
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

	virtual void			Release_UI() override {}

public:
	virtual void			Notify(void* pMessage)			override;

protected:
	virtual _int			VerifyChecksum(void* pMessage)	override;

private:
	std::list<UIHPBARMONDESC*>	m_listRenderHPBar;

	const _float	m_fDefaultScale = 0.5f;

	_float	m_fScaleOffset = 1.f;
	
	_float	m_fPosOffset = 0.f;

	_float	m_fScreenOffsetUpperLineY = 15.f;
	_float	m_fScreenOffsetLowerLineY = 15.f;
	_float	m_fScreenOffsetDownCircleX = 123.75f;
	_float	m_fScreenOffsetDownCircleY = 18.5f;

	_float	m_fDFLHpBarScaleX = 0.5225f;
	_float	m_fDFLHpBarScaleY = 0.35f;

	_tchar*	m_szBossNameElemKey = nullptr;
};

END