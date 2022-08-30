// EffectTab.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool.h"
#include "EffectTab.h"
#include "afxdialogex.h"
#include "GameInstance.h"
#include "MainFrm.h"
#include "ToolDefines.h"
#include "EffectMesh2Tab.h"

#include "ToolParticle.h"
#include "Client_Struct.h"
#include "MainFrm.h"
#include "ToolView.h"
#include "MainTab.h"
#include "EffectMeshTab.h"
#include <fstream>
// CEffectTab 대화 상자입니다.

IMPLEMENT_DYNAMIC(CEffectTab, CDialog)

CEffectTab::CEffectTab(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_EFFECTTAB, pParent)
	, m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);
}

CEffectTab::~CEffectTab()
{
	Safe_Release(m_pGameInstance);

	//for (auto& pIter : m_vecTexProtoPath)
	//	Safe_Delete(pIter);
	//m_vecTexProtoPath.clear();

	for (auto& pIter : m_listParticle)
		Safe_Release(pIter);
	m_listParticle.clear();

	if (nullptr != m_pCurrentParticle)
	{
		Safe_Release(m_pCurrentParticle);
		m_pCurrentParticle = nullptr;
	}


	for (auto& pIter : m_tGroupParticleDesc.listParticleDesc)
	{
		if (pIter->tParticleShaderDesc.vecShaderName.size() > 0)
		{
			for (size_t i = 0; i < pIter->tParticleShaderDesc.vecShaderName.size(); i++)
			{
				Safe_Delete(pIter->tParticleShaderDesc.vecShaderName[i]);
			}
			pIter->tParticleShaderDesc.vecShaderName.clear();
		}
		Safe_Delete(pIter);
	}

	m_tGroupParticleDesc.listParticleDesc.clear();

}

void CEffectTab::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EFFECT_TEXTURELIST, m_ListBox_Texture);
	DDX_Control(pDX, IDC_EFFECT_PARTICLE_LIST, m_ListBox_Particle);
}

//void CEffectTab::Load_FilePath()
//{
//	wifstream fin;
//	fin.open(L"Data/PathInfo.txt");
//
//	CMainFrame* pMain = GET_MAINFRM;
//	CToolView* pToolView = GET_TOOLVIEW;
//	if (!fin.fail())
//	{
//		TCHAR szPath[MAX_PATH]{};
//		TCHAR szName[MAX_PATH]{};
//
//		TCHAR szTexturePrototype[MAX_PATH]{ L"Prototype_Component_Texture_" };
//		TCHAR szResultName[MAX_PATH] = TEXT("item_");
//		TCHAR szTexturePathName[MAX_PATH];
//		while (true)
//		{
//			wcscpy_s(szTexturePrototype, L"Prototype_Component_Texture_");
//			wcscpy_s(szResultName, TEXT("item_"));
//
//			fin.getline(szPath, MAX_PATH, L'|');
//			fin.getline(szName, MAX_PATH);
//
//			wcscpy_s(szTexturePathName, szPath);
//			wcscat_s(szTexturePathName, szName);
//
//			wcscat_s(szTexturePrototype, szName);
//
//			CString str;
//			// TCHAR to CString
//			str.Format(_T("%s"), szTexturePrototype);
//			m_ListBox_Texture.AddString(str);
//			wcscpy_s(szTexturePrototype, str);
//
//
//			TEXNAMEPROTOPATH* pProtoPath = new TEXNAMEPROTOPATH;
//			wcscpy_s(pProtoPath->szProto, szTexturePrototype);
//			wcscpy_s(pProtoPath->szPath, szTexturePathName);
//			m_vecTexProtoPath.emplace_back(pProtoPath);
//			if (FAILED(m_pGameInstance->Add_Component((_uint)LEVEL::STATIC, pProtoPath->szProto, CTexture::Create(pToolView->Get_GraphicDevice(), pToolView->Get_ContextDevice(), pProtoPath->szPath))))
//				assert(false);
//
//
//			if (fin.eof())
//			{
//				break;
//			}
//		}
//	}
//}

//void CEffectTab::Load_Meshes(CString RelativePath, CString AbsolutePath)
//{
//	CFileFind		FileFind;
//
//	BOOL			bContinue;
//	if (AbsolutePath.Compare(L"") == 0)
//		bContinue = FileFind.FindFile(RelativePath + L"/*.*");
//	else
//		bContinue = FileFind.FindFile(AbsolutePath + L"/*.*");
//
//
//	CString			strFilter = L".dat";
//	CString			strFolderItem = L"";
//	CString			strFileExt = L"";
//	CString			strPrototype = L"Prototype_Component_Model_";
//
//	CMainFrame*	pMain = GET_MAINFRM;
//	CToolView* pToolView = GET_TOOLVIEW;
//	while (bContinue)
//	{
//		bContinue = FileFind.FindNextFile();
//		if (!FileFind.IsDots() && FileFind.IsDirectory())
//		{
//			strFolderItem = FileFind.GetFilePath();
//
//			//m_Effect_MeshList.AddString(FileFind.GetFileName());
//			Load_Meshes(RelativePath, strFolderItem);
//		}
//		strFolderItem = FileFind.GetFileName();
//		strFileExt = strFolderItem.Mid(strFolderItem.ReverseFind('.'));
//
//		if (!FileFind.IsDots())
//		{
//			// if is not .fbx, continue
//			if (lstrcmp(strFileExt, strFilter) != 0)
//				continue;
//
//			
//
//
//			MODELPATHNAME* tModelPathName = new MODELPATHNAME;
//			CString TempAbsStr = AbsolutePath + L"\\";
//			TCHAR* szPath = (TCHAR*)(LPCTSTR)(TempAbsStr);
//			// CString to char
//			// CString to TCHAR
//			// TCHAR to char
//			WideCharToMultiByte(CP_ACP, 0, szPath, MAX_PATH, tModelPathName->szPath, MAX_PATH, NULL, NULL);
//			CString TempName = FileFind.GetFileName();
//			TCHAR* szName = (TCHAR*)(LPCTSTR)TempName;
//			WideCharToMultiByte(CP_ACP, 0, szName, MAX_PATH, tModelPathName->szName, MAX_PATH, NULL, NULL);
//			CString TempProto = strPrototype + FileFind.GetFileName();
//			_tcscpy_s(tModelPathName->szProto, TempProto);
//
//			//m_pGameInstance->Add_Component((_uint)LEVEL::STATIC, tModelPathName->szProto,
//			//	CAssimpModel::Create(pToolView->Get_GraphicDevice(), pToolView->Get_ContextDevice(),
//			//		CAssimpModel::TYPE::NONANIM, tModelPathName->chPath, tModelPathName->chName, TEXT("../Client/Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique"));
//
//
//
//
//			if (FAILED(m_pGameInstance->Add_Component((_uint)LEVEL::STATIC, tModelPathName->szProto,
//				CModel::Create(pToolView->Get_GraphicDevice(), pToolView->Get_ContextDevice(), szPath, 
//					TEXT("../Client/Bin/ShaderFiles/Shader_Mesh.hlsl"), "DefaultTechnique"))))
//			{
//				assert(false);
//			}
//
//			m_Effect_MeshListBox.AddString(FileFind.GetFileName());
//
//			m_vecModelPathName.emplace_back(tModelPathName);
//		}
//	}
//	FileFind.Close();
//}

