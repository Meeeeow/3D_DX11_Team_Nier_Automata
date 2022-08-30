#pragma once

#include "ToolObjectBase.h"

class CToolLight final : public CToolObjectBase
{
private:
	CToolLight(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	explicit CToolLight(const CToolLight& rhs);
	virtual ~CToolLight() DEFAULT;

public:
	virtual	HRESULT				NativeConstruct_Prototype() override;
	virtual HRESULT				NativeConstruct(void* pArg) override;
	virtual _int				Tick(_double dTimeDelta) override;
	virtual _int				LateTick(_double dTimeDelta) override;
	virtual HRESULT				Render() override;

public:
	LIGHTDESC					Get_LightDesc();

public:
	HRESULT						Set_LightDesc();

private:
	class CVIBuffer_Sphere*		m_pVIBuffer = nullptr;
	LIGHTDESC					m_tLightDesc; 

public:
	static	CToolLight*			Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual	CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;

};

