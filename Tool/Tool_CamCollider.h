#pragma once

#include "ToolObjectBase.h"

class CTool_CamCollider final : public CToolObjectBase
{
private:
	CTool_CamCollider(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual ~CTool_CamCollider() DEFAULT;

public:
	virtual	HRESULT					NativeConstruct_Prototype(void* pArg);
	virtual _int					Tick(_double dTimeDelta);
	virtual _int					LateTick(_double dTimeDelta);
	virtual HRESULT					Render();

public:
	virtual void					Picking();
	_bool							Picking(_fvector vOrigin, _fvector vRay, _float3& vPos, _float& fDistance);


public:
	static	CTool_CamCollider*		Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, void* pArg);
	virtual void					Free();



};

