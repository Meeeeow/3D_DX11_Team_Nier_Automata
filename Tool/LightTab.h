#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CLightTab 대화 상자입니다.

class CLightTab : public CDialog
{
	DECLARE_DYNAMIC(CLightTab)

public:
	CLightTab(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CLightTab();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LIGHTTAB };
#endif

public:
	LIGHTDESC				m_tLightDesc;
	LIGHTDEPTHDESC			m_tDepthDesc;

	LIGHTDATA				m_tLightData;

public:
	void					Init_Light();

public:
	HTREEITEM				Get_CurrentWorld();
	HTREEITEM				Get_CurrentContainer();
	HTREEITEM				Get_CurrentItem();

private:
	_pGraphicDevice			m_pGraphicDevice = nullptr;
	_pContextDevice			m_pContextDevice = nullptr;
	map<CString, _uint>		m_mapIndex;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void			OnTvnSelchangedTreectrlLight(NMHDR *pNMHDR, LRESULT *pResult);
	HRESULT					Change_Info(LIGHTDESC* pLightDesc);
	CTreeCtrl				m_treeCtrlLight;
	float					m_fLightRed;
	float					m_fLightBlue;
	float					m_fLightGreen;
	float					m_fLightAlpha;
	afx_msg void			OnBnClickedLightcreate();

	CString					m_strLightName = L"DefaultLight";
	afx_msg void			OnEnChangeLightname();

	float					m_fPoint_X;
	float					m_fPoint_Y;
	float					m_fPoint_Z;
	float					m_fPoint_Radius;
	float					m_fDir_X;
	float					m_fDir_Y;
	float					m_fDir_Z;

	afx_msg void			OnEnChangeLightPositionx();
	afx_msg void			OnEnChangeLightPositiony();
	afx_msg void			OnEnChangeLightPositionz();
	afx_msg void			OnEnChangeLightRadius();
	afx_msg void			OnEnChangeLightDirectionx();
	afx_msg void			OnEnChangeLightDirectiony();
	afx_msg void			OnEnChangeLightDirectionz();
	afx_msg void			OnEnChangeRed();
	afx_msg void			OnEnChangeBlue();
	afx_msg void			OnEnChangeGreen();
	afx_msg void			OnEnChangeAlpha();
	afx_msg void			OnBnClickedLightdelete();
	virtual BOOL			OnInitDialog();
	HTREEITEM				m_hRootItem;
	CImageList				m_imagelist;
	UINT					m_iRadio_Index = 0;
	afx_msg void			RadioCtrl(UINT ID);
	afx_msg void			OnBnClickedDirection();
	afx_msg void			OnBnClickedPoint();
	afx_msg void			OnBnClickedSpotlight();
	CButton					m_checkLightOn;
	afx_msg void			OnBnClickedLighton();

	float					m_fAmbient_Red;
	float					m_fAmbient_Blue;
	float					m_fAmbient_Green;
	float					m_fAmbient_Alpha;
	float					m_fSpecular_Red;
	float					m_fSpecular_Blue;
	float					m_fSpecular_Green;
	float					m_fSpecular_Alpha;
	afx_msg void			OnEnChangeAmbientRed();
	afx_msg void			OnEnChangeAmbientBlue();
	afx_msg void			OnEnChangeAmbientGreen();
	afx_msg void			OnEnChangeAmbientAlpha();
	afx_msg void			OnEnChangeSpecularRed();
	afx_msg void			OnEnChangeSpecularBlue();
	afx_msg void			OnEnChangeSpecularGreen();
	afx_msg void			OnEnChangeSpecularAlpha();
	afx_msg void			OnBnClickedSave();
	afx_msg void			OnBnClickedLoad();
	void					LoadObject(CString tag, LIGHTDESC tLightDesc);
};
