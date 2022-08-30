// EffectMeshTab.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Engine_Struct.h"
#include "Tool.h"
#include "EffectMeshTab.h"
#include "afxdialogex.h"
#include "ToolEffectMeshObject.h"
#include "MainFrm.h"
#include "ToolView.h"
#include "ToolDefines.h"
#include <math.h>
#include "RandomManager.h"
#include "MainTab.h"
#include "EffectMesh2Tab.h"
#include "EffectTab.h"

// CEffectMeshTab 대화 상자입니다.

IMPLEMENT_DYNAMIC(CEffectMeshTab, CDialog)

CEffectMeshTab::CEffectMeshTab(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_EFFECTMESHTAB, pParent)
	, m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);
}

CEffectMeshTab::~CEffectMeshTab()
{
	Safe_Release(m_pGameInstance);

	for (auto& pIter : m_vecTexProtoPath)
		Safe_Delete(pIter);
	m_vecTexProtoPath.clear();
	           
	for (auto& pIter : m_vecModelPathName)
		Safe_Delete(pIter);
	m_vecModelPathName.clear();

	for (auto& pIter : m_listEffectMesh)
		Safe_Release(pIter);
	m_listEffectMesh.clear();

	if (m_tCurrentEffectDesc.tEffectMeshShader.vecShaderName.size() > 0)
	{
		for (_int i = 0; i < m_tCurrentEffectDesc.tEffectMeshShader.vecShaderName.size(); ++i)
			Safe_Delete(m_tCurrentEffectDesc.tEffectMeshShader.vecShaderName[i]);
		m_tCurrentEffectDesc.tEffectMeshShader.vecShaderName.clear();
	}

	if (m_tEffectGroupDescription.listEffectMeshDesc.size() > 0)
	{
		for (auto& pIter : m_tEffectGroupDescription.listEffectMeshDesc)
		{
			for (auto& pDescIter : pIter->tEffectMeshShader.vecShaderName)
				Safe_Delete(pDescIter);
			pIter->tEffectMeshShader.vecShaderName.clear();
			Safe_Delete(pIter);
		}
		m_tEffectGroupDescription.listEffectMeshDesc.clear();
	}




	Safe_Release(m_pCurrentMeshEffect);
}

void CEffectMeshTab::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EFFECTMESH_EFFECTMESH_LIST, m_EffectMesh_ListBox);
}


BEGIN_MESSAGE_MAP(CEffectMeshTab, CDialog)
	ON_EN_CHANGE(IDC_EFFECTMESH_POSITION_X_EDIT, &CEffectMeshTab::OnEnChangeEffectmeshPositionXEdit)
	ON_EN_CHANGE(IDC_EFFECTMESH_POSITION_Y_EDIT, &CEffectMeshTab::OnEnChangeEffectmeshPositionYEdit)
	ON_EN_CHANGE(IDC_EFFECTMESH_POSITION_Z_EDIT, &CEffectMeshTab::OnEnChangeEffectmeshPositionZEdit)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EFFECTMESH_POSITION_X_SPINCTRL, &CEffectMeshTab::OnDeltaposEffectmeshPositionXSpinctrl)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EFFECTMESH_POSITION_Y_SPINCTRL, &CEffectMeshTab::OnDeltaposEffectmeshPositionYSpinctrl)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EFFECTMESH_POSITION_Z_SPINCTRL, &CEffectMeshTab::OnDeltaposEffectmeshPositionZSpinctrl)
	ON_EN_CHANGE(IDC_EFFECTMESH_SCALE_X_EDIT, &CEffectMeshTab::OnEnChangeEffectmeshScaleXEdit)
	ON_EN_CHANGE(IDC_EFFECTMESH_SCALE_Y_EDIT, &CEffectMeshTab::OnEnChangeEffectmeshScaleYEdit)
	ON_EN_CHANGE(IDC_EFFECTMESH_SCALE_Z_EDIT, &CEffectMeshTab::OnEnChangeEffectmeshScaleZEdit)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EFFECTMESH_SCALE_X_SPINCTRL, &CEffectMeshTab::OnDeltaposEffectmeshScaleXSpinctrl)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EFFECTMESH_SCALE_Y_SPINCTRL, &CEffectMeshTab::OnDeltaposEffectmeshScaleYSpinctrl)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EFFECTMESH_SCALE_Z_SPINCTRL, &CEffectMeshTab::OnDeltaposEffectmeshScaleZSpinctrl)
	ON_EN_CHANGE(IDC_EFFECTMESH_ROTATION_X_EDIT, &CEffectMeshTab::OnEnChangeEffectmeshRotationXEdit)
	ON_EN_CHANGE(IDC_EFFECTMESH_ROTATION_Y_EDIT, &CEffectMeshTab::OnEnChangeEffectmeshRotationYEdit)
	ON_EN_CHANGE(IDC_EFFECTMESH_ROTATION_Z_EDIT, &CEffectMeshTab::OnEnChangeEffectmeshRotationZEdit)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EFFECTMESH_ROTATION_X_SPINCTRL, &CEffectMeshTab::OnDeltaposEffectmeshRotationXSpinctrl)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EFFECTMESH_ROTATION_Y_SPINCTRL, &CEffectMeshTab::OnDeltaposEffectmeshRotationYSpinctrl)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EFFECTMESH_ROTATION_Z_SPINCTRL, &CEffectMeshTab::OnDeltaposEffectmeshRotationZSpinctrl)
	//ON_EN_CHANGE(IDC_EFFECTMESH_GROUP_NAME_EDIT, &CEffectMeshTab::OnEnChangeEffectmeshGroupNameEdit)
	//ON_EN_CHANGE(IDC_EFFECTMESH_CURRENT_EFFET_NAME_EDIT, &CEffectMeshTab::OnEnChangeEffectmeshCurrentEffetNameEdit)
	ON_BN_CLICKED(IDC_EFFECTMESH_CREATE_MESH_BUTTON, &CEffectMeshTab::OnBnClickedEffectmeshCreateMeshButton)
	ON_BN_CLICKED(IDC_EFFECTMESH_CREATE_IMAGELESS_MESH_BUTTON, &CEffectMeshTab::OnBnClickedEffectmeshCreateImagelessMeshButton)
	ON_BN_CLICKED(IDC_EFFECTMESH_CREATE_RECT_BUTTON, &CEffectMeshTab::OnBnClickedEffectmeshCreateRectButton)
	ON_BN_CLICKED(IDC_EFFECTMESH_TEXTURE_DEFAULT_CHECK, &CEffectMeshTab::OnBnClickedEffectmeshTextureDefaultCheck)
	ON_BN_CLICKED(IDC_EFFECTMESH_TEXTURE_DEFAULT_BROWSE_BUTTON, &CEffectMeshTab::OnBnClickedEffectmeshTextureDefaultBrowseButton)
	ON_BN_CLICKED(IDC_EFFECTMESH_TEXTURE_ALPHAONE_CHECK, &CEffectMeshTab::OnBnClickedEffectmeshTextureAlphaoneCheck)
	ON_BN_CLICKED(IDC_EFFECTMESH_TEXTURE_ALPHAONE_BROWSE_BUTTON, &CEffectMeshTab::OnBnClickedEffectmeshTextureAlphaoneBrowseButton)
	ON_BN_CLICKED(IDC_EFFECTMESH_TEXTURE_MASK_CHECK, &CEffectMeshTab::OnBnClickedEffectmeshTextureMaskCheck)
	ON_BN_CLICKED(IDC_EFFECTMESH_TEXTURE_MASK_BROWSE_BUTTON, &CEffectMeshTab::OnBnClickedEffectmeshTextureMaskBrowseButton)
	ON_LBN_SELCHANGE(IDC_EFFECTMESH_EFFECTMESH_LIST, &CEffectMeshTab::OnLbnSelchangeEffectmeshEffectmeshList)
	ON_BN_CLICKED(IDC_EFFECTMESH_APPLY_GROUP_NAME_BUTTON, &CEffectMeshTab::OnBnClickedEffectmeshApplyGroupNameButton)
	ON_BN_CLICKED(IDC_EFFECTMESH_APPLY_CURRENT_EFFECT_NAME_BUTTON, &CEffectMeshTab::OnBnClickedEffectmeshApplyCurrentEffectNameButton)
	ON_BN_CLICKED(IDC_EFFECTMESH_DUPLICATE_BUTTON, &CEffectMeshTab::OnBnClickedEffectmeshDuplicateButton)
	ON_BN_CLICKED(IDC_EFFECTMESH_DELETE_BUTTON, &CEffectMeshTab::OnBnClickedEffectmeshDeleteButton)
	ON_EN_CHANGE(IDC_EFFECTMESH_CREATE_TIME_EDIT, &CEffectMeshTab::OnEnChangeEffectmeshCreateTimeEdit)
	ON_EN_CHANGE(IDC_EFFECTMESH_DEATH_TIME_EDIT, &CEffectMeshTab::OnEnChangeEffectmeshDeathTimeEdit)
	ON_BN_CLICKED(IDC_EFFECTMESH_CREATE_DEATH_REPEAT, &CEffectMeshTab::OnBnClickedEffectmeshCreateDeathRepeat)
	ON_BN_CLICKED(IDC_EFFECTMESH_PLAY_BUTTON, &CEffectMeshTab::OnBnClickedEffectmeshPlayButton)
	ON_BN_CLICKED(IDC_EFFECTMESH_STOP_BUTTON, &CEffectMeshTab::OnBnClickedEffectmeshStopButton)
	ON_EN_CHANGE(IDC_EFFECTMESH_PATTERN_POSITION_START_TIME_EDIT, &CEffectMeshTab::OnEnChangeEffectmeshPatternPositionStartTimeEdit)
	ON_EN_CHANGE(IDC_EFFECTMESH_PATTERN_POSITION_END_TIME_EDIT, &CEffectMeshTab::OnEnChangeEffectmeshPatternPositionEndTimeEdit)
	ON_BN_CLICKED(IDC_EFFECTMESH_PATTERN_POSITION_REPEAT_CHECK, &CEffectMeshTab::OnBnClickedEffectmeshPatternPositionRepeatCheck)
	ON_EN_CHANGE(IDC_EFFECTMESH_PATTERN_POSITION_X_EDIT, &CEffectMeshTab::OnEnChangeEffectmeshPatternPositionXEdit)
	ON_EN_CHANGE(IDC_EFFECTMESH_PATTERN_POSITION_Y_EDIT, &CEffectMeshTab::OnEnChangeEffectmeshPatternPositionYEdit)
	ON_EN_CHANGE(IDC_EFFECTMESH_PATTERN_POSITION_Z_EDIT, &CEffectMeshTab::OnEnChangeEffectmeshPatternPositionZEdit)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EFFECTMESH_PATTERN_POSITION_X_SPIN_CTRL, &CEffectMeshTab::OnDeltaposEffectmeshPatternPositionXSpinCtrl)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EFFECTMESH_PATTERN_POSITION_Y_SPIN_CTRL, &CEffectMeshTab::OnDeltaposEffectmeshPatternPositionYSpinCtrl)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EFFECTMESH_PATTERN_POSITION_Z_SPIN_CTRL, &CEffectMeshTab::OnDeltaposEffectmeshPatternPositionZSpinCtrl)
	ON_BN_CLICKED(IDC_EFFECTMESH_PATTERN_POSITION_SELECT_ALL_CHECK, &CEffectMeshTab::OnBnClickedEffectmeshPatternPositionSelectAllCheck)
	ON_EN_CHANGE(IDC_EFFECTMESH_PATTERN_ROTATION_START_TIME_EDIT, &CEffectMeshTab::OnEnChangeEffectmeshPatternRotationStartTimeEdit)
	ON_EN_CHANGE(IDC_EFFECTMESH_PATTERN_ROTATION_END_TIME_EDIT, &CEffectMeshTab::OnEnChangeEffectmeshPatternRotationEndTimeEdit)
	ON_BN_CLICKED(IDC_EFFECTMESH_PATTERN_ROTATION_REPEAT_CHECK, &CEffectMeshTab::OnBnClickedEffectmeshPatternRotationRepeatCheck)
	ON_EN_CHANGE(IDC_EFFECTMESH_PATTERN_ROTATION_X_EDIT, &CEffectMeshTab::OnEnChangeEffectmeshPatternRotationXEdit)
	ON_EN_CHANGE(IDC_EFFECTMESH_PATTERN_ROTATION_Y_EDIT, &CEffectMeshTab::OnEnChangeEffectmeshPatternRotationYEdit)
	ON_EN_CHANGE(IDC_EFFECTMESH_PATTERN_ROTATION_Z_EDIT, &CEffectMeshTab::OnEnChangeEffectmeshPatternRotationZEdit)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EFFECTMESH_PATTERN_ROTATION_X_SPIN_CTRL, &CEffectMeshTab::OnDeltaposEffectmeshPatternRotationXSpinCtrl)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EFFECTMESH_PATTERN_ROTATION_Y_SPIN_CTRL, &CEffectMeshTab::OnDeltaposEffectmeshPatternRotationYSpinCtrl)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EFFECTMESH_PATTERN_ROTATION_Z_SPIN_CTRL, &CEffectMeshTab::OnDeltaposEffectmeshPatternRotationZSpinCtrl)
	ON_BN_CLICKED(IDC_EFFECTMESH_PATTERN_ROTATION_SELECT_ALL_CHECK, &CEffectMeshTab::OnBnClickedEffectmeshPatternRotationSelectAllCheck)
	ON_EN_CHANGE(IDC_EFFECTMESH_PATTERN_SCALE_START_TIME_EDIT, &CEffectMeshTab::OnEnChangeEffectmeshPatternScaleStartTimeEdit)
	ON_EN_CHANGE(IDC_EFFECTMESH_PATTERN_SCALE_END_TIME_EDIT, &CEffectMeshTab::OnEnChangeEffectmeshPatternScaleEndTimeEdit)
	ON_BN_CLICKED(IDC_EFFECTMESH_PATTERN_SCALE_REPEAT_CHECK, &CEffectMeshTab::OnBnClickedEffectmeshPatternScaleRepeatCheck)
	ON_EN_CHANGE(IDC_EFFECTMESH_PATTERN_SCALE_X_EDIT, &CEffectMeshTab::OnEnChangeEffectmeshPatternScaleXEdit)
	ON_EN_CHANGE(IDC_EFFECTMESH_PATTERN_SCALE_Y_EDIT, &CEffectMeshTab::OnEnChangeEffectmeshPatternScaleYEdit)
	ON_EN_CHANGE(IDC_EFFECTMESH_PATTERN_SCALE_Z_EDIT, &CEffectMeshTab::OnEnChangeEffectmeshPatternScaleZEdit)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EFFECTMESH_PATTERN_SCALE_X_SPIN_CTRL, &CEffectMeshTab::OnDeltaposEffectmeshPatternScaleXSpinCtrl)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EFFECTMESH_PATTERN_SCALE_Y_SPIN_CTRL, &CEffectMeshTab::OnDeltaposEffectmeshPatternScaleYSpinCtrl)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EFFECTMESH_PATTERN_SCALE_Z_SPIN_CTRL, &CEffectMeshTab::OnDeltaposEffectmeshPatternScaleZSpinCtrl)
	ON_BN_CLICKED(IDC_EFFECTMESH_PATTERN_SCALE_SELECT_ALL_CHECK, &CEffectMeshTab::OnBnClickedEffectmeshPatternScaleSelectAllCheck)
	ON_BN_CLICKED(IDC_EFFECTMESH_SAVE_BUTTON, &CEffectMeshTab::OnBnClickedEffectmeshSaveButton)
	ON_BN_CLICKED(IDC_EFFECTMESH_LOAD_BUTTON, &CEffectMeshTab::OnBnClickedEffectmeshLoadButton)
	ON_EN_CHANGE(IDC_EFFECTMESH_TEXTURE_DEFAULT_NUMBER_EDIT, &CEffectMeshTab::OnEnChangeEffectmeshTextureDefaultNumberEdit)
	ON_EN_CHANGE(IDC_EFFECTMESH_TEXTURE_ALPHAONE_NUMBER_EDIT, &CEffectMeshTab::OnEnChangeEffectmeshTextureAlphaoneNumberEdit)
	ON_EN_CHANGE(IDC_EFFECTMESH_TEXTURE_MASK_NUMBER_EDIT, &CEffectMeshTab::OnEnChangeEffectmeshTextureMaskNumberEdit)
	ON_EN_CHANGE(IDC_EFFECTMESH_TEXTURE_DEFAULT_INTERVAL_EDIT, &CEffectMeshTab::OnEnChangeEffectmeshTextureDefaultIntervalEdit)
	ON_EN_CHANGE(IDC_EFFECTMESH_TEXTURE_ALPHAONE_INTERVAL_EDIT, &CEffectMeshTab::OnEnChangeEffectmeshTextureAlphaoneIntervalEdit)
	ON_EN_CHANGE(IDC_EFFECTMESH_TEXTURE_MASK_INTERVAL_EDIT, &CEffectMeshTab::OnEnChangeEffectmeshTextureMaskIntervalEdit)
	ON_BN_CLICKED(IDC_EFFECTMESH_PATTERN_POSITION_PROJECTILE_CHECK, &CEffectMeshTab::OnBnClickedEffectmeshPatternPositionProjectileCheck)
	ON_EN_CHANGE(IDC_EFFECTMESH_PATTERN_POSITION_RANDOM_LENGTH_MIN_EDIT, &CEffectMeshTab::OnEnChangeEffectmeshPatternPositionRandomLengthMinEdit)
	ON_EN_CHANGE(IDC_EFFECTMESH_PATTERN_POSITION_RANDOM_ANGLE_MIN_EDIT, &CEffectMeshTab::OnEnChangeEffectmeshPatternPositionRandomAngleMinEdit)
	ON_EN_CHANGE(IDC_EFFECTMESH_PATTERN_POSITION_RANDOM_ANGLE_MAX_EDIT, &CEffectMeshTab::OnEnChangeEffectmeshPatternPositionRandomAngleMaxEdit)
	ON_EN_CHANGE(IDC_EFFECTMESH_PATTERN_POSITION_RANDOM_LENGTH_MAX_EDIT, &CEffectMeshTab::OnEnChangeEffectmeshPatternPositionRandomLengthMaxEdit)
	ON_BN_CLICKED(IDC_MESHEFFECT_PATTERN_POSITION_PROJECTILE_RANDOM_LENGTH_ANGLE_CACULATE_BUTTON, &CEffectMeshTab::OnBnClickedMesheffectPatternPositionProjectileRandomLengthAngleCaculateButton)
	ON_BN_CLICKED(IDC_EFFECTMESH_UPDATE_SHADER_BUTTON, &CEffectMeshTab::OnBnClickedEffectmeshUpdateShaderButton)
	ON_BN_CLICKED(IDC_EFFECTMESH_TEXTURE_DEFAULT_SPRITE_REPEAT_CHECK, &CEffectMeshTab::OnBnClickedEffectmeshTextureDefaultSpriteRepeatCheck)
	ON_BN_CLICKED(IDC_EFFECTMESH_TEXTURE_ALPHAONE_SPRITE_REPEAT_CHECK, &CEffectMeshTab::OnBnClickedEffectmeshTextureAlphaoneSpriteRepeatCheck)
	ON_BN_CLICKED(IDC_EFFECTMESH_TEXTURE_MASK_SPRITE_REPEAT_CHECK, &CEffectMeshTab::OnBnClickedEffectmeshTextureMaskSpriteRepeatCheck)
	ON_EN_CHANGE(IDC_EFFECTMESH_FADE_IN_START_TIME_EDIT, &CEffectMeshTab::OnEnChangeEffectmeshFadeInStartTimeEdit)
	ON_EN_CHANGE(IDC_EFFECTMESH_FADE_IN_END_TIME_EDIT, &CEffectMeshTab::OnEnChangeEffectmeshFadeInEndTimeEdit)
	ON_EN_CHANGE(IDC_EFFECTMESH_FADE_IN_DEGREE_EDIT, &CEffectMeshTab::OnEnChangeEffectmeshFadeInDegreeEdit)
	ON_BN_CLICKED(IDC_EFFECTMESH_FADE_IN_CHECK, &CEffectMeshTab::OnBnClickedEffectmeshFadeInCheck)
	ON_EN_CHANGE(IDC_EFFECTMESH_FADE_OUT_START_TIME_EDIT, &CEffectMeshTab::OnEnChangeEffectmeshFadeOutStartTimeEdit)
	ON_EN_CHANGE(IDC_EFFECTMESH_FADE_OUT_END_TIME_EDIT, &CEffectMeshTab::OnEnChangeEffectmeshFadeOutEndTimeEdit)
	ON_EN_CHANGE(IDC_EFFECTMESH_FADE_OUT_DEGREE_EDIT, &CEffectMeshTab::OnEnChangeEffectmeshFadeOutDegreeEdit)
	ON_BN_CLICKED(IDC_EFFECTMESH_FADE_OUT_CHECK, &CEffectMeshTab::OnBnClickedEffectmeshFadeOutCheck)
	ON_BN_CLICKED(IDC_EFFECTMESH_LOAD_WITH_PARTICLE_BUTTON, &CEffectMeshTab::OnBnClickedEffectmeshLoadWithParticleButton)
	ON_BN_CLICKED(IDC_EFFECTMESH_UV_ANIMATION_CHECK, &CEffectMeshTab::OnBnClickedEffectmeshUvAnimationCheck)
	ON_EN_CHANGE(IDC_EFFECTMESH_UV_ANIMATION_START_TIME_EDIT, &CEffectMeshTab::OnEnChangeEffectmeshUvAnimationStartTimeEdit)
	ON_EN_CHANGE(IDC_EFFECTMESH_UV_ANIMATION_END_TIME_EDIT, &CEffectMeshTab::OnEnChangeEffectmeshUvAnimationEndTimeEdit)
	ON_EN_CHANGE(IDC_EFFECTMESH_UV_ANIMATION_X_EDIT, &CEffectMeshTab::OnEnChangeEffectmeshUvAnimationXEdit)
		ON_EN_CHANGE(IDC_EFFECTMESH_UV_ANIMATION_Y_EDIT, &CEffectMeshTab::OnEnChangeEffectmeshUvAnimationYEdit)
		END_MESSAGE_MAP()