BEGIN_MESSAGE_MAP(CEffectTab, CDialog)
	ON_EN_CHANGE(IDC_EFFECT_POSITION_X_EDIT, &CEffectTab::OnEnChangeEffectPositionXEdit)
	ON_EN_CHANGE(IDC_EFFECT_POSITION_Y_EDIT, &CEffectTab::OnEnChangeEffectPositionYEdit)
	ON_EN_CHANGE(IDC_EFFECT_POSITION_Z_EDIT, &CEffectTab::OnEnChangeEffectPositionZEdit)
	ON_EN_CHANGE(IDC_EFFECT_SCALE_X_EDIT, &CEffectTab::OnEnChangeEffectScaleXEdit)
	ON_EN_CHANGE(IDC_EFFECT_SCALE_Y_EDIT, &CEffectTab::OnEnChangeEffectScaleYEdit)
	ON_EN_CHANGE(IDC_EFFECT_SCALE_Z_EDIT, &CEffectTab::OnEnChangeEffectScaleZEdit)
	ON_EN_CHANGE(IDC_EFFECT_ROTATION_X_EDIT, &CEffectTab::OnEnChangeEffectRotationXEdit)
	ON_EN_CHANGE(IDC_EFFECT_ROTATION_Y_EDIT, &CEffectTab::OnEnChangeEffectRotationYEdit)
	ON_EN_CHANGE(IDC_EFFECT_ROTATION_Z_EDIT, &CEffectTab::OnEnChangeEffectRotationZEdit)
	ON_EN_CHANGE(IDC_EFFECT_DIRECTION_X_EDIT, &CEffectTab::OnEnChangeEffectDirectionXEdit)
	ON_EN_CHANGE(IDC_EFFECT_DIRECTION_Y_EDIT, &CEffectTab::OnEnChangeEffectDirectionYEdit)
	ON_EN_CHANGE(IDC_EFFECT_DIRECTION_Z_EDIT, &CEffectTab::OnEnChangeEffectDirectionZEdit)
	ON_EN_CHANGE(IDC_EFFECT_LIFETIME_EDIT, &CEffectTab::OnEnChangeEffectLifetimeEdit)
	ON_EN_CHANGE(IDC_EFFECT_NUMBER_EDIT, &CEffectTab::OnEnChangeEffectNumberEdit)
	ON_BN_CLICKED(IDC_EFFECT_CREATE_BUTTON, &CEffectTab::OnBnClickedEffectCreateButton)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EFFECT_POSITION_X_SPIN_CTRL, &CEffectTab::OnDeltaposEffectPositionXSpinCtrl)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EFFECT_POSITION_Y_SPIN_CTRL, &CEffectTab::OnDeltaposEffectPositionYSpinCtrl)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EFFECT_POSITION_Z_SPIN_CTRL, &CEffectTab::OnDeltaposEffectPositionZSpinCtrl)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EFFECT_SCALE_X_SPIN_CTRL, &CEffectTab::OnDeltaposEffectScaleXSpinCtrl)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EFFECT_SCALE_Y_SPIN_CTRL, &CEffectTab::OnDeltaposEffectScaleYSpinCtrl)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EFFECT_SCALE_Z_SPIN_CTRL, &CEffectTab::OnDeltaposEffectScaleZSpinCtrl)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EFFECT_ROTATION_X_SPIN_CTRL, &CEffectTab::OnDeltaposEffectRotationXSpinCtrl)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EFFECT_ROTATION_Y_SPIN_CTRL, &CEffectTab::OnDeltaposEffectRotationYSpinCtrl)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EFFECT_ROTATION_Z_SPIN_CTRL, &CEffectTab::OnDeltaposEffectRotationZSpinCtrl)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EFFECT_DIRECTION_X_SPIN_CTRL, &CEffectTab::OnDeltaposEffectDirectionXSpinCtrl)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EFFECT_DIRECTION_Y_SPIN_CTRL, &CEffectTab::OnDeltaposEffectDirectionYSpinCtrl)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EFFECT_DIRECTION_Z_SPIN_CTRL, &CEffectTab::OnDeltaposEffectDirectionZSpinCtrl)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EFFECT_LIFETIME_SPIN_CTRL, &CEffectTab::OnDeltaposEffectLifetimeSpinCtrl)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EFFECT_NUMBER_SPIN_CTRL, &CEffectTab::OnDeltaposEffectNumberSpinCtrl)
	ON_EN_CHANGE(IDC_EFFECT_DENSITY_EDIT, &CEffectTab::OnEnChangeEffectDensityEdit)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EFFECT_DENSITY_SPIN_CTRL, &CEffectTab::OnDeltaposEffectDensitySpinCtrl)
	ON_BN_CLICKED(IDC_EFFECT_OPEN_TEXTURE, &CEffectTab::OnBnClickedEffectOpenTexture)
	ON_LBN_SELCHANGE(IDC_EFFECT_TEXTURELIST, &CEffectTab::OnLbnSelchangeEffectTexturelist)
	ON_EN_CHANGE(IDC_EFFECT_DIRECTION_DENSITY_EDIT, &CEffectTab::OnEnChangeEffectDirectionDensityEdit)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EFFECT_DIRECTION_DENSITY_SPIN_CTRL, &CEffectTab::OnDeltaposEffectDirectionDensitySpinCtrl)
	ON_EN_CHANGE(IDC_EFFECT_SPEED_EDIT, &CEffectTab::OnEnChangeEffectSpeedEdit)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EFFECT_SPEED_SPIN_CTRL, &CEffectTab::OnDeltaposEffectSpeedSpinCtrl)
	ON_EN_CHANGE(IDC_EFFECT_SPEED_DENSITY_EDIT, &CEffectTab::OnEnChangeEffectSpeedDensityEdit)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EFFECT_SPEED_DENSITY_SPIN_CTRL, &CEffectTab::OnDeltaposEffectSpeedDensitySpinCtrl)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_EFFECT_LIFETIME_DENSITY_ON_RADIO, IDC_EFFECT_LIFETIME_DENSITY_OFF_RADIO, &CEffectTab::OnBnClicked_LifeTimeDensity)

	ON_EN_CHANGE(IDC_EFFECT_WIDTH_EDIT, &CEffectTab::OnEnChangeEffectWidthEdit)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EFFECT_WIDTH_SPIN_CTRL, &CEffectTab::OnDeltaposEffectWidthSpinCtrl)
	ON_EN_CHANGE(IDC_EFFECT_HEIGHT_EDIT, &CEffectTab::OnEnChangeEffectHeightEdit)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EFFECT_HEIGHT_SPIN_CTRL, &CEffectTab::OnDeltaposEffectHeightSpinCtrl)
	ON_EN_CHANGE(IDC_EFFECT_SPRITE_INTERVAL_EDIT, &CEffectTab::OnEnChangeEffectSpriteIntervalEdit)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EFFECT_SPRITE_INTERVAL_SPIN_CTRL, &CEffectTab::OnDeltaposEffectSpriteIntervalSpinCtrl)
	ON_EN_CHANGE(IDC_EFFECT_TEXTURE_NUMBER_EDIT, &CEffectTab::OnEnChangeEffectTextureNumberEdit)
	ON_BN_CLICKED(IDC_EFFECT_DELETE_BUTTON, &CEffectTab::OnBnClickedEffectDeleteButton)
	ON_EN_CHANGE(IDC_EFFECT_PARTICLE_NAME_EDIT, &CEffectTab::OnEnChangeEffectParticleNameEdit)
	ON_BN_CLICKED(IDC_EFFECT_APPLY_NAME, &CEffectTab::OnBnClickedEffectApplyName)
	ON_BN_CLICKED(IDC_EFFECT_APPLY_TEXTURE_BUTTON, &CEffectTab::OnBnClickedEffectApplyTextureButton)
	ON_BN_CLICKED(IDC_EFFECT_REPEAT, &CEffectTab::OnBnClickedEffectRepeat)
	ON_EN_CHANGE(IDC_EFFECT_MULTI_TEXTURE_INTERVAL_EDIT, &CEffectTab::OnEnChangeEffectMultiTextureIntervalEdit)
	ON_BN_CLICKED(IDC_EFFECT_PLAY_BUTTON, &CEffectTab::OnBnClickedEffectPlayButton)
	ON_BN_CLICKED(IDC_EFFECT_STOP_BUTTON, &CEffectTab::OnBnClickedEffectStopButton)
	ON_BN_CLICKED(IDC_EFFECT_FADE_IN_CHECK, &CEffectTab::OnBnClickedEffectFadeInCheck)
	ON_EN_CHANGE(IDC_EFFECT_FADE_IN_EDIT, &CEffectTab::OnEnChangeEffectFadeInEdit)
	ON_BN_CLICKED(IDC_EFFECT_FADE_OUT_CHECK, &CEffectTab::OnBnClickedEffectFadeOutCheck)
	ON_EN_CHANGE(IDC_EFFECT_FADE_OUT_EDIT, &CEffectTab::OnEnChangeEffectFadeOutEdit)
	ON_BN_CLICKED(IDC_EFFECT_MULTI_TEXTURE_SPRITE_REPEAT, &CEffectTab::OnBnClickedEffectMultiTextureSpriteRepeat)
	ON_LBN_SELCHANGE(IDC_EFFECT_PARTICLE_LIST, &CEffectTab::OnLbnSelchangeEffectParticleList)
	ON_BN_CLICKED(IDC_EFFECT_SAVE_BUTTON, &CEffectTab::OnBnClickedEffectSaveButton)
	ON_BN_CLICKED(IDC_EFFECT_LOAD_BUTTON, &CEffectTab::OnBnClickedEffectLoadButton)
	ON_EN_CHANGE(IDC_EFFECT_FADE_OUT_END_EDIT, &CEffectTab::OnEnChangeEffectFadeOutEndEdit)
	ON_EN_CHANGE(IDC_EFFECT_LIFETIME_DENSITY_END_EDIT, &CEffectTab::OnEnChangeEffectLifetimeDensityEndEdit)
	ON_EN_CHANGE(IDC_EFFECT_PSIZE_EDIT, &CEffectTab::OnEnChangeEffectPsizeEdit)
	ON_BN_CLICKED(IDC_EFFECT_UPDATE_SHADER_BUTTON, &CEffectTab::OnBnClickedEffectUpdateShaderButton)
