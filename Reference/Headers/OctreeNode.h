#pragma once
#ifndef __ENGINE_OCTREE_NODE_H__
#define __ENGINE_OCTREE_NODE_H__

#include "Base.h"
BEGIN(Engine)
class ENGINE_DLL COctreeNode final : public CBase
{
public:
	COctreeNode();
	virtual ~COctreeNode();
};
END
#endif // !__ENGINE_OCTREE_NODE_H__