// CEffectMeshTab 메시지 처리기입니다.
void CEffectMeshTab::Add_EffectMesh_ToList(CGameObject * pGameObj)
{
	m_listEffectMesh.emplace_back(pGameObj);
	Safe_AddRef(pGameObj);

	m_EffectMesh_ListBox.AddString(dynamic_cast<CToolEffectMeshObject*>(pGameObj)->Get_Desc().tInfo.szEffectMeshName);

	UpdateData(FALSE);
}

void CEffectMeshTab::Update_Shaders()
{
	if (m_tCurrentEffectDesc.tEffectMeshShader.vecShaderName.size() > 0)
	{
		for (_int i = 0; i < m_tCurrentEffectDesc.tEffectMeshShader.vecShaderName.size(); ++i)
			Safe_Delete(m_tCurrentEffectDesc.tEffectMeshShader.vecShaderName[i]);

		m_tCurrentEffectDesc.tEffectMeshShader.vecShaderName.clear();
	}

	CToolView* pToolView = GET_TOOLVIEW;

	for (auto& pIter : pToolView->m_pMainTab->m_pEffectMesh2Tab->m_listShaderName)
	{
		SHADERNAME* tShaderName = new SHADERNAME;
		_tcscpy_s(tShaderName->szName, pIter->szName);
		m_tCurrentEffectDesc.tEffectMeshShader.vecShaderName.emplace_back(tShaderName);
	}



}

void CEffectMeshTab::CopyDescription(EFFECTMESHDESC & tDestDesc, const EFFECTMESHDESC & tSourDesc)
{
	memcpy(&tDestDesc.tInfo, &tSourDesc.tInfo, sizeof(EFFECTMESHDESC::EFFECTMESHINFO));
	memcpy(&tDestDesc.tFade_InOut, &tSourDesc.tFade_InOut, sizeof(EFFECTMESHDESC::FADE_INOUT));
	memcpy(&tDestDesc.tPattern, &tSourDesc.tPattern, sizeof(EFFECTMESHDESC::EFFECTMESHPATTERN));
	memcpy(&tDestDesc.tTexture, &tSourDesc.tTexture, sizeof(EFFECTMESHDESC::EFFECTMESHTEXTURE));
	memcpy(&tDestDesc.tUV_Animation, &tSourDesc.tUV_Animation, sizeof(EFFECTMESHDESC::UV_ANIMATION));
	memcpy(&tDestDesc.tUV_Sprite, &tSourDesc.tUV_Sprite, sizeof(EFFECTMESHDESC::UV_SPRITE));

	if (tDestDesc.tEffectMeshShader.vecShaderName.size() > 0)
	{
		for (_int i = 0; i < tDestDesc.tEffectMeshShader.vecShaderName.size(); ++i)
			Safe_Delete(tDestDesc.tEffectMeshShader.vecShaderName[i]);
		tDestDesc.tEffectMeshShader.vecShaderName.clear();    
	}


	for (_int i = 0; i < tSourDesc.tEffectMeshShader.vecShaderName.size(); ++i)
	{
		SHADERNAME* tShaderName = new SHADERNAME;
		_tcscpy_s(tShaderName->szName, tSourDesc.tEffectMeshShader.vecShaderName[i]->szName);
		tDestDesc.tEffectMeshShader.vecShaderName.emplace_back(tShaderName);
	}
}

BOOL CEffectMeshTab::OnInitDialog()
{
	CDialog::OnInitDialog();
	Init_Descriptions();

	UpdateData(TRUE);

	return TRUE;
}

void CEffectMeshTab::Init_Descriptions()
{
	ZeroMemory(&m_tCurrentEffectDesc.tFade_InOut, sizeof(EFFECTMESHDESC::FADE_INOUT));
	ZeroMemory(&m_tCurrentEffectDesc.tInfo, sizeof(EFFECTMESHDESC::EFFECTMESHINFO));
	ZeroMemory(&m_tCurrentEffectDesc.tPattern, sizeof(EFFECTMESHDESC::EFFECTMESHPATTERN));
	ZeroMemory(&m_tCurrentEffectDesc.tTexture, sizeof(EFFECTMESHDESC::EFFECTMESHPATTERN));
	ZeroMemory(&m_tCurrentEffectDesc.tUV_Animation, sizeof(EFFECTMESHDESC::UV_ANIMATION));
	ZeroMemory(&m_tCurrentEffectDesc.tUV_Sprite, sizeof(EFFECTMESHDESC::UV_SPRITE));

	m_tCurrentEffectDesc.tEffectMeshShader.fDissolveSpeed = 0.f;
	_int iCount = 0;
	for (_int i = 0; i < iCount; i++)
		Safe_Delete(m_tCurrentEffectDesc.tEffectMeshShader.vecShaderName[i]);
	m_tCurrentEffectDesc.tEffectMeshShader.vecShaderName.clear();

	ZeroMemory(&(m_tEffectGroupDescription.szGroupName), MAX_PATH * sizeof(_tchar));
}

void CEffectMeshTab::Update_CurrentEffectMesh_Desc()
{
	if (m_pCurrentMeshEffect)
	{
		dynamic_cast<CToolEffectMeshObject*>(m_pCurrentMeshEffect)->Update_Desc(m_tCurrentEffectDesc);
	}
}

void CEffectMeshTab::Update_Tool_ByDesc()
{
	CString strTemp = {};

	// Update Transform

	strTemp.Format(L"%f", m_tCurrentEffectDesc.tInfo.vPosition.x);
	SetDlgItemText(IDC_EFFECTMESH_POSITION_X_RESULT, strTemp);
	strTemp.Format(L"%f", m_tCurrentEffectDesc.tInfo.vPosition.y);
	SetDlgItemText(IDC_EFFECTMESH_POSITION_Y_RESULT, strTemp);
	strTemp.Format(L"%f", m_tCurrentEffectDesc.tInfo.vPosition.z);
	SetDlgItemText(IDC_EFFECTMESH_POSITION_Z_RESULT, strTemp);

	strTemp.Format(L"%f", m_tCurrentEffectDesc.tInfo.vScale.x);
	SetDlgItemText(IDC_EFFECTMESH_SCALE_X_RESULT, strTemp);
	strTemp.Format(L"%f", m_tCurrentEffectDesc.tInfo.vScale.y);
	SetDlgItemText(IDC_EFFECTMESH_SCALE_Y_RESULT, strTemp);
	strTemp.Format(L"%f", m_tCurrentEffectDesc.tInfo.vScale.z);
	SetDlgItemText(IDC_EFFECTMESH_SCALE_Z_RESULT, strTemp);

	strTemp.Format(L"%f", m_tCurrentEffectDesc.tInfo.vRotation.x);
	SetDlgItemText(IDC_EFFECTMESH_ROTATION_X_RESULT, strTemp);
	strTemp.Format(L"%f", m_tCurrentEffectDesc.tInfo.vRotation.y);
	SetDlgItemText(IDC_EFFECTMESH_ROTATION_Y_RESULT, strTemp);
	strTemp.Format(L"%f", m_tCurrentEffectDesc.tInfo.vRotation.z);
	SetDlgItemText(IDC_EFFECTMESH_ROTATION_Z_RESULT, strTemp);

	// Names
	strTemp.Format(L"%s", m_tCurrentEffectDesc.tInfo.szMeshName);
	SetDlgItemText(IDC_EFFECTMESH_MESH_NAME, strTemp);
	strTemp.Format(L"%s", m_tEffectGroupDescription.szGroupName);
	SetDlgItemText(IDC_EFFECTMESH_GROUP_NAME_RESULT, strTemp);
	strTemp.Format(L"%s", m_tCurrentEffectDesc.tInfo.szEffectMeshName);
	SetDlgItemText(IDC_EFFECTMESH_CURRENT_EFFECT_NAME_RESULT, strTemp);


	

	// Textures
	((CButton*)GetDlgItem(IDC_EFFECTMESH_DEFAULT_TEXTURE_CHECK))->SetCheck(m_tCurrentEffectDesc.tTexture.bDefaultTexture);
	strTemp.Format(L"%d", m_tCurrentEffectDesc.tTexture.iDefaultTextureNumber);
	SetDlgItemText(IDC_EFFECTMESH_TEXTURE_DEFAULT_NUMBER_RESULT, strTemp);
	strTemp.Format(L"%f", m_tCurrentEffectDesc.tTexture.fDefaultTextureInterval);
	SetDlgItemText(IDC_EFFECTMESH_TEXTURE_DEFAULT_INTERVAL_RESULT, strTemp);
	strTemp.Format(L"%s", m_tCurrentEffectDesc.tTexture.szDefaultTextureName);
	SetDlgItemText(IDC_EFFECTMESH_TEXTURE_DEFAULT_RESULT, strTemp);
	((CButton*)GetDlgItem(IDC_EFFECTMESH_TEXTURE_DEFAULT_SPRITE_REPEAT_CHECK))->SetCheck(m_tCurrentEffectDesc.tTexture.bDefaultTextureSpriteRepeat);

	((CButton*)GetDlgItem(IDC_EFFECTMESH_TEXTURE_ALPHAONE_CHECK))->SetCheck(m_tCurrentEffectDesc.tTexture.bAlphaOneTexture);
	strTemp.Format(L"%d", m_tCurrentEffectDesc.tTexture.iAlphaOneTextureNumber);
	SetDlgItemText(IDC_EFFECTMESH_TEXTURE_ALPHAONE_NUMBER_RESULT, strTemp);
	strTemp.Format(L"%f", m_tCurrentEffectDesc.tTexture.fAlphaOneTextureInterval);
	SetDlgItemText(IDC_EFFECTMESH_TEXTURE_ALPHAONE_INTERVAL_RESULT, strTemp);
	strTemp.Format(L"%s", m_tCurrentEffectDesc.tTexture.szAlphaOneTextureName);
	SetDlgItemText(IDC_EFFECTMESH_TEXTURE_ALPHAONE_RESULT, strTemp);
	((CButton*)GetDlgItem(IDC_EFFECTMESH_TEXTURE_ALPHAONE_SPRITE_REPEAT_CHECK))->SetCheck(m_tCurrentEffectDesc.tTexture.bAlphaOneTextureSpriteRepeat);

	((CButton*)GetDlgItem(IDC_EFFECTMESH_TEXTURE_MASK_CHECK))->SetCheck(m_tCurrentEffectDesc.tTexture.bMaskTexture);
	strTemp.Format(L"%d", m_tCurrentEffectDesc.tTexture.iMaskTextureNumber);
	SetDlgItemText(IDC_EFFECTMESH_TEXTURE_MASK_NUMBER_RESULT, strTemp);
	strTemp.Format(L"%f", m_tCurrentEffectDesc.tTexture.fMaskTextureInterval);
	SetDlgItemText(IDC_EFFECTMESH_TEXTURE_MASK_INTERVAL_RESULT, strTemp);
	strTemp.Format(L"%s", m_tCurrentEffectDesc.tTexture.szMaskTextureName);
	SetDlgItemText(IDC_EFFECTMESH_TEXTURE_MASK_RESULT, strTemp);
	((CButton*)GetDlgItem(IDC_EFFECTMESH_TEXTURE_MASK_SPRITE_REPEAT_CHECK))->SetCheck(m_tCurrentEffectDesc.tTexture.bMaskTextureSpriteRepeat);

	// Create / Death Time

	
	strTemp.Format(L"%f", m_tEffectGroupDescription.vCreateDeathTime.x);
	SetDlgItemText(IDC_EFFECTMESH_CREATE_TIME_EDIT, strTemp);
	strTemp.Format(L"%f", m_tEffectGroupDescription.vCreateDeathTime.x);
	SetDlgItemText(IDC_EFFECTMESH_CREATE_TIME_RESULT, strTemp);
	strTemp.Format(L"%f", m_tEffectGroupDescription.vCreateDeathTime.y);
	SetDlgItemText(IDC_EFFECTMESH_DEATH_TIME_EDIT, strTemp);
	strTemp.Format(L"%f", m_tEffectGroupDescription.vCreateDeathTime.y);
	SetDlgItemText(IDC_EFFECTMESH_DEATH_TIME_RESULT, strTemp);
	((CButton*)GetDlgItem(IDC_EFFECTMESH_CREATE_DEATH_REPEAT))->SetCheck(m_tEffectGroupDescription.bCreateDeathRepeat);


	// Pattern
	// Position
	strTemp.Format(L"%f", m_tCurrentEffectDesc.tPattern.vPattern_Position_StartEndTime.x);
	SetDlgItemText(IDC_EFFECTMESH_PATTERN_POSITION_START_TIME_EDIT, strTemp);
	strTemp.Format(L"%f", m_tCurrentEffectDesc.tPattern.vPattern_Position_StartEndTime.x);
	SetDlgItemText(IDC_EFFECTMESH_PATTERN_POSITION_START_TIME_RESULT, strTemp);
	strTemp.Format(L"%f", m_tCurrentEffectDesc.tPattern.vPattern_Position_StartEndTime.y);
	SetDlgItemText(IDC_EFFECTMESH_PATTERN_POSITION_END_TIME_EDIT, strTemp);
	strTemp.Format(L"%f", m_tCurrentEffectDesc.tPattern.vPattern_Position_StartEndTime.y);
	SetDlgItemText(IDC_EFFECTMESH_PATTERN_POSITION_END_TIME_RESULT, strTemp);


	strTemp.Format(L"%f", m_tCurrentEffectDesc.tPattern.vPattern_Position_Goal.x);
	SetDlgItemText(IDC_EFFECTMESH_PATTERN_POSITION_X_EDIT, strTemp);
	strTemp.Format(L"%f", m_tCurrentEffectDesc.tPattern.vPattern_Position_Goal.x);
	SetDlgItemText(IDC_EFFECTMESH_PATTERN_POSITION_X_RESULT, strTemp);
	strTemp.Format(L"%f", m_tCurrentEffectDesc.tPattern.vPattern_Position_Goal.y);
	SetDlgItemText(IDC_EFFECTMESH_PATTERN_POSITION_Y_EDIT, strTemp);
	strTemp.Format(L"%f", m_tCurrentEffectDesc.tPattern.vPattern_Position_Goal.y);
	SetDlgItemText(IDC_EFFECTMESH_PATTERN_POSITION_Y_RESULT, strTemp);
	strTemp.Format(L"%f", m_tCurrentEffectDesc.tPattern.vPattern_Position_Goal.z);
	SetDlgItemText(IDC_EFFECTMESH_PATTERN_POSITION_Z_EDIT, strTemp);
	strTemp.Format(L"%f", m_tCurrentEffectDesc.tPattern.vPattern_Position_Goal.z);
	SetDlgItemText(IDC_EFFECTMESH_PATTERN_POSITION_Z_RESULT, strTemp);
	((CButton*)GetDlgItem(IDC_EFFECTMESH_PATTERN_POSITION_SELECT_ALL_CHECK))->SetCheck(m_bPatternPositionCheckAll);

	((CButton*)GetDlgItem(IDC_EFFECTMESH_PATTERN_POSITION_PROJECTILE_CHECK))->SetCheck(m_tCurrentEffectDesc.tPattern.bPattern_Position_Projectile);
	strTemp.Format(L"%f", m_tCurrentEffectDesc.tPattern.fPattern_Position_Projectile_Length_Minimum);
	SetDlgItemText(IDC_EFFECTMESH_PATTERN_POSITION_PROJECTILE_RANDOM_LENGTH_MIN_RESULT, strTemp);
	strTemp.Format(L"%f", m_tCurrentEffectDesc.tPattern.fPattern_Position_Projectile_Length_Maximum);
	SetDlgItemText(IDC_EFFECTMESH_PATTERN_POSITION_PROJECTILE_RANDOM_LENGTH_MAX_RESULT, strTemp);
	strTemp.Format(L"%f", m_tCurrentEffectDesc.tPattern.fPattern_Position_Projectile_Length_Result);
	SetDlgItemText(IDC_EFFECTMESH_PATTERN_POSITION_RANDOM_LENGTH_RESULT, strTemp);
	
	strTemp.Format(L"%f", m_tCurrentEffectDesc.tPattern.fPattern_Position_Projectile_Angle_Minimum);
	SetDlgItemText(IDC_EFFECTMESH_PATTERN_POSITION_PROJECTILE_RANDOM_ANGLE_MIN_RESULT, strTemp);
	strTemp.Format(L"%f", m_tCurrentEffectDesc.tPattern.fPattern_Position_Projectile_Angle_Maximum);
	SetDlgItemText(IDC_EFFECTMESH_PATTERN_POSITION_PROJECTILE_RANDOM_ANGLE_MAX_RESULT, strTemp);
	strTemp.Format(L"%f", m_tCurrentEffectDesc.tPattern.fPattern_Position_Projectile_Angle_Result);
	SetDlgItemText(IDC_EFFECTMESH_PATTERN_POSITION_RANDOM_ANGLE_RESULT, strTemp);

	// Rotation
	strTemp.Format(L"%f", m_tCurrentEffectDesc.tPattern.vPattern_Rotation_StartEndTime.x);
	SetDlgItemText(IDC_EFFECTMESH_PATTERN_ROTATION_START_TIME_EDIT, strTemp);
	strTemp.Format(L"%f", m_tCurrentEffectDesc.tPattern.vPattern_Rotation_StartEndTime.x);
	SetDlgItemText(IDC_EFFECTMESH_PATTERN_ROTATION_START_TIME_RESULT, strTemp);
	strTemp.Format(L"%f", m_tCurrentEffectDesc.tPattern.vPattern_Rotation_StartEndTime.y);
	SetDlgItemText(IDC_EFFECTMESH_PATTERN_ROTATION_END_TIME_EDIT, strTemp);
	strTemp.Format(L"%f", m_tCurrentEffectDesc.tPattern.vPattern_Rotation_StartEndTime.y);
	SetDlgItemText(IDC_EFFECTMESH_PATTERN_ROTATION_END_TIME_RESULT, strTemp);


	strTemp.Format(L"%f", m_tCurrentEffectDesc.tPattern.vPattern_Rotation_Goal.x);
	SetDlgItemText(IDC_EFFECTMESH_PATTERN_ROTATION_X_EDIT, strTemp);
	strTemp.Format(L"%f", m_tCurrentEffectDesc.tPattern.vPattern_Rotation_Goal.x);
	SetDlgItemText(IDC_EFFECTMESH_PATTERN_ROTATION_X_RESULT, strTemp);
	strTemp.Format(L"%f", m_tCurrentEffectDesc.tPattern.vPattern_Rotation_Goal.y);
	SetDlgItemText(IDC_EFFECTMESH_PATTERN_ROTATION_Y_EDIT, strTemp);
	strTemp.Format(L"%f", m_tCurrentEffectDesc.tPattern.vPattern_Rotation_Goal.y);
	SetDlgItemText(IDC_EFFECTMESH_PATTERN_ROTATION_Y_RESULT, strTemp);
	strTemp.Format(L"%f", m_tCurrentEffectDesc.tPattern.vPattern_Rotation_Goal.z);
	SetDlgItemText(IDC_EFFECTMESH_PATTERN_ROTATION_Z_EDIT, strTemp);
	strTemp.Format(L"%f", m_tCurrentEffectDesc.tPattern.vPattern_Rotation_Goal.z);
	SetDlgItemText(IDC_EFFECTMESH_PATTERN_ROTATION_Z_RESULT, strTemp);
	((CButton*)GetDlgItem(IDC_EFFECTMESH_PATTERN_ROTATION_SELECT_ALL_CHECK))->SetCheck(m_bPatternRotationCheckAll);

	// Scale
	strTemp.Format(L"%f", m_tCurrentEffectDesc.tPattern.vPattern_Scale_StartEndTime.x);
	SetDlgItemText(IDC_EFFECTMESH_PATTERN_SCALE_START_TIME_EDIT, strTemp);
	strTemp.Format(L"%f", m_tCurrentEffectDesc.tPattern.vPattern_Scale_StartEndTime.x);
	SetDlgItemText(IDC_EFFECTMESH_PATTERN_SCALE_START_TIME_RESULT, strTemp);
	strTemp.Format(L"%f", m_tCurrentEffectDesc.tPattern.vPattern_Scale_StartEndTime.y);
	SetDlgItemText(IDC_EFFECTMESH_PATTERN_SCALE_END_TIME_EDIT, strTemp);
	strTemp.Format(L"%f", m_tCurrentEffectDesc.tPattern.vPattern_Scale_StartEndTime.y);
	SetDlgItemText(IDC_EFFECTMESH_PATTERN_SCALE_END_TIME_RESULT, strTemp);

	strTemp.Format(L"%f", m_tCurrentEffectDesc.tPattern.vPattern_Scale_Goal.x);
	SetDlgItemText(IDC_EFFECTMESH_PATTERN_SCALE_X_EDIT, strTemp);
	strTemp.Format(L"%f", m_tCurrentEffectDesc.tPattern.vPattern_Scale_Goal.x);
	SetDlgItemText(IDC_EFFECTMESH_PATTERN_SCALE_X_RESULT, strTemp);
	strTemp.Format(L"%f", m_tCurrentEffectDesc.tPattern.vPattern_Scale_Goal.y);
	SetDlgItemText(IDC_EFFECTMESH_PATTERN_SCALE_Y_EDIT, strTemp);
	strTemp.Format(L"%f", m_tCurrentEffectDesc.tPattern.vPattern_Scale_Goal.y);
	SetDlgItemText(IDC_EFFECTMESH_PATTERN_SCALE_Y_RESULT, strTemp);
	strTemp.Format(L"%f", m_tCurrentEffectDesc.tPattern.vPattern_Scale_Goal.z);
	SetDlgItemText(IDC_EFFECTMESH_PATTERN_SCALE_Z_EDIT, strTemp);
	strTemp.Format(L"%f", m_tCurrentEffectDesc.tPattern.vPattern_Scale_Goal.z);
	SetDlgItemText(IDC_EFFECTMESH_PATTERN_SCALE_Z_RESULT, strTemp);
	((CButton*)GetDlgItem(IDC_EFFECTMESH_PATTERN_SCALE_SELECT_ALL_CHECK))->SetCheck(m_bPatternScaleCheckAll);


	// Fade In & Out
	strTemp.Format(L"%f", m_tCurrentEffectDesc.tFade_InOut.vFadeIn_StartEndTime.x);
	SetDlgItemText(IDC_EFFECTMESH_FADE_IN_START_TIME_RESULT, strTemp);
	strTemp.Format(L"%f", m_tCurrentEffectDesc.tFade_InOut.vFadeIn_StartEndTime.y);
	SetDlgItemText(IDC_EFFECTMESH_FADE_IN_END_TIME_RESULT, strTemp);
	strTemp.Format(L"%f", m_tCurrentEffectDesc.tFade_InOut.fFadeIn_Degree);
	SetDlgItemText(IDC_EFFECTMESH_FADE_IN_DEGREE_RESULT, strTemp);
	((CButton*)GetDlgItem(IDC_EFFECTMESH_FADE_IN_CHECK))->SetCheck(m_tCurrentEffectDesc.tFade_InOut.bFadeIn);

	strTemp.Format(L"%f", m_tCurrentEffectDesc.tFade_InOut.vFadeOut_StartEndTime.x);
	SetDlgItemText(IDC_EFFECTMESH_FADE_OUT_START_TIME_RESULT, strTemp);
	strTemp.Format(L"%f", m_tCurrentEffectDesc.tFade_InOut.vFadeOut_StartEndTime.y);
	SetDlgItemText(IDC_EFFECTMESH_FADE_OUT_END_TIME_RESULT, strTemp);
	strTemp.Format(L"%f", m_tCurrentEffectDesc.tFade_InOut.fFadeOut_Degree);
	SetDlgItemText(IDC_EFFECTMESH_FADE_OUT_DEGREE_RESULT, strTemp);
	((CButton*)GetDlgItem(IDC_EFFECTMESH_FADE_OUT_CHECK))->SetCheck(m_tCurrentEffectDesc.tFade_InOut.bFadeOut);


	// UV_Animation
	((CButton*)GetDlgItem(IDC_EFFECTMESH_UV_ANIMATION_CHECK))->SetCheck(m_tCurrentEffectDesc.tUV_Animation.bUVAnimation);
	strTemp.Format(L"%f", m_tCurrentEffectDesc.tUV_Animation.vUV_StartEndTime.x);
	SetDlgItemText(IDC_EFFECTMESH_UV_ANIMATION_START_TIME_RESULT, strTemp);
	strTemp.Format(L"%f", m_tCurrentEffectDesc.tUV_Animation.vUV_StartEndTime.x);
	SetDlgItemText(IDC_EFFECTMESH_UV_ANIMATION_END_TIME_RESULT, strTemp);
	strTemp.Format(L"%f", m_tCurrentEffectDesc.tUV_Animation.vUV_Val.x);
	SetDlgItemText(IDC_EFFECTMESH_UV_ANIMATION_X_RESULT, strTemp);
	strTemp.Format(L"%f", m_tCurrentEffectDesc.tUV_Animation.vUV_Val.y);
	SetDlgItemText(IDC_EFFECTMESH_UV_ANIMATION_Y_RESULT, strTemp);


	Update_Shader_ListBox();
}

