#pragma once
#include "afxwin.h"
#include "Client_Struct.h"

class CEffectMeshTab : public CDialog
{
	DECLARE_DYNAMIC(CEffectMeshTab)

public:
	CEffectMeshTab(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CEffectMeshTab();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EFFECTMESHTAB };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	vector<TEXNAMEPROTOPATH*>		m_vecTexProtoPath;
	vector<MODELPATHNAME*>		m_vecModelPathName;

private:
	CGameInstance*	m_pGameInstance = nullptr;
	CGameObject*	m_pCurrentMeshEffect = nullptr;
	list<CGameObject*>	m_listEffectMesh;

private:
	void Add_EffectMesh_ToList(CGameObject* pGameObj);

public:
	void Update_Shaders();

	void CopyDescription(EFFECTMESHDESC& tDestDesc, const EFFECTMESHDESC& tSourDesc);


public:
	virtual BOOL OnInitDialog();

private:
	EffectGroupDescription	m_tEffectGroupDescription;
	
private:
	EffectMeshDescription	m_tCurrentEffectDesc;
	_bool					m_bPatternPositionCheckAll = false;
	_bool					m_bPatternRotationCheckAll = false;
	_bool					m_bPatternScaleCheckAll = false;
private:
	void Init_Descriptions();

	void Update_CurrentEffectMesh_Desc();
	void Update_Tool_ByDesc();
	void Update_Shader_ListBox();


	CListBox m_EffectMesh_ListBox;

public:
	afx_msg void OnEnChangeEffectmeshPositionXEdit();
	afx_msg void OnEnChangeEffectmeshPositionYEdit();
	afx_msg void OnEnChangeEffectmeshPositionZEdit();
	afx_msg void OnDeltaposEffectmeshPositionXSpinctrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposEffectmeshPositionYSpinctrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposEffectmeshPositionZSpinctrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEffectmeshScaleXEdit();
	afx_msg void OnEnChangeEffectmeshScaleYEdit();
	afx_msg void OnEnChangeEffectmeshScaleZEdit();
	afx_msg void OnDeltaposEffectmeshScaleXSpinctrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposEffectmeshScaleYSpinctrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposEffectmeshScaleZSpinctrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEffectmeshRotationXEdit();
	afx_msg void OnEnChangeEffectmeshRotationYEdit();
	afx_msg void OnEnChangeEffectmeshRotationZEdit();
	afx_msg void OnDeltaposEffectmeshRotationXSpinctrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposEffectmeshRotationYSpinctrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposEffectmeshRotationZSpinctrl(NMHDR *pNMHDR, LRESULT *pResult);

public:
	//afx_msg void OnEnChangeEffectmeshGroupNameEdit();
	//afx_msg void OnEnChangeEffectmeshCurrentEffetNameEdit();
	afx_msg void OnBnClickedEffectmeshApplyGroupNameButton();
	afx_msg void OnBnClickedEffectmeshApplyCurrentEffectNameButton();

	afx_msg void OnBnClickedEffectmeshCreateMeshButton();
	afx_msg void OnBnClickedEffectmeshCreateImagelessMeshButton();
	afx_msg void OnBnClickedEffectmeshCreateRectButton();

	// Texture
	afx_msg void OnBnClickedEffectmeshTextureDefaultCheck();
	afx_msg void OnBnClickedEffectmeshTextureDefaultBrowseButton();
	afx_msg void OnEnChangeEffectmeshTextureDefaultNumberEdit();
	afx_msg void OnEnChangeEffectmeshTextureDefaultIntervalEdit();
	afx_msg void OnBnClickedEffectmeshTextureDefaultSpriteRepeatCheck();
	afx_msg void OnBnClickedEffectmeshTextureAlphaoneCheck();
	afx_msg void OnBnClickedEffectmeshTextureAlphaoneBrowseButton();
	afx_msg void OnEnChangeEffectmeshTextureAlphaoneNumberEdit();
	afx_msg void OnEnChangeEffectmeshTextureAlphaoneIntervalEdit();
	afx_msg void OnBnClickedEffectmeshTextureAlphaoneSpriteRepeatCheck();
	afx_msg void OnBnClickedEffectmeshTextureMaskCheck();
	afx_msg void OnBnClickedEffectmeshTextureMaskBrowseButton();
	afx_msg void OnEnChangeEffectmeshTextureMaskNumberEdit();
	afx_msg void OnEnChangeEffectmeshTextureMaskIntervalEdit();
	afx_msg void OnBnClickedEffectmeshTextureMaskSpriteRepeatCheck();

public:
	afx_msg void OnLbnSelchangeEffectmeshEffectmeshList();
	afx_msg void OnBnClickedEffectmeshDuplicateButton();
	afx_msg void OnBnClickedEffectmeshDeleteButton();

public:
	// Create
	afx_msg void OnEnChangeEffectmeshCreateTimeEdit();
	afx_msg void OnEnChangeEffectmeshDeathTimeEdit();
	afx_msg void OnBnClickedEffectmeshCreateDeathRepeat();

