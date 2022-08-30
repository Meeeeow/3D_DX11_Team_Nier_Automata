#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
BEGIN(Engine)
class CRenderer;
class CTransform;
class CVIBuffer_Trail;
class CTexture;
END



BEGIN(Client)

class CTrail : public CGameObject
{
public:
	typedef struct tagTrailDescription
	{
		_int	iNumTrails;
	} TRAIL_DESC;
private:
	TRAIL_DESC					m_tDesc;

protected:
	explicit CTrail(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	explicit CTrail(const CTrail& rhs);
	virtual ~CTrail() DEFAULT;

public:
	static	CTrail*				Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual	CGameObject*		Clone(void * pArg);
	virtual void				Free()							override;
	virtual	HRESULT				NativeConstruct_Prototype()		override;
	virtual HRESULT				NativeConstruct(void* pArg)		override;

public:
	virtual _int				Tick(_double dTimeDelta)		override;
	virtual _int				LateTick(_double dTimeDelta)	override;
	virtual HRESULT				Render()						override;

protected:
	virtual HRESULT				SetUp_Components();
	virtual HRESULT				SetUp_ConstantTable();

public:
	// For Visual Debugging
	virtual const char*			Get_PassName(_uint iIndex);
	virtual _uint				Get_MaxPassIndex();

public:
	virtual void				Notify(void* pMessage) override;
protected:
	virtual _int				VerifyChecksum(void* pMessage) override;

public:
	HRESULT						Add_Position(_float3 _vMinPos, _float3 _vMaxPos);
	HRESULT						Init_TrailPosition(_float3 _vMinPos, _float3 _vMaxPos);
	


protected:
	CRenderer*					m_pRenderer = nullptr;
	CTransform*					m_pTransform = nullptr;
	CVIBuffer_Trail*			m_pModel = nullptr;
	CTexture*					m_pTexture = nullptr;
};

END