END_MESSAGE_MAP()


// CEffectTab 메시지 처리기입니다.




void CEffectTab::Update_Tool_ByDesc()
{
	if (!m_pCurrentParticle) return;

	//memcpy(&m_tParticleDesc, &dynamic_cast<CToolParticle*>(m_pCurrentParticle)->Get_Desc(), sizeof(PARTICLEDESC));



	_tcscpy_s(m_tParticleDesc.szParticleName, dynamic_cast<CToolParticle*>(m_pCurrentParticle)->Get_Desc().szParticleName);
	memcpy(&m_tParticleDesc.vPosition, &dynamic_cast<CToolParticle*>(m_pCurrentParticle)->Get_Desc().vPosition, sizeof(_float3));
	memcpy(&m_tParticleDesc.vScale, &dynamic_cast<CToolParticle*>(m_pCurrentParticle)->Get_Desc().vScale, sizeof(_float3));
	memcpy(&m_tParticleDesc.vRotation, &dynamic_cast<CToolParticle*>(m_pCurrentParticle)->Get_Desc().vRotation, sizeof(_float3));

	memcpy(&m_tParticleDesc.iTextureNumber, &dynamic_cast<CToolParticle*>(m_pCurrentParticle)->Get_Desc().iTextureNumber, sizeof(_int));
	_tcscpy_s(m_tParticleDesc.szTextureName, dynamic_cast<CToolParticle*>(m_pCurrentParticle)->Get_Desc().szTextureName);
	_tcscpy_s(m_tParticleDesc.szTextureProto, dynamic_cast<CToolParticle*>(m_pCurrentParticle)->Get_Desc().szTextureProto);
	_tcscpy_s(m_tParticleDesc.szTexturePath, dynamic_cast<CToolParticle*>(m_pCurrentParticle)->Get_Desc().szTexturePath);

	memcpy(&m_tParticleDesc.fMultiTextureInterval, &dynamic_cast<CToolParticle*>(m_pCurrentParticle)->Get_Desc().fMultiTextureInterval, sizeof(_float));
	memcpy(&m_tParticleDesc.bMultiTextureRepeat, &dynamic_cast<CToolParticle*>(m_pCurrentParticle)->Get_Desc().bMultiTextureRepeat, sizeof(_bool));

	if (m_tParticleDesc.tParticleShaderDesc.vecShaderName.size() > 0)
	{
		for (_int i = 0; i < m_tParticleDesc.tParticleShaderDesc.vecShaderName.size(); i++)
		{
			Safe_Delete(m_tParticleDesc.tParticleShaderDesc.vecShaderName[i]);
		}
		m_tParticleDesc.tParticleShaderDesc.vecShaderName.clear();
	}

	for (_int i = 0; i < dynamic_cast<CToolParticle*>(m_pCurrentParticle)->Get_Desc().tParticleShaderDesc.vecShaderName.size(); ++i)
	{
		SHADERNAME* pShaderName = new SHADERNAME;
		_tcscpy_s(pShaderName->szName, dynamic_cast<CToolParticle*>(m_pCurrentParticle)->Get_Desc().tParticleShaderDesc.vecShaderName[i]->szName);
		m_tParticleDesc.tParticleShaderDesc.vecShaderName.emplace_back(pShaderName);
	}

	memcpy(&m_tParticleDesc.tPointInst_Desc, &dynamic_cast<CToolParticle*>(m_pCurrentParticle)->Get_Desc().tPointInst_Desc, sizeof(POINTINST_DESC));


	CString str = {};
	str.Format(L"%f", m_tParticleDesc.vPosition.x);
	SetDlgItemText(IDC_EFFECT_POSITION_X_EDIT, str);
	str.Format(L"%f", m_tParticleDesc.vPosition.y);
	SetDlgItemText(IDC_EFFECT_POSITION_Y_EDIT, str);
	str.Format(L"%f", m_tParticleDesc.vPosition.z);
	SetDlgItemText(IDC_EFFECT_POSITION_Z_EDIT, str);
	str.Format(L"%f", m_tParticleDesc.vScale.x);
	SetDlgItemText(IDC_EFFECT_SCALE_X_EDIT, str);
	str.Format(L"%f", m_tParticleDesc.vScale.y);
	SetDlgItemText(IDC_EFFECT_SCALE_Y_EDIT, str);
	str.Format(L"%f", m_tParticleDesc.vScale.z);
	SetDlgItemText(IDC_EFFECT_SCALE_Z_EDIT, str);
	str.Format(L"%f", m_tParticleDesc.vRotation.z);
	SetDlgItemText(IDC_EFFECT_ROTATION_X_EDIT, str);
	str.Format(L"%f", m_tParticleDesc.vRotation.z);
	SetDlgItemText(IDC_EFFECT_ROTATION_Y_EDIT, str);
	str.Format(L"%f", m_tParticleDesc.vRotation.z);
	SetDlgItemText(IDC_EFFECT_ROTATION_Z_EDIT, str);
	str.Format(L"%f", m_tParticleDesc.tPointInst_Desc.vDirection.x);
	SetDlgItemText(IDC_EFFECT_DIRECTION_X_EDIT, str);
	str.Format(L"%f", m_tParticleDesc.tPointInst_Desc.vDirection.y);
	SetDlgItemText(IDC_EFFECT_DIRECTION_Y_EDIT, str);
	str.Format(L"%f", m_tParticleDesc.tPointInst_Desc.vDirection.z);
	SetDlgItemText(IDC_EFFECT_DIRECTION_Z_EDIT, str);
	str.Format(L"%f", m_tParticleDesc.tPointInst_Desc.fDirectionDensity);
	SetDlgItemText(IDC_EFFECT_DIRECTION_DENSITY_EDIT, str);
	str.Format(L"%f", m_tParticleDesc.tPointInst_Desc.fLifeTime);
	SetDlgItemText(IDC_EFFECT_LIFETIME_EDIT, str);
	((CButton*)GetDlgItem(IDC_EFFECT_DIRECTION_DENSITY_EDIT))->SetCheck(m_tParticleDesc.tPointInst_Desc.iLifeTimeDensity);
	str.Format(L"%f", m_tParticleDesc.tPointInst_Desc.fLifeTimeDensityEnd);
	SetDlgItemText(IDC_EFFECT_LIFETIME_DENSITY_END_EDIT, str);



	str.Format(L"%d", m_tParticleDesc.tPointInst_Desc.iNumber);
	SetDlgItemText(IDC_EFFECT_NUMBER_EDIT, str);
	str.Format(L"%f", m_tParticleDesc.tPointInst_Desc.fSpeed);
	SetDlgItemText(IDC_EFFECT_SPEED_EDIT, str);
	str.Format(L"%f", m_tParticleDesc.tPointInst_Desc.fSpeedDensity);
	SetDlgItemText(IDC_EFFECT_SPEED_DENSITY_EDIT, str);
	str.Format(L"%f", m_tParticleDesc.tPointInst_Desc.fDensity);
	SetDlgItemText(IDC_EFFECT_DENSITY_EDIT, str);
	str.Format(L"%d", m_tParticleDesc.tPointInst_Desc.iWidth);
	SetDlgItemText(IDC_EFFECT_WIDTH_EDIT, str);
	str.Format(L"%d", m_tParticleDesc.tPointInst_Desc.iHeight);
	SetDlgItemText(IDC_EFFECT_HEIGHT_EDIT, str);
	str.Format(L"%f", m_tParticleDesc.tPointInst_Desc.fSpriteInterval);
	SetDlgItemText(IDC_EFFECT_SPRITE_INTERVAL_EDIT, str);
	((CButton*)GetDlgItem(IDC_EFFECT_REPEAT))->SetCheck(m_tParticleDesc.tPointInst_Desc.bRepeat);
	((CButton*)GetDlgItem(IDC_EFFECT_MULTI_TEXTURE_SPRITE_REPEAT))->SetCheck(m_tParticleDesc.bMultiTextureRepeat);
	str.Format(L"%f", m_tParticleDesc.fMultiTextureInterval);
	SetDlgItemText(IDC_EFFECT_MULTI_TEXTURE_INTERVAL_EDIT, str);
	((CButton*)GetDlgItem(IDC_EFFECT_FADE_IN_CHECK))->SetCheck(m_tParticleDesc.tPointInst_Desc.bFadeIn);
	str.Format(L"%f", m_tParticleDesc.tPointInst_Desc.fFadeInTime);
	SetDlgItemText(IDC_EFFECT_FADE_IN_EDIT, str);
	str.Format(L"%f", m_tParticleDesc.tPointInst_Desc.fFadeOutTime);
	SetDlgItemText(IDC_EFFECT_FADE_OUT_EDIT, str);
	str.Format(L"%f", m_tParticleDesc.tPointInst_Desc.fFadeOutEndTime);
	SetDlgItemText(IDC_EFFECT_FADE_OUT_END_EDIT, str);
	str.Format(L"%f", m_tParticleDesc.tPointInst_Desc.fPSize);
	SetDlgItemText(IDC_EFFECT_PSIZE_EDIT, str);

}

