#pragma once
#ifndef __ENGINE_GAME_INSTANCE_H__
#define __ENGINE_GAME_INSTANCE_H__

/* System Category */
#include "GraphicDevice.h"
#include "InputDevice.h"
#include "TimerManager.h"

/* Utility Category */
#include "ComponentManager.h"
#include "LevelManager.h"
#include "GameObjectManager.h"
#include "PipeLine.h"
#include "LightManager.h"
#include "RenderTargetManager.h"
#include "Picking.h"
#include "Frustum.h"
#include "ObserverManager.h"
#include "SubjectManager.h"
#include "RandomManager.h"
#include "FontManager.h"

/* Graphic Debugger */
#include "GraphicDebugger.h"



BEGIN(Engine)
class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)

private:
	explicit CGameInstance();
	virtual ~CGameInstance() DEFAULT;

public:
#ifdef _DEBUG
	HRESULT							Initialize(HWND hMainWnd);
#endif // _DEBUG
	HRESULT							Initialize(HINSTANCE hInstance, CGraphicDevice::DESC& tGraphicDesc, _uint iNumLevels
		, _pGraphicDevice* ppOutGraphicDevice, _pContextDevice* ppOutContextDevice);
	// For Tool
	HRESULT							Initialize(HINSTANCE hInstance, CGraphicDevice::DESC& tGraphicDesc, HWND hWnd, _uint iNumLevels
		, _pGraphicDevice* ppOutGraphicDevice, _pContextDevice* ppOutContextDevice);
	_int							Tick(_double dTimeDelta);
	void							Clear_Container(_uint iNumLevels);
	HWND							Get_hWnd() const;


public:
	/* System Category Function */
/*  00. Graphic Device */
	HRESULT							Clear_BackBufferView(XMFLOAT4 vClearColor = XMFLOAT4(0.f, 0.f, 0.f, 1.f));
	HRESULT							Clear_DepthStencilView();
	HRESULT							Present();

/*  01. Input Device */
	_byte							Get_DIKeyState(_ubyte btKeyID);
	_byte							Get_DIMouseButtonState(CInputDevice::DIMB eMouseButton);
	_long							Get_DIMouseMoveState(CInputDevice::DIMM eMouseMove);

	_bool							Key_Pressing(_ubyte btKeyID);
	_bool							Key_Down(_ubyte btKeyID);
	_bool							Key_Up(_ubyte btKeyID);

	_bool							Mouse_Pressing(CInputDevice::DIMB eMouseButton);
	_bool							Mouse_Down(CInputDevice::DIMB eMouseButton);
	_bool							Mouse_Up(CInputDevice::DIMB eMouseButton);

/*  02. Timer Manager */
	HRESULT							Add_Timer(const _tchar* pTimerTag);
	_double							Compute_Timer(const _tchar* pTimerTag);

	/* Utility Category Function */
/*  00. Component Manager */
	HRESULT							Add_Component(_uint iLevelIndex, const _tchar* pComponentTag, CComponent* pComponent);
	CComponent*						Clone_Component(_uint iLevelIndex, const _tchar* pComponentTag, void* pArg = nullptr);

/*  01. Level Manager */
	HRESULT							Open_Level(CLevel* pLevel);
	HRESULT							Render_Level();
	_uint							Get_LevelID() const;
	const _uint&					Get_NextLevelID() const;
	void							Set_NextLevelID(const _uint& iLevelID);
	CLevel*							Get_CurrentLevel();

/*  02. GameObject Manager */
	CGameObject*					Get_GameObjectPtr(_uint iLevelIndex, const _tchar* pLayerTag, _uint iIndex = 0);
	CGameObject*					Clone_GameObject(const _tchar* pPrototypeTag, void* pArg = nullptr);
	CComponent*						Get_ComponentPtr(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pComponentTag, _uint iIndex = 0);
	HRESULT							Add_GameObject(const _tchar* pPrototypeTag, CGameObject* pPrototype);
	HRESULT							Add_GameObjectToLayer(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pLayerTag, void* pArg = nullptr);
	HRESULT							Add_GameObjectToLayer(_uint iLevelIndex, class CGameObject* pInstance, const _tchar* pLayerTag);
	HRESULT							Add_GameObjectToLayerWithPtr(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pLayerTag, class CGameObject** ppOut, void* pArg = nullptr);
	HRESULT							Add_CollisionTag(_uint iFirstLevel, const wchar_t* pFirstTag,_uint iFirstType, _uint iSecondLevel, const wchar_t* pSecondTag, _uint iSecondType);
	HRESULT							Clear_CollisionTag();
	void							Clear_GameObject(_uint iIndex);

	/* Tool */
	_bool							IsFind_Prototype(const _tchar* pPrototypeTag);
	list<CGameObject*>*				Get_ObjectList(_uint iLevelIndex, const _tchar* pLayerTag);