void CEffectMeshTab::Update_Shader_ListBox()
{
	CToolView* pToolView = GET_TOOLVIEW;

	for (auto& pIter : pToolView->m_pMainTab->m_pEffectMesh2Tab->m_listShaderName)
		Safe_Delete(pIter);
	pToolView->m_pMainTab->m_pEffectMesh2Tab->m_listShaderName.clear();


	// Update list

	for (_int i = 0; i < m_tCurrentEffectDesc.tEffectMeshShader.vecShaderName.size(); i++)
	{
		SHADERNAME* tShaderName = new SHADERNAME;
		_tcscpy_s(tShaderName->szName, m_tCurrentEffectDesc.tEffectMeshShader.vecShaderName[i]->szName);
		pToolView->m_pMainTab->m_pEffectMesh2Tab->m_listShaderName.emplace_back(tShaderName);
	}

	// Update ListBox
	_int iCount = pToolView->m_pMainTab->m_pEffectMesh2Tab->m_AddedShaderList.GetCount();
	for (_int i = 0; i < iCount; ++i)
		pToolView->m_pMainTab->m_pEffectMesh2Tab->m_AddedShaderList.DeleteString(0);


	for (auto& pIter : pToolView->m_pMainTab->m_pEffectMesh2Tab->m_listShaderName)
		pToolView->m_pMainTab->m_pEffectMesh2Tab->m_AddedShaderList.AddString(pIter->szName);
}






void CEffectMeshTab::OnEnChangeEffectmeshPositionXEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");
	GetDlgItemText(IDC_EFFECTMESH_POSITION_X_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;

	m_tCurrentEffectDesc.tInfo.vPosition.x = _tstof(strVal);

	SetDlgItemText(IDC_EFFECTMESH_POSITION_X_RESULT, strVal);

	if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();

	UpdateData(FALSE);
}


void CEffectMeshTab::OnEnChangeEffectmeshPositionYEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");
	GetDlgItemText(IDC_EFFECTMESH_POSITION_Y_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;

	m_tCurrentEffectDesc.tInfo.vPosition.y = _tstof(strVal);

	SetDlgItemText(IDC_EFFECTMESH_POSITION_Y_RESULT, strVal);

	if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();

	UpdateData(FALSE);
}


void CEffectMeshTab::OnEnChangeEffectmeshPositionZEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");
	GetDlgItemText(IDC_EFFECTMESH_POSITION_Z_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;

	m_tCurrentEffectDesc.tInfo.vPosition.z = _tstof(strVal);

	SetDlgItemText(IDC_EFFECTMESH_POSITION_Z_RESULT, strVal);

	if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();

	UpdateData(FALSE);
}


void CEffectMeshTab::OnDeltaposEffectmeshPositionXSpinctrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateData(TRUE);

	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	if (pNMUpDown->iDelta < 0)
		m_tCurrentEffectDesc.tInfo.vPosition.x += 0.1f;
	else
		m_tCurrentEffectDesc.tInfo.vPosition.x -= 0.1f;

	CString str = {};
	str.Format(_T("%f"), m_tCurrentEffectDesc.tInfo.vPosition.x);

	SetDlgItemText(IDC_EFFECTMESH_POSITION_X_EDIT, str);
	SetDlgItemText(IDC_EFFECTMESH_POSITION_X_RESULT, str);

	if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();

	UpdateData(FALSE);
}


void CEffectMeshTab::OnDeltaposEffectmeshPositionYSpinctrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateData(TRUE);

	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	if (pNMUpDown->iDelta < 0)
		m_tCurrentEffectDesc.tInfo.vPosition.y += 0.1f;
	else
		m_tCurrentEffectDesc.tInfo.vPosition.y -= 0.1f;

	CString str = {};
	str.Format(_T("%f"), m_tCurrentEffectDesc.tInfo.vPosition.y);

	SetDlgItemText(IDC_EFFECTMESH_POSITION_Y_EDIT, str);
	SetDlgItemText(IDC_EFFECTMESH_POSITION_Y_RESULT, str);

	if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();

	UpdateData(FALSE);
}


void CEffectMeshTab::OnDeltaposEffectmeshPositionZSpinctrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateData(TRUE);

	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	if (pNMUpDown->iDelta < 0)
		m_tCurrentEffectDesc.tInfo.vPosition.z += 0.1f;
	else
		m_tCurrentEffectDesc.tInfo.vPosition.z -= 0.1f;

	CString str = {};
	str.Format(_T("%f"), m_tCurrentEffectDesc.tInfo.vPosition.z);

	SetDlgItemText(IDC_EFFECTMESH_POSITION_Z_EDIT, str);
	SetDlgItemText(IDC_EFFECTMESH_POSITION_Z_RESULT, str);

	if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();

	UpdateData(FALSE);
}


void CEffectMeshTab::OnEnChangeEffectmeshScaleXEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");
	GetDlgItemText(IDC_EFFECTMESH_SCALE_X_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;
	if (_tstof(strVal) == 0) return;

	m_tCurrentEffectDesc.tInfo.vScale.x = _tstof(strVal);

	SetDlgItemText(IDC_EFFECTMESH_SCALE_X_RESULT, strVal);

	if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();

	UpdateData(FALSE);
}


void CEffectMeshTab::OnEnChangeEffectmeshScaleYEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");
	GetDlgItemText(IDC_EFFECTMESH_SCALE_Y_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;
	if (_tstof(strVal) == 0) return;

	m_tCurrentEffectDesc.tInfo.vScale.y = _tstof(strVal);

	SetDlgItemText(IDC_EFFECTMESH_SCALE_Y_RESULT, strVal);

	if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();

	UpdateData(FALSE);
}


void CEffectMeshTab::OnEnChangeEffectmeshScaleZEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");
	GetDlgItemText(IDC_EFFECTMESH_SCALE_Z_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;
	if (_tstof(strVal) == 0) return;

	m_tCurrentEffectDesc.tInfo.vScale.z = _tstof(strVal);

	SetDlgItemText(IDC_EFFECTMESH_SCALE_Z_RESULT, strVal);

	if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();

	UpdateData(FALSE);
}


void CEffectMeshTab::OnDeltaposEffectmeshScaleXSpinctrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateData(TRUE);

	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	*pResult = 0;

	if (pNMUpDown->iDelta < 0)
	{
		m_tCurrentEffectDesc.tInfo.vScale.x += 0.1f;
		if (m_tCurrentEffectDesc.tInfo.vScale.x == 0.f)
		{
			m_tCurrentEffectDesc.tInfo.vScale.x -= 0.1f;
			return;
		}
	}
	else
	{
		m_tCurrentEffectDesc.tInfo.vScale.x -= 0.1f;
		if (m_tCurrentEffectDesc.tInfo.vScale.x == 0.f)
		{
			m_tCurrentEffectDesc.tInfo.vScale.x += 0.1f;
			return;
		}

	}

	CString str = {};
	str.Format(_T("%f"), m_tCurrentEffectDesc.tInfo.vScale.x);

	SetDlgItemText(IDC_EFFECTMESH_SCALE_X_EDIT, str);
	SetDlgItemText(IDC_EFFECTMESH_SCALE_X_RESULT, str);

	if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();

	UpdateData(FALSE);
}


void CEffectMeshTab::OnDeltaposEffectmeshScaleYSpinctrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateData(TRUE);

	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	*pResult = 0;

	if (pNMUpDown->iDelta < 0)
	{
		m_tCurrentEffectDesc.tInfo.vScale.y += 0.1f;
		if (m_tCurrentEffectDesc.tInfo.vScale.y == 0.f)
		{
			m_tCurrentEffectDesc.tInfo.vScale.y -= 0.1f;
			return;
		}
	}
	else
	{
		m_tCurrentEffectDesc.tInfo.vScale.y -= 0.1f;
		if (m_tCurrentEffectDesc.tInfo.vScale.y == 0.f)
		{
			m_tCurrentEffectDesc.tInfo.vScale.y += 0.1f;
			return;
		}

	}

	CString str = {};
	str.Format(_T("%f"), m_tCurrentEffectDesc.tInfo.vScale.y);

	SetDlgItemText(IDC_EFFECTMESH_SCALE_Y_EDIT, str);
	SetDlgItemText(IDC_EFFECTMESH_SCALE_Y_RESULT, str);

	if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();

	UpdateData(FALSE);
}


void CEffectMeshTab::OnDeltaposEffectmeshScaleZSpinctrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateData(TRUE);

	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	*pResult = 0;

	if (pNMUpDown->iDelta < 0)
	{
		m_tCurrentEffectDesc.tInfo.vScale.z += 0.1f;
		if (m_tCurrentEffectDesc.tInfo.vScale.z == 0.f)
		{
			m_tCurrentEffectDesc.tInfo.vScale.z -= 0.1f;
			return;
		}
	}
	else
	{
		m_tCurrentEffectDesc.tInfo.vScale.z -= 0.1f;
		if (m_tCurrentEffectDesc.tInfo.vScale.z == 0.f)
		{
			m_tCurrentEffectDesc.tInfo.vScale.z += 0.1f;
			return;
		}

	}

	CString str = {};
	str.Format(_T("%f"), m_tCurrentEffectDesc.tInfo.vScale.z);

	SetDlgItemText(IDC_EFFECTMESH_SCALE_Z_EDIT, str);
	SetDlgItemText(IDC_EFFECTMESH_SCALE_Z_RESULT, str);

	if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();

	UpdateData(FALSE);
}


void CEffectMeshTab::OnEnChangeEffectmeshRotationXEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");
	GetDlgItemText(IDC_EFFECTMESH_ROTATION_X_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;

	m_tCurrentEffectDesc.tInfo.vRotation.x = _tstof(strVal);

	SetDlgItemText(IDC_EFFECTMESH_ROTATION_X_RESULT, strVal);

	if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();

	UpdateData(FALSE);
}


void CEffectMeshTab::OnEnChangeEffectmeshRotationYEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");
	GetDlgItemText(IDC_EFFECTMESH_ROTATION_Y_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;

	m_tCurrentEffectDesc.tInfo.vRotation.y = _tstof(strVal);

	SetDlgItemText(IDC_EFFECTMESH_ROTATION_Y_RESULT, strVal);

	if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();

	UpdateData(FALSE);
}


void CEffectMeshTab::OnEnChangeEffectmeshRotationZEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");
	GetDlgItemText(IDC_EFFECTMESH_ROTATION_Z_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;

	m_tCurrentEffectDesc.tInfo.vRotation.z = _tstof(strVal);

	SetDlgItemText(IDC_EFFECTMESH_ROTATION_Z_RESULT, strVal);

	if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();

	UpdateData(FALSE);
}


void CEffectMeshTab::OnDeltaposEffectmeshRotationXSpinctrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateData(TRUE);

	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	if (pNMUpDown->iDelta < 0)
		m_tCurrentEffectDesc.tInfo.vRotation.x += 0.1f;
	else
		m_tCurrentEffectDesc.tInfo.vRotation.x -= 0.1f;

	CString str = {};
	str.Format(_T("%f"), m_tCurrentEffectDesc.tInfo.vRotation.x);

	SetDlgItemText(IDC_EFFECTMESH_ROTATION_X_EDIT, str);
	SetDlgItemText(IDC_EFFECTMESH_ROTATION_X_RESULT, str);

	if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();

	UpdateData(FALSE);
}


void CEffectMeshTab::OnDeltaposEffectmeshRotationYSpinctrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateData(TRUE);

	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	if (pNMUpDown->iDelta < 0)
		m_tCurrentEffectDesc.tInfo.vRotation.y += 0.1f;
	else
		m_tCurrentEffectDesc.tInfo.vRotation.y -= 0.1f;

	CString str = {};
	str.Format(_T("%f"), m_tCurrentEffectDesc.tInfo.vRotation.y);

	SetDlgItemText(IDC_EFFECTMESH_ROTATION_Y_EDIT, str);
	SetDlgItemText(IDC_EFFECTMESH_ROTATION_Y_RESULT, str);

	if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();

	UpdateData(FALSE);
}


void CEffectMeshTab::OnDeltaposEffectmeshRotationZSpinctrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateData(TRUE);

	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	if (pNMUpDown->iDelta < 0)
		m_tCurrentEffectDesc.tInfo.vRotation.z += 0.1f;
	else
		m_tCurrentEffectDesc.tInfo.vRotation.z -= 0.1f;

	CString str = {};
	str.Format(_T("%f"), m_tCurrentEffectDesc.tInfo.vRotation.z);

	SetDlgItemText(IDC_EFFECTMESH_ROTATION_Z_EDIT, str);
	SetDlgItemText(IDC_EFFECTMESH_ROTATION_Z_RESULT, str);

	if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();

	UpdateData(FALSE);
}


//void CEffectMeshTab::OnEnChangeEffectmeshGroupNameEdit()
//{
//	UpdateData(TRUE);
//
//	CString str = {};
//	GetDlgItemText(IDC_EFFECTMESH_GROUP_NAME_EDIT, str);
//	TCHAR* szName = (TCHAR*)(LPCTSTR)str;
//	_tcscpy_s(m_EffectGroupDescription.szGroupName, szName);
//	
//	SetDlgItemText(IDC_EFFECTMESH_GROUP_NAME_RESULT, str);
//	UpdateData(FALSE);
//
//	if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();
//}
//
//
//void CEffectMeshTab::OnEnChangeEffectmeshCurrentEffetNameEdit()
//{
//	UpdateData(TRUE);
//
//	CString str = {};
//	GetDlgItemText(IDC_EFFECTMESH_CURRENT_EFFET_NAME_EDIT, str);
//	TCHAR* szName = (TCHAR*)(LPCTSTR)str;
//	_tcscpy_s(m_CurrentEffectDesc.tInfo.szEffectMeshName, szName);
//
//	SetDlgItemText(IDC_EFFECTMESH_CURRENT_EFFECT_NAME_RESULT, str);
//	UpdateData(FALSE);
//	if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();
//}


void CEffectMeshTab::OnBnClickedEffectmeshCreateMeshButton()
{
	UpdateData(TRUE);

	CString MeshName = {};
	GetDlgItemText(IDC_EFFECTMESH_CURRENT_EFFECT_NAME_EDIT, MeshName);
	if (MeshName.GetLength() < 4)
		return;

	// Check Duplicate
	CString strInListBox = {};
	for (size_t i = 0; i < m_EffectMesh_ListBox.GetCount(); i++)
	{
		m_EffectMesh_ListBox.GetText(i, strInListBox);
		if (!strInListBox.Compare(MeshName))
		{
			return;
		}
	}
	SetDlgItemText(IDC_EFFECTMESH_CURRENT_EFFECT_NAME_RESULT, MeshName);


	TCHAR* szCurrentEffectMeshName = (TCHAR*)(LPCTSTR)MeshName;
	_tcscpy_s(m_tCurrentEffectDesc.tInfo.szEffectMeshName, szCurrentEffectMeshName);

	m_tCurrentEffectDesc.tInfo.iMeshKind = (_int)EFFECTMESHDESC::MESH_KIND::MESH;

	if (m_pCurrentMeshEffect)
	{
		Safe_Release(m_pCurrentMeshEffect);
		m_pCurrentMeshEffect = nullptr;
	}

	CString strProtoName = L"Prototype_Component_Model_";

	// Mesh Load
	CFileDialog Dlg(TRUE, L"dat", L"*.dat");

	TCHAR szBuf[MAX_PATH]{};
	GetCurrentDirectory(MAX_PATH, szBuf);
	PathRemoveFileSpec(szBuf);

	lstrcat(szBuf, L"\\Client\\Bin\\Data\\EffectMeshData");
	Dlg.m_ofn.lpstrInitialDir = szBuf;

	CToolView* pToolView = GET_TOOLVIEW;

	CString wstrPath;
	CString FileName;
	if (IDOK == Dlg.DoModal())
	{
		wstrPath = Dlg.GetPathName();
		FileName = wstrPath.Mid(wstrPath.ReverseFind('\\') + 1);
	}
	else
	{
		return;
	}
	strProtoName = strProtoName + FileName;

	wstrPath = wstrPath.Mid(wstrPath.Find(L"\\Bin"));
	CString TwoDot = L"..\\Client";

	wstrPath = TwoDot + wstrPath;

	TCHAR* szProtoName = (TCHAR*)(LPCTSTR)strProtoName;
	_tcscpy_s(m_tCurrentEffectDesc.tInfo.szMeshPrototypeName, szProtoName);
	TCHAR* szName = (TCHAR*)(LPCTSTR)FileName;
	_tcscpy_s(m_tCurrentEffectDesc.tInfo.szMeshName, szName);
	SetDlgItemText(IDC_EFFECTMESH_MESH_NAME, m_tCurrentEffectDesc.tInfo.szMeshName);
	TCHAR* szPath = (TCHAR*)(LPCTSTR)wstrPath;

	MODELPATHNAME* pModelPathName = new MODELPATHNAME;
	m_vecModelPathName.emplace_back(pModelPathName);

	_tcscpy_s(pModelPathName->szProto, szProtoName);
	_tcscpy_s(pModelPathName->szPath, szPath);




	if (FAILED(m_pGameInstance->Add_Component((_uint)LEVEL::STATIC, pModelPathName->szProto,
		CModel::Create(pToolView->Get_GraphicDevice(), pToolView->Get_ContextDevice(), pModelPathName->szPath,
			TEXT("../Client/Bin/ShaderFiles/Shader_EffectMesh.hlsl"), "DefaultTechnique", true))))
	{
		assert(false);
	}



	EffectMeshDescription NewDesc;

	ZeroMemory(&NewDesc.tFade_InOut, sizeof(EFFECTMESHDESC::FADE_INOUT));
	ZeroMemory(&NewDesc.tInfo, sizeof(EFFECTMESHDESC::EFFECTMESHINFO));
	ZeroMemory(&NewDesc.tPattern, sizeof(EFFECTMESHDESC::EFFECTMESHPATTERN));
	ZeroMemory(&NewDesc.tTexture, sizeof(EFFECTMESHDESC::EFFECTMESHPATTERN));
	ZeroMemory(&NewDesc.tUV_Animation, sizeof(EFFECTMESHDESC::UV_ANIMATION));
	ZeroMemory(&NewDesc.tUV_Sprite, sizeof(EFFECTMESHDESC::UV_SPRITE));

	memcpy(&NewDesc.tInfo, &m_tCurrentEffectDesc.tInfo, sizeof(EffectMeshDescription::EffectMeshInfo));

	if (FAILED(m_pGameInstance->Add_GameObjectToLayerWithPtr((_uint)LEVEL::STATIC, TEXT("Prototype_GameObject_ToolEffectMeshObject"), TEXT("Layer_EffectMesh"), &m_pCurrentMeshEffect, &NewDesc)))
		assert(false);

	CopyDescription(m_tCurrentEffectDesc, NewDesc);

	Safe_AddRef(m_pCurrentMeshEffect);
	Add_EffectMesh_ToList(m_pCurrentMeshEffect);



	Update_Tool_ByDesc();

	UpdateData(FALSE);

}


