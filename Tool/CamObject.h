#pragma once

#include "ToolObjectBase.h"

#include "Tool_CamCollider.h"

class CCamObject final : public CToolObjectBase
{
public:
	CCamObject(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);

	virtual ~CCamObject() DEFAULT;

public:
	virtual HRESULT			NativeConstruct_Prototype() override;
	virtual HRESULT			NativeConstruct(void* pArg) override;
	virtual _int			Tick(_double dTimeDelta) override;
	virtual _int			LateTick(_double dTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	vector<_float4>* Get_MovingPoints() { return &m_vecMovePoints; }
	vector<_float4>* Get_At() { return &m_vecAt; }
	vector<class CTool_CamCollider*>* Get_MovingPoints_ColliderGroup() { return &m_vecMovePoints_ColliderGroup; }
	vector<class CTool_CamCollider*>* Get_At_ColliderGroup() { return &m_vecAt_ColliderGroup; }
	_float			 Get_Speed() { return m_fSpeed; }
	_uint			 Get_NumPoints() { return m_iNumPoints; }
	_uint			 Get_NumAt() { return m_iNumAt; }

public:
	void			Set_Speed(_float fPositionSpeed) { m_fSpeed = fPositionSpeed; }
	void			Set_NumPoints(_int iNum) { m_iNumPoints = iNum; }
	void			Set_NumAt(_int iNum) { m_iNumAt = iNum; }
	void			Set_MovingPoints(_float4 vPosition) { m_vecMovePoints.push_back(vPosition); }
	void			Set_At(_float4 vPosition) { m_vecAt.push_back(vPosition); }
	void			Set_Points_Plus() { ++m_iNumPoints; }
	void			Set_Points_Minus() { --m_iNumPoints; }
	void			Set_NumAt_Plus() { ++m_iNumAt; }
	void			Set_NumAt_Minus() { --m_iNumAt; }
	void			Set_vecMovePoints(vector<_float4>& pVec) { m_vecMovePoints = pVec; }
	void			Set_vecAt(vector<_float4>& pVec) { m_vecAt = pVec; }
	void			Set_vecMovePoints_Collider(vector<CTool_CamCollider*>& pVec) { m_vecMovePoints_ColliderGroup = pVec; }
	void			Set_vecAt_ColliderGroup(vector<CTool_CamCollider*>& pVec) { m_vecAt_ColliderGroup = pVec; }

public:
	virtual void					Free();

private:
	vector<_float4>				m_vecMovePoints;
	vector<_float4>				m_vecAt;
	vector<class CTool_CamCollider*>			m_vecMovePoints_ColliderGroup;
	vector<class CTool_CamCollider*>			m_vecAt_ColliderGroup;
	_float						m_fSpeed = 2.f;
	_uint						m_iNumPoints = 0;
	_uint						m_iNumAt = 0;

};

