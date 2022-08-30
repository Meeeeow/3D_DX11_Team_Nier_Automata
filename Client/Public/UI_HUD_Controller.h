#pragma once
#include "UI.h"

#define COMMAND_HUD_INITIALIZE		0x00000000
#define COMMAND_HUD_ENABLE			0x00000001
#define COMMAND_HUD_DISABLE			0x00000002
#define COMMAND_HUD_RELEASE			0x00000004


BEGIN(Client)

class CUI_HUD_Controller final : public CUI
{
public:
	typedef struct tagCommandDesc
	{
		_uint			iCommandFlag = COMMAND_HUD_INITIALIZE;
	}COMMANDDESC;

private:
	explicit CUI_HUD_Controller(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CUI_HUD_Controller(const CUI_HUD_Controller& rhs);
	virtual ~CUI_HUD_Controller() = default;

public:
	static CUI_HUD_Controller*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
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
	_uint						Command_Enable_HUD(_double dTimeDelta);
	_uint						Command_Disable_HUD(_double dTimeDelta);
	_uint						Command_Release_HUD(_double dTimeDelta);

public:
	virtual void				Notify(void* pMessage)			override;

protected:
	virtual _int				VerifyChecksum(void* pMessage)	override;

private:
	_uint		m_iCommandFlag = COMMAND_HUD_INITIALIZE;

	_double		m_dTimeOpenCoreMenu = 0.0;
	_double		m_dTimeCloseCoreMenu = 0.0;

	_double		m_dTimeCloseCoreMenuMax = 2.0;
};

END