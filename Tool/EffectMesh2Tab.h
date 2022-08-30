#pragma once
#include "afxwin.h"


// CEffectMesh2Tab ��ȭ �����Դϴ�.

class CEffectMesh2Tab : public CDialog
{
	DECLARE_DYNAMIC(CEffectMesh2Tab)

public:
	CEffectMesh2Tab(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CEffectMesh2Tab();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EFFECTMESH2TAB };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

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
