#pragma once
#include "UI.h"

BEGIN(Client)

class CUI_HUD_Target final : public CUI
{
public:
	enum class TARGETTYPE
	{
		NPC		= 0,
		OBJECT	= 1,
		MONSTER = 2,
		NONE	= 3
	};

	typedef struct tagUITargetDesc
	{
		_uint		iObjID;				// �� ƽ���� list�� ��ȸ�ϸ鼭 ObjID�� ���� ��ü�� ���� �з����� �ֽ�ȭ ����
		TARGETTYPE	eTargetType;		// ���Ϳ����� Render ��� �ٸ����� �� ���
		_float4		vWorldPos;			// UI�� Render�� �� �ݿ��� ������ ���� ��ġ
		_double		dTimeDuration;		// UI ����ð�
	}UITARGETDESC;

private:
	CUI_HUD_Target(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CUI_HUD_Target(const CUI_HUD_Target& rhs);
	virtual ~CUI_HUD_Target() = default;

public:
	static CUI_HUD_Target*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
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
	std::list<UITARGETDESC*>	m_listRenderTarget;
	
	_double		m_dAlphaAnimTimeAcc = 0.0;
	_float		m_fRotateDegree = -1.0f;
};

END