void CEffectMeshTab::OnBnClickedEffectmeshCreateImagelessMeshButton()
{
	UpdateData(TRUE);

	CString MeshName = {};
	GetDlgItemText(IDC_EFFECTMESH_CURRENT_EFFECT_NAME_EDIT, MeshName);
	if (MeshName.GetLength() < 4)
		return;

	// Check Duplicate
	CString strInListBox = {};
	for (size_t i = 0; i < m_EffectMesh_ListBox.GetCount(); i++)
	{
		m_EffectMesh_ListBox.GetText(i, strInListBox);
		if (!strInListBox.Compare(MeshName))
		{
			return;
		}
	}
	SetDlgItemText(IDC_EFFECTMESH_CURRENT_EFFECT_NAME_RESULT, MeshName);


	TCHAR* szCurrentEffectMeshName = (TCHAR*)(LPCTSTR)MeshName;
	_tcscpy_s(m_tCurrentEffectDesc.tInfo.szEffectMeshName, szCurrentEffectMeshName);

	m_tCurrentEffectDesc.tInfo.iMeshKind = (_int)EFFECTMESHDESC::MESH_KIND::IMAGELESS_MESH;

	if (m_pCurrentMeshEffect)
	{
		Safe_Release(m_pCurrentMeshEffect);
		m_pCurrentMeshEffect = nullptr;
	}

	CString strProtoName = L"Prototype_Component_Model_";

	// Mesh Load
	CFileDialog Dlg(TRUE, L"dat", L"*.dat");

	TCHAR szBuf[MAX_PATH]{};
	GetCurrentDirectory(MAX_PATH, szBuf);
	PathRemoveFileSpec(szBuf);

	lstrcat(szBuf, L"\\Client\\Bin\\Data\\EffectMeshData");
	Dlg.m_ofn.lpstrInitialDir = szBuf;

	CToolView* pToolView = GET_TOOLVIEW;

	CString wstrPath;
	CString FileName;
	if (IDOK == Dlg.DoModal())
	{
		wstrPath = Dlg.GetPathName();
		FileName = wstrPath.Mid(wstrPath.ReverseFind('\\') + 1);
	}
	else
	{
		return;
	}

	wstrPath = wstrPath.Mid(wstrPath.Find(L"\\Bin"));
	CString TwoDot = L"..\\Client";
	wstrPath = TwoDot + wstrPath;

	strProtoName = strProtoName + FileName;
	TCHAR* szProtoName = (TCHAR*)(LPCTSTR)strProtoName;
	_tcscpy_s(m_tCurrentEffectDesc.tInfo.szMeshPrototypeName, szProtoName);
	TCHAR* szName = (TCHAR*)(LPCTSTR)FileName;
	_tcscpy_s(m_tCurrentEffectDesc.tInfo.szMeshName, szName);
	SetDlgItemText(IDC_EFFECTMESH_MESH_NAME, m_tCurrentEffectDesc.tInfo.szMeshName);
	TCHAR* szPath = (TCHAR*)(LPCTSTR)wstrPath;





	_bool iFound = false;
	for (auto& pIter : m_vecModelPathName)
	{
		if (!_tcscmp(pIter->szName, szName))
		{
			iFound = true;
			break;
		}
	}

	if (!iFound)
	{




		MODELPATHNAME* pModelPathName = new MODELPATHNAME;
		m_vecModelPathName.emplace_back(pModelPathName);


		_tcscpy_s(pModelPathName->szProto, szProtoName);
		_tcscpy_s(pModelPathName->szPath, szPath);
		_tcscpy_s(pModelPathName->szName, szName);

		if (FAILED(m_pGameInstance->Add_Component((_uint)LEVEL::STATIC, pModelPathName->szProto,
			CModel::Create(pToolView->Get_GraphicDevice(), pToolView->Get_ContextDevice(), pModelPathName->szPath,
				TEXT("../Client/Bin/ShaderFiles/Shader_EffectMesh.hlsl"), "DefaultTechnique", true))))
		{
			assert(false);
		}

	}







	EffectMeshDescription NewDesc;

	ZeroMemory(&NewDesc.tFade_InOut, sizeof(EFFECTMESHDESC::FADE_INOUT));
	ZeroMemory(&NewDesc.tInfo, sizeof(EFFECTMESHDESC::EFFECTMESHINFO));
	ZeroMemory(&NewDesc.tPattern, sizeof(EFFECTMESHDESC::EFFECTMESHPATTERN));
	ZeroMemory(&NewDesc.tTexture, sizeof(EFFECTMESHDESC::EFFECTMESHTEXTURE));
	ZeroMemory(&NewDesc.tUV_Animation, sizeof(EFFECTMESHDESC::UV_ANIMATION));
	ZeroMemory(&NewDesc.tUV_Sprite, sizeof(EFFECTMESHDESC::UV_SPRITE));
	memcpy(&NewDesc.tInfo, &m_tCurrentEffectDesc.tInfo, sizeof(EffectMeshDescription::EffectMeshInfo));

	if (FAILED(m_pGameInstance->Add_GameObjectToLayerWithPtr((_uint)LEVEL::STATIC, TEXT("Prototype_GameObject_ToolEffectMeshObject"), TEXT("Layer_EffectMesh"), &m_pCurrentMeshEffect, &NewDesc)))
		assert(false);

	CopyDescription(m_tCurrentEffectDesc, NewDesc);
	memcpy(&m_tCurrentEffectDesc, &NewDesc, sizeof(EffectMeshDescription));

	Safe_AddRef(m_pCurrentMeshEffect);
	Add_EffectMesh_ToList(m_pCurrentMeshEffect);



	Update_Tool_ByDesc();

	UpdateData(FALSE);
}


void CEffectMeshTab::OnBnClickedEffectmeshCreateRectButton()
{
	UpdateData(TRUE);




	CString MeshName = {};
	GetDlgItemText(IDC_EFFECTMESH_CURRENT_EFFECT_NAME_EDIT, MeshName);
	if (MeshName.GetLength() < 4)
		return;

	// 중복 검사
	CString strInListBox = {};
	for (size_t i = 0; i < m_EffectMesh_ListBox.GetCount(); i++)
	{
		m_EffectMesh_ListBox.GetText(i, strInListBox);
		if (!strInListBox.Compare(MeshName))
		{
			return;
		}
	}
	SetDlgItemText(IDC_EFFECTMESH_CURRENT_EFFECT_NAME_RESULT, MeshName);


	TCHAR* szCurrentEffectMeshName = (TCHAR*)(LPCTSTR)MeshName;
	_tcscpy_s(m_tCurrentEffectDesc.tInfo.szEffectMeshName, szCurrentEffectMeshName);

	m_tCurrentEffectDesc.tInfo.iMeshKind = (_int)EFFECTMESHDESC::MESH_KIND::RECT;

	if (m_pCurrentMeshEffect)
	{
		Safe_Release(m_pCurrentMeshEffect);
		m_pCurrentMeshEffect = nullptr;
	}

	_tcscpy_s(m_tCurrentEffectDesc.tInfo.szMeshName, L"Rect");
	SetDlgItemText(IDC_EFFECTMESH_MESH_NAME, m_tCurrentEffectDesc.tInfo.szMeshName);
	


	EffectMeshDescription NewDesc;

	ZeroMemory(&NewDesc.tFade_InOut, sizeof(EFFECTMESHDESC::FADE_INOUT));
	ZeroMemory(&NewDesc.tInfo, sizeof(EFFECTMESHDESC::EFFECTMESHINFO));
	ZeroMemory(&NewDesc.tPattern, sizeof(EFFECTMESHDESC::EFFECTMESHPATTERN));
	ZeroMemory(&NewDesc.tTexture, sizeof(EFFECTMESHDESC::EFFECTMESHTEXTURE));
	ZeroMemory(&NewDesc.tUV_Animation, sizeof(EFFECTMESHDESC::UV_ANIMATION));
	ZeroMemory(&NewDesc.tUV_Sprite, sizeof(EFFECTMESHDESC::UV_SPRITE));


	memcpy(&NewDesc.tInfo, &m_tCurrentEffectDesc.tInfo, sizeof(EffectMeshDescription::EffectMeshInfo));

	if (FAILED(m_pGameInstance->Add_GameObjectToLayerWithPtr((_uint)LEVEL::STATIC, TEXT("Prototype_GameObject_ToolEffectMeshObject"), TEXT("Layer_EffectMesh"), &m_pCurrentMeshEffect, &NewDesc)))
		assert(false);

	CopyDescription(m_tCurrentEffectDesc, NewDesc);

	Safe_AddRef(m_pCurrentMeshEffect);
	Add_EffectMesh_ToList(m_pCurrentMeshEffect);



	Update_Tool_ByDesc();

	UpdateData(FALSE);
}


void CEffectMeshTab::OnBnClickedEffectmeshTextureDefaultCheck()
{
	UpdateData(TRUE);

	if (((CButton*)GetDlgItem(IDC_EFFECTMESH_DEFAULT_TEXTURE_CHECK))->GetCheck())
		m_tCurrentEffectDesc.tTexture.bDefaultTexture = true;
	else 
		m_tCurrentEffectDesc.tTexture.bDefaultTexture = false;

	UpdateData(FALSE);

	if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();
}


void CEffectMeshTab::OnBnClickedEffectmeshTextureDefaultBrowseButton()
{
	CFileDialog Dlg(TRUE, L"dds", L"*.dds");
	TCHAR szBuf[MAX_PATH]{};
	GetCurrentDirectory(MAX_PATH, szBuf);

	PathRemoveFileSpec(szBuf);
	lstrcat(szBuf, L"\\Client\\Bin\\Resources\\Effect\\ImageEffect");
	Dlg.m_ofn.lpstrInitialDir = szBuf;

	DWORD dwByte = 0;
	const CString			strPrototype = L"Prototype_Component_Texture_";
	CToolView* pToolView = GET_TOOLVIEW;

	if (IDOK == Dlg.DoModal())
	{
		// Get Absolute Path
		CString wstrPath = Dlg.GetPathName();
		CString FileNameWithExt = Dlg.GetFileName();
		CString FinalProtoName = strPrototype + FileNameWithExt;

		TEXNAMEPROTOPATH* pTexProtoPath = new TEXNAMEPROTOPATH;
		m_vecTexProtoPath.push_back(pTexProtoPath);

		wstrPath = wstrPath.Mid(wstrPath.Find(L"\\Bin"));
		CString TwoDot = L"..\\Client";

		wstrPath = TwoDot + wstrPath;

		TCHAR* szName = (TCHAR*)(LPCTSTR)wstrPath;
		_tcscpy_s(pTexProtoPath->szPath, szName);
		szName = (TCHAR*)(LPCTSTR)FinalProtoName;
		_tcscpy_s(pTexProtoPath->szProto, szName);
		szName = (TCHAR*)(LPCTSTR)FileNameWithExt;
		_tcscpy_s(pTexProtoPath->szName, szName);
		pTexProtoPath->iTextureNumber = m_tCurrentEffectDesc.tTexture.iDefaultTextureNumber;

		if (m_tCurrentEffectDesc.tTexture.iDefaultTextureNumber > 1)
		{
			_tchar					szExt[MAX_PATH] = TEXT("");
			_tchar					szFileName[MAX_PATH] = TEXT("");
			_wsplitpath_s(pTexProtoPath->szPath, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);
			
			_tchar					szSplitType[MAX_PATH] = TEXT("_");
			TCHAR*					szNextToken;

			_tchar					szNewFileName[MAX_PATH] = TEXT("");
			_tcscpy_s(szNewFileName, szFileName);
			_tcstok_s(szNewFileName, szSplitType, &szNextToken);

			_tcscat_s(szNewFileName, szSplitType);
			_tcscat_s(szNewFileName, L"%d");
			_tcscat_s(szNewFileName, szExt);

			_int iFileNameLength = _tcslen(szFileName) + _tcslen(szExt);
			_int iFullPathLength = _tcslen(pTexProtoPath->szPath);
			_tcsncpy_s(pTexProtoPath->szPath, pTexProtoPath->szPath, iFullPathLength - iFileNameLength);

			_tcscat_s(pTexProtoPath->szPath, szNewFileName);

		}

		
		// Load Texture   
		if (FAILED(m_pGameInstance->Add_Component((_uint)LEVEL::STATIC, pTexProtoPath->szProto, CTexture::Create(pToolView->Get_GraphicDevice(), pToolView->Get_ContextDevice(), pTexProtoPath->szPath, m_tCurrentEffectDesc.tTexture.iDefaultTextureNumber))))
		{
			int a = 0;
			//assert(false);
		}

		_tcscpy_s(m_tCurrentEffectDesc.tTexture.szDefaultTextureName, pTexProtoPath->szName);
		_tcscpy_s(m_tCurrentEffectDesc.tTexture.szDefaultTexturePrototypeName, pTexProtoPath->szProto);
		SetDlgItemText(IDC_EFFECTMESH_TEXTURE_DEFAULT_RESULT, FileNameWithExt);
		
		if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();
	}
}

void CEffectMeshTab::OnEnChangeEffectmeshTextureDefaultNumberEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");
	GetDlgItemText(IDC_EFFECTMESH_TEXTURE_DEFAULT_NUMBER_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;

	m_tCurrentEffectDesc.tTexture.iDefaultTextureNumber = _ttoi(strVal);

	SetDlgItemText(IDC_EFFECTMESH_TEXTURE_DEFAULT_NUMBER_RESULT, strVal);

	if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();

	UpdateData(FALSE);
}

void CEffectMeshTab::OnEnChangeEffectmeshTextureDefaultIntervalEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");
	GetDlgItemText(IDC_EFFECTMESH_TEXTURE_DEFAULT_INTERVAL_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;

	m_tCurrentEffectDesc.tTexture.fDefaultTextureInterval = _tstof(strVal);

	SetDlgItemText(IDC_EFFECTMESH_TEXTURE_DEFAULT_INTERVAL_RESULT, strVal);

	if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();

	UpdateData(FALSE);
}

void CEffectMeshTab::OnBnClickedEffectmeshTextureDefaultSpriteRepeatCheck()
{
	UpdateData(TRUE);

	if (((CButton*)GetDlgItem(IDC_EFFECTMESH_TEXTURE_DEFAULT_SPRITE_REPEAT_CHECK))->GetCheck())
		m_tCurrentEffectDesc.tTexture.bDefaultTextureSpriteRepeat = true;
	else
		m_tCurrentEffectDesc.tTexture.bDefaultTextureSpriteRepeat = false;

	UpdateData(FALSE);

	if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();
}


void CEffectMeshTab::OnBnClickedEffectmeshTextureAlphaoneCheck()
{
	UpdateData(TRUE);


	if (((CButton*)GetDlgItem(IDC_EFFECTMESH_TEXTURE_ALPHAONE_CHECK))->GetCheck())
		m_tCurrentEffectDesc.tTexture.bAlphaOneTexture = true;
	else
		m_tCurrentEffectDesc.tTexture.bAlphaOneTexture = false;

	UpdateData(FALSE);

	if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();
}


void CEffectMeshTab::OnBnClickedEffectmeshTextureAlphaoneBrowseButton()
{
	CFileDialog Dlg(TRUE, L"dds", L"*.dds");
	TCHAR szBuf[MAX_PATH]{};
	GetCurrentDirectory(MAX_PATH, szBuf);

	PathRemoveFileSpec(szBuf);
	lstrcat(szBuf, L"\\Client\\Bin\\Resources\\Effect\\ImageEffect");
	Dlg.m_ofn.lpstrInitialDir = szBuf;

	DWORD dwByte = 0;
	const CString			strPrototype = L"Prototype_Component_Texture_";
	CToolView* pToolView = GET_TOOLVIEW;



	if (IDOK == Dlg.DoModal())
	{
		// Get Absolute Path
		CString wstrPath = Dlg.GetPathName();
		CString FileNameWithExt = Dlg.GetFileName();
		CString FinalProtoName = strPrototype + FileNameWithExt;

		TEXNAMEPROTOPATH* pTexProtoPath = new TEXNAMEPROTOPATH;
		m_vecTexProtoPath.push_back(pTexProtoPath);

		wstrPath = wstrPath.Mid(wstrPath.Find(L"\\Bin"));
		CString TwoDot = L"..\\Client";

		wstrPath = TwoDot + wstrPath;

		TCHAR* szName = (TCHAR*)(LPCTSTR)wstrPath;
		_tcscpy_s(pTexProtoPath->szPath, szName);
		szName = (TCHAR*)(LPCTSTR)FinalProtoName;
		_tcscpy_s(pTexProtoPath->szProto, szName);
		szName = (TCHAR*)(LPCTSTR)FileNameWithExt;
		_tcscpy_s(pTexProtoPath->szName, szName);
		pTexProtoPath->iTextureNumber = m_tCurrentEffectDesc.tTexture.iAlphaOneTextureNumber;

		

		// Load Texture
		if (FAILED(m_pGameInstance->Add_Component((_uint)LEVEL::STATIC, pTexProtoPath->szProto, CTexture::Create(pToolView->Get_GraphicDevice(), pToolView->Get_ContextDevice(), pTexProtoPath->szPath, pTexProtoPath->iTextureNumber))))
		{
			int a = 0;
			//assert(false);
		}

		_tcscpy_s(m_tCurrentEffectDesc.tTexture.szAlphaOneTextureName, pTexProtoPath->szName);
		_tcscpy_s(m_tCurrentEffectDesc.tTexture.szAlphaOneTexturePrototypeName, pTexProtoPath->szProto);
		SetDlgItemText(IDC_EFFECTMESH_TEXTURE_ALPHAONE_RESULT, FileNameWithExt);

		if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();
	}
}


void CEffectMeshTab::OnEnChangeEffectmeshTextureAlphaoneNumberEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");
	GetDlgItemText(IDC_EFFECTMESH_TEXTURE_ALPHAONE_NUMBER_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;

	m_tCurrentEffectDesc.tTexture.iAlphaOneTextureNumber = _ttoi(strVal);

	SetDlgItemText(IDC_EFFECTMESH_TEXTURE_ALPHAONE_NUMBER_RESULT, strVal);

	if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();

	UpdateData(FALSE);
}

void CEffectMeshTab::OnEnChangeEffectmeshTextureAlphaoneIntervalEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");
	GetDlgItemText(IDC_EFFECTMESH_TEXTURE_ALPHAONE_INTERVAL_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;

	m_tCurrentEffectDesc.tTexture.fAlphaOneTextureInterval = _tstof(strVal);

	SetDlgItemText(IDC_EFFECTMESH_TEXTURE_ALPHAONE_INTERVAL_RESULT, strVal);

	if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();

	UpdateData(FALSE);
}

void CEffectMeshTab::OnBnClickedEffectmeshTextureAlphaoneSpriteRepeatCheck()
{
	UpdateData(TRUE);

	if (((CButton*)GetDlgItem(IDC_EFFECTMESH_TEXTURE_ALPHAONE_SPRITE_REPEAT_CHECK))->GetCheck())
		m_tCurrentEffectDesc.tTexture.bAlphaOneTextureSpriteRepeat = true;
	else
		m_tCurrentEffectDesc.tTexture.bAlphaOneTextureSpriteRepeat = false;

	UpdateData(FALSE);

	if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();
}

void CEffectMeshTab::OnBnClickedEffectmeshTextureMaskCheck()
{
	UpdateData(TRUE);

	if (((CButton*)GetDlgItem(IDC_EFFECTMESH_TEXTURE_MASK_CHECK))->GetCheck())
		m_tCurrentEffectDesc.tTexture.bMaskTexture = true;
	else
		m_tCurrentEffectDesc.tTexture.bMaskTexture = false;

	UpdateData(FALSE);

	if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();
}


void CEffectMeshTab::OnBnClickedEffectmeshTextureMaskBrowseButton()
{
	CFileDialog Dlg(TRUE, L"dds", L"*.dds");
	TCHAR szBuf[MAX_PATH]{};
	GetCurrentDirectory(MAX_PATH, szBuf);

	PathRemoveFileSpec(szBuf);
	lstrcat(szBuf, L"\\Client\\Bin\\Resources\\Effect\\ImageEffect");
	Dlg.m_ofn.lpstrInitialDir = szBuf;

	DWORD dwByte = 0;
	const CString			strPrototype = L"Prototype_Component_Texture_";
	CToolView* pToolView = GET_TOOLVIEW;

	if (IDOK == Dlg.DoModal())
	{
		// Get Absolute Path
		CString wstrPath = Dlg.GetPathName();
		CString FileNameWithExt = Dlg.GetFileName();
		CString FinalProtoName = strPrototype + FileNameWithExt;

		TEXNAMEPROTOPATH* pTexProtoPath = new TEXNAMEPROTOPATH;
		m_vecTexProtoPath.push_back(pTexProtoPath);

		TCHAR* szName = (TCHAR*)(LPCTSTR)wstrPath;
		_tcscpy_s(pTexProtoPath->szPath, szName);
		szName = (TCHAR*)(LPCTSTR)FinalProtoName;
		_tcscpy_s(pTexProtoPath->szProto, szName);
		szName = (TCHAR*)(LPCTSTR)FileNameWithExt;
		_tcscpy_s(pTexProtoPath->szName, szName);
		pTexProtoPath->iTextureNumber = m_tCurrentEffectDesc.tTexture.iMaskTextureNumber;

		wstrPath = wstrPath.Mid(wstrPath.Find(L"\\Bin"));
		CString TwoDot = L"..\\Client";
		wstrPath = TwoDot + wstrPath;

		// Load Texture
		if (FAILED(m_pGameInstance->Add_Component((_uint)LEVEL::STATIC, pTexProtoPath->szProto, CTexture::Create(pToolView->Get_GraphicDevice(), pToolView->Get_ContextDevice(), pTexProtoPath->szPath, pTexProtoPath->iTextureNumber))))
		{
			int a = 0;
			//assert(false);
		}

		_tcscpy_s(m_tCurrentEffectDesc.tTexture.szMaskTextureName, pTexProtoPath->szName);
		_tcscpy_s(m_tCurrentEffectDesc.tTexture.szMaskTexturePrototypeName, pTexProtoPath->szProto);
		SetDlgItemText(IDC_EFFECTMESH_TEXTURE_MASK_RESULT, FileNameWithExt);

		if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();
	}
}

