#pragma once

#include "Client_Defines.h"

BEGIN(Client)

class CCam_Object : public CBase
{
public:
	CCam_Object();
	~CCam_Object();

public:
	vector<_float4>* Get_MovingPoints() { return &m_vecMovePoints; }
	vector<_float4>* Get_At() { return &m_vecAt; }
	_float			 Get_Speed() { return m_fSpeed; }
	_uint			 Get_NumPoints() { return m_iNumPoints; }
	_uint			 Get_NumAt() { return m_iNumAt; }

public:
	void			Set_Speed(_float fSpeed) { m_fSpeed = fSpeed; }
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

private:
	vector<_float4>				m_vecMovePoints;
	vector<_float4>				m_vecAt;
	_float						m_fSpeed = 0.f;
	_uint						m_iNumPoints = 0;
	_uint						m_iNumAt = 0;

	// CBase을(를) 통해 상속됨
	virtual void Free() override;
};

END