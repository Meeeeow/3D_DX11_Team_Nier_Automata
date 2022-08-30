#pragma once
#include "afxwin.h"


// CEffectMesh2Tab 대화 상자입니다.

class CEffectMesh2Tab : public CDialog
{
	DECLARE_DYNAMIC(CEffectMesh2Tab)

public:
	CEffectMesh2Tab(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CEffectMesh2Tab();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EFFECTMESH2TAB };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

public:
	CListBox m_ShaderList;
	CListBox m_AddedShaderList;

	CString	m_strShaderListSelected;
	CString	m_strAddedShaderListSelected;

	list<SHADERNAME*> m_listShaderName;

public:
	afx_msg void OnLbnSelchangeEffectmesh2ShaderList();
	afx_msg void OnBnClickedMesheffect2AddShaderButton();
	afx_msg void OnBnClickedEffectmesh2DeleteShaderButton();
	afx_msg void OnLbnSelchangeEffectmesh2AddedShaderList();
};