void CEffectMeshTab::OnEnChangeEffectmeshTextureMaskNumberEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");
	GetDlgItemText(IDC_EFFECTMESH_TEXTURE_MASK_NUMBER_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;

	m_tCurrentEffectDesc.tTexture.iMaskTextureNumber = _ttoi(strVal);

	SetDlgItemText(IDC_EFFECTMESH_TEXTURE_MASK_NUMBER_RESULT, strVal);

	if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();

	UpdateData(FALSE);
}


void CEffectMeshTab::OnEnChangeEffectmeshTextureMaskIntervalEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");
	GetDlgItemText(IDC_EFFECTMESH_TEXTURE_MASK_INTERVAL_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;

	m_tCurrentEffectDesc.tTexture.fMaskTextureInterval = _tstof(strVal);

	SetDlgItemText(IDC_EFFECTMESH_TEXTURE_MASK_INTERVAL_RESULT, strVal);

	if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();

	UpdateData(FALSE);
}



void CEffectMeshTab::OnLbnSelchangeEffectmeshEffectmeshList()
{
	int iIndex = m_EffectMesh_ListBox.GetCurSel();
	if (iIndex >= m_EffectMesh_ListBox.GetCount() || iIndex < 0)
		return;
	CString SelectedEffectMesh;

	m_EffectMesh_ListBox.GetText(iIndex, SelectedEffectMesh);

	// If Slelected one is same with current, return;
	if (!SelectedEffectMesh.Compare(m_tCurrentEffectDesc.tInfo.szEffectMeshName))
		return;

	for (auto& pIter : m_listEffectMesh)
	{
		if (0 == SelectedEffectMesh.Compare(dynamic_cast<CToolEffectMeshObject*>(pIter)->Get_Desc().tInfo.szEffectMeshName))
		{
			if (m_pCurrentMeshEffect)
			{
				Safe_Release(m_pCurrentMeshEffect);
				m_pCurrentMeshEffect = nullptr;
			}
			m_pCurrentMeshEffect = pIter;
			Safe_AddRef(m_pCurrentMeshEffect);
			CopyDescription(m_tCurrentEffectDesc, dynamic_cast<CToolEffectMeshObject*>(m_pCurrentMeshEffect)->Get_Desc());

			Update_Tool_ByDesc();
			break;
		}
	}

	

}

void CEffectMeshTab::OnBnClickedEffectmeshTextureMaskSpriteRepeatCheck()
{
	UpdateData(TRUE);

	if (((CButton*)GetDlgItem(IDC_EFFECTMESH_TEXTURE_MASK_SPRITE_REPEAT_CHECK))->GetCheck())
		m_tCurrentEffectDesc.tTexture.bMaskTextureSpriteRepeat = true;
	else
		m_tCurrentEffectDesc.tTexture.bMaskTextureSpriteRepeat = false;

	UpdateData(FALSE);

	if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();
}

void CEffectMeshTab::OnBnClickedEffectmeshApplyGroupNameButton()
{
	UpdateData(TRUE);

	CString EditName = {};
	GetDlgItemText(IDC_EFFECTMESH_GROUP_NAME_EDIT, EditName);
	TCHAR* szName = (TCHAR*)(LPCTSTR)EditName;
	_tcscpy_s(m_tEffectGroupDescription.szGroupName, szName);



	SetDlgItemText(IDC_EFFECTMESH_GROUP_NAME_RESULT, EditName);
	UpdateData(FALSE);

	if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();
}


void CEffectMeshTab::OnBnClickedEffectmeshApplyCurrentEffectNameButton()
{
	UpdateData(TRUE);

	CString EditName = {};
	GetDlgItemText(IDC_EFFECTMESH_CURRENT_EFFECT_NAME_EDIT, EditName);
	TCHAR* szName = (TCHAR*)(LPCTSTR)EditName;
	_tcscpy_s(m_tCurrentEffectDesc.tInfo.szEffectMeshName, szName);

	// Update ListBox
	if (m_pCurrentMeshEffect)
	{
		CString ExistingName = {};
		GetDlgItemText(IDC_EFFECTMESH_CURRENT_EFFECT_NAME_RESULT, ExistingName);
		CString strInListBox = {};
		_int iIndex = m_EffectMesh_ListBox.DeleteString(m_EffectMesh_ListBox.FindString(-1, ExistingName));
		m_EffectMesh_ListBox.InsertString(iIndex, EditName);
	}
	SetDlgItemText(IDC_EFFECTMESH_CURRENT_EFFECT_NAME_RESULT, EditName);
	UpdateData(FALSE);
	if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();
}


void CEffectMeshTab::OnBnClickedEffectmeshDuplicateButton()
{
	if (m_pCurrentMeshEffect)
	{
		Safe_Release(m_pCurrentMeshEffect);

		m_tCurrentEffectDesc.tInfo.vPosition.x += 1.f;
		m_tCurrentEffectDesc.tInfo.vPosition.y += 1.f;
		m_tCurrentEffectDesc.tInfo.vPosition.z += 1.f;


		_tcscat_s(m_tCurrentEffectDesc.tInfo.szEffectMeshName, L"_Copy");


		// Check Duplicate
		CString strInListBox = {};
		for (size_t i = 0; i < m_EffectMesh_ListBox.GetCount(); i++)
		{
			m_EffectMesh_ListBox.GetText(i, strInListBox);
			if (!strInListBox.Compare(m_tCurrentEffectDesc.tInfo.szEffectMeshName))
			{
				_tcscat_s(m_tCurrentEffectDesc.tInfo.szEffectMeshName, L"2");
			}
		}







		if (FAILED(m_pGameInstance->Add_GameObjectToLayerWithPtr((_uint)LEVEL::STATIC, TEXT("Prototype_GameObject_ToolEffectMeshObject"), TEXT("Layer_EffectMesh"), &m_pCurrentMeshEffect, &m_tCurrentEffectDesc)))
			assert(false);
		Safe_AddRef(m_pCurrentMeshEffect);
		Add_EffectMesh_ToList(m_pCurrentMeshEffect);
		Update_Tool_ByDesc();

		UpdateData(FALSE);
	}
}


void CEffectMeshTab::OnBnClickedEffectmeshDeleteButton()
{
	if (m_pCurrentMeshEffect)
	{
		// Erase ListBox Name
		CString ExistingName = {};
		GetDlgItemText(IDC_EFFECTMESH_CURRENT_EFFECT_NAME_RESULT, ExistingName);
		CString strInListBox = {};
		m_EffectMesh_ListBox.DeleteString(m_EffectMesh_ListBox.FindString(-1, ExistingName));

		// Init Desc
		ZeroMemory(&m_tCurrentEffectDesc.tFade_InOut, sizeof(EFFECTMESHDESC::FADE_INOUT));
		ZeroMemory(&m_tCurrentEffectDesc.tInfo, sizeof(EFFECTMESHDESC::EFFECTMESHINFO));
		ZeroMemory(&m_tCurrentEffectDesc.tPattern, sizeof(EFFECTMESHDESC::EFFECTMESHPATTERN));
		ZeroMemory(&m_tCurrentEffectDesc.tTexture, sizeof(EFFECTMESHDESC::EFFECTMESHPATTERN));
		ZeroMemory(&m_tCurrentEffectDesc.tUV_Animation, sizeof(EFFECTMESHDESC::UV_ANIMATION));
		ZeroMemory(&m_tCurrentEffectDesc.tUV_Sprite, sizeof(EFFECTMESHDESC::UV_SPRITE));

		// Delete Object from List
		list<CGameObject*>::iterator iter;

		for (iter = m_listEffectMesh.begin(); iter != m_listEffectMesh.end(); ++iter)
		{
			if (*iter == m_pCurrentMeshEffect) break;
		}
		m_listEffectMesh.erase(iter);
		Safe_Release(m_pCurrentMeshEffect);

		m_pCurrentMeshEffect->Set_State(OBJSTATE::DEAD);

		Update_Tool_ByDesc();
	}
}


void CEffectMeshTab::OnEnChangeEffectmeshCreateTimeEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");
	GetDlgItemText(IDC_EFFECTMESH_CREATE_TIME_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;

	SetDlgItemText(IDC_EFFECTMESH_CREATE_TIME_RESULT, strVal);

	m_tEffectGroupDescription.vCreateDeathTime.x = _tstof(strVal);

	UpdateData(FALSE);
}


void CEffectMeshTab::OnEnChangeEffectmeshDeathTimeEdit()
{
	//UpdateData(TRUE);

	CString strVal = _T("");
	GetDlgItemText(IDC_EFFECTMESH_DEATH_TIME_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;

	SetDlgItemText(IDC_EFFECTMESH_DEATH_TIME_RESULT, strVal);

	EffectMeshDescription tTempDesc = {};

	m_tEffectGroupDescription.vCreateDeathTime.y = _tstof(strVal);

	UpdateData(FALSE);
}


void CEffectMeshTab::OnBnClickedEffectmeshCreateDeathRepeat()
{
	UpdateData(TRUE);
	EffectMeshDescription tTempDesc = {};

	if (((CButton*)GetDlgItem(IDC_EFFECTMESH_CREATE_DEATH_REPEAT))->GetCheck())
	{
		m_tEffectGroupDescription.bCreateDeathRepeat = true;
	}
	else
	{
		m_tEffectGroupDescription.bCreateDeathRepeat = false;
	}

	UpdateData(FALSE);
}

void CEffectMeshTab::OnBnClickedEffectmeshPlayButton()
{
	Play();

	CToolView* pToolView = GET_TOOLVIEW;
	pToolView->m_pMainTab->m_pEffectTab->Play();
}


void CEffectMeshTab::OnBnClickedEffectmeshStopButton()
{
	Stop();

	CToolView* pToolView = GET_TOOLVIEW;
	pToolView->m_pMainTab->m_pEffectTab->Stop();
}

void CEffectMeshTab::Play()
{
	for (auto& pIter : m_listEffectMesh)
	{
		dynamic_cast<CToolEffectMeshObject*>(pIter)->Play(m_tEffectGroupDescription.vCreateDeathTime, m_tEffectGroupDescription.bCreateDeathRepeat);
	}
}

void CEffectMeshTab::Stop()
{
	for (auto& pIter : m_listEffectMesh)
	{
		dynamic_cast<CToolEffectMeshObject*>(pIter)->Stop();
	}
	Update_Tool_ByDesc();

	//Invalidate();
}


void CEffectMeshTab::OnEnChangeEffectmeshPatternPositionStartTimeEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");
	GetDlgItemText(IDC_EFFECTMESH_PATTERN_POSITION_START_TIME_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;

	
	m_tCurrentEffectDesc.tPattern.vPattern_Position_StartEndTime.x = _tstof(strVal);

	SetDlgItemText(IDC_EFFECTMESH_PATTERN_POSITION_START_TIME_RESULT, strVal);

	if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();

	UpdateData(FALSE);
}


void CEffectMeshTab::OnEnChangeEffectmeshPatternPositionEndTimeEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");
	GetDlgItemText(IDC_EFFECTMESH_PATTERN_POSITION_END_TIME_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;


	m_tCurrentEffectDesc.tPattern.vPattern_Position_StartEndTime.y = _tstof(strVal);

	SetDlgItemText(IDC_EFFECTMESH_PATTERN_POSITION_END_TIME_RESULT, strVal);

	if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();

	UpdateData(FALSE);
}


void CEffectMeshTab::OnBnClickedEffectmeshPatternPositionRepeatCheck()
{
	UpdateData(TRUE);

	
	if (((CButton*)GetDlgItem(IDC_EFFECTMESH_PATTERN_POSITION_REPEAT_CHECK))->GetCheck())
		m_tCurrentEffectDesc.tPattern.bPattern_Position = true;
	else
		m_tCurrentEffectDesc.tPattern.bPattern_Position = false;

	UpdateData(FALSE);

	if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();
}


void CEffectMeshTab::OnEnChangeEffectmeshPatternPositionXEdit()
{
	UpdateData(TRUE);
	
	CString strVal = _T("");
	
	GetDlgItemText(IDC_EFFECTMESH_PATTERN_POSITION_X_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;


	m_tCurrentEffectDesc.tPattern.vPattern_Position_Goal.x = _tstof(strVal);
	//m_CurrentEffectDesc.tPattern.vPattern_Position_Init.x = _tstof(strVal);

	SetDlgItemText(IDC_EFFECTMESH_PATTERN_POSITION_X_RESULT, strVal);

	EffectMeshDescription tTempDesc = {};
	if (m_bPatternPositionCheckAll)
	{
		for (auto& pIter : m_listEffectMesh)
		{
			CopyDescription(tTempDesc, dynamic_cast<CToolEffectMeshObject*>(pIter)->Get_Desc());
			//tTempDesc.tPattern.vPattern_Position_Init = m_CurrentEffectDesc.tPattern.vPattern_Position_Init;
			tTempDesc.tPattern.vPattern_Position_Goal = m_tCurrentEffectDesc.tPattern.vPattern_Position_Goal;
			dynamic_cast<CToolEffectMeshObject*>(pIter)->Update_Desc(tTempDesc);
		}
	}
	else if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();


	UpdateData(FALSE);
}


void CEffectMeshTab::OnEnChangeEffectmeshPatternPositionYEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");

	GetDlgItemText(IDC_EFFECTMESH_PATTERN_POSITION_Y_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;


	m_tCurrentEffectDesc.tPattern.vPattern_Position_Goal.y = _tstof(strVal);
	//m_CurrentEffectDesc.tPattern.vPattern_Position_Init.y = _tstof(strVal);

	SetDlgItemText(IDC_EFFECTMESH_PATTERN_POSITION_Y_RESULT, strVal);

	EffectMeshDescription tTempDesc = {};
	if (m_bPatternPositionCheckAll)
	{
		for (auto& pIter : m_listEffectMesh)
		{
			CopyDescription(tTempDesc, dynamic_cast<CToolEffectMeshObject*>(pIter)->Get_Desc());
			//tTempDesc.tPattern.vPattern_Position_Init = m_CurrentEffectDesc.tPattern.vPattern_Position_Init;
			tTempDesc.tPattern.vPattern_Position_Goal = m_tCurrentEffectDesc.tPattern.vPattern_Position_Goal;
			dynamic_cast<CToolEffectMeshObject*>(pIter)->Update_Desc(tTempDesc);
		}
	}
	else if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();


	UpdateData(FALSE);
}


void CEffectMeshTab::OnEnChangeEffectmeshPatternPositionZEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");

	GetDlgItemText(IDC_EFFECTMESH_PATTERN_POSITION_Z_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;


	m_tCurrentEffectDesc.tPattern.vPattern_Position_Goal.z = _tstof(strVal);
	//m_CurrentEffectDesc.tPattern.vPattern_Position_Init.z = m_CurrentEffectDesc.tPattern.vPattern_Position_Goal.z;
	SetDlgItemText(IDC_EFFECTMESH_PATTERN_POSITION_Z_RESULT, strVal);


	EffectMeshDescription tTempDesc = {};
	if (m_bPatternPositionCheckAll)
	{
		for (auto& pIter : m_listEffectMesh)
		{
			CopyDescription(tTempDesc, dynamic_cast<CToolEffectMeshObject*>(pIter)->Get_Desc());
			//tTempDesc.tPattern.vPattern_Position_Init = m_CurrentEffectDesc.tPattern.vPattern_Position_Init;
			tTempDesc.tPattern.vPattern_Position_Goal = m_tCurrentEffectDesc.tPattern.vPattern_Position_Goal;
			dynamic_cast<CToolEffectMeshObject*>(pIter)->Update_Desc(tTempDesc);
		}
	}
	else if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();

	UpdateData(FALSE);
}


void CEffectMeshTab::OnDeltaposEffectmeshPatternPositionXSpinCtrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateData(TRUE);

	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	*pResult = 0;


	if (pNMUpDown->iDelta < 0)
		m_tCurrentEffectDesc.tPattern.vPattern_Position_Goal.x += 0.1f;
	else
		m_tCurrentEffectDesc.tPattern.vPattern_Position_Goal.x -= 0.1f;

	CString str = {};
	str.Format(_T("%f"), m_tCurrentEffectDesc.tPattern.vPattern_Position_Goal.x);

	SetDlgItemText(IDC_EFFECTMESH_PATTERN_POSITION_X_EDIT, str);
	SetDlgItemText(IDC_EFFECTMESH_PATTERN_POSITION_X_RESULT, str);

	EffectMeshDescription tTempDesc = {};

	if (m_bPatternPositionCheckAll)
	{
		for (auto& pIter : m_listEffectMesh)
		{
			CopyDescription(tTempDesc, dynamic_cast<CToolEffectMeshObject*>(pIter)->Get_Desc());
			//tTempDesc.tPattern.vPattern_Position_Init = m_CurrentEffectDesc.tPattern.vPattern_Position_Init;
			tTempDesc.tPattern.vPattern_Position_Goal = m_tCurrentEffectDesc.tPattern.vPattern_Position_Goal;
			dynamic_cast<CToolEffectMeshObject*>(pIter)->Update_Desc(tTempDesc);
		}
	}
	else if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();

	UpdateData(FALSE);
}


void CEffectMeshTab::OnDeltaposEffectmeshPatternPositionYSpinCtrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateData(TRUE);

	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	*pResult = 0;


	if (pNMUpDown->iDelta < 0)
		m_tCurrentEffectDesc.tPattern.vPattern_Position_Goal.y += 0.1f;
	else
		m_tCurrentEffectDesc.tPattern.vPattern_Position_Goal.y -= 0.1f;

	CString str = {};
	str.Format(_T("%f"), m_tCurrentEffectDesc.tPattern.vPattern_Position_Goal.y);
	SetDlgItemText(IDC_EFFECTMESH_PATTERN_POSITION_Y_EDIT, str);
	SetDlgItemText(IDC_EFFECTMESH_PATTERN_POSITION_Y_RESULT, str);

	EffectMeshDescription tTempDesc = {};

	if (m_bPatternPositionCheckAll)
	{
		for (auto& pIter : m_listEffectMesh)
		{
			CopyDescription(tTempDesc, dynamic_cast<CToolEffectMeshObject*>(pIter)->Get_Desc());
			//tTempDesc.tPattern.vPattern_Position_Init = m_CurrentEffectDesc.tPattern.vPattern_Position_Init;
			tTempDesc.tPattern.vPattern_Position_Goal = m_tCurrentEffectDesc.tPattern.vPattern_Position_Goal;
			dynamic_cast<CToolEffectMeshObject*>(pIter)->Update_Desc(tTempDesc);
		}
	}
	else if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();

	UpdateData(FALSE);
}


void CEffectMeshTab::OnDeltaposEffectmeshPatternPositionZSpinCtrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateData(TRUE);

	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	*pResult = 0;


	if (pNMUpDown->iDelta < 0)
		m_tCurrentEffectDesc.tPattern.vPattern_Position_Goal.z += 0.1f;
	else
		m_tCurrentEffectDesc.tPattern.vPattern_Position_Goal.z -= 0.1f;

	CString str = {};
	str.Format(_T("%f"), m_tCurrentEffectDesc.tPattern.vPattern_Position_Goal.z);
	SetDlgItemText(IDC_EFFECTMESH_PATTERN_POSITION_Z_EDIT, str);
	SetDlgItemText(IDC_EFFECTMESH_PATTERN_POSITION_Z_RESULT, str);

	EffectMeshDescription tTempDesc = {};

	if (m_bPatternPositionCheckAll)
	{
		for (auto& pIter : m_listEffectMesh)
		{
			CopyDescription(tTempDesc, dynamic_cast<CToolEffectMeshObject*>(pIter)->Get_Desc());
			//tTempDesc.tPattern.vPattern_Position_Init = m_CurrentEffectDesc.tPattern.vPattern_Position_Init;
			tTempDesc.tPattern.vPattern_Position_Goal = m_tCurrentEffectDesc.tPattern.vPattern_Position_Goal;
			dynamic_cast<CToolEffectMeshObject*>(pIter)->Update_Desc(tTempDesc);
		}
	}
	else if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();

	UpdateData(FALSE);
}


void CEffectMeshTab::OnBnClickedEffectmeshPatternPositionSelectAllCheck()
{
	UpdateData(TRUE);

	if (((CButton*)GetDlgItem(IDC_EFFECTMESH_PATTERN_POSITION_REPEAT_CHECK))->GetCheck())
		m_bPatternPositionCheckAll = true;
	else
		m_bPatternPositionCheckAll = false;

	UpdateData(FALSE);
}

void CEffectMeshTab::OnBnClickedEffectmeshPatternPositionProjectileCheck()
{
	UpdateData(TRUE);

	if (((CButton*)GetDlgItem(IDC_EFFECTMESH_PATTERN_POSITION_PROJECTILE_CHECK))->GetCheck())
		m_tCurrentEffectDesc.tPattern.bPattern_Position_Projectile = true;
	else
		m_tCurrentEffectDesc.tPattern.bPattern_Position_Projectile = false;

	UpdateData(FALSE);
}


void CEffectMeshTab::OnEnChangeEffectmeshPatternPositionRandomLengthMinEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");

	GetDlgItemText(IDC_EFFECTMESH_PATTERN_POSITION_RANDOM_LENGTH_MIN_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;

	m_tCurrentEffectDesc.tPattern.fPattern_Position_Projectile_Length_Minimum = _tstof(strVal);
	SetDlgItemText(IDC_EFFECTMESH_PATTERN_POSITION_RANDOM_LENGTH_MIN_RESULT, strVal);


	EffectMeshDescription tTempDesc = {};
	if (m_bPatternPositionCheckAll)
	{
		for (auto& pIter : m_listEffectMesh)
		{
			CopyDescription(tTempDesc, dynamic_cast<CToolEffectMeshObject*>(pIter)->Get_Desc());
			tTempDesc.tPattern.fPattern_Position_Projectile_Length_Minimum = m_tCurrentEffectDesc.tPattern.fPattern_Position_Projectile_Length_Minimum;
			dynamic_cast<CToolEffectMeshObject*>(pIter)->Update_Desc(tTempDesc);
		}
	}
	else if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();

	UpdateData(FALSE);
}

