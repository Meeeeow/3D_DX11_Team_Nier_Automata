#pragma once
#ifndef __ENGINE_COLLISION_H__
#define __ENGINE_COLLISION_H__

#include "Base.h"
BEGIN(Engine)
class ENGINE_DLL CCollision
{
public:
	enum class RESULT
	{
		A		= 1,
		B		= 2,
		C		= 3,
		NONE	= 4
	};

	static	void	Check_LayerCollision(class CLayer* pFirstLayer, unsigned int FirstColliderType, class CLayer* pSecondLayer,unsigned int SecondColliderType ,_double dTimeDelta);
};
END
#endif // !__ENGINE_COLLISION_H__