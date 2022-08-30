#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CMeshTab 대화 상자입니다.

class CMeshTab : public CDialog
{
	DECLARE_DYNAMIC(CMeshTab)

public:
	map<CString, CString> m_map;

public:
	CMeshTab(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CMeshTab();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MESHTAB };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	HRESULT				Init_List(CString strFolder, HTREEITEM hRoot, HTREEITEM hMainRoot);

	HTREEITEM			m_hFirstRoot;

	HTREEITEM			m_hEffectRoot;


	CString				m_strFilePath = L"";
	CString				m_strFileName = L"";
	CString				m_strComponentPrototype = L"";

	CImageList			m_imagelist;


public:
	_int				Tick(_double dTimeDelta);
	_int				LateTick(_double dTimeDelta);
	HRESULT				Render();


public:
	
	virtual BOOL		OnInitDialog();
	CTreeCtrl			m_TreeCtrl;
	afx_msg void		OnTvnSelchangedMeshlist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void		OnBnClickedCreatemesh();

	typedef vector<CString>			PROTOTYPES;
	afx_msg void		OnLbnSelchangePrototype();			// Sel Change Prototype List
	CString				m_strCurModelTag;

	PROTOTYPES			m_vecModels;

	HRESULT				Sort_Objects();
	class CToolModel*	Find_Object(MO_INFO tObjInfo);

private:
	_pGraphicDevice		m_pGraphicDevice = nullptr;
	_pContextDevice		m_pContextDevice = nullptr;
	
	class CToolModel*	m_pCurrentModel = nullptr;
	class CToolObject*	m_pCurrentObject = nullptr;
	CJussimpModel::TYPE	m_eAnim;
public:
	afx_msg void		OnBnClickedAnimation();
	CButton				m_CheckAnim;
	
	CListBox			m_listPrototype;
	afx_msg void		OnBnClickedInserttocontainer();
	afx_msg void		OnBnClickedDeleteprototype();
	afx_msg void		OnBnClickedLoadprototype();

	CString				m_strObjectName;
	afx_msg void		OnEnChangeObjectname();
	afx_msg void		OnBnClickedForanimationtool();
	float				m_fRotationDegree;
	UINT				m_iRadioMode = 0;
	afx_msg void		RadioCtrl(UINT ID);
	_bool				m_bIdentity = false;
	_bool				m_bRotateX = false;
	_bool				m_bRotateY = false;
	_bool				m_bRotateZ = false;
	afx_msg void		OnEnChangeRotationdegree();
	afx_msg void		OnBnClickedIdentity();
	afx_msg void		OnBnClickedPivotx();
	afx_msg void		OnBnClickedPivoty();
	afx_msg void		OnBnClickedPivotz();


	afx_msg void		OnBnClickedLoadallmodel();
	void				Load_AllModel(CString strPath);
};
