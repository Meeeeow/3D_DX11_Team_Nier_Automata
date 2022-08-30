#pragma once
#ifndef __ENGINE_RENDERER_H__
#define __ENGINE_RENDERER_H__

#include "Component.h"

BEGIN(Engine)
class ENGINE_DLL CRenderer final : public CComponent
{
private:
	CRenderer(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual ~CRenderer() DEFAULT;

public:
	enum class RENDERGROUP : _uint {
		PRIORITY		= 0,
		ALPHATEST		= 1,
		SHADOWDEPTH		= 2,
		NONLIGHT		= 3,
		ALPHABLEND		= 4,
		UI				= 5,
		CURSOR			= 6,
		MOTIONBLUR		= 7,
		RIMDODGE		= 8,
		COUNT			= 9
	};

	enum class DEBUGGROUP : _uint {
		NAVIGATION		= 0,
		COLLIDER		= 1,
		COUNT			= 2
	};

	enum class PASS_INDEX : _uint {
		DEFFERED										= 0,
		SSAO											= 1,
		LIGHT_DIR										= 2,
		LIGHT_POINT										= 3,
		LIGHT_SPOT										= 4,
		BLEND											= 5,
		OUTLINE											= 6,
		GLOW_HORIZONTAL_19								= 7,
		GLOW_VERTICAL_19								= 8,
		GLOW_HORIZONTAL_9								= 9,
		GLOW_VERTICAL_9									= 10,
		GLOW_HORIZONTAL_41								= 11,
		GLOW_VERTICAL_41								= 12,
		POST_PROCESSING_BEFOREUI						= 13,
		POST_PROCESSING_BEFOREUI_BLUR					= 14,
		POST_PROCESSING_BEFOREUI_DISTORTION				= 15,
		DODGE											= 16,
		POST_PROCESSING_BEFOREUI_DOWNSAMPLING			= 17,
		POST_PROCESSING_BEFOREUI_GAUSSIANBLUR_X			= 18,
		POST_PROCESSING_BEFOREUI_GAUSSIANBLUR			= 19,
		POST_PROCESSING_BEFOREUI_DOF					= 20,
		FINALCOLOR										= 21
	};

	enum class TONE_MAP : int {
		ROMBINDAHOUSETONEMAP							= 0,
		DIRECTXBASED									= 1,
		UNCHARTED2V2									= 2,
		FILMIC											= 3,
		LUMINANCEBASEDREINHARDTONE						= 4,
		ACES											= 5,
		ACES2											= 6,
		MIX0											= 7,
		MIX1											= 8,
		MIX2											= 9,
		MIX3											= 10,
		COUNT											= 11
	};

	enum class DISTORTIONTYPE : _uint {
		DISTORTION_A								= 0,
		DISTORTION_B								= 1,
		DISTORTION_C								= 2,
		NOISE_A											= 3,
		NOISE_B											= 4,
		COUNT											= 5
	};

public:
	typedef list<class CGameObject*>		RENDEROBJECTS;
	typedef list<CComponent*>				RENDERCOMPONENTS;

public:
	HRESULT						NativeConstruct_Prototype(const _tchar* pShaderFilePath, const char* pTechniqueName);
	HRESULT						NativeConstruct(void* pArg) override;

public:
	HRESULT						Add_RenderGroup(RENDERGROUP eGroup, class CGameObject* pGameObject);
	HRESULT						Add_DebugGroup(DEBUGGROUP eGroup, CComponent* pComponent);


private:
	HRESULT						Ready_Default_RenderTarget();
	HRESULT						Ready_Deferred_RenderTargets();
	HRESULT						Ready_SSAO_RenderTargets();
	HRESULT						Ready_SSAO_GaussianBlur_RenderTargets();
	HRESULT						Ready_OutLine_RenderTargets();
	HRESULT						Ready_Shadow_RenderTarget();
	HRESULT						Ready_Light_RenderTargets();
	HRESULT						Ready_AfterDeferred_RenderTargets();
	HRESULT						Ready_GlowHorizontal_RenderTarget();
	HRESULT						Ready_GlowVertical_RenderTarget();
	HRESULT						Ready_MotionBlur_RenderTarget();
	HRESULT						Ready_DeferredResult_RenderTargets();

	HRESULT						Ready_PostProcess0_Default_RenderTargets();
	HRESULT						Ready_PostProcess0_DownSampling_RenderTargets();
	HRESULT						Ready_PostProcess0_GaussianBlur_RenderTargets();
	HRESULT						Ready_PostProcess0_DepthOfField_RenderTargets();
	HRESULT						Ready_PostProcess0_MotionBlur_RenderTargets();

	HRESULT						Ready_PostPrecess2_Distortion_RenderTargets();


	HRESULT						Ready_RimDodge_RenderTargets();
	HRESULT						Ready_Dodge_RenderTargets();
	
public:
	HRESULT						Draw_RenderGroup();

private:
	HRESULT						Render(RENDERGROUP eRenderSequence);

	HRESULT						Sequence_Priority();
	HRESULT						Sequence_ShadowDepth();
	HRESULT						Sequence_AlphaTest();
	HRESULT						Sequence_SSAO();
	HRESULT						Sequence_SSAO_GaussianBlur();
	HRESULT						Sequence_LightAcc();
	HRESULT						Sequence_AlphaBlend();
	HRESULT						Sequence_OutLine();
	HRESULT						Sequence_NonLight();
	HRESULT						Sequence_Result();
	HRESULT						Sequence_Luminance();
	HRESULT						Sequence_Glow_Horizontal_19();
	HRESULT						Sequence_Glow_Vertical_19();
	HRESULT						Sequence_Glow_Horizontal_9();
	HRESULT						Sequence_Glow_Vertical_9();
	HRESULT						Sequence_Glow_Horizontal_41();
	HRESULT						Sequence_Glow_Vertical_41();
	HRESULT						Sequence_MotionBlur();
	HRESULT						Sequence_RimDodge();
	HRESULT						Sequence_PostProcessing_BeforeUI();
	HRESULT						Sequence_UI();
	HRESULT						Sequence_Dodge();

public:
	void Set_PostProcessing_BeforeUI_SRV(MYSRV* tSRV) {
		m_vecPP_BeforeUI_SRV.push_back(tSRV);
	}

	void Set_PostProcessing_AfterUI_SRV(MYSRV* tSRV) {
		m_vecPP_AfterUI_SRV.push_back(tSRV);
	}

	void Set_PostProcessing_Distortion() {
		m_bDistortion = true;
	}

	void Set_PostProcessing_GrayDistortion() {
		m_bGrayDistortion = true;
	}

	void Set_PostProcessing_Dodge() {
		m_bDodge = true;
	}

	void						Set_UVDistance(const _float& fDistance);
	void						ReSet_UVDistance();
	void						Set_Fog(const _float& fFogBegin, const _float& fFogEnd, const _float4& fFogColor);
	void						Set_Fogging(const _int& iFogging);

	_float4*					Get_DOF_Parmas();
	void						Set_DOF_Params(const _float4& vDOF);

	const _uint&				Get_HitIndex();

public:
	const _bool&				Get_Distortion() const { return m_bDistortion; }
	const _bool&				Get_GrayDistortion() const { return m_bGrayDistortion; }
	const _bool&				Get_Dodge() const { return m_bDodge; }
	const _int&					Get_ToneMap() const { return m_iToneMap; }

public:
	ID3D11ShaderResourceView*	Get_DistortionTexture(DISTORTIONTYPE eType);

private:
	vector<MYSRV*>				m_vecPP_BeforeUI_SRV;
	vector<MYSRV*>				m_vecPP_AfterUI_SRV;

public:
	HRESULT						Draw_DebugGroup();

private:
	HRESULT						Render_Navigation();
	HRESULT						Render_Collider();

private:
	RENDEROBJECTS				m_listRenderObjects[(_uint)RENDERGROUP::COUNT];
	RENDERCOMPONENTS			m_listRenderComponents[(_uint)DEBUGGROUP::COUNT];

private:
	class CVIBuffer_Viewport*	m_pVIBuffer = nullptr;

private:
	_bool						m_bBlur = false;
	_bool						m_bDodge = false;
	_bool						m_bGodRay = false;

	_bool						m_bDistortion = false;
	_bool						m_bGrayDistortion = false;
	class CTexture*				m_pDistortionTexture = nullptr;


public:
	const _float&				Get_FogBegin() const { return m_fFogBegin; }
	const _float&				Get_FogEnd() const { return m_fFogEnd; }
	const _float4&				Get_FogColor() const { return m_vFogColor; }
	const _int&					Get_Fogging() const { return m_iFogging; }


private:
	_uint						m_iViewportX = 0;
	_uint						m_iViewportY = 0;
	_float						m_fUVDistance = 0.f;

	_float						m_fFogBegin = 0.f;
	_float						m_fFogEnd = 0.f;
	_float4						m_vFogColor;
	_int						m_iFogging = 0;
	_float2						m_vScreenSize = _float2(0.f,0.f);

private:
	_float4					m_vTargetDOF_Params = _float4(0.f, 75.f, 150.f, 1.f);
	_float						m_fTargetDOF_Distance = 100.f;

public:
	void						Set_DOF_Distance(const _float& fDist);

private:
	_int						m_iToneMap = 0;
	_uint						m_iHit = 0;

private:
	_uint						m_iDebugCnt = 0;
	
	_bool						m_bHalfRender = false;
	_bool						m_bRenderTarget = false;

private:
	_bool						m_bSSAO = true;

public:
	void						SSAO();

public:
	void						Add_RenderDebug();
	void						Half_Rendering();
	void						RenderTarget_Rendering();

public:
	void						Next_ToneMap();
	void						Pre_ToneMap();

public:
	static	CRenderer*			Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, const _tchar* pShaderFilePath, const char* pTechniqueName);
	virtual CComponent*			Clone(void* pArg) override;
	virtual void				Free() override;

};
END
#endif // !__ENGINE_RENDERER_H__