/*  03. Pipe Line */
	_matrix							Get_Transform(CPipeLine::TRANSFORM eState);
	_matrix							Get_Transpose(CPipeLine::TRANSFORM eState);
	_matrix							Get_Inverse(CPipeLine::TRANSFORM eState);
	_matrix							Get_OldTransform(CPipeLine::TRANSFORM eState);
	_matrix							Get_OldTranspose(CPipeLine::TRANSFORM eState);
	_vector							Get_CameraPosition();
	_float							Get_Far();
	_float							Get_Near();
	_float							Get_FOV();
	_double							Get_TimeDelta();


/*  04. Light Manager */
	HRESULT							Add_Light(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, const _tchar* pLightTag, const _tchar* pShaderFilePath, const char* pTechniqueName, const LIGHTDESC& tLightDesc, const LIGHTDEPTHDESC& tDepthDesc = LIGHTDEPTHDESC());
	HRESULT							Add_Light_ForTool(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, const _tchar* pLightTag, const _tchar* pShaderFilePath, const char* pTechniqueName, const LIGHTDESC& tLightDesc, const LIGHTDEPTHDESC& tDepthDesc = LIGHTDEPTHDESC());

	// Prototype Pattern - Create
	HRESULT							Add_Light(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, const _tchar* pPrototypeTag, const _tchar* pShaderFilePath, const char* pTechniqueName);
	// Prototype Pattern - Clone
	HRESULT							Add_Light(const _tchar* pPrototypeLightTag, const _tchar* pLightTag, const LIGHTDESC& tLightDesc);

	// Light Trace Function
	void							Trace_SpotLight(const _tchar* pLightTag, _fvector vTargetPosition);
	void							Trace_SpotLight_EyeToAt(const _tchar* pLightTag, _fvector vEye, _fvector vAt);

	HRESULT							Light_Active(const _tchar* pLightTag);
	HRESULT							Light_DeActive(const _tchar* pLightTag);

	class CLight*					Get_Light(const _tchar* pLightTag, _uint iLightIndex = 0);
	LIGHTDESC*						Get_LightDesc(const _tchar* pLightTag, _uint iLightIndex = 0);
	LIGHTDEPTHDESC*					Get_LightDepthDesc(const _tchar* pLightTag, _uint iLightIndex = 0);
	list<class CLight*>*			Get_LightList(const _tchar* pLightTag);

	void							ResetPosition_Shadow_DirectionLight(const _tchar* pLightTag, _fvector vPosition, _uint iLightIndex = 0);
	
	void							Increase_LightColor(const _tchar* pLightTag, _float4 vIncreasePower, _float4 vMaxIncreasePower, _uint iColor);
	void							Decrease_LightColor(const _tchar* pLightTag, _float4 vDecreasePower, _float4 vMinIncreasePower, _uint iColor);
	void							Increase_InnerDegree(const _tchar* pLightTag, _float fIncreaseDegree, _float fMaxDegree);
	void							Decrease_InnerDegree(const _tchar* pLightTag, _float fDecreaseDegree);
	void							Increase_OuterDegree(const _tchar* pLightTag, _float fIncreaseDegree, _float fMaxDegree);
	void							Decrease_OuterDegree(const _tchar* pLightTag, _float fDecreaseDegree);
	void							Increase_Range(const _tchar* pLightTag, _float fIncreaseRange, _float fMaxRange);
	void							Decrease_Range(const _tchar* pLightTag, _float fDecreaseRange, _float fMinRange);

	HRESULT							Delete_LightIndex(const _tchar* pLightTag, _uint iLightIndex);
	HRESULT							Delete_LightList(const _tchar* pLightTag);
	HRESULT							DeActiveAll();
	HRESULT							Clear_Light();

/*  05. RenderTarget Manager */
	HRESULT							Add_RenderTarget(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, const _tchar* pTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, _uint iRenderIndex = 0,_float4 vClearColor = XMFLOAT4(0.f, 0.f, 0.f, 0.f));
	HRESULT							Add_MRT(const _tchar* pMRTTag, const _tchar* pTargetTag);
	ID3D11ShaderResourceView*		Get_SRV(const _tchar* pTargetTag);

