#include "..\Public\Animation.h"

CAnimation::CAnimation()
{
}

HRESULT CAnimation::NativeConstruct(_double Duration, _double PlaySpeedPerSec)
{
	m_Duration = Duration;
	m_PlaySpeedPerSec = PlaySpeedPerSec;

	ZeroMemory(m_szAnimationName, sizeof(MAX_PATH));
	m_RootMotionPos = _float4(0.f, 0.f, 0.f, 1.f);

	return S_OK;
}

HRESULT CAnimation::Add_Channel(CChannel * pChannel)
{
	if (nullptr == pChannel)
		return E_FAIL;

	m_Channels.push_back(pChannel);

	return S_OK;
}

/* 0.2*/

/* 매프레임마다 호출된다. */
/* 현재애니메이션의 재생 시간을 측정한다.(누적) */
/* 현재 애니메이션의 재생 시간에 따른 키프레임사이의 행렬을 구성한다. */
void CAnimation::Update_TransformationMatrix(_double TimeDelta, _bool bContinue)
{
	if (m_bBlend)
		m_BlendTime += TimeDelta;
	else
		m_TimeAcc += (m_PlaySpeedPerSec * TimeDelta);

	if (m_TimeAcc >= m_Duration)
	{
		if (bContinue)
		{
			m_TimeAcc = 0.0;
			m_bOriginUpdate = true;
		}

		m_isFinished = true;

	}
	else
		m_isFinished = false;

	_uint		iNumChannel = (_uint)m_Channels.size();

	for (_uint i = 0; i < iNumChannel; ++i)
	{
		CChannel*		pChannel = m_Channels[i];
		_bool			bRootMotion = false;

		if (nullptr == pChannel)
			return;

		if (!strcmp(pChannel->Get_Name(), "bone-1"))
			bRootMotion = true;

		_uint		iNumKeyFrames = pChannel->Get_NumKeyFrames();

		_uint		iCurrentKeyFrameIndex = pChannel->Get_CurrentKeyFrameIndex();

		if (true == m_isFinished)
		{
			iCurrentKeyFrameIndex = 0;

			pChannel->Set_CurrentKeyFrameIndex(iCurrentKeyFrameIndex);
		}

		vector<KEYFRAME*>	KeyFrames = pChannel->Get_KeyFrames();

		_vector		vScale, vRotation, vPosition;
		vScale = XMVectorSet(1.f, 1.f, 1.f, 0.f);
		vRotation = XMVectorZero();
		vPosition = XMVectorSet(0.f, 0.f, 0.f, 1.f);

		_double Ratio = 0.0;

		if (m_bBlend)
		{
			Ratio = m_BlendTime * 4;
			if (Ratio > 1.0)
				Ratio = 1.0;

			if (i >= (m_NextChannels)->size())
				continue;

			CChannel* pNextChannel = ((*m_NextChannels)[i]);
			if (nullptr == pNextChannel)
				return;

			vector<KEYFRAME*> NextKeyFrames = pNextChannel->Get_KeyFrames();

			vScale = XMVectorLerp(XMLoadFloat4(&KeyFrames[iCurrentKeyFrameIndex]->vScale),
				XMLoadFloat4(&NextKeyFrames[0]->vScale), (_float)Ratio);

			vRotation = XMQuaternionSlerp(XMLoadFloat4(&KeyFrames[iCurrentKeyFrameIndex]->vRotation),
				XMLoadFloat4(&NextKeyFrames[0]->vRotation), (_float)Ratio);

			if (bRootMotion)
			{
				if (m_bBlend)
				{
					_int iPreKeyFrameIndex = iCurrentKeyFrameIndex - 5;
					if (0 > iPreKeyFrameIndex)
						iPreKeyFrameIndex = 0;
					else
					{
						if (0 < m_iBlendLevel)
						{
							_uint iIndex = (_uint)NextKeyFrames.size() - 1;

							_float4 vLevelPos = _float4(0.f, 0.f, 0.f, 0.f);

							XMStoreFloat4(&m_RootMotionPos, XMVectorLerp(XMLoadFloat4(&KeyFrames[iCurrentKeyFrameIndex]->vPosition),
								XMLoadFloat4(&KeyFrames[iPreKeyFrameIndex]->vPosition), (_float)Ratio));

							XMStoreFloat4(&vLevelPos, XMVectorLerp(XMLoadFloat4(&NextKeyFrames[0]->vPosition),
								XMLoadFloat4(&NextKeyFrames[iIndex / m_iBlendLevel]->vPosition), (_float)Ratio));

							_float4 vTempPos = _float4(0.f, 0.f, 0.f, 0.f);
							XMStoreFloat4(&vTempPos, XMLoadFloat4(&vLevelPos) + XMLoadFloat4(&m_RootMotionPos));

							m_RootMotionPos = vTempPos;
						}
						else
						{
							XMStoreFloat4(&m_RootMotionPos, XMVectorLerp(XMLoadFloat4(&KeyFrames[iCurrentKeyFrameIndex]->vPosition),
								XMLoadFloat4(&KeyFrames[iPreKeyFrameIndex]->vPosition), (_float)Ratio));
						}
					}
				}
				else
				{
					XMStoreFloat4(&m_RootMotionPos, XMVectorLerp(XMLoadFloat4(&KeyFrames[iCurrentKeyFrameIndex]->vPosition),
						XMLoadFloat4(&NextKeyFrames[0]->vPosition), (_float)Ratio));
				}
			}
			else
			{
				vPosition = XMVectorLerp(XMLoadFloat4(&KeyFrames[iCurrentKeyFrameIndex]->vPosition),
					XMLoadFloat4(&NextKeyFrames[0]->vPosition), (_float)Ratio);
			}
		}
		else
		{
			/* 처음 시작이다. */
			if (m_TimeAcc <= KeyFrames[0]->dTime)
			{
				vScale = XMLoadFloat4(&KeyFrames[0]->vScale);
				vRotation = XMLoadFloat4(&KeyFrames[0]->vRotation);

				if (bRootMotion)
					XMStoreFloat4(&m_RootMotionPos, XMLoadFloat4(&KeyFrames[0]->vPosition));
				else
					vPosition = XMLoadFloat4(&KeyFrames[0]->vPosition);
			}

			else if (m_TimeAcc >= KeyFrames[iNumKeyFrames - 1]->dTime)
			{
				vScale = XMLoadFloat4(&KeyFrames[iNumKeyFrames - 1]->vScale);
				vRotation = XMLoadFloat4(&KeyFrames[iNumKeyFrames - 1]->vRotation);
				if (bRootMotion)
					XMStoreFloat4(&m_RootMotionPos, XMLoadFloat4(&KeyFrames[iNumKeyFrames - 1]->vPosition));
				else
					vPosition = XMLoadFloat4(&KeyFrames[iNumKeyFrames - 1]->vPosition);
			}

			else
			{
				while (m_TimeAcc >= KeyFrames[iCurrentKeyFrameIndex + 1]->dTime)
					pChannel->Set_CurrentKeyFrameIndex(++iCurrentKeyFrameIndex);

				Ratio = (m_TimeAcc - KeyFrames[iCurrentKeyFrameIndex]->dTime) /
					(KeyFrames[iCurrentKeyFrameIndex + 1]->dTime - KeyFrames[iCurrentKeyFrameIndex]->dTime);

				vScale = XMVectorLerp(XMLoadFloat4(&KeyFrames[iCurrentKeyFrameIndex]->vScale),
					XMLoadFloat4(&KeyFrames[iCurrentKeyFrameIndex + 1]->vScale), _float(Ratio));

				vRotation = XMQuaternionSlerp(XMLoadFloat4(&KeyFrames[iCurrentKeyFrameIndex]->vRotation),
					XMLoadFloat4(&KeyFrames[iCurrentKeyFrameIndex + 1]->vRotation), _float(Ratio));

				if (bRootMotion)
				{
					XMStoreFloat4(&m_RootMotionPos, XMVectorLerp(XMLoadFloat4(&KeyFrames[iCurrentKeyFrameIndex]->vPosition),
						XMLoadFloat4(&KeyFrames[iCurrentKeyFrameIndex + 1]->vPosition), _float(Ratio)));
				}
				else
				{
					vPosition = XMVectorLerp(XMLoadFloat4(&KeyFrames[iCurrentKeyFrameIndex]->vPosition),
						XMLoadFloat4(&KeyFrames[iCurrentKeyFrameIndex + 1]->vPosition), _float(Ratio));
				}
			}
		}

		_matrix		TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

		pChannel->SetUp_TransformationMatrix(TransformationMatrix);
	}
}

