#include "..\Public\Channel.h"



CChannel::CChannel()
{
}

HRESULT CChannel::NativeConstruct(const char* pName)
{
	strcpy_s(m_szName, pName);

	return S_OK;
}

HRESULT CChannel::Add_KeyFrame(KEYFRAME * pKeyFrame)
{
	if (nullptr == pKeyFrame)
		return E_FAIL;

	m_KeyFrames.push_back(pKeyFrame);

	return S_OK;
}

CChannel * CChannel::Create(const char* pName)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->NativeConstruct(pName)))
	{
		MSGBOX("Failed To Creating CChannel");
		Safe_Release(pInstance);
	}

	return pInstance;

}

void CChannel::Free()
{
	for (auto& pKeyFrame : m_KeyFrames)
		Safe_Delete(pKeyFrame);

	m_KeyFrames.clear();
}

