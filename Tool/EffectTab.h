#pragma once
#include "afxwin.h"
#include "ToolDefines.h"
#include "Client_Struct.h"
// CEffectTab 대화 상자입니다.

class CEffectTab : public CDialog
{
	DECLARE_DYNAMIC(CEffectTab)

public:
	CEffectTab(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CEffectTab();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EFFECTTAB };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

private:
	//void Load_FilePath();
	//void Load_Meshes(CString RelativePath, CString AbsolutePath = L"");

private:
	CGameInstance* m_pGameInstance = nullptr;

private:
	PARTICLEDESC			m_tParticleDesc;
	PARTICLE_GROUP_DESC		m_tGroupParticleDesc;

private:
	//vector<TEXNAMEPROTOPATH*> m_vecTexProtoPath;
	list<CGameObject*> m_listParticle;
	void Add_Particle(CGameObject* pGameObj);
	void Add_Particle_ToList(CGameObject* pGameObj);
	void Update_ParticleListBox_WithObjList();
public:
	void Update_Tool_ByDesc();
	void Update_CurrentParticle();





public:
	//const vector<TEXNAMEPROTOPATH*>& Get_vecTexProtoPath() {
	//	return m_vecTexProtoPath;
	//}
	PARTICLE_GROUP_DESC& Get_GroupDesc() {
		return m_tGroupParticleDesc;
	}
	void Create_From_GroupList_FromLoad();


	void Init();

private:
	CGameObject* m_pCurrentParticle = nullptr;

private:
	CString			m_strCurrentTexture;



public:
	afx_msg void OnEnChangeEffectPositionXEdit();
	afx_msg void OnEnChangeEffectPositionYEdit();
	afx_msg void OnEnChangeEffectPositionZEdit();
	afx_msg void OnEnChangeEffectScaleXEdit();
	afx_msg void OnEnChangeEffectScaleYEdit();
	afx_msg void OnEnChangeEffectScaleZEdit();
	afx_msg void OnEnChangeEffectRotationXEdit();
	afx_msg void OnEnChangeEffectRotationYEdit();
	afx_msg void OnEnChangeEffectRotationZEdit();
	afx_msg void OnEnChangeEffectDirectionXEdit();
	afx_msg void OnEnChangeEffectDirectionYEdit();
	afx_msg void OnEnChangeEffectDirectionZEdit();
	afx_msg void OnEnChangeEffectLifetimeEdit();
	afx_msg void OnEnChangeEffectLifetimeDensityEdit();
	afx_msg void OnEnChangeEffectNumberEdit();
	afx_msg void OnEnChangeEffectDensityEdit();
	afx_msg void OnEnChangeEffectDirectionDensityEdit();
	afx_msg void OnEnChangeEffectSpeedEdit();
	afx_msg void OnEnChangeEffectSpeedDensityEdit();
	afx_msg void OnEnChangeEffectWidthEdit();
	afx_msg void OnEnChangeEffectHeightEdit();
	afx_msg void OnEnChangeEffectSpriteIntervalEdit();
	afx_msg void OnBnClicked_LifeTimeDensity(UINT _uiID);
	afx_msg void OnBnClickedEffectCreateButton();
	void Create_Particle(PARTICLEDESC& _tDesc);
	void Create_Particle_FromLoad(PARTICLEDESC& _tDesc);


public:
	afx_msg void OnDeltaposEffectPositionXSpinCtrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposEffectPositionYSpinCtrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposEffectPositionZSpinCtrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposEffectScaleXSpinCtrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposEffectScaleYSpinCtrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposEffectScaleZSpinCtrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposEffectRotationXSpinCtrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposEffectRotationYSpinCtrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposEffectRotationZSpinCtrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposEffectDirectionXSpinCtrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposEffectDirectionYSpinCtrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposEffectDirectionZSpinCtrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposEffectLifetimeSpinCtrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposEffectNumberSpinCtrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposEffectDensitySpinCtrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposEffectDirectionDensitySpinCtrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposEffectSpeedSpinCtrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposEffectSpeedDensitySpinCtrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposEffectWidthSpinCtrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposEffectHeightSpinCtrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposEffectSpriteIntervalSpinCtrl(NMHDR *pNMHDR, LRESULT *pResult);


public:
	afx_msg void OnBnClickedEffectOpenTexture();
	virtual BOOL OnInitDialog();
	CListBox m_ListBox_Texture;
	CListBox m_ListBox_Particle;
	afx_msg void OnLbnSelchangeEffectTexturelist();
	afx_msg void OnEnChangeEffectTextureNumberEdit();
	afx_msg void OnBnClickedEffectDeleteButton();
	afx_msg void OnEnChangeEffectParticleNameEdit();
	afx_msg void OnBnClickedEffectApplyName();
	afx_msg void OnBnClickedEffectApplyTextureButton();
	afx_msg void OnBnClickedEffectRepeat();
	afx_msg void OnEnChangeEffectMultiTextureIntervalEdit();

public:
	afx_msg void OnBnClickedEffectPlayButton();
	afx_msg void OnBnClickedEffectStopButton();
	void Play();
	void Stop();
public:
	afx_msg void OnBnClickedEffectFadeInCheck();
	afx_msg void OnEnChangeEffectFadeInEdit();
	afx_msg void OnBnClickedEffectFadeOutCheck();
	afx_msg void OnEnChangeEffectFadeOutEdit();
	afx_msg void OnEnChangeEffectFadeOutEndEdit();

public:
	afx_msg void OnBnClickedEffectMultiTextureSpriteRepeat();
	afx_msg void OnLbnSelchangeEffectParticleList();
	afx_msg void OnBnClickedEffectSaveButton();
	afx_msg void OnBnClickedEffectLoadButton();
	afx_msg void OnEnChangeEffectLifetimeDensityEndEdit();
	afx_msg void OnEnChangeEffectPsizeEdit();
	afx_msg void OnBnClickedEffectUpdateShaderButton();
};
