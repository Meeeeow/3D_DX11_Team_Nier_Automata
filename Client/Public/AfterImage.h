#pragma once
#include "Client_Defines.h"

BEGIN(Engine)
class CTransform;
class CVIBuffer_Pointbillboard;
class CRenderer;
END

class CAfterImage : public CGameObject
{
public:
	typedef struct AfterImageDescription
	{
		EFFECT_MOVEMENT				tEffectMovement;
		_float						fMaxLifeTime;
		ID3D11ShaderResourceView*	pSRV;
	} AFTERIMAGE_DESC;

private:
	EFFECT_MOVEMENT							m_tMovement;
	_float									m_fMaxLifeTime = -1.f;
	_float									m_fCurLifeTime = 0.f;

	

private:
	CRenderer*								m_pRenderer = nullptr;
	CTransform*								m_pTransform = nullptr;
	CVIBuffer_Pointbillboard*				m_pPointbillboard = nullptr;
	CTexture*								m_pAlphaOneTexture = nullptr;

private:
	ID3D11ShaderResourceView*				m_pSRV = nullptr;
	ID3D11Texture2D*						m_pTexture2D = nullptr;

private:
	_int									m_iAlphaOneTextureIndex = 0;
	_float									m_fAlphaOneTextureElapsedTime = 0.f;
private:
	HRESULT									SetUp_Components();
	HRESULT									SetUp_ConstantTable();

private:
	explicit CAfterImage(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CAfterImage(const CAfterImage& rhs);
	virtual ~CAfterImage() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

public:
	static CAfterImage* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();
};