void CEffectTab::Update_CurrentParticle()
{
	if (!m_pCurrentParticle) return;

	dynamic_cast<CToolParticle*>(m_pCurrentParticle)->Update_ByDesc(m_tParticleDesc);

	for (auto& pIter : m_tGroupParticleDesc.listParticleDesc)
	{
		// Exclude Changing names... 
		if (!_tcscmp(pIter->szParticleName, m_tParticleDesc.szParticleName))
		{
			_tcscpy_s(pIter->szParticleName, m_tParticleDesc.szParticleName);
			memcpy(&pIter->vPosition, &m_tParticleDesc.vPosition, sizeof(_float3));
			memcpy(&pIter->vScale, &m_tParticleDesc.vScale, sizeof(_float3));
			memcpy(&pIter->vRotation, &m_tParticleDesc.vRotation, sizeof(_float3));

			memcpy(&pIter->iTextureNumber, &m_tParticleDesc.iTextureNumber, sizeof(_int));
			_tcscpy_s(pIter->szTextureName, m_tParticleDesc.szTextureName);
			_tcscpy_s(pIter->szTextureProto, m_tParticleDesc.szTextureProto);
			_tcscpy_s(pIter->szTexturePath, m_tParticleDesc.szTexturePath);

			memcpy(&pIter->fMultiTextureInterval, &m_tParticleDesc.fMultiTextureInterval, sizeof(_float));
			memcpy(&pIter->bMultiTextureRepeat, &m_tParticleDesc.bMultiTextureRepeat, sizeof(_bool));

			if (pIter->tParticleShaderDesc.vecShaderName.size() > 0)
			{
				for (_int i = 0; i < pIter->tParticleShaderDesc.vecShaderName.size(); i++)
				{
					Safe_Delete(pIter->tParticleShaderDesc.vecShaderName[i]);
				}
				pIter->tParticleShaderDesc.vecShaderName.clear();
			}

			for (_int i = 0; i < m_tParticleDesc.tParticleShaderDesc.vecShaderName.size(); ++i)
			{
				SHADERNAME* pShaderName = new SHADERNAME;
				_tcscpy_s(pShaderName->szName, m_tParticleDesc.tParticleShaderDesc.vecShaderName[i]->szName);
				pIter->tParticleShaderDesc.vecShaderName.emplace_back(pShaderName);
			}


			//memcpy(pIter, &m_tParticleDesc, sizeof(PARTICLEDESC));
			break;
		}
	}
}

void CEffectTab::Add_Particle(CGameObject * pGameObj)
{
	m_listParticle.emplace_back(pGameObj);
	Safe_AddRef(pGameObj);

	PARTICLEDESC* pDesc = new PARTICLEDESC;
	memcpy(pDesc, &dynamic_cast<CToolParticle*>(pGameObj)->Get_Desc(), sizeof(PARTICLEDESC));
	m_tGroupParticleDesc.listParticleDesc.emplace_back(pDesc);

}

void CEffectTab::Add_Particle_ToList(CGameObject * pGameObj)
{
	m_listParticle.emplace_back(pGameObj);
	Safe_AddRef(pGameObj);
}

void CEffectTab::Update_ParticleListBox_WithObjList()
{
	_int iCount = m_ListBox_Particle.GetCount();
	for (_int i = 0; i < iCount; ++i)
		m_ListBox_Particle.DeleteString(0);

	for (auto& pIter : m_listParticle)
	{
		m_ListBox_Particle.AddString(dynamic_cast<CToolParticle*>(pIter)->Get_Desc().szParticleName);
	}
}

void CEffectTab::Create_From_GroupList_FromLoad()
{
	for (auto& pIter : m_tGroupParticleDesc.listParticleDesc)
	{
		Create_Particle_FromLoad(*pIter);
	}

	Update_ParticleListBox_WithObjList();
}

void CEffectTab::Init()
{
	for (auto& pIter : m_tGroupParticleDesc.listParticleDesc)
	{
		Safe_Delete(pIter);
	}
	m_tGroupParticleDesc.listParticleDesc.clear();

	if (m_pCurrentParticle)
	{
		Safe_Release(m_pCurrentParticle);
		m_pCurrentParticle = nullptr;
	}

	for (auto& pIter : m_listParticle)
	{
		pIter->Set_State(OBJSTATE::DEAD);
		Safe_Release(pIter);
	}
	m_listParticle.clear();
}

void CEffectTab::OnEnChangeEffectPositionXEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");
	GetDlgItemText(IDC_EFFECT_POSITION_X_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;

	m_tParticleDesc.vPosition.x = _tstof(strVal);
	Update_CurrentParticle();

}


void CEffectTab::OnEnChangeEffectPositionYEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");
	GetDlgItemText(IDC_EFFECT_POSITION_Y_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;

	m_tParticleDesc.vPosition.y = _tstof(strVal);
	Update_CurrentParticle();

}


void CEffectTab::OnEnChangeEffectPositionZEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");
	GetDlgItemText(IDC_EFFECT_POSITION_Z_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;

	m_tParticleDesc.vPosition.z = _tstof(strVal);
	Update_CurrentParticle();

}


void CEffectTab::OnEnChangeEffectScaleXEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");
	GetDlgItemText(IDC_EFFECT_SCALE_X_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;

	m_tParticleDesc.vScale.x = _tstof(strVal);
	Update_CurrentParticle();

}


