#include "..\Public\Collision.h"
#include "Layer.h"

void CCollision::Check_LayerCollision(CLayer* pFirstLayer, unsigned int FirstColliderType, CLayer* pSecondLayer, unsigned int SecondColliderType, _double dTimeDelta)
{
	// 첫번째 레이어 태그의 레이어로 오브젝트 순회
	if (pFirstLayer->Get_ListSize() != 0)
	{
		for (auto& pFirstObj : *pFirstLayer->Get_ObjectList())
		{
			// 충돌하지 않는 대상이면 Continue;
			if (pFirstObj->Get_Collision() == false)
				continue;

			// 두번째 레이어 태그의 레이어로 오브젝트 순회
			if (pSecondLayer->Get_ListSize() != 0)
			{
				for (auto& pSecondObj : *pSecondLayer->Get_ObjectList())
				{
					// 충돌하지 않는 대상이면 Continue;
					if (pSecondObj->Get_Collision() == false)
						continue;

					// 같은 레이어의 같은 객체에 대해서는 처리하지 않는다.
					if (pFirstObj == pSecondObj)
						continue;

					// 첫번째 레이어의 객체에서 충돌 검사를 해준다.
					// 두번째 레이어의 객체와 콜라이더 타입을 전해준다.
					// 자식에서 오버라이딩 해서 사용해야함
					if (pFirstObj->Check_Collision(pSecondObj, SecondColliderType))
					{
						// 충돌 이벤트를 진행한다. dynamic_cast<Type*> 으로 캐스팅해서 처리해야함.
						// First가 충돌 당하는 대상
						pFirstObj->Collision(pSecondObj);
					}
				}
			}
		}
	}
}