void CEffectMeshTab::OnEnChangeEffectmeshPatternPositionRandomLengthMaxEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");

	GetDlgItemText(IDC_EFFECTMESH_PATTERN_POSITION_RANDOM_LENGTH_MAX_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;

	m_tCurrentEffectDesc.tPattern.fPattern_Position_Projectile_Length_Maximum = _tstof(strVal);
	SetDlgItemText(IDC_EFFECTMESH_PATTERN_POSITION_RANDOM_LENGTH_MAX_RESULT, strVal);


	EffectMeshDescription tTempDesc = {};
	if (m_bPatternPositionCheckAll)
	{
		for (auto& pIter : m_listEffectMesh)
		{
			CopyDescription(tTempDesc, dynamic_cast<CToolEffectMeshObject*>(pIter)->Get_Desc());
			tTempDesc.tPattern.fPattern_Position_Projectile_Length_Maximum = m_tCurrentEffectDesc.tPattern.fPattern_Position_Projectile_Length_Maximum;
			dynamic_cast<CToolEffectMeshObject*>(pIter)->Update_Desc(tTempDesc);
		}
	}
	else if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();

	UpdateData(FALSE);
}

void CEffectMeshTab::OnEnChangeEffectmeshPatternPositionRandomAngleMinEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");

	GetDlgItemText(IDC_EFFECTMESH_PATTERN_POSITION_RANDOM_ANGLE_MIN_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;

	m_tCurrentEffectDesc.tPattern.fPattern_Position_Projectile_Angle_Minimum = _tstof(strVal);
	SetDlgItemText(IDC_EFFECTMESH_PATTERN_POSITION_RANDOM_ANGLE_MIN_RESULT, strVal);


	EffectMeshDescription tTempDesc = {};
	if (m_bPatternPositionCheckAll)
	{
		for (auto& pIter : m_listEffectMesh)
		{
			CopyDescription(tTempDesc, dynamic_cast<CToolEffectMeshObject*>(pIter)->Get_Desc());
			tTempDesc.tPattern.fPattern_Position_Projectile_Angle_Minimum = m_tCurrentEffectDesc.tPattern.fPattern_Position_Projectile_Angle_Minimum;
			dynamic_cast<CToolEffectMeshObject*>(pIter)->Update_Desc(tTempDesc);
		}
	}
	else if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();

	UpdateData(FALSE);
}


void CEffectMeshTab::OnEnChangeEffectmeshPatternPositionRandomAngleMaxEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");

	GetDlgItemText(IDC_EFFECTMESH_PATTERN_POSITION_RANDOM_ANGLE_MAX_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;

	m_tCurrentEffectDesc.tPattern.fPattern_Position_Projectile_Angle_Maximum = _tstof(strVal);
	SetDlgItemText(IDC_EFFECTMESH_PATTERN_POSITION_RANDOM_ANGLE_MAX_RESULT, strVal);
				   

	EffectMeshDescription tTempDesc = {};
	if (m_bPatternPositionCheckAll)
	{
		for (auto& pIter : m_listEffectMesh)
		{
			CopyDescription(tTempDesc, dynamic_cast<CToolEffectMeshObject*>(pIter)->Get_Desc());
			tTempDesc.tPattern.fPattern_Position_Projectile_Angle_Maximum = m_tCurrentEffectDesc.tPattern.fPattern_Position_Projectile_Angle_Maximum;
			dynamic_cast<CToolEffectMeshObject*>(pIter)->Update_Desc(tTempDesc);
		}
	}
	else if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();

	UpdateData(FALSE);
}

void CEffectMeshTab::OnBnClickedMesheffectPatternPositionProjectileRandomLengthAngleCaculateButton()
{
	if (m_tCurrentEffectDesc.tPattern.fPattern_Position_Projectile_Angle_Minimum > m_tCurrentEffectDesc.tPattern.fPattern_Position_Projectile_Angle_Maximum)
		return;
	if (m_tCurrentEffectDesc.tPattern.fPattern_Position_Projectile_Length_Minimum > m_tCurrentEffectDesc.tPattern.fPattern_Position_Projectile_Length_Maximum)
		return;

	_int iRandom = 0;
	//m_pGameInstance->Random_Range(static_cast<_int>(m_tCurrentEffectDesc.tPattern.fPattern_Position_Projectile_Angle_Minimum * 100)
	//	, static_cast<_int>(m_tCurrentEffectDesc.tPattern.fPattern_Position_Projectile_Angle_Maximum * 100));
	//iRandom = m_pGameInstance->Random();
	iRandom = m_pGameInstance->Random_From_10000(static_cast<_int>(m_tCurrentEffectDesc.tPattern.fPattern_Position_Projectile_Angle_Minimum * 100),
		m_tCurrentEffectDesc.tPattern.fPattern_Position_Projectile_Angle_Maximum * 100);
	m_tCurrentEffectDesc.tPattern.fPattern_Position_Projectile_Angle_Result = static_cast<_float>(iRandom) * 0.01f;

	//m_pGameInstance->Random_Range(static_cast<_int>(m_tCurrentEffectDesc.tPattern.fPattern_Position_Projectile_Length_Minimum * 100)
	//	, static_cast<_int>(m_tCurrentEffectDesc.tPattern.fPattern_Position_Projectile_Length_Maximum * 100));
	//iRandom = m_pGameInstance->Random();
	iRandom = m_pGameInstance->Random_From_10000(static_cast<_int>(m_tCurrentEffectDesc.tPattern.fPattern_Position_Projectile_Length_Minimum * 100),
		m_tCurrentEffectDesc.tPattern.fPattern_Position_Projectile_Length_Maximum * 100);
	m_tCurrentEffectDesc.tPattern.fPattern_Position_Projectile_Length_Result = static_cast<_float>(iRandom) * 0.01f;


	m_pGameInstance->Random_Range(-1 * static_cast<_int>(m_tCurrentEffectDesc.tPattern.fPattern_Position_Projectile_Length_Result * 100), static_cast<_int>(m_tCurrentEffectDesc.tPattern.fPattern_Position_Projectile_Length_Result * 100));
	_int iRandomX = m_pGameInstance->Random();
	_float fRandomX = static_cast<_float>(iRandomX) * 0.01f;

	_float fC = 0.f;

	// Quadratic formula with Equation of Circle
	fC = m_tCurrentEffectDesc.tPattern.fPattern_Position_Projectile_Length_Result * m_tCurrentEffectDesc.tPattern.fPattern_Position_Projectile_Length_Result
		- (fRandomX * fRandomX)
		+ 2 * m_tCurrentEffectDesc.tInfo.vPosition.x * fRandomX
		- m_tCurrentEffectDesc.tInfo.vPosition.x * m_tCurrentEffectDesc.tInfo.vPosition.x
		- m_tCurrentEffectDesc.tInfo.vPosition.z * m_tCurrentEffectDesc.tInfo.vPosition.z;
	fC *= -1.f;

	_float fResultZ1
		= (2 * m_tCurrentEffectDesc.tInfo.vPosition.z
			+ sqrt(4 * m_tCurrentEffectDesc.tInfo.vPosition.z * m_tCurrentEffectDesc.tInfo.vPosition.z
				+ 4 * (m_tCurrentEffectDesc.tPattern.fPattern_Position_Projectile_Length_Result * m_tCurrentEffectDesc.tPattern.fPattern_Position_Projectile_Length_Result
					- fRandomX * fRandomX + 2 * m_tCurrentEffectDesc.tInfo.vPosition.x
					- m_tCurrentEffectDesc.tInfo.vPosition.x * m_tCurrentEffectDesc.tInfo.vPosition.x
					- m_tCurrentEffectDesc.tInfo.vPosition.z * m_tCurrentEffectDesc.tInfo.vPosition.z))) * 0.5f;

	_float fResultZ2
		= (2 * m_tCurrentEffectDesc.tInfo.vPosition.z
			- sqrt(4 * m_tCurrentEffectDesc.tInfo.vPosition.z * m_tCurrentEffectDesc.tInfo.vPosition.z
				+ 4 * (m_tCurrentEffectDesc.tPattern.fPattern_Position_Projectile_Length_Result * m_tCurrentEffectDesc.tPattern.fPattern_Position_Projectile_Length_Result
					- fRandomX * fRandomX + 2 * m_tCurrentEffectDesc.tInfo.vPosition.x
					- m_tCurrentEffectDesc.tInfo.vPosition.x * m_tCurrentEffectDesc.tInfo.vPosition.x
					- m_tCurrentEffectDesc.tInfo.vPosition.z * m_tCurrentEffectDesc.tInfo.vPosition.z))) * 0.5f;

	m_pGameInstance->Random_Range(0, static_cast<_int>(m_tCurrentEffectDesc.tPattern.fPattern_Position_Projectile_Angle_Maximum));

	m_tCurrentEffectDesc.tPattern.vPattern_Position_Goal.x = fRandomX;
	if (m_pGameInstance->Random() % 2)
		m_tCurrentEffectDesc.tPattern.vPattern_Position_Goal.z = fResultZ1;
	else
		m_tCurrentEffectDesc.tPattern.vPattern_Position_Goal.z = fResultZ2;


	_float4 vStartPos = { m_tCurrentEffectDesc.tInfo.vPosition.x, m_tCurrentEffectDesc.tInfo.vPosition.y, m_tCurrentEffectDesc.tInfo.vPosition.z, 1.f };
	_float4 vTargetPos = { m_tCurrentEffectDesc.tPattern.vPattern_Position_Goal.x, m_tCurrentEffectDesc.tPattern.vPattern_Position_Goal.y, m_tCurrentEffectDesc.tPattern.vPattern_Position_Goal.z, 1.f };
	m_tCurrentEffectDesc.tPattern.vPattern_Position_Projectile_Power
		= dynamic_cast<CToolEffectMeshObject*>(m_pCurrentMeshEffect)->Get_ProjectilePowerToTarget(vStartPos, vTargetPos
		, m_tCurrentEffectDesc.tPattern.vPattern_Position_StartEndTime.y - m_tCurrentEffectDesc.tPattern.vPattern_Position_StartEndTime.x
		, m_tCurrentEffectDesc.tPattern.fPattern_Position_Projectile_Angle_Result);


	CString strVal;
	strVal.Format(_T("%f"), m_tCurrentEffectDesc.tPattern.fPattern_Position_Projectile_Length_Result);
	SetDlgItemText(IDC_EFFECTMESH_PATTERN_POSITION_RANDOM_LENGTH_RESULT, strVal);
	strVal.Format(_T("%f"), m_tCurrentEffectDesc.tPattern.fPattern_Position_Projectile_Angle_Result);
	SetDlgItemText(IDC_EFFECTMESH_PATTERN_POSITION_RANDOM_ANGLE_RESULT, strVal);

	strVal.Format(_T("%f"), m_tCurrentEffectDesc.tPattern.vPattern_Position_Goal.x);
	SetDlgItemText(IDC_EFFECTMESH_PATTERN_POSITION_X_RESULT, strVal);
	strVal.Format(_T("%f"), m_tCurrentEffectDesc.tPattern.vPattern_Position_Goal.z);
	SetDlgItemText(IDC_EFFECTMESH_PATTERN_POSITION_Z_RESULT, strVal);

	if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();
}


void CEffectMeshTab::OnEnChangeEffectmeshPatternRotationStartTimeEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");
	GetDlgItemText(IDC_EFFECTMESH_PATTERN_ROTATION_START_TIME_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;


	m_tCurrentEffectDesc.tPattern.vPattern_Rotation_StartEndTime.x = _tstof(strVal);

	SetDlgItemText(IDC_EFFECTMESH_PATTERN_ROTATION_START_TIME_RESULT, strVal);

	if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();

	UpdateData(FALSE);
}


void CEffectMeshTab::OnEnChangeEffectmeshPatternRotationEndTimeEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");
	GetDlgItemText(IDC_EFFECTMESH_PATTERN_ROTATION_END_TIME_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;


	m_tCurrentEffectDesc.tPattern.vPattern_Rotation_StartEndTime.y = _tstof(strVal);

	SetDlgItemText(IDC_EFFECTMESH_PATTERN_ROTATION_END_TIME_RESULT, strVal);

	if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();

	UpdateData(FALSE);
}


void CEffectMeshTab::OnBnClickedEffectmeshPatternRotationRepeatCheck()
{
	UpdateData(TRUE);


	if (((CButton*)GetDlgItem(IDC_EFFECTMESH_PATTERN_ROTATION_REPEAT_CHECK))->GetCheck())
		m_tCurrentEffectDesc.tPattern.bPattern_Scale = true;
	else
		m_tCurrentEffectDesc.tPattern.bPattern_Scale = false;

	UpdateData(FALSE);

	if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();
}


void CEffectMeshTab::OnEnChangeEffectmeshPatternRotationXEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");

	GetDlgItemText(IDC_EFFECTMESH_PATTERN_ROTATION_X_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;


	m_tCurrentEffectDesc.tPattern.vPattern_Rotation_Goal.x = _tstof(strVal);

	SetDlgItemText(IDC_EFFECTMESH_PATTERN_ROTATION_X_RESULT, strVal);

	EffectMeshDescription tTempDesc = {};
	if (m_bPatternRotationCheckAll)
	{
		for (auto& pIter : m_listEffectMesh)
		{
			CopyDescription(tTempDesc, dynamic_cast<CToolEffectMeshObject*>(pIter)->Get_Desc());
			//tTempDesc.tPattern.vPattern_Rotation_Init = m_CurrentEffectDesc.tPattern.vPattern_Rotation_Init;
			tTempDesc.tPattern.vPattern_Rotation_Goal = m_tCurrentEffectDesc.tPattern.vPattern_Rotation_Goal;
			dynamic_cast<CToolEffectMeshObject*>(pIter)->Update_Desc(tTempDesc);
		}
	}
	else if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();


	UpdateData(FALSE);
}


void CEffectMeshTab::OnEnChangeEffectmeshPatternRotationYEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");

	GetDlgItemText(IDC_EFFECTMESH_PATTERN_ROTATION_Y_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;


	m_tCurrentEffectDesc.tPattern.vPattern_Rotation_Goal.y = _tstof(strVal);

	SetDlgItemText(IDC_EFFECTMESH_PATTERN_ROTATION_Y_RESULT, strVal);

	EffectMeshDescription tTempDesc = {};
	if (m_bPatternRotationCheckAll)
	{
		for (auto& pIter : m_listEffectMesh)
		{
			CopyDescription(tTempDesc, dynamic_cast<CToolEffectMeshObject*>(pIter)->Get_Desc());
			//tTempDesc.tPattern.vPattern_Rotation_Init = m_CurrentEffectDesc.tPattern.vPattern_Rotation_Init;
			tTempDesc.tPattern.vPattern_Rotation_Goal = m_tCurrentEffectDesc.tPattern.vPattern_Rotation_Goal;
			dynamic_cast<CToolEffectMeshObject*>(pIter)->Update_Desc(tTempDesc);
		}
	}
	else if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();


	UpdateData(FALSE);
}


void CEffectMeshTab::OnEnChangeEffectmeshPatternRotationZEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");

	GetDlgItemText(IDC_EFFECTMESH_PATTERN_ROTATION_Z_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;


	m_tCurrentEffectDesc.tPattern.vPattern_Rotation_Goal.z = _tstof(strVal);

	SetDlgItemText(IDC_EFFECTMESH_PATTERN_ROTATION_Z_RESULT, strVal);

	EffectMeshDescription tTempDesc = {};
	if (m_bPatternRotationCheckAll)
	{
		for (auto& pIter : m_listEffectMesh)
		{
			CopyDescription(tTempDesc, dynamic_cast<CToolEffectMeshObject*>(pIter)->Get_Desc());
			//tTempDesc.tPattern.vPattern_Rotation_Init = m_CurrentEffectDesc.tPattern.vPattern_Rotation_Init;
			tTempDesc.tPattern.vPattern_Rotation_Goal = m_tCurrentEffectDesc.tPattern.vPattern_Rotation_Goal;
			dynamic_cast<CToolEffectMeshObject*>(pIter)->Update_Desc(tTempDesc);
		}
	}
	else if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();


	UpdateData(FALSE);
}


void CEffectMeshTab::OnDeltaposEffectmeshPatternRotationXSpinCtrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateData(TRUE);

	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	*pResult = 0;


	if (pNMUpDown->iDelta < 0)
		m_tCurrentEffectDesc.tPattern.vPattern_Rotation_Goal.x += 0.1f;
	else
		m_tCurrentEffectDesc.tPattern.vPattern_Rotation_Goal.x -= 0.1f;

	CString str = {};
	str.Format(_T("%f"), m_tCurrentEffectDesc.tPattern.vPattern_Rotation_Goal.x);

	SetDlgItemText(IDC_EFFECTMESH_PATTERN_ROTATION_X_EDIT, str);
	SetDlgItemText(IDC_EFFECTMESH_PATTERN_ROTATION_X_RESULT, str);
//	m_CurrentEffectDesc.tPattern.vPattern_Rotation_Init.x = m_CurrentEffectDesc.tPattern.vPattern_Rotation_Goal.x;

	EffectMeshDescription tTempDesc = {};

	if (m_bPatternRotationCheckAll)
	{
		for (auto& pIter : m_listEffectMesh)
		{
			CopyDescription(tTempDesc, dynamic_cast<CToolEffectMeshObject*>(pIter)->Get_Desc());
			//tTempDesc.tPattern.vPattern_Rotation_Init = m_CurrentEffectDesc.tPattern.vPattern_Rotation_Init;
			tTempDesc.tPattern.vPattern_Rotation_Goal = m_tCurrentEffectDesc.tPattern.vPattern_Rotation_Goal;
			dynamic_cast<CToolEffectMeshObject*>(pIter)->Update_Desc(tTempDesc);
		}
	}
	else if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();

	UpdateData(FALSE);
}


void CEffectMeshTab::OnDeltaposEffectmeshPatternRotationYSpinCtrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateData(TRUE);

	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	*pResult = 0;


	if (pNMUpDown->iDelta < 0)
		m_tCurrentEffectDesc.tPattern.vPattern_Rotation_Goal.y += 0.1f;
	else
		m_tCurrentEffectDesc.tPattern.vPattern_Rotation_Goal.y -= 0.1f;

	CString str = {};
	str.Format(_T("%f"), m_tCurrentEffectDesc.tPattern.vPattern_Rotation_Goal.y);

	SetDlgItemText(IDC_EFFECTMESH_PATTERN_ROTATION_Y_EDIT, str);
	SetDlgItemText(IDC_EFFECTMESH_PATTERN_ROTATION_Y_RESULT, str);
	//m_CurrentEffectDesc.tPattern.vPattern_Rotation_Init.y = m_CurrentEffectDesc.tPattern.vPattern_Rotation_Goal.y;

	EffectMeshDescription tTempDesc = {};

	if (m_bPatternRotationCheckAll)
	{
		for (auto& pIter : m_listEffectMesh)
		{
			CopyDescription(tTempDesc, dynamic_cast<CToolEffectMeshObject*>(pIter)->Get_Desc());
			//tTempDesc.tPattern.vPattern_Rotation_Init = m_CurrentEffectDesc.tPattern.vPattern_Rotation_Init;
			tTempDesc.tPattern.vPattern_Rotation_Goal = m_tCurrentEffectDesc.tPattern.vPattern_Rotation_Goal;
			dynamic_cast<CToolEffectMeshObject*>(pIter)->Update_Desc(tTempDesc);
		}
	}
	else if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();

	UpdateData(FALSE);
}


void CEffectMeshTab::OnDeltaposEffectmeshPatternRotationZSpinCtrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateData(TRUE);

	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	*pResult = 0;


	if (pNMUpDown->iDelta < 0)
		m_tCurrentEffectDesc.tPattern.vPattern_Rotation_Goal.z += 0.1f;
	else
		m_tCurrentEffectDesc.tPattern.vPattern_Rotation_Goal.z -= 0.1f;

	CString str = {};
	str.Format(_T("%f"), m_tCurrentEffectDesc.tPattern.vPattern_Rotation_Goal.z);

	SetDlgItemText(IDC_EFFECTMESH_PATTERN_ROTATION_Z_EDIT, str);
	SetDlgItemText(IDC_EFFECTMESH_PATTERN_ROTATION_Z_RESULT, str);
	//m_CurrentEffectDesc.tPattern.vPattern_Rotation_Init.z = m_CurrentEffectDesc.tPattern.vPattern_Rotation_Goal.z;

	EffectMeshDescription tTempDesc = {};

	if (m_bPatternRotationCheckAll)
	{
		for (auto& pIter : m_listEffectMesh)
		{
			CopyDescription(tTempDesc, dynamic_cast<CToolEffectMeshObject*>(pIter)->Get_Desc());
			//tTempDesc.tPattern.vPattern_Rotation_Init = m_CurrentEffectDesc.tPattern.vPattern_Rotation_Init;
			tTempDesc.tPattern.vPattern_Rotation_Goal = m_tCurrentEffectDesc.tPattern.vPattern_Rotation_Goal;
			dynamic_cast<CToolEffectMeshObject*>(pIter)->Update_Desc(tTempDesc);
		}
	}
	else if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();

	UpdateData(FALSE);
}


void CEffectMeshTab::OnBnClickedEffectmeshPatternRotationSelectAllCheck()
{
	UpdateData(TRUE);

	if (((CButton*)GetDlgItem(IDC_EFFECTMESH_PATTERN_ROTATION_REPEAT_CHECK))->GetCheck())
		m_bPatternRotationCheckAll = true;
	else
		m_bPatternRotationCheckAll = false;

	UpdateData(FALSE);
}


void CEffectMeshTab::OnEnChangeEffectmeshPatternScaleStartTimeEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");
	GetDlgItemText(IDC_EFFECTMESH_PATTERN_SCALE_START_TIME_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;


	m_tCurrentEffectDesc.tPattern.vPattern_Scale_StartEndTime.x = _tstof(strVal);

	SetDlgItemText(IDC_EFFECTMESH_PATTERN_SCALE_START_TIME_RESULT, strVal);

	if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();

	UpdateData(FALSE);
}