void CEffectTab::OnEnChangeEffectScaleYEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");
	GetDlgItemText(IDC_EFFECT_SCALE_Y_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;

	m_tParticleDesc.vScale.y = _tstof(strVal);
	Update_CurrentParticle();

}


void CEffectTab::OnEnChangeEffectScaleZEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");
	GetDlgItemText(IDC_EFFECT_SCALE_Z_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;

	m_tParticleDesc.vScale.z = _tstof(strVal);
	Update_CurrentParticle();

}


void CEffectTab::OnEnChangeEffectRotationXEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");
	GetDlgItemText(IDC_EFFECT_ROTATION_X_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;

	m_tParticleDesc.vRotation.x = _tstof(strVal);
	Update_CurrentParticle();

}


void CEffectTab::OnEnChangeEffectRotationYEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");
	GetDlgItemText(IDC_EFFECT_ROTATION_Y_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;

	m_tParticleDesc.vRotation.y = _tstof(strVal);
	Update_CurrentParticle();

}


void CEffectTab::OnEnChangeEffectRotationZEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");
	GetDlgItemText(IDC_EFFECT_ROTATION_Z_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;

	m_tParticleDesc.vRotation.z = _tstof(strVal);
	Update_CurrentParticle();

}


void CEffectTab::OnEnChangeEffectDirectionXEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");
	GetDlgItemText(IDC_EFFECT_DIRECTION_X_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;

	m_tParticleDesc.tPointInst_Desc.vDirection.x = _tstof(strVal);
	Update_CurrentParticle();

}


void CEffectTab::OnEnChangeEffectDirectionYEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");
	GetDlgItemText(IDC_EFFECT_DIRECTION_Y_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;

	m_tParticleDesc.tPointInst_Desc.vDirection.y = _tstof(strVal);
	Update_CurrentParticle();

}


void CEffectTab::OnEnChangeEffectDirectionZEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");
	GetDlgItemText(IDC_EFFECT_DIRECTION_Z_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;

	m_tParticleDesc.tPointInst_Desc.vDirection.z = _tstof(strVal);
	Update_CurrentParticle();

}


void CEffectTab::OnEnChangeEffectLifetimeEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");
	GetDlgItemText(IDC_EFFECT_LIFETIME_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;

	m_tParticleDesc.tPointInst_Desc.fLifeTime = _tstof(strVal);
	Update_CurrentParticle();

}


void CEffectTab::OnEnChangeEffectNumberEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");
	GetDlgItemText(IDC_EFFECT_NUMBER_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;

	m_tParticleDesc.tPointInst_Desc.iNumber = _ttoi(strVal);
	Update_CurrentParticle();

}

void CEffectTab::OnEnChangeEffectDensityEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");
	GetDlgItemText(IDC_EFFECT_DENSITY_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;

	m_tParticleDesc.tPointInst_Desc.fDensity = _tstof(strVal);
	Update_CurrentParticle();

}

void CEffectTab::OnEnChangeEffectDirectionDensityEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");
	GetDlgItemText(IDC_EFFECT_DIRECTION_DENSITY_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;

	m_tParticleDesc.tPointInst_Desc.fDirectionDensity = _tstof(strVal);
	Update_CurrentParticle();

}

void CEffectTab::OnEnChangeEffectSpeedEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");
	GetDlgItemText(IDC_EFFECT_SPEED_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;

	m_tParticleDesc.tPointInst_Desc.fSpeed = _tstof(strVal);
	Update_CurrentParticle();

}

void CEffectTab::OnEnChangeEffectSpeedDensityEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");
	GetDlgItemText(IDC_EFFECT_SPEED_DENSITY_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;

	m_tParticleDesc.tPointInst_Desc.fSpeedDensity = _tstof(strVal);
	Update_CurrentParticle();

}



void CEffectTab::OnEnChangeEffectLifetimeDensityEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");
	GetDlgItemText(IDC_EFFECT_SPEED_DENSITY_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;

	m_tParticleDesc.tPointInst_Desc.fSpeedDensity = _tstof(strVal);
	Update_CurrentParticle();

}

void CEffectTab::OnEnChangeEffectWidthEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");
	GetDlgItemText(IDC_EFFECT_WIDTH_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;

	m_tParticleDesc.tPointInst_Desc.iWidth = _ttoi(strVal);
	Update_CurrentParticle();

}



void CEffectTab::OnEnChangeEffectHeightEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");
	GetDlgItemText(IDC_EFFECT_HEIGHT_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;

	m_tParticleDesc.tPointInst_Desc.iHeight = _ttoi(strVal);
	Update_CurrentParticle();

}


void CEffectTab::OnEnChangeEffectSpriteIntervalEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");
	GetDlgItemText(IDC_EFFECT_SPRITE_INTERVAL_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;

	m_tParticleDesc.tPointInst_Desc.fSpriteInterval = _tstof(strVal);
	Update_CurrentParticle();

}




void CEffectTab::OnBnClickedEffectCreateButton()
{
	Create_Particle(m_tParticleDesc);
}

void CEffectTab::Create_Particle(PARTICLEDESC & _tDesc)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	CString strVal;
	GetDlgItemText(IDC_EFFECT_PARTICLE_NAME_EDIT, strVal);

	if (LB_ERR != m_ListBox_Particle.FindString(0, strVal)) return;
	m_ListBox_Particle.AddString(strVal);
	TCHAR* szName = (TCHAR*)(LPCTSTR)strVal;

	_tcscpy_s(_tDesc.szParticleName, szName);

	if (_tDesc.tPointInst_Desc.iLifeTimeDensity == IDC_EFFECT_LIFETIME_DENSITY_ON_RADIO
		|| _tDesc.tPointInst_Desc.iLifeTimeDensity == 1)
		_tDesc.tPointInst_Desc.iLifeTimeDensity = 1;
	else
		_tDesc.tPointInst_Desc.iLifeTimeDensity = 0;

	if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr((_uint)LEVEL::STATIC, TEXT("Prototype_GameObject_ToolParticle"), TEXT("Layer_Particle"), (CGameObject**)&m_pCurrentParticle, &_tDesc)))
		assert(false);
	Safe_AddRef(m_pCurrentParticle);

	Add_Particle(m_pCurrentParticle);
}

void CEffectTab::Create_Particle_FromLoad(PARTICLEDESC & _tDesc)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	if (m_pCurrentParticle)
		Safe_Release(m_pCurrentParticle);
	m_pCurrentParticle = nullptr;

	if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr((_uint)LEVEL::STATIC, TEXT("Prototype_GameObject_ToolParticle"), TEXT("Layer_Particle"), (CGameObject**)&m_pCurrentParticle, &_tDesc)))
		assert(false);
	Safe_AddRef(m_pCurrentParticle);

	Add_Particle_ToList(m_pCurrentParticle);
}


void CEffectTab::OnDeltaposEffectPositionXSpinCtrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateData(TRUE);

	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	if (pNMUpDown->iDelta < 0)
		m_tParticleDesc.vPosition.x += 0.1f;
	else
		m_tParticleDesc.vPosition.x -= 0.1f;

	CString str = {};
	str.Format(_T("%f"), m_tParticleDesc.vPosition.x);

	SetDlgItemText(IDC_EFFECT_POSITION_X_EDIT, str);
	Update_CurrentParticle();

	UpdateData(FALSE);
}


void CEffectTab::OnDeltaposEffectPositionYSpinCtrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateData(TRUE);

	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	if (pNMUpDown->iDelta < 0)
		m_tParticleDesc.vPosition.y += 0.1f;
	else
		m_tParticleDesc.vPosition.y -= 0.1f;

	CString str = {};
	str.Format(_T("%f"), m_tParticleDesc.vPosition.y);

	SetDlgItemText(IDC_EFFECT_POSITION_Y_EDIT, str);
	Update_CurrentParticle();

	UpdateData(FALSE);
}


