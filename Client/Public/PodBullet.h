#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTransform;
class CRenderer;
class CVIBuffer_Rect;
class CTexture;
class CCollider;
END

BEGIN(Client)

class CPodBullet final : public CGameObject
{
private:
	CPodBullet(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CPodBullet(const CPodBullet& rhs);
	virtual ~CPodBullet() = default;

public:
	virtual HRESULT					NativeConstruct_Prototype();
	virtual HRESULT					NativeConstruct(void* pArg);
	virtual _int					Tick(_double TimeDelta);
	virtual _int					LateTick(_double TimeDelta);
	virtual HRESULT					Render();

public:
	void							Go_Dir(_double TimeDelta);

public:
	void							Check_Dead(_double TimeDelta);

private:
	HRESULT							SetUp_Components();
	HRESULT							SetUp_ConstantTable();

private:
	class CRenderer*				m_pRenderer = nullptr;
	class CTransform*				m_pTransform = nullptr;
	class CVIBuffer_Rect*			m_pModel = nullptr;
	class CTexture*					m_pTexture = nullptr;
	class CCollider*				m_pSphere = nullptr;

private:
	_double							m_DeadTime = 0.0;

public:
	static CPodBullet*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*			Clone(void* pArg);
	virtual void					Free();
};

END	

