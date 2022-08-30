#pragma once

#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CTransform;
class CNavigation;
class CVIBuffer_Terrain;
END

class CTerrain final : public CGameObject
{
private:
	CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CTerrain(const CTerrain& rhs);
	virtual ~CTerrain() = default;
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

public:
	void Picking();

public:
	void Set_Picking(_bool bCheck) {
		m_bPicking = bCheck;
	}

public:
	void Set_Pass(_uint iPass) {
		m_iPass = iPass;
	}

	void Set_Range(_uint iRange) {
		m_iRange = iRange;
	}

	void Set_Circle(_bool bCheck) {
		m_bCircle = bCheck;
		m_bRectangle = !bCheck;
	}

	void Set_Rectangle(_bool bCheck) {
		m_bCircle = !bCheck;
		m_bRectangle = bCheck;
	}

	void Set_Dir(_bool bCheck) {
		m_bDir = bCheck;
	}

private:
	CTexture*				m_pTextureCom = nullptr;
	CTexture*				m_pFilterCom = nullptr;
	CTexture*				m_pBrushCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CNavigation*			m_pNavigationCom = nullptr;
	CVIBuffer_Terrain*		m_pModelCom = nullptr;

private:
	_uint					m_iVerticesX = 0;
	_uint					m_iVerticesZ = 0;
	_uint					m_iPass = 0;
	_uint					m_iRange = 0;

private:
	_bool					m_bPicking = false;
	_bool					m_bCircle = false;
	_bool					m_bRectangle = false;
	
private:
	_bool					m_bDir = true; // true: Up, false: Down

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();
};