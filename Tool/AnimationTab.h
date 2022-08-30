#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "GameObject.h"


// CAnimationTab 대화 상자입니다.

class CAnimationTab : public CDialog
{
	DECLARE_DYNAMIC(CAnimationTab)

public:
	CAnimationTab(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CAnimationTab();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ANIMATIONTAB };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();

public:
	_int		 Tick(_double TimeDelta);
	_int		 LateTick(_double TimeDelta);

public:
	void Update_Data();
	void Update_Slider();
	void Update_Ratio();
	void Update_RootBoneKeyFrameIndex();
	void Update_BonePosition();

public:
	void Create_Prototype_Layer(const _tchar* pTag);
	void Setting_Render(const _tchar* pTag);
	void Setting_BoneList();
	void Setting_AnimationList();

public:
	void Create_Layer(const _tchar* pTag, class CToolObject* pObject);

public:
	afx_msg void OnCbnSelchangeAnimationcombobox();
	afx_msg void OnLbnSelchangeAnimationlist();
	afx_msg void OnBnClickedButtonPlay();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedAnimationbuttonSTOP();

	afx_msg void OnEnChangeAnimationeditPlaySpeedPerSec();
	afx_msg void OnDeltaposAnimationspinPlaySpeedPerSec(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnBnClickedAnimationbuttonSave();
	afx_msg void OnBnClickedAnimationbuttonLoad();

	afx_msg void OnDeltaposAnimationspinPlayTime(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeAnimationeditPlayTime();
	afx_msg void OnBnClickedButtonTimeReset();

public:
	afx_msg void OnBnClickedButtonCamReset();

public:
	afx_msg void OnBnClickedAnimationcheckCollRenderCheckBox();
	afx_msg void OnLbnSelchangeAnimationlistBoneListBox();
	afx_msg void OnBnClickedAnimationbuttonApply();
	afx_msg void OnEnChangeAnimationeditTimeRatio();
	afx_msg void OnBnClickedButtonBlendMaxTimeApply();
	afx_msg void OnEnChangeAnimationeditBlendMaxTime();
	afx_msg void OnDeltaposAnimationspinBlendingLevel(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonBlendingLevelApply();
	afx_msg void OnBnClickedAnimationcheckNoneUpdatePosition();

	unordered_map<const _tchar*, class CGameObject*>		m_MapLayer;
	const _tchar*		m_pCurModelTag = nullptr;

	CSpinButtonCtrl		m_SpinCtrlPlaySpeed;
	CSpinButtonCtrl		m_SpinCtrlPlayTime;

	CComboBox			m_CharacterComboBox;
	CSliderCtrl			m_SliderPlayTime;
	CListBox			m_AnimationListBox;
	CListBox			m_BoneListBox;

	_double				m_PlayTime;
	_double				m_Duration;
	_double				m_PlaySpeedPerSec;
	
	CButton				m_CollRenderCheckBox;
	CButton				m_bNoneUpdatePostion;
	
	_double				m_BlendMaxTime;
	_double				m_RatioTime;
	_double				m_LinkMinRatio;
	_double				m_LinkMaxRatio;

	_uint				m_iBlendingLevel;
	_int				m_iAnimationIndex;

	_uint				m_iRootBoneKeyFrameIndex;

	_pGraphicDevice		m_pGraphicDevice = nullptr;
	_pContextDevice		m_pContextDevice = nullptr;
	
	double				m_BonePositionX;
	double				m_BonePositionY;
	double				m_BonePositionZ;
	_uint				m_eDirection;
	afx_msg void		OnEnChangeAnimationeditDirection();
	double m_RootMotionValue;
	CSpinButtonCtrl m_SpinCtrlRootMotionValue;
	afx_msg void OnDeltaposAnimationspinRootMotionValue(NMHDR *pNMHDR, LRESULT *pResult);
	double m_CollisionMinRatio;
	double m_CollisionMaxRatio;
	CSpinButtonCtrl m_SpinCtrlCollisionMinRatio;
	CSpinButtonCtrl m_SpinCtrlCollisionMaxRatio;
	afx_msg void OnDeltaposAnimationspinCollisionMinRatio(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposAnimationspinCollisionMaxRatio(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeAnimationeditCollisionMinRatio();
	afx_msg void OnEnChangeAnimationeditCollisionMaxRatio();
};