void CEffectTab::OnDeltaposEffectPositionZSpinCtrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateData(TRUE);

	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	if (pNMUpDown->iDelta < 0)
		m_tParticleDesc.vPosition.z += 0.1f;
	else
		m_tParticleDesc.vPosition.z -= 0.1f;

	CString str = {};
	str.Format(_T("%f"), m_tParticleDesc.vPosition.z);

	SetDlgItemText(IDC_EFFECT_POSITION_Z_EDIT, str);
	Update_CurrentParticle();

	UpdateData(FALSE);
}


void CEffectTab::OnDeltaposEffectScaleXSpinCtrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateData(TRUE);

	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	if (pNMUpDown->iDelta < 0)
		m_tParticleDesc.vScale.x += 0.1f;
	else
		m_tParticleDesc.vScale.x -= 0.1f;

	CString str = {};
	str.Format(_T("%f"), m_tParticleDesc.vScale.x);

	SetDlgItemText(IDC_EFFECT_SCALE_X_EDIT, str);
	Update_CurrentParticle();

	UpdateData(FALSE);
}


void CEffectTab::OnDeltaposEffectScaleYSpinCtrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateData(TRUE);

	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	if (pNMUpDown->iDelta < 0)
		m_tParticleDesc.vScale.y += 0.1f;
	else
		m_tParticleDesc.vScale.y -= 0.1f;

	CString str = {};
	str.Format(_T("%f"), m_tParticleDesc.vScale.y);

	SetDlgItemText(IDC_EFFECT_SCALE_Y_EDIT, str);
	Update_CurrentParticle();

	UpdateData(FALSE);
}


void CEffectTab::OnDeltaposEffectScaleZSpinCtrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateData(TRUE);

	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	if (pNMUpDown->iDelta < 0)
		m_tParticleDesc.vScale.z += 0.1f;
	else
		m_tParticleDesc.vScale.z -= 0.1f;

	CString str = {};
	str.Format(_T("%f"), m_tParticleDesc.vScale.z);

	SetDlgItemText(IDC_EFFECT_SCALE_Z_EDIT, str);
	Update_CurrentParticle();

	UpdateData(FALSE);
}


void CEffectTab::OnDeltaposEffectRotationXSpinCtrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateData(TRUE);

	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	if (pNMUpDown->iDelta < 0)
		m_tParticleDesc.vRotation.x += 0.1f;
	else
		m_tParticleDesc.vRotation.x -= 0.1f;

	CString str = {};
	str.Format(_T("%f"), m_tParticleDesc.vRotation.x);

	SetDlgItemText(IDC_EFFECT_ROTATION_X_EDIT, str);
	Update_CurrentParticle();

	UpdateData(FALSE);
}


void CEffectTab::OnDeltaposEffectRotationYSpinCtrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateData(TRUE);

	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	if (pNMUpDown->iDelta < 0)
		m_tParticleDesc.vRotation.y += 0.1f;
	else
		m_tParticleDesc.vRotation.y -= 0.1f;

	CString str = {};
	str.Format(_T("%f"), m_tParticleDesc.vRotation.y);

	SetDlgItemText(IDC_EFFECT_ROTATION_Y_EDIT, str);
	Update_CurrentParticle();

	UpdateData(FALSE);
}


void CEffectTab::OnDeltaposEffectRotationZSpinCtrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateData(TRUE);

	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	if (pNMUpDown->iDelta < 0)
		m_tParticleDesc.vRotation.z += 0.1f;
	else
		m_tParticleDesc.vRotation.z -= 0.1f;

	CString str = {};
	str.Format(_T("%f"), m_tParticleDesc.vRotation.z);

	SetDlgItemText(IDC_EFFECT_ROTATION_Z_EDIT, str);
	Update_CurrentParticle();

	UpdateData(FALSE);
}


void CEffectTab::OnDeltaposEffectDirectionXSpinCtrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateData(TRUE);

	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	if (pNMUpDown->iDelta < 0)
		m_tParticleDesc.tPointInst_Desc.vDirection.x += 0.1f;
	else
		m_tParticleDesc.tPointInst_Desc.vDirection.x -= 0.1f;

	CString str = {};
	str.Format(_T("%f"), m_tParticleDesc.tPointInst_Desc.vDirection.x);

	SetDlgItemText(IDC_EFFECT_DIRECTION_X_EDIT, str);
	Update_CurrentParticle();

	UpdateData(FALSE);
}

void CEffectTab::OnDeltaposEffectDirectionYSpinCtrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateData(TRUE);

	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	if (pNMUpDown->iDelta < 0)
		m_tParticleDesc.tPointInst_Desc.vDirection.y += 0.1f;
	else
		m_tParticleDesc.tPointInst_Desc.vDirection.y -= 0.1f;

	CString str = {};
	str.Format(_T("%f"), m_tParticleDesc.vRotation.z);

	SetDlgItemText(IDC_EFFECT_DIRECTION_Y_EDIT, str);
	Update_CurrentParticle();

	UpdateData(FALSE);
}


void CEffectTab::OnDeltaposEffectDirectionZSpinCtrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateData(TRUE);

	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	if (pNMUpDown->iDelta < 0)
		m_tParticleDesc.tPointInst_Desc.vDirection.z += 0.1f;
	else
		m_tParticleDesc.tPointInst_Desc.vDirection.z -= 0.1f;

	CString str = {};
	str.Format(_T("%f"), m_tParticleDesc.vRotation.z);

	SetDlgItemText(IDC_EFFECT_DIRECTION_Z_EDIT, str);
	Update_CurrentParticle();

	UpdateData(FALSE);
}


void CEffectTab::OnDeltaposEffectLifetimeSpinCtrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateData(TRUE);

	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	if (pNMUpDown->iDelta < 0)
		m_tParticleDesc.tPointInst_Desc.fLifeTime += 0.1f;
	else
		m_tParticleDesc.tPointInst_Desc.fLifeTime -= 0.1f;

	CString str = {};
	str.Format(_T("%f"), m_tParticleDesc.tPointInst_Desc.fLifeTime);

	SetDlgItemText(IDC_EFFECT_LIFETIME_EDIT, str);
	Update_CurrentParticle();

	UpdateData(FALSE);
}


void CEffectTab::OnDeltaposEffectNumberSpinCtrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateData(TRUE);

	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	if (pNMUpDown->iDelta < 0)
		m_tParticleDesc.tPointInst_Desc.iNumber += 1;
	else
		m_tParticleDesc.tPointInst_Desc.iNumber -= 1;

	CString str = {};
	str.Format(_T("%d"), m_tParticleDesc.tPointInst_Desc.iNumber);

	SetDlgItemText(IDC_EFFECT_DIRECTION_Z_EDIT, str);
	Update_CurrentParticle();

	UpdateData(FALSE);
}







void CEffectTab::OnDeltaposEffectDensitySpinCtrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateData(TRUE);

	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	if (pNMUpDown->iDelta < 0)
		m_tParticleDesc.tPointInst_Desc.fDensity += 0.1f;
	else
		m_tParticleDesc.tPointInst_Desc.fDensity -= 0.1f;

	CString str = {};
	str.Format(_T("%f"), m_tParticleDesc.tPointInst_Desc.fDensity);

	SetDlgItemText(IDC_EFFECT_DENSITY_EDIT, str);
	Update_CurrentParticle();

	UpdateData(FALSE);
}

void CEffectTab::OnDeltaposEffectDirectionDensitySpinCtrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateData(TRUE);

	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	if (pNMUpDown->iDelta < 0)
		m_tParticleDesc.tPointInst_Desc.fDirectionDensity += 0.1f;
	else
		m_tParticleDesc.tPointInst_Desc.fDirectionDensity -= 0.1f;

	CString str = {};
	str.Format(_T("%f"), m_tParticleDesc.tPointInst_Desc.fDirectionDensity);

	SetDlgItemText(IDC_EFFECT_DIRECTION_DENSITY_EDIT, str);
	Update_CurrentParticle();

	UpdateData(FALSE);
}




