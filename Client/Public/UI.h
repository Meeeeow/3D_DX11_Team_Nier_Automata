#pragma once

#include "Client_Defines.h"
#include "Atlas_Manager.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
class CObserver;
END

BEGIN(Client)

class CUI abstract : public CGameObject
{
public:
	enum class UISTATE : _uint
	{
		ACTIVATE	= 0,
		ENABLE		= 1,
		INACTIVATE	= 2,
		DISABLE		= 3,
		PRESSED		= 4,
		RELEASED	= 5,
		BACKGROUND	= 6,
		NONE		= 7
	};

	//enum class UILAYER : _uint
	//{
	//	HUD_COMMON		= 0,
	//	HUD_HPBAR		= 1,
	//	CORE_BACKGROUND	= 4,
	//	CORE_HEADBUTTON	= 5,
	//	CORE_TITLE		= 5,
	//	CORE_SCROLLLIST	= 6,
	//	CORE_LISTLINE	= 7,
	//	CORE_CURSOR		= 
	//};

	typedef struct tagUIOrthoDesc
	{
		_float	fOffsetX	= 0.f;
		_float	fOffsetY	= 0.f;
		_float	fTexSizeX	= 0.f;
		_float	fTexSizeY	= 0.f;
		_float	fUIPosX		= 0.f;
		_float	fUIPosY		= 0.f;
		_float	fUIScaleX	= 0.f;
		_float	fUIScaleY	= 0.f;
	}UIORTHODESC;

	typedef struct tagTextureUVRatio
	{
		_float	fOriginLeft		= 0.f;
		_float	fOriginRight	= 0.f;
		_float	fOriginTop		= 0.f;
		_float	fOriginBottom	= 0.f;
		_float	fLeft			= 0.f;
		_float	fRight			= 0.f;
		_float	fTop			= 0.f;
		_float	fBottom			= 0.f;
	}TEXUVRATIO;

protected:
	CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CUI(const CUI& rhs);
	virtual ~CUI() = default;

public:
	virtual CGameObject*	Clone(void* pArg) PURE;
	virtual void			Free();
	virtual HRESULT			NativeConstruct_Prototype();
	virtual HRESULT			NativeConstruct(void* pArg);

protected:
	virtual HRESULT			SetUp_Components();

public:
	virtual _int			Tick(_double TimeDelta)			override;
	virtual _int			LateTick(_double TimeDelta)		override;
	virtual HRESULT			Render()						override;

protected:
	virtual HRESULT			Activate(_double dTimeDelta)	PURE;
	virtual HRESULT			Enable(_double dTimeDelta)		PURE;
	virtual HRESULT			Inactivate(_double dTimeDelta)	PURE;
	virtual HRESULT			Disable(_double dTimeDelta)		PURE;
	virtual HRESULT			Pressed(_double dTimeDelta)		PURE;
	virtual HRESULT			Released(_double dTimeDelta)	PURE;

	virtual void			Release_UI()					PURE;

protected:
	void					SetUp_AtlasUV(_uint iCategory, const _tchar* szElemTag);

	// For Orthogonal Projection
	void					SetUp_WorldToScreen(_fvector vWorldPos);
	HRESULT					SetUp_Transform(_float fPosX, _float fPosY, _float fScaleX = 1.f, _float fScaleY = 1.f);
	HRESULT					SetUp_Transform_AlignLeft(_float fPosX, _float fPosY, _float fScaleX = 1.f, _float fScaleY = 1.f);
	HRESULT					SetUp_Transform_AlignRight(_float fPosX, _float fPosY, _float fScaleX = 1.f, _float fScaleY = 1.f);
	HRESULT					SetUp_ScreenPosRect(_float fPosX, _float fPosY, _float fScaleX = 1.f, _float fScaleY = 1.f);
	HRESULT					SetUp_BillBoarding();
	HRESULT					SetUp_DefaultRawValue();

	// For Perspective Projection
	HRESULT					SetUp_Transform_ForPP(_fmatrix	matAffine);
	HRESULT					SetUp_DefaultRawvalue_ForPP();

	// For Atlas Perspective Projection
	HRESULT					SetUp_Transform_ForAtlasPP(_fmatrix	matAffine);
	HRESULT					SetUp_Transform_AlignLeft_ForAtlasPP(_fmatrix matAffine);
	HRESULT					SetUp_Transform_AlignRight_ForAtlasPP(_fmatrix	matAffine);
	HRESULT					SetUp_DefaultRawvalue_ForAtlasPP();

public:
	const UISTATE&			Get_UISTATE() const			{ return m_eUIState;		}
	const RECT&				Get_ScreenPosRect() const	{ return m_ScreenPosRect;	}
	

public:
	virtual void			Notify(void* pMessage)			override;

protected:
	virtual _int			VerifyChecksum(void* pMessage)	override;

protected:
	// Component
	CTexture*				m_pTextureCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CVIBuffer_Rect*			m_pModelCom = nullptr;
	CObserver*				m_pObserver = nullptr;

	// Transform
	UIORTHODESC				m_OrthoDesc;
	TEXUVRATIO				m_AtlasUV;
	_float4x4				m_ProjMatrix;
	_float					m_fWorldToScreenX;
	_float					m_fWorldToScreenY;

	// Color
	std::vector<_float4>	m_vecColorOverlays;

	// Interaction
	UISTATE					m_eUIState = UISTATE::DISABLE;
	RECT					m_ScreenPosRect;
	
};

END