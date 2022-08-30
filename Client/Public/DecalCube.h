#pragma once
#ifndef __CLIENT_DECAL_CUBE_H__
#define __CLIENT_DECAL_CUBE_H__

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CVIBuffer_Cube;
class CTexture;
END

BEGIN(Client)
class CDecalCube final : public CGameObject
{
private:
	CDecalCube(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	explicit CDecalCube(const CDecalCube& rhs);
	virtual ~CDecalCube() DEFAULT;

public:
	enum class TYPE : _uint {
		BREAK				= 0,
		BURNINGFIELD	= 1,
		CRACK0				= 2,
		CRACK1				= 3,
		SPIN					= 4,
		BOOM				= 5,
		ARCH				= 6,
		FOGRING			= 7,
		COUNT				= 8,
	};

public:
	typedef struct tagDecalCubeInfo {
		tagDecalCubeInfo() {
			ZeroMemory(this, sizeof(tagDecalCubeInfo));
			eType = CDecalCube::TYPE::BREAK;
			dIndexTime = 0.04;
			bAlpha = true;
			vRotationAxis = _float4(0.f, 1.f, 0.f, 0.f);
		}
		_float4		vScale;
		_float4		vPosition;
		_uint			iImageIndex;
		_double		dDuration;
		TYPE			eType;
		_bool			bContinue;
		_double		dIndexTime;
		_int			iIsGlow;
		_int			iIsColorGlow;
		_float4		vGlowColor;
		_bool			bAlpha;
		_double		dRotationSpeed;
		_float4		vRotationAxis;
		_double		dMoveSpeed;
		_float4		vDir;
		_float			fAddScale;
	}DECALCUBE;

public:
	virtual	HRESULT			NativeConstruct_Prototype() override;
	virtual	HRESULT			NativeConstruct(void* pArg) override;
	virtual _int			Tick(_double dTimeDelta) override;
	virtual _int			LateTick(_double dTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	void					Calc_Alpha();
	void					Calc_Index();

	void					Move(_double dTimeDelta);

private:
	HRESULT					SetUp_Components(DECALCUBE* pInfo);

private:
	HRESULT					SetUp_ConstantTable();

private:
	CVIBuffer_Cube*		m_pVIBuffer		= nullptr;
	CTexture*				m_pTexture		= nullptr;
	CTransform*				m_pTransform	= nullptr;
	CRenderer*				m_pRenderer		= nullptr;

	_uint					m_iImageIndex = 0;
	_uint					m_iMaxIndex = 0;
	_double					m_dDuration = 0.0;
	_double					m_dMaxTime = 0.0;
	_bool					m_bContinue = false;
	_double					m_dIndexTimer = 0.0;
	_double					m_dIndexTime = 0.0;
	_float4					m_vDir = _float4(0.f, 0.f, 0.f, 0.f);
	_float4					m_vRotationAxis = _float4(0.f, 1.f, 0.f, 0.f);
	_bool					m_bAlpha = true;

	_int					m_iGlow = 0;
	_int					m_iColorGlow = 0;
	_float4					m_vGlowColor;
	_float					m_fAddScale = 0.f;

public:
	static	CDecalCube*		Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual CDecalCube*		Clone(void* pArg) override;
	virtual void			Free() override;

};
END
#endif // !__CLIENT_DECAL_CUBE_H__