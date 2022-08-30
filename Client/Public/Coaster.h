#pragma once
#ifndef __CLIENT_COASTER_H__
#define __CLIENT_COASTER_H__

#include "GameObjectModel.h"

BEGIN(Client)
class CCoaster final : public CGameObjectModel
{
public:
	enum class ANIMSTATE : _uint
	{
		RUNNING = 0,
		COMING = 1,
		IDLE = 2,
		NONE = 999
	};

private:
	explicit CCoaster(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	explicit CCoaster(const CCoaster& rhs);
	virtual ~CCoaster() DEFAULT;

public:
	virtual	HRESULT					NativeConstruct_Prototype() override;
	virtual HRESULT					NativeConstruct(void* pArg) override;
	virtual _int					Tick(_double dTimeDelta);
	virtual _int					LateTick(_double dTimeDelta);
	virtual HRESULT					Render();

private:
	void							Update_Bone();
	void							Update_Bone_1();
	void							Update_Bone000();
	void							Update_Bone001();
	void							Update_Bone002();

public:
	void							Update_Navigation(_double TimeDelta);
	void							Set_PlayablePosition();

public:
	void							Start_Coaster();

public:
	_float4x4						Get_Bone002Matrix() { return m_CamBone002_Matrix; }
	_float4x4						Get_Bone001Matrix() { return m_CamBone001_Matrix; }
	_float4x4						Get_Bone000Matrix() { return m_CamBone000_Matrix; }
	_float4x4						Get_Bone_1Matrix() { return m_CamBone_1_Matrix; }
	ANIMSTATE						Get_Coaster_Animstate() { return m_eAnimState; }

private:
	HRESULT							SetUp_Components();
	HRESULT							SetUp_ConstantTable();

public:
	static	CCoaster*					Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual	CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;

private:
	ANIMSTATE				m_eAnimState = ANIMSTATE::NONE;
	_bool					m_bRunningStart = false;
	_uint					m_iAnimIndex = 999;

private:
	_bool					m_bStart = false;

	//for Debug
	_float4x4				m_CamBone_1_Matrix;
	_float4x4				m_CamBone000_Matrix;
	_float4x4				m_CamBone001_Matrix;
	_float4x4				m_CamBone002_Matrix;

	//for Debug
	CCollider*		m_bone_1 = nullptr;
	CCollider*		m_bone000 = nullptr;
	CCollider*		m_bone001 = nullptr;
	CCollider*		m_bone002 = nullptr;
};
END

#endif

