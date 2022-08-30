#include "..\Public\Collision.h"
#include "Layer.h"

void CCollision::Check_LayerCollision(CLayer* pFirstLayer, unsigned int FirstColliderType, CLayer* pSecondLayer, unsigned int SecondColliderType, _double dTimeDelta)
{
	// ù��° ���̾� �±��� ���̾�� ������Ʈ ��ȸ
	if (pFirstLayer->Get_ListSize() != 0)
	{
		for (auto& pFirstObj : *pFirstLayer->Get_ObjectList())
		{
			// �浹���� �ʴ� ����̸� Continue;
			if (pFirstObj->Get_Collision() == false)
				continue;

			// �ι�° ���̾� �±��� ���̾�� ������Ʈ ��ȸ
			if (pSecondLayer->Get_ListSize() != 0)
			{
				for (auto& pSecondObj : *pSecondLayer->Get_ObjectList())
				{
					// �浹���� �ʴ� ����̸� Continue;
					if (pSecondObj->Get_Collision() == false)
						continue;

					// ���� ���̾��� ���� ��ü�� ���ؼ��� ó������ �ʴ´�.
					if (pFirstObj == pSecondObj)
						continue;

					// ù��° ���̾��� ��ü���� �浹 �˻縦 ���ش�.
					// �ι�° ���̾��� ��ü�� �ݶ��̴� Ÿ���� �����ش�.
					// �ڽĿ��� �������̵� �ؼ� ����ؾ���
					if (pFirstObj->Check_Collision(pSecondObj, SecondColliderType))
					{
						// �浹 �̺�Ʈ�� �����Ѵ�. dynamic_cast<Type*> ���� ĳ�����ؼ� ó���ؾ���.
						// First�� �浹 ���ϴ� ���
						pFirstObj->Collision(pSecondObj);
					}
				}
			}
		}
	}
}
