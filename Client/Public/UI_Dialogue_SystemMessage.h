#pragma once
#include "UI.h"

BEGIN(Client)

class CUI_Dialogue_SystemMessage final : public CUI
{
public:
	enum class DIALOGUESTATE : _uint
	{
		CURRENT		= 0,
		PREVIOUS	= 1,
		NONE		= 2
	};
	const static _uint	iMsgUsed		= 0;
	const static _uint	iMsgEquiped		= 1;
	const static _uint	iMsgAchieved	= 2;
	const static _uint	iMsgOnQuest		= 3;
	const static _uint	iMsgNeedChip	= 4;

	typedef struct tagUISysMsgDesc 
	{
		DIALOGUESTATE	eDialogueState		= DIALOGUESTATE::CURRENT;
		_uint			iOffsetLineCount	= 0;
		_float			fOffsetPosY			= 0.f;
		_double			dTextPushTimeAcc	= 0.0;
		_double			dTimeDurationMax	= 3.0;
		_float			fExpiredAlpha		= 0.f;

		_uint			iItemCode;
		_uint			iItemCount;
		_uint			iMsgCategory;

		tagUISysMsgDesc() {}
		tagUISysMsgDesc(const _uint& ItemCode, const _uint& ItemCount, const _uint& MsgCategory)
			: iItemCode(ItemCode)
			, iItemCount(ItemCount)
			, iMsgCategory(MsgCategory)	{}
	}UISYSMSGDESC;

private:
	CUI_Dialogue_SystemMessage(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CUI_Dialogue_SystemMessage(const CUI_Dialogue_SystemMessage& rhs);
	virtual ~CUI_Dialogue_SystemMessage() DEFAULT;

public:
	static CUI_Dialogue_SystemMessage*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
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
	void									Find_ElemKey(_uint iItemCode, _tchar** szElemKey);

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
	std::list<UISYSMSGDESC*>	m_listRenderSysMsg;

	_float		m_fOffsetYSpeed			= 150.f;
	_float		m_fExpiredAlphaSpeed	= 450.f;
	_double		m_dTimeTextPushMax		= 0.25;

	_uint		m_iRenderNumber = 0;
	_uint		m_iBufferNumber = 0;
	_float		m_fAdditionalTextOffsetX = 0.f;

	const	_float	m_fBasePosX = 1250.f;
	const	_float	m_fBasePosY = 600.f;
	const	_float	m_fBaseScale = 1.f;

	const	_float	m_fBasePositionX = 1260.f;
	
};

END