void CEffectTab::OnDeltaposEffectSpeedSpinCtrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateData(TRUE);

	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	if (pNMUpDown->iDelta < 0)
		m_tParticleDesc.tPointInst_Desc.fSpeed += 0.1f;
	else
		m_tParticleDesc.tPointInst_Desc.fSpeed -= 0.1f;

	CString str = {};
	str.Format(_T("%f"), m_tParticleDesc.tPointInst_Desc.fSpeed);

	SetDlgItemText(IDC_EFFECT_SPEED_EDIT, str);
	Update_CurrentParticle();

	UpdateData(FALSE);
}





void CEffectTab::OnDeltaposEffectSpeedDensitySpinCtrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateData(TRUE);

	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	if (pNMUpDown->iDelta < 0)
		m_tParticleDesc.tPointInst_Desc.fSpeedDensity += 0.1f;
	else
		m_tParticleDesc.tPointInst_Desc.fSpeedDensity -= 0.1f;

	CString str = {};
	str.Format(_T("%f"), m_tParticleDesc.tPointInst_Desc.fSpeedDensity);

	SetDlgItemText(IDC_EFFECT_SPEED_DENSITY_EDIT, str);
	Update_CurrentParticle();

	UpdateData(FALSE);
}

void CEffectTab::OnDeltaposEffectWidthSpinCtrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateData(TRUE);

	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	if (pNMUpDown->iDelta < 0)
		m_tParticleDesc.tPointInst_Desc.iWidth += 1;
	else
		m_tParticleDesc.tPointInst_Desc.iWidth -= 1;

	CString str = {};
	str.Format(_T("%i"), m_tParticleDesc.tPointInst_Desc.iWidth);

	SetDlgItemText(IDC_EFFECT_WIDTH_EDIT, str);
	Update_CurrentParticle();

	UpdateData(FALSE);
}


void CEffectTab::OnDeltaposEffectHeightSpinCtrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateData(TRUE);

	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	if (pNMUpDown->iDelta < 0)
		m_tParticleDesc.tPointInst_Desc.iHeight += 1;
	else
		m_tParticleDesc.tPointInst_Desc.iHeight -= 1;

	CString str = {};
	str.Format(_T("%i"), m_tParticleDesc.tPointInst_Desc.iHeight);

	SetDlgItemText(IDC_EFFECT_HEIGHT_EDIT, str);
	Update_CurrentParticle();

	UpdateData(FALSE);
}


void CEffectTab::OnDeltaposEffectSpriteIntervalSpinCtrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateData(TRUE);

	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	if (pNMUpDown->iDelta < 0)
		m_tParticleDesc.tPointInst_Desc.fSpriteInterval += 0.1f;
	else
		m_tParticleDesc.tPointInst_Desc.fSpriteInterval -= 0.1f;

	CString str = {};
	str.Format(_T("%f"), m_tParticleDesc.tPointInst_Desc.fSpriteInterval);

	SetDlgItemText(IDC_EFFECT_SPRITE_INTERVAL_EDIT, str);
	Update_CurrentParticle();

	UpdateData(FALSE);
}


void CEffectTab::OnBnClickedEffectOpenTexture()
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

		

		pToolView->m_pMainTab->m_pEffectMeshTab->m_vecTexProtoPath.push_back(pTexProtoPath);

		wstrPath = wstrPath.Mid(wstrPath.Find(L"\\Bin"));
		CString TwoDot = L"..\\Client";

		wstrPath = TwoDot + wstrPath;

		TCHAR* szName = (TCHAR*)(LPCTSTR)wstrPath;
		_tcscpy_s(pTexProtoPath->szPath, szName);
		szName = (TCHAR*)(LPCTSTR)FinalProtoName;
		_tcscpy_s(pTexProtoPath->szProto, szName);
		szName = (TCHAR*)(LPCTSTR)FileNameWithExt;
		_tcscpy_s(pTexProtoPath->szName, szName);
		pTexProtoPath->iTextureNumber = m_tParticleDesc.iTextureNumber;

		if (pTexProtoPath->iTextureNumber > 1)
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
		if (FAILED(m_pGameInstance->Add_Component((_uint)LEVEL::STATIC, pTexProtoPath->szProto, CTexture::Create(pToolView->Get_GraphicDevice(), pToolView->Get_ContextDevice(), pTexProtoPath->szPath, pTexProtoPath->iTextureNumber))))
		{
			int a = 0;
			//assert(false);
		}

		m_ListBox_Texture.AddString(pTexProtoPath->szName);
	}
}


BOOL CEffectTab::OnInitDialog()
{
	CDialog::OnInitDialog();

	//Load_FilePath();
	//Load_Meshes(TEXT("../Client/Bin/Data/EffectMeshData"));
	return TRUE;
}


void CEffectTab::OnLbnSelchangeEffectTexturelist()
{
	UpdateData(TRUE);

	int index = m_ListBox_Texture.GetCurSel();//리스트 박스에서 현재 선택된 행의 인덱스를 받아온다.
	m_ListBox_Texture.GetText(index, m_strCurrentTexture);
}







void CEffectTab::OnBnClicked_LifeTimeDensity(UINT _uiID)
{
	m_tParticleDesc.tPointInst_Desc.iLifeTimeDensity = _uiID;

	Update_CurrentParticle();

}

void CEffectTab::OnEnChangeEffectTextureNumberEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");

	GetDlgItemText(IDC_EFFECT_TEXTURE_NUMBER_EDIT, strVal);

	m_tParticleDesc.iTextureNumber = _ttoi(strVal);

	SetDlgItemText(IDC_EFFECT_TEXTURE_NUMBER_RESULT, strVal);

	UpdateData(FALSE);

	Update_CurrentParticle();

}


void CEffectTab::OnBnClickedEffectDeleteButton()
{
	if (!m_pCurrentParticle) return;

	list<CGameObject*>::iterator iter;

	for (iter = m_listParticle.begin(); iter != m_listParticle.end(); ++iter)
	{
		if (!_tcscmp(dynamic_cast<CToolParticle*>(*iter)->Get_Desc().szParticleName, dynamic_cast<CToolParticle*>(m_pCurrentParticle)->Get_Desc().szParticleName))
		{
			break;
		}
	}

	if (iter != m_listParticle.end())
	{
		Safe_Release(*iter);
		m_listParticle.erase(iter);
	}
	else return;


	list<PARTICLEDESC*>::iterator descIter;

	for (descIter = m_tGroupParticleDesc.listParticleDesc.begin(); descIter != m_tGroupParticleDesc.listParticleDesc.end(); ++descIter)
	{
		if (!_tcscmp((*descIter)->szParticleName, dynamic_cast<CToolParticle*>(m_pCurrentParticle)->Get_Desc().szParticleName))
		{
			break;
		}
	}

	if (descIter != m_tGroupParticleDesc.listParticleDesc.end())
	{
		Safe_Delete(*descIter);
		m_tGroupParticleDesc.listParticleDesc.erase(descIter);
	}


	m_ListBox_Particle.DeleteString(m_ListBox_Particle.FindString(-1, dynamic_cast<CToolParticle*>(m_pCurrentParticle)->Get_Desc().szParticleName));

	m_pCurrentParticle->Set_State(OBJSTATE::DEAD);

	Safe_Release(m_pCurrentParticle);
	m_pCurrentParticle = nullptr;
}

void CEffectTab::OnEnChangeEffectParticleNameEdit()
{
	UpdateData(TRUE);
	UpdateData(FALSE);
}


void CEffectTab::OnBnClickedEffectApplyName()
{
	UpdateData(TRUE);

	CString strVal = _T("");
	GetDlgItemText(IDC_EFFECT_PARTICLE_NAME_EDIT, strVal);
	TCHAR* szName = (TCHAR*)(LPCTSTR)strVal;
	_tcscpy_s(m_tParticleDesc.szParticleName, szName);
	SetDlgItemText(IDC_EFFECT_PARTICLE_NAME_RESULT, strVal);

	UpdateData(FALSE);
}


