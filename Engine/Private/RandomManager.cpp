#include "../Public/RandomManager.h"

IMPLEMENT_SINGLETON(CRandomManager)

mt19937 CRandomManager::engine((unsigned int)time(NULL));

uniform_int_distribution<int> CRandomManager::distribution;

_Binder<_Unforced, uniform_int_distribution<int>&, mt19937&> CRandomManager::generator(bind(distribution, engine));

CRandomManager::CRandomManager()
{
}

void CRandomManager::Free()
{
}

int CRandomManager::Random_From_10000(int min, int max)
{
	if (max < min)
		return -1;
	
	int iSub = max - min;
	if (FAILED(Range(0, 10000))) return -1;

	if (iSub == 0)
		return min;

	return min + Random() % iSub;
}