	// Play & Stop
	afx_msg void OnBnClickedEffectmeshPlayButton();
	afx_msg void OnBnClickedEffectmeshStopButton();
	void Play();
	void Stop();


public:
	// Pattern
	// Position
	afx_msg void OnEnChangeEffectmeshPatternPositionStartTimeEdit();
	afx_msg void OnEnChangeEffectmeshPatternPositionEndTimeEdit();
	afx_msg void OnBnClickedEffectmeshPatternPositionRepeatCheck();
	afx_msg void OnEnChangeEffectmeshPatternPositionXEdit();
	afx_msg void OnEnChangeEffectmeshPatternPositionYEdit();
	afx_msg void OnEnChangeEffectmeshPatternPositionZEdit();
	afx_msg void OnDeltaposEffectmeshPatternPositionXSpinCtrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposEffectmeshPatternPositionYSpinCtrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposEffectmeshPatternPositionZSpinCtrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedEffectmeshPatternPositionSelectAllCheck();

	afx_msg void OnBnClickedEffectmeshPatternPositionProjectileCheck();
	afx_msg void OnEnChangeEffectmeshPatternPositionRandomLengthMinEdit();
	afx_msg void OnEnChangeEffectmeshPatternPositionRandomLengthMaxEdit();
	afx_msg void OnEnChangeEffectmeshPatternPositionRandomAngleMinEdit();
	afx_msg void OnEnChangeEffectmeshPatternPositionRandomAngleMaxEdit();
	afx_msg void OnBnClickedMesheffectPatternPositionProjectileRandomLengthAngleCaculateButton();

	// Rotation

	afx_msg void OnEnChangeEffectmeshPatternRotationStartTimeEdit();
	afx_msg void OnEnChangeEffectmeshPatternRotationEndTimeEdit();
	afx_msg void OnBnClickedEffectmeshPatternRotationRepeatCheck();
	afx_msg void OnEnChangeEffectmeshPatternRotationXEdit();
	afx_msg void OnEnChangeEffectmeshPatternRotationYEdit();
	afx_msg void OnEnChangeEffectmeshPatternRotationZEdit();
	afx_msg void OnDeltaposEffectmeshPatternRotationXSpinCtrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposEffectmeshPatternRotationYSpinCtrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposEffectmeshPatternRotationZSpinCtrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedEffectmeshPatternRotationSelectAllCheck();

	// Scale
	afx_msg void OnEnChangeEffectmeshPatternScaleStartTimeEdit();
	afx_msg void OnEnChangeEffectmeshPatternScaleEndTimeEdit();
	afx_msg void OnBnClickedEffectmeshPatternScaleRepeatCheck();
	afx_msg void OnEnChangeEffectmeshPatternScaleXEdit();
	afx_msg void OnEnChangeEffectmeshPatternScaleYEdit();
	afx_msg void OnEnChangeEffectmeshPatternScaleZEdit();
	afx_msg void OnDeltaposEffectmeshPatternScaleXSpinCtrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposEffectmeshPatternScaleYSpinCtrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposEffectmeshPatternScaleZSpinCtrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedEffectmeshPatternScaleSelectAllCheck();

public:
	// Save & Load
	afx_msg void OnBnClickedEffectmeshSaveButton();
	afx_msg void OnBnClickedEffectmeshLoadButton();
	void Save();
	void Load();
	void Load_WithParticle();
private:
	void Init();



	afx_msg void OnBnClickedEffectmeshUpdateShaderButton();


	// Fade In & Out
	afx_msg void OnEnChangeEffectmeshFadeInStartTimeEdit();
	afx_msg void OnEnChangeEffectmeshFadeInEndTimeEdit();
	afx_msg void OnEnChangeEffectmeshFadeInDegreeEdit();
	afx_msg void OnBnClickedEffectmeshFadeInCheck();
	afx_msg void OnEnChangeEffectmeshFadeOutStartTimeEdit();
	afx_msg void OnEnChangeEffectmeshFadeOutEndTimeEdit();
	afx_msg void OnEnChangeEffectmeshFadeOutDegreeEdit();
	afx_msg void OnBnClickedEffectmeshFadeOutCheck();
public:
	afx_msg void OnBnClickedEffectmeshLoadWithParticleButton();


public:
	// UV Animation
	afx_msg void OnBnClickedEffectmeshUvAnimationCheck();
	afx_msg void OnEnChangeEffectmeshUvAnimationStartTimeEdit();
	afx_msg void OnEnChangeEffectmeshUvAnimationEndTimeEdit();
	afx_msg void OnEnChangeEffectmeshUvAnimationXEdit();
	afx_msg void OnEnChangeEffectmeshUvAnimationYEdit();
};
