#pragma once
#include "afxcmn.h"
#include "afxwin.h"

#include "Event_Collider.h"

// CEventColliderTab 대화 상자입니다.

class CEventColliderTab : public CDialog
{
	DECLARE_DYNAMIC(CEventColliderTab)

public:
	CEventColliderTab(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CEventColliderTab();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EVENTCOLLIDERTAB };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

public:
	_int						Tick(_double dTimeDelta);
	_int						LateTick(_double dTimeDelta);
	_int						Picking(_double dTimeDelta);

public:
	void Update_Transform();
	void Update_Data();
	void Update_ColliderDescScale();
	void Update_ColliderDescPivot();

private:
	_pGraphicDevice				m_pGraphicDevice = nullptr;
	_pContextDevice				m_pContextDevice = nullptr;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLbnSelchangeEventlist1();
	float m_fPosX;
	float m_fPosY;
	float m_fPosZ;
	float m_fPivotX;
	float m_fPivotY;
	float m_fPivotZ;
	float m_fScaleX;
	float m_fScaleY;
	float m_fScaleZ;
	CSpinButtonCtrl m_Spin_PosX;
	CSpinButtonCtrl m_Spin_PosY;
	CSpinButtonCtrl m_Spin_PosZ;
	CSpinButtonCtrl m_Spin_PivotX;
	CSpinButtonCtrl m_Spin_PivotY;
	CSpinButtonCtrl m_Spin_PivotZ;
	CSpinButtonCtrl m_Spin_ScaleX;
	CSpinButtonCtrl m_Spin_ScaleY;
	CSpinButtonCtrl m_Spin_ScaleZ;
	afx_msg void OnBnClickedEventSavebutton();
	afx_msg void OnBnClickedEventLoadbutton();
	afx_msg void OnBnClickedEventCreatebutton();
	afx_msg void OnBnClickedEventDeletebutton();
	CString m_strName;
	afx_msg void OnEnChangeEventPosX();
	afx_msg void OnEnChangeEventPosY();
	afx_msg void OnEnChangeEventPosZ();
	afx_msg void OnEnChangeEventPivotX();
	afx_msg void OnEnChangeEventPivotY();
	afx_msg void OnEnChangeEventPivotZ();
	afx_msg void OnEnChangeEventScaleX();
	afx_msg void OnEnChangeEventScaleY();
	afx_msg void OnEnChangeEventScaleZ();
	afx_msg void OnDeltaposEventspinPosX(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposEventspinPosY(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposEventspinPosZ(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposEventspinPivotX(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposEventspinPivotY(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposEventspinPivotZ(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposEventspinScaleX(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposEventspinScaleY(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposEventspinScaleZ(NMHDR *pNMHDR, LRESULT *pResult);
	CListBox m_EventColiiderListBox;

public:
	vector<CEvent_Collider*>	m_vecEvent_Collider;
	virtual BOOL OnInitDialog();
};
