#pragma once
#include "ToolObjectBase.h"

BEGIN(Engine)
class CModel;
END

class CToolModel final : public CToolObjectBase
{
private:
	CToolModel(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	explicit CToolModel(const CToolModel& rhs);
	virtual ~CToolModel() DEFAULT;

public:
	typedef struct tagName {
		wchar_t				szModelTag[32];
	}DESC;

public:
	virtual HRESULT			NativeConstruct_Prototype();
	virtual HRESULT			NativeConstruct(void* pArg) override;

	virtual _int			Tick(_double dTimeDelta) override;

	virtual _int			LateTick(_double dTimeDelta) override;
	_int					SelectLateTick(_double dTimeDelta);
	virtual HRESULT			Render() override;
	HRESULT					SelectRender();
	virtual HRESULT			SetUp_ConstantTable() override;

	void					Set_LoadData(MO_INFO tInfo);

public:
	void					Set_Position(_vector vPosition);
	void					Add_Position(_vector vPosition);

	void					Set_Scale(_vector vScale);
	void					Set_Rotation(_float3 vAngle);

public:
	virtual void			Picking() override;
	_bool					Picking(_float3* pOut, _float& fDistance);

public:
	_vector					Get_Position();
	_vector					Get_Scale();

public:
	void					Set_ModelObjectTag(const wchar_t* pTag);
	void					Set_ModelComponentTag(const wchar_t* pTag);
	void					Syncronize_Matrix();
	void					Syncronize_Matrix(_float4x4 matWorld);

	const	MO_INFO&		Get_SaveData();

private:
	CModel*					m_pModel = nullptr;
	_bool					m_bAnim = false;
	_bool					m_bTest = false;

private:
	MO_INFO					m_tModelObjectInfo;

public:
	static	CToolModel*		Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;

};

