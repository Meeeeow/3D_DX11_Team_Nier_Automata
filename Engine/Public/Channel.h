#pragma once
#ifndef __ENGINE_CHANNEL_H__
#define __ENGINE_CHANNEL_H__
 


#include "Base.h"

/* 애니메이션 재생을 위한 하나의 뼈의 정보. */

BEGIN(Engine)

class ENGINE_DLL CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;
public:
	_uint Get_NumKeyFrames() {
		return (_uint)m_KeyFrames.size();
	}

	_uint Get_CurrentKeyFrameIndex() {
		return m_iCurrentKeyFrame;
	}

	vector<KEYFRAME*> Get_KeyFrames() {
		return m_KeyFrames;
	}

	_matrix Get_TransformationMatrix() const {
		return XMLoadFloat4x4(&m_TransformationMatrix);
	}

	char*	Get_Name() {
		return m_szName;
	}

	void Set_CurrentKeyFrameIndex(_uint iKeyFrameIndex) {
		m_iCurrentKeyFrame = iKeyFrameIndex;
	}

	void SetUp_TransformationMatrix(_fmatrix TransformationMatrix) {
		XMStoreFloat4x4(&m_TransformationMatrix, TransformationMatrix);
	}
public:
	HRESULT NativeConstruct(const char* pName);
	HRESULT Add_KeyFrame(KEYFRAME* pKeyFrame);
private:
	char			m_szName[MAX_PATH] = "";

private:
	vector<KEYFRAME*>				m_KeyFrames;
	typedef vector<KEYFRAME*>		KEYFRAMES;

private:
	_float4x4		m_TransformationMatrix;

private:
	_uint			m_iCurrentKeyFrame = 0;
public:
	static CChannel* Create(const char* pName);
	virtual void Free() override;
};

END

#endif // !__ENGINE_CHANNEL_H__