#pragma once
#include "afxwin.h"
#include "ToolNavigation.h"

// CNavigationTab 대화 상자입니다.

class CNavigationTab : public CDialog
{
	DECLARE_DYNAMIC(CNavigationTab)

public:
	CNavigationTab(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CNavigationTab();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_NAVIGATIONTAB };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	typedef struct tagPickingNavMeshDesc {
		_float3					vPoint = _float3(0.f, 0.f, 0.f);
		_float					fDistance = 0.f;
	}PNMDESC;

public:
	_int						Tick(_double dTimeDelta);
	_int						LateTick(_double dTimeDelta);
	_int						Picking(_double dTimeDelta);

public:
	void						Insert_NavDesc(PNMDESC tDesc);
	void						Make_Point();
	void						Update_Point();
	void						Reset_Navigation();
	void						Render_All();

private:
	_pGraphicDevice				m_pGraphicDevice = nullptr;
	_pContextDevice				m_pContextDevice = nullptr;
	_uint						m_iCurrentPoint = 0;
	vector<CToolNavigation*>	m_vecNavis;
	
	list<PNMDESC>				m_listNM;
	NAVINFO						m_tInfo;
	UINT						m_iRadio_Index = 0;
	_bool						m_bRender_All = false;
	
public:
	afx_msg void				RadioCtrl(UINT id);
	afx_msg void				OnLbnSelchangeNavigation();
	CListBox					m_listNavigation;
	float						m_fPoint0_X;
	float						m_fPoint0_Y;
	float						m_fPoint0_Z;
	float						m_fPoint1_X;
	float						m_fPoint1_Y;
	float						m_fPoint1_Z;
	float						m_fPoint2_X;
	float						m_fPoint2_Y;
	float						m_fPoint2_Z;
	CButton						m_checkPicking;
	CButton						m_checkAutoCreate;
	afx_msg void				OnBnClickedCreate();
	afx_msg void				OnBnClickedSave();
	afx_msg void				OnBnClickedLoad();

	void						CreateNavMesh();
	afx_msg void				OnBnClickedFall();
	afx_msg void				OnBnClickedNone();
	virtual BOOL				OnInitDialog();
	int							m_iNavCount;
	int							m_iCurNavIndex;
	afx_msg void				OnBnClickedNaviDelete();
	afx_msg void OnBnClickedMagical();
};
