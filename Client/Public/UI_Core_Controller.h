#pragma once
#include "UI.h"

#define COMMAND_CORE_INITIALIZE			0x00000000
#define COMMAND_CORE_OPEN				0x00000001
#define COMMAND_CORE_CLOSE				0x00000002
#define COMMAND_CORE_BUTTON_LOCK		0x00000004
#define COMMAND_CORE_BUTTON_UNLOCK		0x00000008
#define COMMAND_CORE_OPENING			0x00000010
#define COMMAND_CORE_CLOSING			0x00000020
#define COMMAND_CORE_LINE_LOCK			0x00000040
#define COMMAND_CORE_LINE_UNLOCK		0x00000080
#define COMMAND_CORE_SCROLL_UP			0x00000100
#define COMMAND_CORE_SCROLL_DOWN		0x00000200
#define COMMAND_CORE_ITEM_BODY_OPEN		0x00000400
#define COMMAND_CORE_WEAPON_BODY_OPEN	0x00000800
#define COMMAND_CORE_CHIP_BODY_OPEN		0x00001000
#define COMMAND_CORE_ALL_BODY_CLOSE		0x00008000

BEGIN(Client)

class CUI_Core_Controller final : public CUI
{
public:
	enum class INPUTLIMITLEVEL : _uint
	{
		BACKGROUND		= 1,
		HEADBUTTON		= 2,
		LISTLINE		= 3,
		SELECTLIST		= 4,
		NONE			= 999
	};

	typedef struct tagCommandDesc
	{
		//CUI::UISTATE	eUIState = CUI::UISTATE::ACTIVATE;
		_uint			iCommandFlag = COMMAND_CORE_INITIALIZE;
	}COMMANDDESC;

private:
	explicit CUI_Core_Controller(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CUI_Core_Controller(const CUI_Core_Controller& rhs);
	virtual ~CUI_Core_Controller() = default;

public:
	static CUI_Core_Controller*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject *		Clone(void * pArg)				override;
	virtual void				Free()							override;
	virtual HRESULT				NativeConstruct_Prototype()		override;
	virtual HRESULT				NativeConstruct(void* pArg)		override;

private:
	virtual HRESULT				SetUp_Components()				override;

public:
	virtual _int				Tick(_double TimeDelta)			override;
	virtual _int				LateTick(_double TimeDelta)		override;
	virtual HRESULT				Render()						override;

private:
	virtual HRESULT				Activate(_double dTimeDelta)	override;
	virtual HRESULT				Enable(_double dTimeDelta)		override;
	virtual HRESULT				Inactivate(_double dTimeDelta)	override;
	virtual HRESULT				Disable(_double dTimeDelta)		override;
	virtual HRESULT				Pressed(_double dTimeDelta)		override;
	virtual HRESULT				Released(_double dTimeDelta)	override;
	
	virtual void				Release_UI()					override;

private:
	_uint						Command_Open_UI(_double dTimeDelta);
	_uint						Command_Close_UI(_double dTimeDelta);
	_uint						Command_HeadButton_Lock(_double dTimeDelta);
	_uint						Command_HeadButton_Unlock(_double dTimeDelta);
	_uint						Command_ListLine_Lock(_double dTimeDelta);
	_uint						Command_ListLine_Unlock(_double dTimeDelta);
	_uint						Command_Scroll_Up(_double dTimeDelta);
	_uint						Command_Scroll_Down(_double dTimeDelta);

	_uint						Command_Item_Body_Open(_double dTimeDelta);
	_uint						Command_Weapon_Body_Open(_double dTimeDelta);
	_uint						Command_Chip_Body_Open(_double dTimeDelta);
	_uint						Command_All_Body_Close(_double dTimeDelta);

public:
	const INPUTLIMITLEVEL&		Get_CurrentInputLimitLevel() const { return m_eCurrentInputLimitLevel; }

public:
	virtual void				Notify(void* pMessage)			override;

protected:
	virtual _int				VerifyChecksum(void* pMessage)	override;

private:
	_uint			m_iCommandFlag = COMMAND_CORE_INITIALIZE;

	INPUTLIMITLEVEL	m_eCurrentInputLimitLevel = INPUTLIMITLEVEL::HEADBUTTON;

	_double			m_dTimeOpenCoreMenu = 0.0;
	_double			m_dTimeCloseCoreMenu = 0.0;
	_double			m_dTimeCloseCoreMenuMax = 2.0;

	_float			m_fBGMVolumeTarget = 1.0f;
	const _float	m_fBGMVolumeTargetMin = 0.2f;
	const _float	m_fBGMVolumeTargetMax = 1.0f;

	_float			m_fBGMVolumeFadeOutTarget = 1.0;
	_float			m_fBGMVolumeFadeInTarget = 1.0;

	_bool			m_bIsOnce = false;
};

END