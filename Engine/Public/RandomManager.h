#pragma once
#ifndef __ENGINE_RANDOM_MANAGER_H__
#define __ENGINE_RANDOM_MANAGER_H__

#include "Base.h"

BEGIN(Engine)
class CRandomManager final : public CBase
{
	DECLARE_SINGLETON(CRandomManager)

private:
	explicit CRandomManager();
	virtual ~CRandomManager() = default;

public:
	virtual void	Free() override;	// CBase을(를) 통해 상속됨

public:
	static HRESULT Range(int min, int max)
	{
		distribution = uniform_int_distribution<int>(min, max);
		generator = bind(distribution, engine);

		return S_OK;
	}
	static int Random() { return generator(); }

	int Random_From_10000(int min, int max);


private:
	static mt19937 engine;
	static uniform_int_distribution<int> distribution;
	static _Binder<_Unforced, uniform_int_distribution<int>&, mt19937&> generator;
};
END

#endif // !__ENGINE_RANDOM_MANAGER_H__