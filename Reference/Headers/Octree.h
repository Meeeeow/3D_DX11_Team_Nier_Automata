#pragma once
#ifndef __ENGINE_OCTREE_H__
#define __ENGINE_OCTREE_H__

#include "Base.h"
BEGIN(Engine)
class ENGINE_DLL COctree final : public CBase
{
	DECLARE_SINGLETON(COctree)

private:
	explicit COctree();
	virtual ~COctree() DEFAULT;


private:
	typedef	list<class CGameObject*>	OCTREELIST;
	OCTREELIST							m_listOctree;

public:
	virtual	void						Free() override;

};
END
#endif // !__ENGINE_OCTREE_H__