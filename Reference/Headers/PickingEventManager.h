#pragma once
#ifndef __ENGINE_PICKING_EVENT_MANAGER_H__
#define __ENGINE_PICKING_EVENT_MANAGER_H__

#include "Base.h"

BEGIN(Engine)
class CPickingEventManager final : public CBase
{
	DECLARE_SINGLETON(CPickingEventManager)

private:
	CPickingEventManager();
	virtual ~CPickingEventManager() DEFAULT;

public:
	virtual	void			Free() override;
};
END
#endif // !__ENGINE_PICKING_EVENT_MANAGER_H__