void CAnimation::Update_CombinedTransformationMatrixNonContinue_Tool(_double TimeDelta)
{
	if (m_isFinished)
		return;

	m_TimeAcc += m_PlaySpeedPerSec * TimeDelta;

	if (m_TimeAcc >= m_Duration)
		m_isFinished = true;
	else
		m_isFinished = false;

	_uint		iNumChannel = (_uint)m_Channels.size();

	for (_uint i = 0; i < iNumChannel; ++i)
	{
		CChannel*		pChannel = m_Channels[i];

		if (nullptr == pChannel)
			return;

		if (!strcmp(pChannel->Get_Name(), "bone-1"))
			m_iRootBoneKeyFrameIndex = pChannel->Get_CurrentKeyFrameIndex();


		_uint		iNumKeyFrames = pChannel->Get_NumKeyFrames();

		_uint		iCurrentKeyFrameIndex = pChannel->Get_CurrentKeyFrameIndex();

		if (true == m_isFinished)
		{
			iCurrentKeyFrameIndex = 0;

			pChannel->Set_CurrentKeyFrameIndex(iCurrentKeyFrameIndex);
			m_TimeAcc = 0.0;
		}

		vector<KEYFRAME*>	KeyFrames = pChannel->Get_KeyFrames();

		_vector		vScale, vRotation, vPosition;
		vScale = XMVectorSet(1.f, 1.f, 1.f, 0.f);
		vRotation = XMVectorZero();
		vPosition = XMVectorSet(0.f, 0.f, 0.f, 1.f);

		/* 처음 시작이다. */
		if (m_TimeAcc <= KeyFrames[0]->dTime)
		{
			vScale = XMLoadFloat4(&KeyFrames[0]->vScale);
			vRotation = XMLoadFloat4(&KeyFrames[0]->vRotation);
			vPosition = XMLoadFloat4(&KeyFrames[0]->vPosition);
		}

		else if (m_TimeAcc >= KeyFrames[iNumKeyFrames - 1]->dTime)
		{
			vScale = XMLoadFloat4(&KeyFrames[iNumKeyFrames - 1]->vScale);
			vRotation = XMLoadFloat4(&KeyFrames[iNumKeyFrames - 1]->vRotation);
			vPosition = XMLoadFloat4(&KeyFrames[iNumKeyFrames - 1]->vPosition);
		}

		else
		{
			while (m_TimeAcc >= KeyFrames[iCurrentKeyFrameIndex + 1]->dTime)
				pChannel->Set_CurrentKeyFrameIndex(++iCurrentKeyFrameIndex);

			_double		Ratio = (m_TimeAcc - KeyFrames[iCurrentKeyFrameIndex]->dTime) /
				(KeyFrames[iCurrentKeyFrameIndex + 1]->dTime - KeyFrames[iCurrentKeyFrameIndex]->dTime);

			vScale = XMVectorLerp(XMLoadFloat4(&KeyFrames[iCurrentKeyFrameIndex]->vScale),
				XMLoadFloat4(&KeyFrames[iCurrentKeyFrameIndex + 1]->vScale), _float(Ratio));

			vRotation = XMQuaternionSlerp(XMLoadFloat4(&KeyFrames[iCurrentKeyFrameIndex]->vRotation),
				XMLoadFloat4(&KeyFrames[iCurrentKeyFrameIndex + 1]->vRotation), _float(Ratio));

			vPosition = XMVectorLerp(XMLoadFloat4(&KeyFrames[iCurrentKeyFrameIndex]->vPosition),
				XMLoadFloat4(&KeyFrames[iCurrentKeyFrameIndex + 1]->vPosition), _float(Ratio));
		}

		_matrix		TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

		pChannel->SetUp_TransformationMatrix(TransformationMatrix);
	}
}

