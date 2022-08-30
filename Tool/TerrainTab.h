#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CTerrainTab 대화 상자입니다.

class CTerrainTab : public CDialog
{
	DECLARE_DYNAMIC(CTerrainTab)

public:
	CTerrainTab(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CTerrainTab();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TERRAINTAB };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

public:
	HRESULT Create_Terrain();

public:
	class CTerrain*		Get_Terrain() {
		return m_pTerrain;
	}

private:
	class CTerrain*			m_pTerrain = nullptr;
public:
	CButton m_WireFrameCheckBox;
	afx_msg void OnBnClickedWireFrameCheckBox();
	afx_msg void OnBnClickedTerrainResetButton();
	int m_iVerticesX;
	int m_iVerticesZ;
	CButton m_Radio_UpButton;
	CButton m_Radio_DownButton;
	afx_msg void OnBnClickedTerrain_UpButton();
	CSliderCtrl m_SliderRange;
	afx_msg void OnNMReleasedcaptureTerrainsliderRange(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedTerrainradioCircle();
	CButton m_RadioCircle;
	afx_msg void OnBnClickedTerrainradioRectangle();
	afx_msg void OnBnClickedTerrainRadioDown();
	CListBox m_SplattingListBox;
};
