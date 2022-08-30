#pragma once
#include "UI.h"

BEGIN(Client)

class CUI_HUD_Hpbar_Player_Frame final : public CUI
{
public:
	enum class PLAYERSTATE
	{
		IDLE		= 0,
		HEALING		= 1,
		DAMAGED		= 2,
		NONE		= 3
	};

	typedef struct tagUIHPBarPlayerFrameDesc
	{
		_uint		iObjID;				// �� ƽ���� list�� ��ȸ�ϸ鼭 ObjID�� ���� ��ü�� ���� �з����� �ֽ�ȭ ����
		PLAYERSTATE	ePlayerState;		// �÷��̾� ���¿� ���� Render ��� �ٸ����� �� ���
		_double		dTimeDuration;		// UI ����ð�
	}UIHPBARPLAYERFRAMEDESC;

private:
	CUI_HUD_Hpbar_Player_Frame(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CUI_HUD_Hpbar_Player_Frame(const CUI_HUD_Hpbar_Player_Frame& rhs);
	virtual ~CUI_HUD_Hpbar_Player_Frame() DEFAULT;

public:
	static CUI_HUD_Hpbar_Player_Frame*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
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

public:
	virtual void						Notify(void* pMessage)			override;

protected:
	virtual _int						VerifyChecksum(void* pMessage)	override;

private:
	std::list<UIHPBARPLAYERFRAMEDESC*>	m_listRenderHPBar;
};

END