void CEffectMeshTab::OnEnChangeEffectmeshPatternScaleEndTimeEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");
	GetDlgItemText(IDC_EFFECTMESH_PATTERN_SCALE_END_TIME_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;

	m_tCurrentEffectDesc.tPattern.vPattern_Scale_StartEndTime.y = _tstof(strVal);

	SetDlgItemText(IDC_EFFECTMESH_PATTERN_SCALE_END_TIME_RESULT, strVal);

	if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();

	UpdateData(FALSE);
}


void CEffectMeshTab::OnBnClickedEffectmeshPatternScaleRepeatCheck()
{
	UpdateData(TRUE);


	if (((CButton*)GetDlgItem(IDC_EFFECTMESH_PATTERN_SCALE_REPEAT_CHECK))->GetCheck())
		m_tCurrentEffectDesc.tPattern.bPattern_Scale = true;
	else
		m_tCurrentEffectDesc.tPattern.bPattern_Scale = false;

	UpdateData(FALSE);

	if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();
}


void CEffectMeshTab::OnEnChangeEffectmeshPatternScaleXEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");

	GetDlgItemText(IDC_EFFECTMESH_PATTERN_SCALE_X_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;


	m_tCurrentEffectDesc.tPattern.vPattern_Scale_Goal.x = _tstof(strVal);

	SetDlgItemText(IDC_EFFECTMESH_PATTERN_SCALE_X_RESULT, strVal);

	EffectMeshDescription tTempDesc = {};
	if (m_bPatternScaleCheckAll)
	{
		for (auto& pIter : m_listEffectMesh)
		{
			CopyDescription(tTempDesc, dynamic_cast<CToolEffectMeshObject*>(pIter)->Get_Desc());
			//tTempDesc.tPattern.vPattern_Scale_Init = m_CurrentEffectDesc.tPattern.vPattern_Scale_Init;
			tTempDesc.tPattern.vPattern_Scale_Goal = m_tCurrentEffectDesc.tPattern.vPattern_Scale_Goal;
			dynamic_cast<CToolEffectMeshObject*>(pIter)->Update_Desc(tTempDesc);
		}
	}
	else if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();


	UpdateData(FALSE);
}


void CEffectMeshTab::OnEnChangeEffectmeshPatternScaleYEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");

	GetDlgItemText(IDC_EFFECTMESH_PATTERN_SCALE_Y_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;


	m_tCurrentEffectDesc.tPattern.vPattern_Scale_Goal.y = _tstof(strVal);

	SetDlgItemText(IDC_EFFECTMESH_PATTERN_SCALE_Y_RESULT, strVal);

	EffectMeshDescription tTempDesc = {};
	if (m_bPatternScaleCheckAll)
	{
		for (auto& pIter : m_listEffectMesh)
		{
			CopyDescription(tTempDesc, dynamic_cast<CToolEffectMeshObject*>(pIter)->Get_Desc());
			//tTempDesc.tPattern.vPattern_Scale_Init = m_CurrentEffectDesc.tPattern.vPattern_Scale_Init;
			tTempDesc.tPattern.vPattern_Scale_Goal = m_tCurrentEffectDesc.tPattern.vPattern_Scale_Goal;
			dynamic_cast<CToolEffectMeshObject*>(pIter)->Update_Desc(tTempDesc);
		}
	}
	else if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();


	UpdateData(FALSE);
}


void CEffectMeshTab::OnEnChangeEffectmeshPatternScaleZEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");

	GetDlgItemText(IDC_EFFECTMESH_PATTERN_SCALE_Z_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;


	m_tCurrentEffectDesc.tPattern.vPattern_Scale_Goal.z = _tstof(strVal);

	SetDlgItemText(IDC_EFFECTMESH_PATTERN_SCALE_Z_RESULT, strVal);

	EffectMeshDescription tTempDesc = {};
	if (m_bPatternScaleCheckAll)
	{
		for (auto& pIter : m_listEffectMesh)
		{
			CopyDescription(tTempDesc, dynamic_cast<CToolEffectMeshObject*>(pIter)->Get_Desc());
			//tTempDesc.tPattern.vPattern_Scale_Init = m_CurrentEffectDesc.tPattern.vPattern_Scale_Init;
			tTempDesc.tPattern.vPattern_Scale_Goal = m_tCurrentEffectDesc.tPattern.vPattern_Scale_Goal;
			dynamic_cast<CToolEffectMeshObject*>(pIter)->Update_Desc(tTempDesc);
		}
	}
	else if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();


	UpdateData(FALSE);
}


void CEffectMeshTab::OnDeltaposEffectmeshPatternScaleXSpinCtrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateData(TRUE);

	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	*pResult = 0;


	if (pNMUpDown->iDelta < 0)
		m_tCurrentEffectDesc.tPattern.vPattern_Scale_Goal.x += 0.1f;
	else
		m_tCurrentEffectDesc.tPattern.vPattern_Scale_Goal.x -= 0.1f;

	CString str = {};
	str.Format(_T("%f"), m_tCurrentEffectDesc.tPattern.vPattern_Scale_Goal.x);

	SetDlgItemText(IDC_EFFECTMESH_PATTERN_SCALE_X_EDIT, str);
	SetDlgItemText(IDC_EFFECTMESH_PATTERN_SCALE_X_RESULT, str);
	//m_CurrentEffectDesc.tPattern.vPattern_Scale_Init.x = m_CurrentEffectDesc.tPattern.vPattern_Scale_Goal.x;

	EffectMeshDescription tTempDesc = {};

	if (m_bPatternScaleCheckAll)
	{
		for (auto& pIter : m_listEffectMesh)
		{
			CopyDescription(tTempDesc, dynamic_cast<CToolEffectMeshObject*>(pIter)->Get_Desc());
			//tTempDesc.tPattern.vPattern_Scale_Init = m_CurrentEffectDesc.tPattern.vPattern_Scale_Init;
			tTempDesc.tPattern.vPattern_Scale_Goal = m_tCurrentEffectDesc.tPattern.vPattern_Scale_Goal;
			dynamic_cast<CToolEffectMeshObject*>(pIter)->Update_Desc(tTempDesc);
		}
	}
	else if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();

	UpdateData(FALSE);
}


void CEffectMeshTab::OnDeltaposEffectmeshPatternScaleYSpinCtrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateData(TRUE);

	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	*pResult = 0;


	if (pNMUpDown->iDelta < 0)
		m_tCurrentEffectDesc.tPattern.vPattern_Scale_Goal.y += 0.1f;
	else
		m_tCurrentEffectDesc.tPattern.vPattern_Scale_Goal.y -= 0.1f;

	CString str = {};
	str.Format(_T("%f"), m_tCurrentEffectDesc.tPattern.vPattern_Scale_Goal.y);

	SetDlgItemText(IDC_EFFECTMESH_PATTERN_SCALE_Y_EDIT, str);
	SetDlgItemText(IDC_EFFECTMESH_PATTERN_SCALE_Y_RESULT, str);
	//m_CurrentEffectDesc.tPattern.vPattern_Scale_Init.y = m_CurrentEffectDesc.tPattern.vPattern_Scale_Goal.y;

	EffectMeshDescription tTempDesc = {};

	if (m_bPatternScaleCheckAll)
	{
		for (auto& pIter : m_listEffectMesh)
		{
			CopyDescription(tTempDesc, dynamic_cast<CToolEffectMeshObject*>(pIter)->Get_Desc());
			//tTempDesc.tPattern.vPattern_Scale_Init = m_CurrentEffectDesc.tPattern.vPattern_Scale_Init;
			tTempDesc.tPattern.vPattern_Scale_Goal = m_tCurrentEffectDesc.tPattern.vPattern_Scale_Goal;
			dynamic_cast<CToolEffectMeshObject*>(pIter)->Update_Desc(tTempDesc);
		}
	}
	else if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();

	UpdateData(FALSE);
}


void CEffectMeshTab::OnDeltaposEffectmeshPatternScaleZSpinCtrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateData(TRUE);

	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	*pResult = 0;


	if (pNMUpDown->iDelta < 0)
		m_tCurrentEffectDesc.tPattern.vPattern_Scale_Goal.z += 0.1f;
	else
		m_tCurrentEffectDesc.tPattern.vPattern_Scale_Goal.z -= 0.1f;

	CString str = {};
	str.Format(_T("%f"), m_tCurrentEffectDesc.tPattern.vPattern_Scale_Goal.z);

	SetDlgItemText(IDC_EFFECTMESH_PATTERN_SCALE_Z_EDIT, str);
	SetDlgItemText(IDC_EFFECTMESH_PATTERN_SCALE_Z_RESULT, str);
	//m_CurrentEffectDesc.tPattern.vPattern_Scale_Init.z = m_CurrentEffectDesc.tPattern.vPattern_Scale_Goal.x;

	EffectMeshDescription tTempDesc = {};

	if (m_bPatternScaleCheckAll)
	{
		for (auto& pIter : m_listEffectMesh)
		{
			CopyDescription(tTempDesc, dynamic_cast<CToolEffectMeshObject*>(pIter)->Get_Desc());
			//tTempDesc.tPattern.vPattern_Scale_Init = m_CurrentEffectDesc.tPattern.vPattern_Scale_Init;
			tTempDesc.tPattern.vPattern_Scale_Goal = m_tCurrentEffectDesc.tPattern.vPattern_Scale_Goal;
			dynamic_cast<CToolEffectMeshObject*>(pIter)->Update_Desc(tTempDesc);
		}
	}
	else if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();

	UpdateData(FALSE);
}


void CEffectMeshTab::OnBnClickedEffectmeshPatternScaleSelectAllCheck()
{
	UpdateData(TRUE);

	if (((CButton*)GetDlgItem(IDC_EFFECTMESH_PATTERN_SCALE_REPEAT_CHECK))->GetCheck())
		m_bPatternScaleCheckAll = true;
	else
		m_bPatternScaleCheckAll = false;

	UpdateData(FALSE);
}


void CEffectMeshTab::OnBnClickedEffectmeshSaveButton()
{
	Save();
}


void CEffectMeshTab::OnBnClickedEffectmeshLoadButton()
{
	Load();
}