/*  06. Picking */
	// 피킹 하기 전에 월드 값 변환 있으면 변환 시켜서 넘겨주십쇼
	_bool							Picking(_fvector v0, _fvector v1, _fvector v2, _float fDistance);
	_fvector						Get_Pivot();
	_fvector						Get_Ray();

/*  07. Frustum */
	_bool							Culling(CTransform* pTransform, _float fRadius = 1.f);
	_bool							Culling(_fvector vPos, _matrix matWorld, _float fRadius = 1.f);
	_bool							Culling(_fvector vPos, _float fRadius = 1.f);
	_bool							Culling(BoundingBox* pBoundingBox);

/*  08. Observer Manager */
	HRESULT							Create_Observer(const _tchar* pObserverTag, class CGameObject* pTarget);
	CObserver*						Get_Observer(const _tchar* pObserverTag);
	HRESULT							Delete_Observer(const _tchar* pObserverTag);
	void							Release_Observer();

/*  09. Subject Manager */
	HRESULT							Create_Subject(const _tchar* pSubjectTag);
	CSubject*						Get_Subject(const _tchar* pSubjectTag);
	HRESULT							Delete_Subject(const _tchar* pSubjectTag);
	void							Release_Subject();

/*  10. Random Manager */
	HRESULT							Random_Range(int min, int max);
	int								Random();
	int								Random_From_10000(int min, int max);

/*  11. Font Manager */
	HRESULT							Add_Font(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, const _tchar* pFontTag, const _tchar* pFontFilePath);
	HRESULT							Render_Font(const _tchar* pFontTag, const _tchar* pText, _float2 vPosition, _fvector vColor = XMVectorSet(0.f, 0.f, 0.f, 1.f));


/* Graphic Debugger */
	void							Notice(const char* pNotify, _bool bLinebreak = true);
	void							Request();

private: // Linked Manager
	HWND							m_hWnd;


	/* System Category */
	CGraphicDevice*					m_pGraphicDevice		= nullptr;
	CInputDevice*					m_pInputDevice			= nullptr;
	CTimerManager*					m_pTimerManager			= nullptr;

	/* Utility Category */
	CComponentManager*				m_pComponentManager		= nullptr;
	CLevelManager*					m_pLevelManager			= nullptr;
	CGameObjectManager*				m_pGameObjectManager	= nullptr;
	CPipeLine*						m_pPipeLine				= nullptr;
	CLightManager*					m_pLightManager			= nullptr;
	CRenderTargetManager*			m_pRenderTargetManager	= nullptr;
	CPicking*						m_pPicking				= nullptr;
	CFrustum*						m_pFrustum				= nullptr;
	CObserverManager*				m_pObserverManager		= nullptr;
	CSubjectManager*				m_pSubjectManager		= nullptr;
	CRandomManager*					m_pRandomManager		= nullptr;
	CFontManager*					m_pFontManager			= nullptr;

	/* Graphic Debugger */
	CGraphicDebugger*				m_pGraphicDebugger		= nullptr;

private:
	_bool							m_bBulletTime			= false;
	_double							m_dBulletTime			= 0.0;
	_double							m_dBulletLevel			= 1.0;
	_uint							m_iMaxLevel				= 0;
	
	_bool							m_bPause				= false;
	_bool							m_bMiniGamePause		= false;
	_bool							m_bDebugDraw			= true;

public:
	_bool							Get_BulletTime() { return m_bBulletTime; }
	_bool							Get_Pause() { return m_bPause; }
	_bool							Get_MiniGamePause() { return m_bMiniGamePause; }
	const _double					Get_BulletRemaining() const { return m_dBulletTime; }
	const _double					Get_BulletLevel() const { return m_dBulletLevel; }
	_bool							Get_DebugDraw() const { return m_bDebugDraw; }

public:
	void							Set_BulletLevel(_double dBulletLevel);
	void							Set_BulletRemaining(_double dBulletTime);
	void							Set_BulletTime(_bool bBulletTime);
	void							Set_Pause(_bool bPause);
	void							Set_MiniGamePause(_bool bPause);
	void							Set_DebugDraw(_bool bDraw) { m_bDebugDraw = bDraw; }

public:
	static	void					Release_Engine();
	virtual	void					Free() override;

};
END
#endif // !__ENGINE_GAME_INSTANCE_H__