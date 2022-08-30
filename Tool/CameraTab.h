#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "ToolCamera.h"
#include "CamObject.h"


// CCameraTab 대화 상자입니다.

class CCameraTab : public CDialog
{
	DECLARE_DYNAMIC(CCameraTab)

public:
	CCameraTab(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CCameraTab();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CAMERATAB };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

public:
	_int						Tick(_double dTimeDelta);
	_int						LateTick(_double dTimeDelta);
	_int						Picking(_double dTimeDelta);

private:
	_pGraphicDevice				m_pGraphicDevice = nullptr;
	_pContextDevice				m_pContextDevice = nullptr;

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_CamMoveListBox;
	CListBox m_MovingPointsListBox;
	float m_fPosX;
	float m_fPosY;
	float m_fPosZ;
	// For Camera Move List Name
	// For Moving Points Name
	CString m_strCameraName;
	CString m_strPointsName;
	float m_fSpeed;
	CSpinButtonCtrl m_CtrlPositionX;
	CSpinButtonCtrl m_CtrlPositionY;
	CSpinButtonCtrl m_CtrlPositionZ;
	CSpinButtonCtrl m_CtrlSpeed;
	afx_msg void OnBnClickedPlay();
	afx_msg void OnBnClickedStop();
	afx_msg void OnBnClickedReset();
	afx_msg void OnDeltaposSpinX(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinY(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinZ(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinSpeed(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedLoad();
	afx_msg void OnBnClickedButtonMoveListCreate();
	afx_msg void OnBnClickedButtonMoveListDelete();
	afx_msg void OnBnClickedButtonMovingPointsCreate();
	afx_msg void OnBnClickedButtonMovingPointsDelete();
	CButton m_Radio_MovingPoints;
	afx_msg void OnBnClickedMovingPoints();
	afx_msg void OnBnClickedButtonAtDelete();
	CString m_strAtName;

public:
	void Update_Data_PointsBox();
	void Update_Data_AtBox();
	void Update_Data_Speed();
	void Set_OnToolCamTrue() { m_bOnCamTab = true; }
	void Set_OnToolCamFalse() { m_bOnCamTab = false; }
	void Update_Data();
private:
	CToolCamera*	m_pToolCam = nullptr;
	_uint			m_iNumPoints = 0;

public:
	_bool			m_bOnCamTab = false;
	class CCamObject*	Get_CurCamObj() { return m_pCurCamObject; }
public:
	afx_msg void OnEnChangePosX();
	afx_msg void OnEnChangePosY();
	afx_msg void OnEnChangePosZ();
	afx_msg void OnEnChangeSpeed();
	float m_fAtX;
	float m_fAtY;
	float m_fAtZ;
	CSpinButtonCtrl m_CtrlAtPosX;
	CSpinButtonCtrl m_CtrlAtPosY;
	CSpinButtonCtrl m_CtrlAtPosZ;
	afx_msg void OnDeltaposSpinAtX(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinAtY(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinAtZ(NMHDR *pNMHDR, LRESULT *pResult);

	map<CString, class CCamObject*>	m_mapCamera;
	class CCamObject*							m_pCurCamObject = nullptr;
	afx_msg void OnBnClickedButtonAtCreate();
	afx_msg void OnLbnSelchangeCameraMoveList();
	afx_msg void OnLbnSelchangeMovingPoints();
	afx_msg void OnLbnSelchangeAtListBox();
	CListBox m_AtListBox;
	afx_msg void OnEnChangeAtX();
	afx_msg void OnEnChangeAtY();
	afx_msg void OnEnChangeAtZ();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CButton m_bCamera_Check;
};
