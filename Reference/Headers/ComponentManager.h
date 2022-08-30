#pragma once
#ifndef __ENGINE_COMPONENT_MANAGER_H__
#define __ENGINE_COMPONENT_MANAGER_H__

#include "Renderer.h"
#include "Texture.h"

#include "Transform.h"

#include "Navigation.h"
#include "VIBuffer_Rect.h"
#include "VIBuffer_Cube.h"
#include "VIBuffer_Sphere.h"
#include "VIBuffer_Triangle.h"
#include "VIBuffer_TriangleLineStrip.h"
#include "VIBuffer_Terrain.h"
#include "VIBuffer_PointInstance.h"
#include "VIBuffer_RectInstance.h"
#include "VIBuffer_Trail.h"
#include "VIBuffer_AnimationTrail.h"

#include "Collider.h"

#include "Model.h"
#include "AssimpModel.h"
#include "JussimpModel.h"

#include "OctreeModel.h"

#include "Subject.h"


BEGIN(Engine)
class CComponentManager final : public CBase
{
	DECLARE_SINGLETON(CComponentManager)

private:
	explicit CComponentManager();
	virtual ~CComponentManager() DEFAULT;

public:
	typedef unordered_map<const _tchar*, CComponent*>		COMPONENTS;

public:
	HRESULT					Reserve_Container(_uint iNumLevels);
	HRESULT					Add_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, CComponent* pComponent);
	CComponent*				Clone_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, void* pArg);

	void					Clear(_uint iLevelIndex);

private:
	COMPONENTS*				m_pComponents = nullptr;
	_uint					m_iNumLevels = 0;

public:
	virtual	void			Free() override;

};
END
#endif // !__ENGINE_COMPONENT_MANAGER_H__