void CAnimation::Initialize_Time()
{
	m_TimeAcc = 0.0;
	m_isFinished = false;

	for (auto& pChannel : m_Channels)
		pChannel->Set_CurrentKeyFrameIndex(0);
}

const _float4 CAnimation::Get_BonePosition(const char* pTag) const
{
	if (pTag == nullptr)
		return _float4(-1.f, -1.f, -1.f, -1.f);

	_uint iBoneIndex = 0;

	for (auto& pChannel : m_Channels)
	{
		if (!strcmp(pTag, pChannel->Get_Name()))
		{
			_matrix TransformMatrix = pChannel->Get_TransformationMatrix();
			_float4x4 TransformFloat4x4;

			XMStoreFloat4x4(&TransformFloat4x4, TransformMatrix);

			_float4 vPos = _float4(0.f, 0.f, 0.f, 1.f);
			memcpy(&vPos, &TransformFloat4x4.m[3], sizeof(_float4));

			return vPos;
		}
		++iBoneIndex;
	}

	return _float4(-1.f, -1.f, -1.f, -1.f);
}

_float4 CAnimation::Get_BeginRootBone_Position()
{
	for (auto& pChannel : m_Channels)
	{
		if (!strcmp("bone-1", pChannel->Get_Name()))
		{
			vector<KEYFRAME*>	vecKeyFrame = pChannel->Get_KeyFrames();
			_float4 vPos = vecKeyFrame.front()->vPosition;

			return _float4(0.f, 0.f, 0.f, 1.f);
		}
	}

	return _float4(-1.f, -1.f, -1.f, 1.f);
}

_float4 CAnimation::Get_BeginRootBone_Position_NonFind()
{
	return m_vBeginRootBonePos;
}

const _bool CAnimation::IsFinish_Animation() const
{
	if (m_TimeAcc >= m_Duration)
		return true;

	return false;
}

CAnimation * CAnimation::Create(_double Duration, _double PlaySpeedPerSec)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->NativeConstruct(Duration, PlaySpeedPerSec)))
	{
		MSGBOX("Failed To Creating CAnimation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimation::Free()
{
	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();

}

