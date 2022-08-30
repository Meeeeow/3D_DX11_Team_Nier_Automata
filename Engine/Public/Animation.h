#pragma once

#include "Channel.h"

BEGIN(Engine)

class ENGINE_DLL CAnimation final : public CBase
{
private:
	CAnimation();
	virtual ~CAnimation() = default;
public:
	HRESULT NativeConstruct(_double Duration, _double PlaySpeedPerSec);
	HRESULT Add_Channel(CChannel* pChannel);
	void Update_TransformationMatrix(_double TimeDelta, _bool bContinue);
	void Update_CombinedTransformationMatrixNonContinue_Tool(_double TimeDelta);

public:
	void Set_AnimationName(char* pszName) {
		strcpy_s(m_szAnimationName, pszName);
	}

	char* Get_AnimationName() {
		return m_szAnimationName;
	}

	void Initialize_Time();

public:
	const _double Get_PlayTime() const {
		return m_TimeAcc;
	}

	const _double Get_Duration() const {
		return m_Duration;
	}

	const _double Get_PlaySpeedPerSec() const {
		return m_PlaySpeedPerSec;
	}

	const _float4 Get_RootMotionPos() const {
		return m_RootMotionPos;
	}

	const _bool Get_OriginUpdate() const {
		return m_bOriginUpdate;
	}

	const _double Get_BlendTime() const {
		return m_BlendTime;
	}

	vector<CChannel*>* Get_Channels() {
		return &m_Channels;
	}

	const _bool Get_Blend() const {
		return m_bBlend;
	}

	const _double Get_LinkMinRatio() const {
		return m_LinkMinRatio;
	}

	const _double Get_LinkMaxRatio() const {
		return m_LinkMaxRatio;
	}

	const _double Get_PlayTimeRatio() const {
		return m_TimeAcc / m_Duration;
	}

	const _double Get_BlendMaxTime() const {
		return m_BlendMaxTime;
	}
	const _uint Get_BlendLevel() const {
		return m_iBlendLevel;
	}

	const _bool Get_NonUpdatePos() const {
		return m_bNoneUpdatePos;
	}

	const DIRECTION Get_Direction() const {
		return m_eDirection;
	}

	const _double Get_CollisionMinRatio() const {
		return m_CollisionMinRatio;
	}

	const _double Get_CollisionMaxRatio() const {
		return m_CollisionMaxRatio;
	}

	const _float4 Get_BonePosition(const char* pTag) const;

	const _double Get_RootMotionValue() const { return m_RootMotionValue; }

public:
	_float4 Get_BeginRootBone_Position();
	_float4 Get_BeginRootBone_Position_NonFind();

public:
	void Set_PlaySpeedPerSec(_double Speed) {
		m_PlaySpeedPerSec = Speed;
	}

	void Set_PlayTime(_double Time) {
		m_TimeAcc = Time;
	}

	void Set_Duration(_double Time) {
		m_Duration = Time;
	}

	void Set_OriginUpdate(_bool bCheck) {
		m_bOriginUpdate = bCheck;
	}

	void Set_RootMotionPos() {
		m_RootMotionPos = m_vBeginRootBonePos;
	}

	void Set_NextAnimChannel(vector<CChannel*>* pChannels) {
		m_NextChannels = pChannels;
	}

	void Set_NextBlendLevel(_uint iLevel) {
		m_iNextBlendLevel = iLevel;
	}

	void Set_Blend(_bool bCheck) {
		m_bBlend = bCheck;
	}

	void Set_BlendTime(_double Time) {
		m_BlendTime = Time;
	}

	void Set_BlendMaxTime(_double Time) {
		m_BlendMaxTime = Time;
	}

	void Set_LinkMinRatio(_double Ratio) {
		m_LinkMinRatio = Ratio;
	}

	void Set_LinkMaxRatio(_double Ratio) {
		m_LinkMaxRatio = Ratio;
	}

	void Set_BlendLevel(_uint iLevel) {
		m_iBlendLevel = iLevel;
	}

	void Set_NonUpdatePos(_bool bCheck) {
		m_bNoneUpdatePos = bCheck;
	}

	void Set_Direction(DIRECTION eCheck) {
		m_eDirection = eCheck;
	}

	void Set_BeginRootBone_Position(_float4 vPos) {
		m_vBeginRootBonePos = vPos;
	}

	void Set_CollisionMinRatio(_double Ratio) {
		m_CollisionMinRatio = Ratio;
	}

	void Set_CollisionMaxRatio(_double Ratio) {
		m_CollisionMaxRatio = Ratio;
	}

	const _uint Get_RootBone_CurrentKeyFrameIndex() const {
		return m_iRootBoneKeyFrameIndex;
	}

	void Set_RootMotionValue(_double dValue) { m_RootMotionValue = dValue; }


	void Initialize_TimeAcc() {
		m_TimeAcc = 0.0;
		for (auto& pChannel : m_Channels)
			pChannel->Set_CurrentKeyFrameIndex(0);
	}

	void Initialize_OriginUpdate() { m_bOriginUpdate = false, m_isFinished = false; }

	void Initialize_Blend() {
		m_BlendTime = 0.0;
		m_bBlend = false;
		m_NextChannels = nullptr;
		m_iNextBlendLevel = 0;
	}

	const _bool IsFinish_Animation() const;

private:
	/* 현재 애니메이션의 표현을 위한 뼈대들의 집합. */
	/* 매프레임마다 수행. 모든 채널들이 애니메이션을 위해 표현해야할 키프레임의 상태행렬을 만든다. */
	vector<CChannel*>			m_Channels;
	typedef vector<CChannel*>	CHANNEL;

private:
	_double				m_Duration = 0.0;
	_double				m_PlaySpeedPerSec = 0.0;
	_double				m_TimeAcc = 0.0;
	_bool				m_isFinished = false;
	char				m_szAnimationName[MAX_PATH];
	_bool				m_bOriginUpdate = false;

	_float4				m_RootMotionPos;
	_float4				m_vBeginRootBonePos = _float4(0.f, 0.f, 0.f, 1.f);

private:
	/* For. AnimationBlending */
	_double				m_BlendTime = 0.0;
	_bool				m_bBlend = false;
	vector<CChannel*>*	m_NextChannels = nullptr;
	_uint				m_iNextBlendLevel = 0;

private:
	_double				m_LinkMinRatio = 0.0;
	_double				m_LinkMaxRatio = 0.0;
	_double				m_BlendMaxTime = 0.25;
	_uint				m_iBlendLevel = 0;
	_bool				m_bNoneUpdatePos = false;
	DIRECTION			m_eDirection = DIRECTION::FORWARD;

	/* For. Collision Control */
private:
	_double					m_CollisionMinRatio = 0.0;
	_double					m_CollisionMaxRatio = 0.0;

private:
	_double				m_RootMotionValue = 1.0;		//Test

private:
	_uint				m_iRootBoneKeyFrameIndex = 0;

public:
	static CAnimation*	Create(_double Duration, _double PlaySpeedPerSec);
	virtual void		Free() override;
};

END