void CEffectTab::OnBnClickedEffectApplyTextureButton()
{
	CToolView* pToolView = GET_TOOLVIEW;


	TCHAR* szName = (TCHAR*)(LPCTSTR)m_strCurrentTexture;

	_tcscpy_s(m_tParticleDesc.szTextureName, szName);
	SetDlgItemText(IDC_EFFECT_CURRENT_TEXTURE_RESULT, m_tParticleDesc.szTextureName);

	for (auto& pIter : pToolView->m_pMainTab->m_pEffectMeshTab->m_vecTexProtoPath)
	{
		if (!_tcscmp(pIter->szName, m_tParticleDesc.szTextureName))
		{
			_tcscpy_s(m_tParticleDesc.szTexturePath, pIter->szPath);
			_tcscpy_s(m_tParticleDesc.szTextureProto, pIter->szProto);
			break;
		}
	}

	if (m_pCurrentParticle) Update_CurrentParticle();

	UpdateData(FALSE);
}


void CEffectTab::OnBnClickedEffectRepeat()
{
	UpdateData(TRUE);

	if (((CButton*)GetDlgItem(IDC_EFFECT_REPEAT))->GetCheck())
		m_tParticleDesc.tPointInst_Desc.bRepeat = true;
	else
		m_tParticleDesc.tPointInst_Desc.bRepeat = false;

	UpdateData(FALSE);

	if (m_pCurrentParticle) Update_CurrentParticle();
}


void CEffectTab::OnEnChangeEffectMultiTextureIntervalEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");
	GetDlgItemText(IDC_EFFECT_MULTI_TEXTURE_INTERVAL_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;

	m_tParticleDesc.fMultiTextureInterval = _tstof(strVal);

	Update_CurrentParticle();
}


void CEffectTab::OnBnClickedEffectPlayButton()
{
	//if (!m_pCurrentParticle) return;

	//dynamic_cast<CToolParticle*>(m_pCurrentParticle)->Play();

	CToolView* pToolView = GET_TOOLVIEW;

	pToolView->m_pMainTab->m_pEffectMeshTab->Play();
	Play();
}


void CEffectTab::OnBnClickedEffectStopButton()
{
	CToolView* pToolView = GET_TOOLVIEW;

	pToolView->m_pMainTab->m_pEffectMeshTab->Stop();

	Stop();
}

void CEffectTab::Play()
{
	for (auto& pIter : m_listParticle)
	{
		dynamic_cast<CToolParticle*>(pIter)->Play();
	}
}

void CEffectTab::Stop()
{
	for (auto& pIter : m_listParticle)
	{
		dynamic_cast<CToolParticle*>(pIter)->Stop();
	}
}


void CEffectTab::OnBnClickedEffectFadeInCheck()
{
	UpdateData(TRUE);

	if (((CButton*)GetDlgItem(IDC_EFFECT_FADE_IN_CHECK))->GetCheck())
		m_tParticleDesc.tPointInst_Desc.bFadeIn = true;
	else
		m_tParticleDesc.tPointInst_Desc.bFadeIn = false;

	UpdateData(FALSE);

	if (m_pCurrentParticle) Update_CurrentParticle();
}


void CEffectTab::OnEnChangeEffectFadeInEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");
	GetDlgItemText(IDC_EFFECT_FADE_IN_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;

	m_tParticleDesc.tPointInst_Desc.fFadeInTime = _tstof(strVal);

	Update_CurrentParticle();
}


void CEffectTab::OnBnClickedEffectFadeOutCheck()
{
	UpdateData(TRUE);

	if (((CButton*)GetDlgItem(IDC_EFFECT_FADE_OUT_CHECK))->GetCheck())
		m_tParticleDesc.tPointInst_Desc.bFadeOut = true;
	else
		m_tParticleDesc.tPointInst_Desc.bFadeOut = false;

	UpdateData(FALSE);

	if (m_pCurrentParticle) Update_CurrentParticle();
}


void CEffectTab::OnEnChangeEffectFadeOutEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");
	GetDlgItemText(IDC_EFFECT_FADE_OUT_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;

	m_tParticleDesc.tPointInst_Desc.fFadeOutTime = _tstof(strVal);

	Update_CurrentParticle();
}
void CEffectTab::OnEnChangeEffectFadeOutEndEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");
	GetDlgItemText(IDC_EFFECT_FADE_OUT_END_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("0") || strVal.Right(1) > _T("9")) return;

	m_tParticleDesc.tPointInst_Desc.fFadeOutEndTime = _tstof(strVal);

	Update_CurrentParticle();
}



void CEffectTab::OnBnClickedEffectMultiTextureSpriteRepeat()
{
	UpdateData(TRUE);

	if (((CButton*)GetDlgItem(IDC_EFFECT_FADE_OUT_CHECK))->GetCheck())
		m_tParticleDesc.bMultiTextureRepeat = true;
	else
		m_tParticleDesc.bMultiTextureRepeat = false;

	UpdateData(FALSE);

	if (m_pCurrentParticle) Update_CurrentParticle();
}


void CEffectTab::OnLbnSelchangeEffectParticleList()
{
	UpdateData(TRUE);
	CString strCurParticle;
	int index = m_ListBox_Particle.GetCurSel();//리스트 박스에서 현재 선택된 행의 인덱스를 받아온다.
	m_ListBox_Particle.GetText(index, strCurParticle);

	TCHAR* szName = (TCHAR*)(LPCTSTR)strCurParticle;


	if (m_pCurrentParticle)
	{
		Safe_Release(m_pCurrentParticle);
		m_pCurrentParticle = nullptr;
	}

	for (auto& pIter : m_listParticle)
	{
		if (!_tcscmp(dynamic_cast<CToolParticle*>(pIter)->Get_Desc().szParticleName, szName))
		{
			m_pCurrentParticle = pIter;
			Safe_AddRef(m_pCurrentParticle);
			break;
		}
	}

	if (m_pCurrentParticle)
	{
		Update_Tool_ByDesc();
	}

}


void CEffectTab::OnBnClickedEffectSaveButton()
{
	CToolView* pToolView = GET_TOOLVIEW;






}


void CEffectTab::OnBnClickedEffectLoadButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}




void CEffectTab::OnEnChangeEffectLifetimeDensityEndEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");
	GetDlgItemText(IDC_EFFECT_LIFETIME_DENSITY_END_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("1") || strVal.Right(1) > _T("9")) return;

	m_tParticleDesc.tPointInst_Desc.fLifeTimeDensityEnd = _tstof(strVal);
	Update_CurrentParticle();
}


void CEffectTab::OnEnChangeEffectPsizeEdit()
{
	UpdateData(TRUE);

	CString strVal = _T("");
	GetDlgItemText(IDC_EFFECT_PSIZE_EDIT, strVal);

	if (!(strVal.GetLength() > 0)) return;
	if (strVal.Right(1) < _T("1") || strVal.Right(1) > _T("9")) return;

	m_tParticleDesc.tPointInst_Desc.fPSize = _tstof(strVal);
	Update_CurrentParticle();
}


void CEffectTab::OnBnClickedEffectUpdateShaderButton()
{
	
	if (m_tParticleDesc.tParticleShaderDesc.vecShaderName.size() > 0)
	{
		for (_int i = 0; i < m_tParticleDesc.tParticleShaderDesc.vecShaderName.size(); ++i)
			Safe_Delete(m_tParticleDesc.tParticleShaderDesc.vecShaderName[i]);

		m_tParticleDesc.tParticleShaderDesc.vecShaderName.clear();
	}



	CToolView* pToolView = GET_TOOLVIEW;

	for (auto& pIter : pToolView->m_pMainTab->m_pEffectMesh2Tab->m_listShaderName)
	{
		SHADERNAME* tShaderName = new SHADERNAME;
		_tcscpy_s(tShaderName->szName, pIter->szName);
		m_tParticleDesc.tParticleShaderDesc.vecShaderName.push_back(tShaderName);
	}
	Update_CurrentParticle();
}
