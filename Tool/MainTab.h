#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CMainTab 대화 상자입니다.

class CMainTab : public CDialog
{
	DECLARE_DYNAMIC(CMainTab)

public:
	CMainTab(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CMainTab();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAINTAB };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	enum class TAB_NUM : _int
	{
		CAMERA = 0,
		MESH = 1,
		TERRAIN = 2,
		ANIMATION = 3,
		NAVIGATION = 4,
		LIGHTTAB = 5,
		EFFECT = 6,
		EFFECTMESH = 7,
		EFFECTMESH2 = 8,
		EVENTCOLLIDER = 9
	};

public:
	afx_msg void					SELCHANGE_MainTab(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL					OnInitDialog();
	_bool							Set_SceneName(CString strSceneName);
	void							Add_World(CString strSceneName);
	

	// 메인 탭
	CTabCtrl						m_tabCtrl;
	class CCameraTab*				m_pCameraTab = nullptr;
	class CMeshTab*					m_pMeshTab = nullptr;
	class CTerrainTab*				m_pTerrainTab = nullptr;
	class CAnimationTab*			m_pAnimationTab = nullptr;
	class CNavigationTab*			m_pNavigationTab = nullptr;
	class CEffectTab*				m_pEffectTab = nullptr;
	class CEffectMeshTab*			m_pEffectMeshTab = nullptr;
	class CEffectMesh2Tab*			m_pEffectMesh2Tab = nullptr;
	class CLightTab*				m_pLightTab = nullptr;
	class CToolCamera*				m_pToolCam = nullptr;
	class CEventColliderTab*		m_pEventColliderTab = nullptr;
	

	// 씬
	CListBox						m_listScene;
	CString							m_strSceneName;

	// 트리 컨트롤
	CTreeCtrl						m_treeWorld;
	CImageList						m_imagelist;
	CString							m_strCurrentWorld;
	CString							m_strCurrentContainer;
	int								m_iCurrentIndex = 0;


	// 오브젝트 맵
	typedef map<CString, vector<class CToolModel*>>	CONTAINERS;
	typedef map<CString, CONTAINERS>				SCENES;

	typedef map<CString, _uint>						NUMCOUNTERS;

	NUMCOUNTERS						m_mapCounter;
	SCENES							m_MeshObjectMap;

public:
	// 탭 제어 함수
	_int							Tick(_double dTimeDelta);
	_int							LateTick(_double dTimeDelta);
	_int							CurrentWorldTick(_double dTimeDelta);
	_int							CurrentWorldLateTick(_double dTimeDelta);

	HRESULT							Render();

	_int							Picking(_double dTimeDelta);

	// 일반적인 Object Insert시 사용될 함수
	HRESULT							Insert_MeshObjectToMap(class CToolModel* pToolModel, CString strObjectName);
	_bool							Insert_ObjectIndex(class CToolModel* pToolModel, CString strObjectName);

	// Load 시 사용될 함수
	HRESULT							Load_Container(HTREEITEM hParent, CString strContainerName, HTREEITEM& hContainer);
	HRESULT							Load_Object(class CToolModel* pToolModel, HTREEITEM hParent, CString strItemName);
	
	// Find 함수
	HTREEITEM						Find_CurrentContainer();
	HTREEITEM						Find_ItemUsingString(CString str, HTREEITEM hContainer);
	HTREEITEM						Find_ItemUsingData(DWORD_PTR ptr, HTREEITEM hContainer);

	// Check Uncheck 함수
	void							Check_ChildItem(HTREEITEM hParent);
	void							UnCheck_ChildItem(HTREEITEM hParent);

	// DWORD_PTR = Current Item's Data
	DWORD_PTR						m_dptrCurrentItem = 0;

	afx_msg void					OnTcnSelchangePrototypelist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void					OnLbnSelchangeScene();				// World list Sel Change
	afx_msg void					OnBnClickedEditscene();				// Button Click Edit Scene
	afx_msg void					OnBnClickedSceneSave();				// Button Click Scene Save
	afx_msg void					OnBnClickedSceneLoad();				// Button Click Scene Load
	afx_msg void					OnBnClickedSceneDelete();			// Button Click Scene Delete

	CScrollBar						m_barVerticalMain;

	afx_msg void					OnBnClickedContainer();				// Button Click Container Creaate
	afx_msg void					OnBnClickedDeletecontainer();		// Button Click Scene Delete( include Container, Item )
	afx_msg void					OnTvnSelchangedWorldcontainer(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void					OnNMClickWorldcontainer(NMHDR *pNMHDR, LRESULT *pResult);


	// Position ///////////////////////////////////////////////////////////////////////////////
	CSpinButtonCtrl					m_spinCtrlPosX;
	CSpinButtonCtrl					m_spinCtrlPosY;
	CSpinButtonCtrl					m_spinCtrlPosZ;

	float							m_fPosX;
	float							m_fPosY;
	float							m_fPosZ;

	afx_msg void					OnDeltaposPositionX(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void					OnDeltaposPositionY(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void					OnDeltaposPositionZ(NMHDR *pNMHDR, LRESULT *pResult);

	void							Move_Target();
	void							Add_MoveTargetX();
	void							Add_MoveTargetY();
	void							Add_MoveTargetZ();
	///////////////////////////////////////////////////////////////////////////////////////////

	// Scale //////////////////////////////////////////////////////////////////////////////////
	CSpinButtonCtrl					m_spinCtrlScaleX;
	CSpinButtonCtrl					m_spinCtrlScaleY;
	CSpinButtonCtrl					m_spinCtrlScaleZ;

	float							m_fScaleX;
	float							m_fScaleY;
	float							m_fScaleZ;
	
	afx_msg void					OnDeltaposScaleX(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void					OnDeltaposScaleY(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void					OnDeltaposScaleZ(NMHDR *pNMHDR, LRESULT *pResult);

	void							Scaling_Target();
	///////////////////////////////////////////////////////////////////////////////////////////

	// Rotation ///////////////////////////////////////////////////////////////////////////////
	CSpinButtonCtrl					m_spinCtrlRotX;
	CSpinButtonCtrl					m_spinCtrlRotY;
	CSpinButtonCtrl					m_spinCtrlRotZ;

	float							m_fRotX;
	float							m_fRotY;
	float							m_fRotZ;

	afx_msg void					OnDeltaposRotX(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void					OnDeltaposRotY(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void					OnDeltaposRotZ(NMHDR *pNMHDR, LRESULT *pResult);

	void							Rotation_Target();
	///////////////////////////////////////////////////////////////////////////////////////////

	// Container Name /////////////////////////////////////////////////////////////////////////
	CString							m_strContainerName;
	afx_msg void					OnEnChangeMainContainername();
	afx_msg void					OnBnClickedMainChangename();
	///////////////////////////////////////////////////////////////////////////////////////////
};