void CEffectMeshTab::Save()
{
	CFileDialog Dlg(FALSE, L"dat", L"*.dat");
	TCHAR szBuf[MAX_PATH]{};
	GetCurrentDirectory(MAX_PATH, szBuf);
	PathRemoveFileSpec(szBuf);
	lstrcat(szBuf, L"\\Client\\Bin\\Data\\EffectSaveData\\");
	Dlg.m_ofn.lpstrInitialDir = szBuf;

	CToolView* pToolView = GET_TOOLVIEW;



	for (auto& pIter : m_tEffectGroupDescription.listEffectMeshDesc)
	{
		_int iVecSize = pIter->tEffectMeshShader.vecShaderName.size();

		for (_int i = 0; i < iVecSize; ++i)
			Safe_Delete(pIter->tEffectMeshShader.vecShaderName[i]);

		pIter->tEffectMeshShader.vecShaderName.clear();
	}

	for (auto& pIter : m_tEffectGroupDescription.listEffectMeshDesc)
	{
		//Safe_Delete(pIter);
		delete pIter;
	}
	m_tEffectGroupDescription.listEffectMeshDesc.clear();
	for (auto& pIter : m_listEffectMesh)
	{
		EFFECTMESHDESC* pDesc = new EFFECTMESHDESC;
		CopyDescription((*pDesc), dynamic_cast<CToolEffectMeshObject*>(pIter)->Get_Desc());
		m_tEffectGroupDescription.listEffectMeshDesc.emplace_back(pDesc);
	}

	

	if (IDOK == Dlg.DoModal())
	{
		CString wstrPath = Dlg.GetPathName();

		HANDLE hFile = CreateFile(wstrPath.GetString(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (INVALID_HANDLE_VALUE == hFile)
			return;

		_ulong	dwByte = 0;

		_int iSize = m_vecTexProtoPath.size();
		WriteFile(hFile, &iSize, sizeof(_int), &dwByte, nullptr);

		for (auto& pIter : m_vecTexProtoPath)
		{
			WriteFile(hFile, dynamic_cast<TEXNAMEPROTOPATH*>(pIter)->szName, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
			WriteFile(hFile, dynamic_cast<TEXNAMEPROTOPATH*>(pIter)->szPath, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
			WriteFile(hFile, dynamic_cast<TEXNAMEPROTOPATH*>(pIter)->szProto, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
			WriteFile(hFile, &dynamic_cast<TEXNAMEPROTOPATH*>(pIter)->iTextureNumber, sizeof(_int), &dwByte, nullptr);
		}

		iSize = m_vecModelPathName.size();
		WriteFile(hFile, &iSize, sizeof(_int), &dwByte, nullptr);

		for (auto& pIter : m_vecModelPathName)
		{
			WriteFile(hFile, dynamic_cast<MODELPATHNAME*>(pIter)->szName, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
			WriteFile(hFile, dynamic_cast<MODELPATHNAME*>(pIter)->szPath, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
			WriteFile(hFile, dynamic_cast<MODELPATHNAME*>(pIter)->szProto, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
		}


		m_tEffectGroupDescription.szGroupName;
		WriteFile(hFile, &m_tEffectGroupDescription.szGroupName, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);

		iSize = m_tEffectGroupDescription.listEffectMeshDesc.size();
		WriteFile(hFile, &iSize, sizeof(_int), &dwByte, nullptr);

		for (auto& pIter : m_tEffectGroupDescription.listEffectMeshDesc)
		{
			WriteFile(hFile, &pIter->tInfo, sizeof(EFFECTMESHDESC::EffectMeshInfo), &dwByte, nullptr);
			WriteFile(hFile, &pIter->tTexture, sizeof(EFFECTMESHDESC::EFFECTMESHTEXTURE), &dwByte, nullptr);
			WriteFile(hFile, &pIter->tPattern, sizeof(EFFECTMESHDESC::EffectMeshPattern), &dwByte, nullptr);

			WriteFile(hFile, &pIter->tFade_InOut, sizeof(EFFECTMESHDESC::Fade_InOut), &dwByte, nullptr);

			WriteFile(hFile, &pIter->tUV_Sprite, sizeof(EFFECTMESHDESC::UV_SPRITE), &dwByte, nullptr);
			WriteFile(hFile, &pIter->tUV_Animation, sizeof(EFFECTMESHDESC::UV_ANIMATION), &dwByte, nullptr);

			_int iSize = pIter->tEffectMeshShader.vecShaderName.size();
			WriteFile(hFile, &iSize, sizeof(_int), &dwByte, nullptr);
			for (auto& pShaderNameIter : pIter->tEffectMeshShader.vecShaderName)
				WriteFile(hFile, &pShaderNameIter->szName, sizeof(TCHAR) * MAX_PATH, &dwByte, nullptr);
			WriteFile(hFile, &pIter->tEffectMeshShader.fDissolveSpeed, sizeof(_float), &dwByte, nullptr);
		}

		WriteFile(hFile, &m_tEffectGroupDescription.bCreateDeathRepeat, sizeof(_bool), &dwByte, nullptr);
		WriteFile(hFile, &m_tEffectGroupDescription.vCreateDeathTime, sizeof(_float2), &dwByte, nullptr);

		_int iParticleSize = pToolView->m_pMainTab->m_pEffectTab->Get_GroupDesc().listParticleDesc.size();
		WriteFile(hFile, &iParticleSize, sizeof(_int), &dwByte, nullptr);

		for (auto& pIter : pToolView->m_pMainTab->m_pEffectTab->Get_GroupDesc().listParticleDesc)
		{
			//WriteFile(hFile, pIter, sizeof(PARTICLEDESC), &dwByte, nullptr);

			WriteFile(hFile, &pIter->szParticleName, sizeof(TCHAR) * MAX_PATH, &dwByte, nullptr);
			WriteFile(hFile, &pIter->vPosition, sizeof(_float3), &dwByte, nullptr);
			WriteFile(hFile, &pIter->vScale, sizeof(_float3), &dwByte, nullptr);
			WriteFile(hFile, &pIter->vRotation, sizeof(_float3), &dwByte, nullptr);
			WriteFile(hFile, &pIter->iTextureNumber, sizeof(_int), &dwByte, nullptr);
			WriteFile(hFile, &pIter->szTextureName, sizeof(TCHAR) * MAX_PATH, &dwByte, nullptr);
			WriteFile(hFile, &pIter->szTextureProto, sizeof(TCHAR) * MAX_PATH, &dwByte, nullptr);
			WriteFile(hFile, &pIter->szTexturePath, sizeof(TCHAR) * MAX_PATH, &dwByte, nullptr);
			WriteFile(hFile, &pIter->fMultiTextureInterval, sizeof(_float), &dwByte, nullptr);
			WriteFile(hFile, &pIter->bMultiTextureRepeat, sizeof(_bool), &dwByte, nullptr);

			_uint uiCount = pIter->tParticleShaderDesc.vecShaderName.size();
			_int iCount = static_cast<_int>(uiCount);
			WriteFile(hFile, &iCount, sizeof(_int), &dwByte, nullptr);
			for (_int i = 0; i < iCount; ++i)
			{ 
				WriteFile(hFile, &pIter->tParticleShaderDesc.vecShaderName[i]->szName, sizeof(TCHAR) * MAX_PATH, &dwByte, nullptr);
			}
			WriteFile(hFile, &pIter->tPointInst_Desc, sizeof(POINTINST_DESC), &dwByte, nullptr);
		}



		CloseHandle(hFile);
	}
}

void CEffectMeshTab::Load()
{
	UpdateData(TRUE);

	// Release
	Init();




	// Load
	CFileDialog Dlg(TRUE, L"dat", L"*.dat");
	TCHAR szBuf[MAX_PATH]{};
	GetCurrentDirectory(MAX_PATH, szBuf);
	PathRemoveFileSpec(szBuf);
	lstrcat(szBuf, L"\\Client\\Bin\\Data\\EffectSaveData\\");
	Dlg.m_ofn.lpstrInitialDir = szBuf;


	CToolView* pToolView = GET_TOOLVIEW;

	if (IDOK == Dlg.DoModal())
	{
		CString wstrPath = Dlg.GetPathName();

		HANDLE hFile = CreateFile(wstrPath.GetString(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (INVALID_HANDLE_VALUE == hFile)
			return;

		_ulong	dwByte = 0;

		_int iSize = 0;
		ReadFile(hFile, &iSize, sizeof(_int), &dwByte, nullptr);

		for (_int i = 0; i < iSize; ++i)
		{
			TEXNAMEPROTOPATH* pTex = new TEXNAMEPROTOPATH;
			ReadFile(hFile, dynamic_cast<TEXNAMEPROTOPATH*>(pTex)->szName, sizeof(TCHAR) * MAX_PATH, &dwByte, nullptr);
			ReadFile(hFile, dynamic_cast<TEXNAMEPROTOPATH*>(pTex)->szPath, sizeof(TCHAR) * MAX_PATH, &dwByte, nullptr);
			ReadFile(hFile, dynamic_cast<TEXNAMEPROTOPATH*>(pTex)->szProto, sizeof(TCHAR) * MAX_PATH, &dwByte, nullptr);
			ReadFile(hFile, &dynamic_cast<TEXNAMEPROTOPATH*>(pTex)->iTextureNumber, sizeof(_int), &dwByte, nullptr);
			m_vecTexProtoPath.push_back(pTex);

			// Load Texture
			if (FAILED(m_pGameInstance->Add_Component((_uint)LEVEL::STATIC, pTex->szProto, CTexture::Create(pToolView->Get_GraphicDevice(), pToolView->Get_ContextDevice(), pTex->szPath, pTex->iTextureNumber))))
			{
			}

		}

		ReadFile(hFile, &iSize, sizeof(_int), &dwByte, nullptr);
		for (_int i = 0; i < iSize; ++i)
		{
			MODELPATHNAME* pModel = new MODELPATHNAME;
			ReadFile(hFile, dynamic_cast<MODELPATHNAME*>(pModel)->szName, sizeof(TCHAR) * MAX_PATH, &dwByte, nullptr);
			ReadFile(hFile, dynamic_cast<MODELPATHNAME*>(pModel)->szPath, sizeof(TCHAR) * MAX_PATH, &dwByte, nullptr);
			ReadFile(hFile, dynamic_cast<MODELPATHNAME*>(pModel)->szProto, sizeof(TCHAR) * MAX_PATH, &dwByte, nullptr);
			m_vecModelPathName.push_back(pModel);

			if (FAILED(m_pGameInstance->Add_Component((_uint)LEVEL::STATIC, pModel->szProto,
				CModel::Create(pToolView->Get_GraphicDevice(), pToolView->Get_ContextDevice(), pModel->szPath,
					TEXT("../Client/Bin/ShaderFiles/Shader_EffectMesh.hlsl"), "DefaultTechnique", true))))
			{
				int a = 0;
			}
		}

		m_tEffectGroupDescription.szGroupName;
		ReadFile(hFile, &m_tEffectGroupDescription.szGroupName, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);

		iSize = 0;
		ReadFile(hFile, &iSize, sizeof(_int), &dwByte, nullptr);

		_int _iShaderCount = 0;
		for (size_t i = 0; i < iSize; i++)
		{
			EFFECTMESHDESC* pDesc = new EFFECTMESHDESC;


			_iShaderCount = pDesc->tEffectMeshShader.vecShaderName.size();
			//for (_int i = 0; i < _iShaderCount; ++i)
			//{
			//	Safe_Delete(tDesc.tEffectMeshShader.vecShaderName[i]);
			//}
			//tDesc.tEffectMeshShader.vecShaderName.clear();


			ReadFile(hFile, &pDesc->tInfo, sizeof(EFFECTMESHDESC::EffectMeshInfo), &dwByte, nullptr);
			ReadFile(hFile, &pDesc->tTexture, sizeof(EFFECTMESHDESC::EFFECTMESHTEXTURE), &dwByte, nullptr);
			ReadFile(hFile, &pDesc->tPattern, sizeof(EFFECTMESHDESC::EffectMeshPattern), &dwByte, nullptr);

			ReadFile(hFile, &pDesc->tFade_InOut, sizeof(EFFECTMESHDESC::Fade_InOut), &dwByte, nullptr);

			ReadFile(hFile, &pDesc->tUV_Sprite, sizeof(EFFECTMESHDESC::UV_SPRITE), &dwByte, nullptr);
			ReadFile(hFile, &pDesc->tUV_Animation, sizeof(EFFECTMESHDESC::UV_ANIMATION), &dwByte, nullptr);

			_int iVecSize = 0;
			ReadFile(hFile, &iVecSize, sizeof(_int), &dwByte, nullptr);
			for (_int i = 0; i < iVecSize; ++i)
			{
				SHADERNAME* tShaderName = new SHADERNAME;
				ZeroMemory(tShaderName, sizeof(SHADERNAME));
				TCHAR szTemp[MAX_PATH] = L"";

				ReadFile(hFile, &szTemp, sizeof(TCHAR) * MAX_PATH, &dwByte, nullptr);
				_tcscpy_s(tShaderName->szName, szTemp);

				pDesc->tEffectMeshShader.vecShaderName.emplace_back(tShaderName);
			}
			ReadFile(hFile, &pDesc->tEffectMeshShader.fDissolveSpeed, sizeof(_float), &dwByte, nullptr);
			m_tEffectGroupDescription.listEffectMeshDesc.emplace_back(pDesc);
		}

		ReadFile(hFile, &m_tEffectGroupDescription.bCreateDeathRepeat, sizeof(_bool), &dwByte, nullptr);
		ReadFile(hFile, &m_tEffectGroupDescription.vCreateDeathTime, sizeof(_float2), &dwByte, nullptr);

		CloseHandle(hFile);
	}
	else
	{
		return;
	}

	for (auto& pIter : m_tEffectGroupDescription.listEffectMeshDesc)
	{
		if (FAILED(m_pGameInstance->Add_GameObjectToLayerWithPtr((_uint)LEVEL::STATIC, TEXT("Prototype_GameObject_ToolEffectMeshObject"), TEXT("Layer_EffectMesh"), &m_pCurrentMeshEffect, pIter)))
			assert(false);
		m_listEffectMesh.emplace_back(m_pCurrentMeshEffect);
		Safe_AddRef(m_pCurrentMeshEffect);
		m_EffectMesh_ListBox.AddString(pIter->tInfo.szEffectMeshName);
	}

	Safe_AddRef(m_pCurrentMeshEffect);

	CopyDescription(m_tCurrentEffectDesc, dynamic_cast<CToolEffectMeshObject*>(m_pCurrentMeshEffect)->Get_Desc());

	Update_Tool_ByDesc();
}

void CEffectMeshTab::Load_WithParticle()
{
	UpdateData(TRUE);

	CToolView* pToolView = GET_TOOLVIEW;

	// Release
	Init();

	pToolView->m_pMainTab->m_pEffectTab->Init();


	// Load
	CFileDialog Dlg(TRUE, L"dat", L"*.dat");
	TCHAR szBuf[MAX_PATH]{};
	GetCurrentDirectory(MAX_PATH, szBuf);
	PathRemoveFileSpec(szBuf);
	lstrcat(szBuf, L"\\Client\\Bin\\Data\\EffectSaveData\\");
	Dlg.m_ofn.lpstrInitialDir = szBuf;



	if (IDOK == Dlg.DoModal())
	{
		CString wstrPath = Dlg.GetPathName();

		HANDLE hFile = CreateFile(wstrPath.GetString(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (INVALID_HANDLE_VALUE == hFile)
			return;

		_ulong	dwByte = 0;

		_int iSize = 0;
		ReadFile(hFile, &iSize, sizeof(_int), &dwByte, nullptr);

		for (_int i = 0; i < iSize; ++i)
		{
			TEXNAMEPROTOPATH* pTex = new TEXNAMEPROTOPATH;
			ReadFile(hFile, dynamic_cast<TEXNAMEPROTOPATH*>(pTex)->szName, sizeof(TCHAR) * MAX_PATH, &dwByte, nullptr);
			ReadFile(hFile, dynamic_cast<TEXNAMEPROTOPATH*>(pTex)->szPath, sizeof(TCHAR) * MAX_PATH, &dwByte, nullptr);
			ReadFile(hFile, dynamic_cast<TEXNAMEPROTOPATH*>(pTex)->szProto, sizeof(TCHAR) * MAX_PATH, &dwByte, nullptr);
			ReadFile(hFile, &dynamic_cast<TEXNAMEPROTOPATH*>(pTex)->iTextureNumber, sizeof(_int), &dwByte, nullptr);
			m_vecTexProtoPath.push_back(pTex);

			// Load Texture
			if (FAILED(m_pGameInstance->Add_Component((_uint)LEVEL::STATIC, pTex->szProto, CTexture::Create(pToolView->Get_GraphicDevice(), pToolView->Get_ContextDevice(), pTex->szPath, pTex->iTextureNumber))))
			{
			}

		}

		ReadFile(hFile, &iSize, sizeof(_int), &dwByte, nullptr);
		for (_int i = 0; i < iSize; ++i)
		{
			MODELPATHNAME* pModel = new MODELPATHNAME;
			ReadFile(hFile, dynamic_cast<MODELPATHNAME*>(pModel)->szName, sizeof(TCHAR) * MAX_PATH, &dwByte, nullptr);
			ReadFile(hFile, dynamic_cast<MODELPATHNAME*>(pModel)->szPath, sizeof(TCHAR) * MAX_PATH, &dwByte, nullptr);
			ReadFile(hFile, dynamic_cast<MODELPATHNAME*>(pModel)->szProto, sizeof(TCHAR) * MAX_PATH, &dwByte, nullptr);
			m_vecModelPathName.push_back(pModel);

			if (FAILED(m_pGameInstance->Add_Component((_uint)LEVEL::STATIC, pModel->szProto,
				CModel::Create(pToolView->Get_GraphicDevice(), pToolView->Get_ContextDevice(), pModel->szPath,
					TEXT("../Client/Bin/ShaderFiles/Shader_EffectMesh.hlsl"), "DefaultTechnique", true))))
			{
				int a = 0;
			}
		}

		m_tEffectGroupDescription.szGroupName;
		ReadFile(hFile, &m_tEffectGroupDescription.szGroupName, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);

		iSize = 0;
		ReadFile(hFile, &iSize, sizeof(_int), &dwByte, nullptr);

		_int _iShaderCount = 0;
		for (size_t i = 0; i < iSize; i++)
		{
			EFFECTMESHDESC* pDesc = new EFFECTMESHDESC;


			_iShaderCount = pDesc->tEffectMeshShader.vecShaderName.size();
			//for (_int i = 0; i < _iShaderCount; ++i)
			//{
			//	Safe_Delete(tDesc.tEffectMeshShader.vecShaderName[i]);
			//}
			//tDesc.tEffectMeshShader.vecShaderName.clear();


			ReadFile(hFile, &pDesc->tInfo, sizeof(EFFECTMESHDESC::EffectMeshInfo), &dwByte, nullptr);
			ReadFile(hFile, &pDesc->tTexture, sizeof(EFFECTMESHDESC::EFFECTMESHTEXTURE), &dwByte, nullptr);
			ReadFile(hFile, &pDesc->tPattern, sizeof(EFFECTMESHDESC::EffectMeshPattern), &dwByte, nullptr);

			ReadFile(hFile, &pDesc->tFade_InOut, sizeof(EFFECTMESHDESC::Fade_InOut), &dwByte, nullptr);

			ReadFile(hFile, &pDesc->tUV_Sprite, sizeof(EFFECTMESHDESC::UV_SPRITE), &dwByte, nullptr);
			ReadFile(hFile, &pDesc->tUV_Animation, sizeof(EFFECTMESHDESC::UV_ANIMATION), &dwByte, nullptr);

			_int iVecSize = 0;
			ReadFile(hFile, &iVecSize, sizeof(_int), &dwByte, nullptr);
			for (_int i = 0; i < iVecSize; ++i)
			{
				SHADERNAME* tShaderName = new SHADERNAME;
				ZeroMemory(tShaderName, sizeof(SHADERNAME));
				TCHAR szTemp[MAX_PATH] = L"";

				ReadFile(hFile, &szTemp, sizeof(TCHAR) * MAX_PATH, &dwByte, nullptr);
				_tcscpy_s(tShaderName->szName, szTemp);

				pDesc->tEffectMeshShader.vecShaderName.emplace_back(tShaderName);
			}
			ReadFile(hFile, &pDesc->tEffectMeshShader.fDissolveSpeed, sizeof(_float), &dwByte, nullptr);
			m_tEffectGroupDescription.listEffectMeshDesc.emplace_back(pDesc);
		}

		ReadFile(hFile, &m_tEffectGroupDescription.bCreateDeathRepeat, sizeof(_bool), &dwByte, nullptr);
		ReadFile(hFile, &m_tEffectGroupDescription.vCreateDeathTime, sizeof(_float2), &dwByte, nullptr);

		// Load Particle
		pToolView->m_pMainTab->m_pEffectTab->Init();
		_int iParticleSize = 0;
		ReadFile(hFile, &iParticleSize, sizeof(_int), &dwByte, nullptr);

		for (_int i = 0; i < iParticleSize; i++)
		{
			PARTICLEDESC* pParticleDesc = new PARTICLEDESC;
			//ReadFile(hFile, pParticleDesc, sizeof(PARTICLEDESC333), &dwByte, nullptr);
			TCHAR szTemp[MAX_PATH] = L"";
			ReadFile(hFile, &szTemp, sizeof(TCHAR) * MAX_PATH, &dwByte, nullptr);
			_tcscpy_s(pParticleDesc->szParticleName, szTemp);
			ReadFile(hFile, &pParticleDesc->vPosition, sizeof(_float3), &dwByte, nullptr);
			ReadFile(hFile, &pParticleDesc->vScale, sizeof(_float3), &dwByte, nullptr);
			ReadFile(hFile, &pParticleDesc->vRotation, sizeof(_float3), &dwByte, nullptr);
			ReadFile(hFile, &pParticleDesc->iTextureNumber, sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, &szTemp, sizeof(TCHAR) * MAX_PATH, &dwByte, nullptr);
			_tcscpy_s(pParticleDesc->szTextureName, szTemp);
			ReadFile(hFile, &szTemp, sizeof(TCHAR) * MAX_PATH, &dwByte, nullptr);
			_tcscpy_s(pParticleDesc->szTextureProto, szTemp);
			ReadFile(hFile, &szTemp, sizeof(TCHAR) * MAX_PATH, &dwByte, nullptr);
			_tcscpy_s(pParticleDesc->szTexturePath, szTemp);
			ReadFile(hFile, &pParticleDesc->fMultiTextureInterval, sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &pParticleDesc->bMultiTextureRepeat, sizeof(_bool), &dwByte, nullptr);

			_int iCount = 0;
			ReadFile(hFile, &iCount, sizeof(_int), &dwByte, nullptr);

			for (_int i = 0; i < iCount; ++i)
			{
				SHADERNAME* tShaderName = new SHADERNAME;
				ZeroMemory(tShaderName, sizeof(SHADERNAME));

				ReadFile(hFile, &szTemp, sizeof(TCHAR) * MAX_PATH, &dwByte, nullptr);
				_tcscpy_s(tShaderName->szName, szTemp);
				pParticleDesc->tParticleShaderDesc.vecShaderName.push_back(tShaderName);
			}

			ReadFile(hFile, &pParticleDesc->tPointInst_Desc, sizeof(POINTINST_DESC), &dwByte, nullptr);
	
			
			
			pToolView->m_pMainTab->m_pEffectTab->Get_GroupDesc().listParticleDesc.emplace_back(pParticleDesc);
		}


		pToolView->m_pMainTab->m_pEffectTab->Create_From_GroupList_FromLoad();


		CloseHandle(hFile);
	}
	else
	{
		return;
	}

	for (auto& pIter : m_tEffectGroupDescription.listEffectMeshDesc)
	{
		if (FAILED(m_pGameInstance->Add_GameObjectToLayerWithPtr((_uint)LEVEL::STATIC, TEXT("Prototype_GameObject_ToolEffectMeshObject"), TEXT("Layer_EffectMesh"), &m_pCurrentMeshEffect, pIter)))
			assert(false);
		m_listEffectMesh.emplace_back(m_pCurrentMeshEffect);
		Safe_AddRef(m_pCurrentMeshEffect);
		m_EffectMesh_ListBox.AddString(pIter->tInfo.szEffectMeshName);
	}

	if (m_pCurrentMeshEffect)
	{
		Safe_AddRef(m_pCurrentMeshEffect);
		CopyDescription(m_tCurrentEffectDesc, dynamic_cast<CToolEffectMeshObject*>(m_pCurrentMeshEffect)->Get_Desc());
	}

	Update_Tool_ByDesc();
	pToolView->m_pMainTab->m_pEffectTab->Update_Tool_ByDesc();

}

void CEffectMeshTab::Init()
{
	Safe_Release(m_pCurrentMeshEffect);
	m_pCurrentMeshEffect = nullptr;

	for (auto& pIter : m_listEffectMesh)
	{
		pIter->Set_State(OBJSTATE::DEAD);
		Safe_Release(pIter);
	}
	m_listEffectMesh.clear();



	_int iCount = m_EffectMesh_ListBox.GetCount();
	for (_int i = 0; i < iCount; ++i)
		m_EffectMesh_ListBox.DeleteString(0);

	if (m_tCurrentEffectDesc.tEffectMeshShader.vecShaderName.size() > 0)
	{
		for (_int i = 0; i < m_tCurrentEffectDesc.tEffectMeshShader.vecShaderName.size(); ++i)
			Safe_Delete(m_tCurrentEffectDesc.tEffectMeshShader.vecShaderName[i]);
		m_tCurrentEffectDesc.tEffectMeshShader.vecShaderName.clear();
	}

	ZeroMemory(&m_tCurrentEffectDesc.tFade_InOut, sizeof(EFFECTMESHDESC::FADE_INOUT));
	ZeroMemory(&m_tCurrentEffectDesc.tInfo, sizeof(EFFECTMESHDESC::EFFECTMESHINFO));
	ZeroMemory(&m_tCurrentEffectDesc.tPattern, sizeof(EFFECTMESHDESC::EFFECTMESHPATTERN));
	ZeroMemory(&m_tCurrentEffectDesc.tTexture, sizeof(EFFECTMESHDESC::EFFECTMESHPATTERN));
	ZeroMemory(&m_tCurrentEffectDesc.tUV_Animation, sizeof(EFFECTMESHDESC::UV_ANIMATION));
	ZeroMemory(&m_tCurrentEffectDesc.tUV_Sprite, sizeof(EFFECTMESHDESC::UV_SPRITE));


	for (auto& pIter : m_tEffectGroupDescription.listEffectMeshDesc)
	{
		for (auto& pShaderIter : pIter->tEffectMeshShader.vecShaderName)
			Safe_Delete(pShaderIter);
		pIter->tEffectMeshShader.vecShaderName.clear();
		Safe_Delete(pIter);
	}
	m_tEffectGroupDescription.listEffectMeshDesc.clear();



	_tcscpy_s(m_tEffectGroupDescription.szGroupName, L"");
	m_tEffectGroupDescription.bCreateDeathRepeat = false;
	ZeroMemory(&m_tEffectGroupDescription.vCreateDeathTime, sizeof(_float2));

	for (auto& pIter : m_vecTexProtoPath)
		Safe_Delete(pIter);
	m_vecTexProtoPath.clear();

	for (auto& pIter : m_vecModelPathName)
		Safe_Delete(pIter);
	m_vecModelPathName.clear();
}




void CEffectMeshTab::OnBnClickedEffectmeshUpdateShaderButton()
{
	Update_Shaders();
	Update_CurrentEffectMesh_Desc();
}

void CEffectMeshTab::OnEnChangeEffectmeshFadeInStartTimeEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");
	GetDlgItemText(IDC_EFFECTMESH_FADE_IN_START_TIME_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;

	m_tCurrentEffectDesc.tFade_InOut.vFadeIn_StartEndTime.x = _tstof(strVal);

	SetDlgItemText(IDC_EFFECTMESH_FADE_IN_START_TIME_RESULT, strVal);

	if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();

	UpdateData(FALSE);
}


void CEffectMeshTab::OnEnChangeEffectmeshFadeInEndTimeEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");
	GetDlgItemText(IDC_EFFECTMESH_FADE_IN_END_TIME_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;

	m_tCurrentEffectDesc.tFade_InOut.vFadeIn_StartEndTime.y = _tstof(strVal);

	SetDlgItemText(IDC_EFFECTMESH_FADE_IN_END_TIME_RESULT, strVal);

	if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();

	UpdateData(FALSE);
}


void CEffectMeshTab::OnEnChangeEffectmeshFadeInDegreeEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");
	GetDlgItemText(IDC_EFFECTMESH_FADE_IN_DEGREE_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;

	m_tCurrentEffectDesc.tFade_InOut.fFadeIn_Degree = _tstof(strVal);

	SetDlgItemText(IDC_EFFECTMESH_FADE_IN_DEGREE_RESULT, strVal);

	if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();

	UpdateData(FALSE);
}


void CEffectMeshTab::OnBnClickedEffectmeshFadeInCheck()
{
	UpdateData(TRUE);

	if (((CButton*)GetDlgItem(IDC_EFFECTMESH_FADE_IN_CHECK))->GetCheck())
		m_tCurrentEffectDesc.tFade_InOut.bFadeIn = true;
	else
		m_tCurrentEffectDesc.tFade_InOut.bFadeIn = false;

	UpdateData(FALSE);
}


void CEffectMeshTab::OnEnChangeEffectmeshFadeOutStartTimeEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");
	GetDlgItemText(IDC_EFFECTMESH_FADE_OUT_START_TIME_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;

	m_tCurrentEffectDesc.tFade_InOut.vFadeOut_StartEndTime.x = _tstof(strVal);

	SetDlgItemText(IDC_EFFECTMESH_FADE_OUT_START_TIME_RESULT, strVal);

	if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();

	UpdateData(FALSE);
}


void CEffectMeshTab::OnEnChangeEffectmeshFadeOutEndTimeEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");
	GetDlgItemText(IDC_EFFECTMESH_FADE_OUT_END_TIME_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;

	m_tCurrentEffectDesc.tFade_InOut.vFadeOut_StartEndTime.y = _tstof(strVal);

	SetDlgItemText(IDC_EFFECTMESH_FADE_OUT_END_TIME_RESULT, strVal);

	if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();

	UpdateData(FALSE);
}


void CEffectMeshTab::OnEnChangeEffectmeshFadeOutDegreeEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");
	GetDlgItemText(IDC_EFFECTMESH_FADE_OUT_DEGREE_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;

	m_tCurrentEffectDesc.tFade_InOut.fFadeOut_Degree = _tstof(strVal);

	SetDlgItemText(IDC_EFFECTMESH_FADE_OUT_DEGREE_RESULT, strVal);

	if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();

	UpdateData(FALSE);
}


void CEffectMeshTab::OnBnClickedEffectmeshFadeOutCheck()
{
	UpdateData(TRUE);

	if (((CButton*)GetDlgItem(IDC_EFFECTMESH_FADE_OUT_CHECK))->GetCheck())
		m_tCurrentEffectDesc.tFade_InOut.bFadeOut = true;
	else
		m_tCurrentEffectDesc.tFade_InOut.bFadeOut = false;

	UpdateData(FALSE);
}


void CEffectMeshTab::OnBnClickedEffectmeshLoadWithParticleButton()
{
	Load_WithParticle();
}


void CEffectMeshTab::OnBnClickedEffectmeshUvAnimationCheck()
{
	UpdateData(TRUE);
	
	if (((CButton*)GetDlgItem(IDC_EFFECTMESH_UV_ANIMATION_CHECK))->GetCheck())
		m_tCurrentEffectDesc.tUV_Animation.bUVAnimation = true;
	else
		m_tCurrentEffectDesc.tUV_Animation.bUVAnimation = false;

	UpdateData(FALSE);
}


void CEffectMeshTab::OnEnChangeEffectmeshUvAnimationStartTimeEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");

	GetDlgItemText(IDC_EFFECTMESH_UV_ANIMATION_START_TIME_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;

	m_tCurrentEffectDesc.tUV_Animation.vUV_StartEndTime.x = _tstof(strVal);
	SetDlgItemText(IDC_EFFECTMESH_UV_ANIMATION_START_TIME_RESULT, strVal);

	if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();

	UpdateData(FALSE);
}


void CEffectMeshTab::OnEnChangeEffectmeshUvAnimationEndTimeEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");

	GetDlgItemText(IDC_EFFECTMESH_UV_ANIMATION_END_TIME_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;

	m_tCurrentEffectDesc.tUV_Animation.vUV_StartEndTime.y = _tstof(strVal);
	SetDlgItemText(IDC_EFFECTMESH_UV_ANIMATION_END_TIME_RESULT, strVal);

	if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();

	UpdateData(FALSE);
}


void CEffectMeshTab::OnEnChangeEffectmeshUvAnimationXEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");

	GetDlgItemText(IDC_EFFECTMESH_UV_ANIMATION_X_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;

	m_tCurrentEffectDesc.tUV_Animation.vUV_Val.x = _tstof(strVal);
	SetDlgItemText(IDC_EFFECTMESH_UV_ANIMATION_X_RESULT, strVal);

	if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();

	UpdateData(FALSE);
}


void CEffectMeshTab::OnEnChangeEffectmeshUvAnimationYEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");

	GetDlgItemText(IDC_EFFECTMESH_UV_ANIMATION_Y_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;

	m_tCurrentEffectDesc.tUV_Animation.vUV_Val.y = _tstof(strVal);
	SetDlgItemText(IDC_EFFECTMESH_UV_ANIMATION_Y_RESULT, strVal);

	if (m_pCurrentMeshEffect) Update_CurrentEffectMesh_Desc();

	UpdateData(FALSE);
}
