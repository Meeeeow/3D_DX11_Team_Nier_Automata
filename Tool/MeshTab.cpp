// MeshTab.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool.h"
#include "MeshTab.h"
#include "afxdialogex.h"
#include "MainFrm.h"
#include "ToolView.h"
#include "ToolModel.h"
#include "GameInstance.h"

#include "MainTab.h"
#include "ToolObject.h"

#include "Animation.h"
#include "AnimationTab.h"

// CMeshTab 대화 상자입니다.

IMPLEMENT_DYNAMIC(CMeshTab, CDialog)

CMeshTab::CMeshTab(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_MESHTAB, pParent)
	, m_strCurModelTag(_T(""))
	, m_strObjectName(_T(""))
	, m_fRotationDegree(0)
{

}

CMeshTab::~CMeshTab()
{
	Safe_Release(m_pGraphicDevice);
	Safe_Release(m_pContextDevice);

	Safe_Release(m_pCurrentModel);
	Safe_Release(m_pCurrentObject);

	m_vecModels.clear();
	m_imagelist.DeleteImageList();
}

void CMeshTab::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, Tree_MeshList, m_TreeCtrl);
	DDX_Control(pDX, Check_Animation, m_CheckAnim);
	DDX_Control(pDX, List_Prototype, m_listPrototype);
	DDX_Text(pDX, Edit_ObjectName, m_strObjectName);
	DDX_Text(pDX, Edit_RotationDegree, m_fRotationDegree);
}


BEGIN_MESSAGE_MAP(CMeshTab, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, Tree_MeshList, &CMeshTab::OnTvnSelchangedMeshlist)
	ON_BN_CLICKED(Button_CreateMesh, &CMeshTab::OnBnClickedCreatemesh)
	ON_BN_CLICKED(Check_Animation, &CMeshTab::OnBnClickedAnimation)
	ON_LBN_SELCHANGE(List_Prototype, &CMeshTab::OnLbnSelchangePrototype)
	ON_BN_CLICKED(Button_InsertToContainer, &CMeshTab::OnBnClickedInserttocontainer)
	ON_BN_CLICKED(Button_DeletePrototype, &CMeshTab::OnBnClickedDeleteprototype)
	ON_BN_CLICKED(Button_LoadPrototype, &CMeshTab::OnBnClickedLoadprototype)
	ON_EN_CHANGE(Edit_ObjectName, &CMeshTab::OnEnChangeObjectname)
	ON_BN_CLICKED(Button_ForAnimationTool, &CMeshTab::OnBnClickedForanimationtool)
	ON_EN_CHANGE(Edit_RotationDegree, &CMeshTab::OnEnChangeRotationdegree)
	ON_BN_CLICKED(Radio_Identity, &CMeshTab::OnBnClickedIdentity)
	ON_BN_CLICKED(Radio_PivotX, &CMeshTab::OnBnClickedPivotx)
	ON_BN_CLICKED(Radio_PivotY, &CMeshTab::OnBnClickedPivoty)
	ON_BN_CLICKED(Radio_PivotZ, &CMeshTab::OnBnClickedPivotz)
	ON_BN_CLICKED(Button_LoadAllModel, &CMeshTab::OnBnClickedLoadallmodel)
END_MESSAGE_MAP()


// CMeshTab 메시지 처리기입니다.


HRESULT CMeshTab::Init_List(CString strFolder, HTREEITEM hRoot, HTREEITEM hMainRoot)
{
	CFileFind		FileFind;

	BOOL			bContinue = FileFind.FindFile(strFolder + L"/*.*");
	CString			strFilter = L".FBX";
	CString			strFolderItem = L"";
	CString			strFileExt = L"";

	while (bContinue)
	{
		bContinue = FileFind.FindNextFile();
		if (!FileFind.IsDots() && FileFind.IsDirectory())
		{
			strFolderItem = FileFind.GetFilePath();
			hRoot = m_TreeCtrl.InsertItem(FileFind.GetFileTitle(), 1, 1, hMainRoot, NULL);
			Init_List(strFolderItem, hRoot, hMainRoot);
		}
		strFolderItem = FileFind.GetFilePath();
		strFileExt = strFolderItem.Mid(strFolderItem.ReverseFind('.'));

		if (!FileFind.IsDots())
		{
			strFileExt.MakeUpper();

			if (lstrcmp(strFileExt, strFilter) != 0)
				continue;

			m_TreeCtrl.InsertItem(FileFind.GetFileTitle(),2,3, hRoot, NULL);
			m_map.emplace(FileFind.GetFileTitle(), strFolder);
		}
	}
	FileFind.Close();
	return S_OK;
}

void CMeshTab::OnTvnSelchangedMeshlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	HTREEITEM	getCurItem = m_TreeCtrl.GetSelectedItem();
	CString		strFileName = m_TreeCtrl.GetItemText(getCurItem);
	
	auto& iter = find_if(m_map.begin(), m_map.end(), CTagFinder(strFileName));
	if (iter == m_map.end())
		return;

	m_strFileName = iter->first;
	m_strFileName += L".FBX";
	m_strFilePath = iter->second;
	m_strFilePath += L"\\";

	CStringA strName, strPath;
	strName = CStringA(m_strFileName);
	strPath = CStringA(m_strFilePath);

	*pResult = 0;
}


_int CMeshTab::Tick(_double dTimeDelta)
{
	return _int();
}

_int CMeshTab::LateTick(_double dTimeDelta)
{
	return _int();
}

HRESULT CMeshTab::Render()
{
	return S_OK;
}

BOOL CMeshTab::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	CBitmap bmp;
	bmp.LoadBitmap(IDB_BITMAP);

	m_imagelist.DeleteImageList();
	m_imagelist.Create(16, 16, ILC_COLOR16 | ILC_MASK, 0, 0);
	m_imagelist.Add(&bmp, RGB(255, 0, 255));
	
	m_TreeCtrl.SetImageList(&m_imagelist, TVSIL_NORMAL);
	m_hFirstRoot = m_TreeCtrl.InsertItem(L"Meshes", 0,0, NULL, NULL);
	m_hEffectRoot = m_TreeCtrl.InsertItem(L"EffectMeshes", 0, 0, NULL, NULL);


	Init_List(TEXT("../Client/Bin/Resources/Meshes"), m_hFirstRoot, m_hFirstRoot);
	Init_List(TEXT("../Client/Bin/Resources/Effect/MeshEffect"), m_hEffectRoot, m_hEffectRoot);

	CMainFrame*	pMain = dynamic_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd());
	CToolView*	pToolView = dynamic_cast<CToolView*>(pMain->GetActiveView());

	m_pGraphicDevice = pToolView->Get_GraphicDevice();
	Safe_AddRef(m_pGraphicDevice);
	m_pContextDevice = pToolView->Get_ContextDevice();
	Safe_AddRef(m_pContextDevice);

	m_pCurrentModel = CToolModel::Create(m_pGraphicDevice, m_pContextDevice);

	m_pCurrentObject = CToolObject::Create(m_pGraphicDevice, m_pContextDevice);


	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CMeshTab::OnBnClickedCreatemesh()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CStringA strFilePath, strFileName;
	strFileName = CStringA(m_strFileName);
	strFilePath = CStringA(m_strFilePath);

	char szFileName[128], szFilePath[128];
	strcpy_s(szFileName, strFileName.GetBuffer());
	strcpy_s(szFilePath, strFilePath.GetBuffer());

	CFileDialog Dlg(FALSE, L"dat", L"*.dat");
	TCHAR szBuf[MAX_PATH]{};
	GetCurrentDirectory(MAX_PATH, szBuf);
	PathRemoveFileSpec(szBuf);
	lstrcat(szBuf, L"\\Client\\Bin\\Data\\");
	Dlg.m_ofn.lpstrInitialDir = szBuf;
	if (IDOK == Dlg.DoModal())
	{ 
		CString wstrPath = Dlg.GetPathName();
		_matrix			matPivot;


		if (m_bIdentity)
		{
			matPivot = XMMatrixIdentity();
		}
		else if (m_bRotateX)
		{
			matPivot = XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(m_fRotationDegree));
		}
		else if (m_bRotateY)
		{
			matPivot = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(m_fRotationDegree));
		}
		else if (m_bRotateZ)
		{ 
			matPivot = XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(m_fRotationDegree));
		}
		else
		{
			matPivot = XMMatrixIdentity();
		}

		CJussimpModel* pModel = CJussimpModel::Create_ForTool(m_pGraphicDevice, m_pContextDevice,
			m_eAnim, szFilePath, szFileName , Shader_Model, DefaultTechnique, wstrPath.GetString(), matPivot);

		Safe_Release(pModel);
	}
}


void CMeshTab::OnBnClickedAnimation()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (TRUE == m_CheckAnim.GetCheck())
		m_eAnim = CJussimpModel::TYPE::ANIM;
	else
		m_eAnim = CJussimpModel::TYPE::NONANIM;
}

void CMeshTab::OnLbnSelchangePrototype()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int iIndex = m_listPrototype.GetCurSel();
	if (iIndex < 0 || iIndex >= m_listPrototype.GetCount())
		return;

	auto Model = m_vecModels.begin() + iIndex;
	UpdateData(TRUE);


	UpdateData(FALSE);
}

HRESULT CMeshTab::Sort_Objects()
{
	/*sort(m_vecModels.begin(), m_vecModels.end(), [](CToolModel* pSour, CToolModel* pDest) {
		if (pSour->Get_ModelObjectInfo().szMOPrototypeName[0] != pDest->Get_ModelObjectInfo().szMOPrototypeName[0])
			return pSour->Get_ModelObjectInfo().szMOPrototypeName[0] < pDest->Get_ModelObjectInfo().szMOPrototypeName[0];
		else if (pSour->Get_ModelObjectInfo().szMOPrototypeName[1] != pDest->Get_ModelObjectInfo().szMOPrototypeName[1])
			return pSour->Get_ModelObjectInfo().szMOPrototypeName[1] < pDest->Get_ModelObjectInfo().szMOPrototypeName[1];
		else
			return lstrlen(pSour->Get_ModelObjectInfo().szMOPrototypeName) < lstrlen(pDest->Get_ModelObjectInfo().szMOPrototypeName);
	});

	m_listPrototype.ResetContent();

	for (auto iter : m_vecModels)
		m_listPrototype.AddString(iter->Get_ModelObjectInfo().szMOPrototypeName);*/

	return S_OK;
}

CToolModel * CMeshTab::Find_Object(MO_INFO tObjInfo)
{
	int iIndex = m_listPrototype.FindString(0, tObjInfo.szModelComName);
	auto iter = m_vecModels.begin() + iIndex;

	CToolModel::DESC tDesc;
	lstrcpy(tDesc.szModelTag, iter->GetString());

	return dynamic_cast<CToolModel*>(m_pCurrentModel->Clone(&tDesc));
}


void CMeshTab::OnBnClickedInserttocontainer()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int iIndex = m_listPrototype.GetCurSel();

	if (iIndex < 0 || iIndex >= m_listPrototype.GetCount())
		return;

	CMainFrame*	pMain = dynamic_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd());
	CToolView*	pToolView = dynamic_cast<CToolView*>(pMain->GetActiveView());

	auto iter = m_vecModels.begin() + iIndex;
	CToolModel::DESC tDesc;
	lstrcpy(tDesc.szModelTag, iter->GetString());
	pToolView->m_pMainTab->Insert_MeshObjectToMap(
		(CToolModel*)m_pCurrentModel->Clone(&tDesc), m_strObjectName);

}


void CMeshTab::OnBnClickedDeleteprototype()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int iIndex = m_listPrototype.GetCurSel();

	if (iIndex < 0 || iIndex >= m_listPrototype.GetCount())
		return;


	CMainFrame*	pMain = dynamic_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd());
	CToolView*	pToolView = dynamic_cast<CToolView*>(pMain->GetActiveView());

	m_listPrototype.DeleteString(iIndex);
	m_vecModels.erase(m_vecModels.begin() + iIndex);
}

//
//void CMeshTab::OnBnClickedSaveprototype()
//{
//	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
//
//	TCHAR szBuf[MAX_PATH]{};
//	GetCurrentDirectory(MAX_PATH, szBuf);
//	PathRemoveFileSpec(szBuf);
//	lstrcat(szBuf, L"\\Client\\Bin\\Data\\PrototypeData\\PrototypeList.dat");
//
//	if (FAILED(Sort_Objects()))
//		return;
//
//	CString wstrPath = szBuf;
//
//
//	HANDLE hFile = CreateFile(wstrPath.GetString(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
//
//	if (hFile == INVALID_HANDLE_VALUE)
//		return;
//
//
//	_ulong dwByte = 0;
//	for (auto& GameObjects : m_vecModels)
//	{
//		MC_INFO tModelComInfo = GameObjects->Get_ModelComponentInfo();
//		MO_INFO tModelObjInfo = GameObjects->Get_ModelObjectInfo();
//
//		WriteFile(hFile, &tModelComInfo, sizeof(MC_INFO), &dwByte, nullptr);
//		WriteFile(hFile, &tModelObjInfo, sizeof(MO_INFO), &dwByte, nullptr);
//	}
//	CloseHandle(hFile);
//}


void CMeshTab::OnBnClickedLoadprototype()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFileDialog Dlg(TRUE, L"dat", L"*.dat");
	TCHAR szBuf[MAX_PATH]{};
	GetCurrentDirectory(MAX_PATH, szBuf);
	PathRemoveFileSpec(szBuf);
	lstrcat(szBuf, L"\\Client\\Bin\\Data\\PrototypeData\\");
	Dlg.m_ofn.lpstrInitialDir = szBuf;
	if (IDOK == Dlg.DoModal())
	{
		CString wstrPath = Dlg.GetPathName();
		CString wstrName = Dlg.GetFileTitle();

		CGameInstance* pGameInstance = CGameInstance::Get_Instance();

		if (FAILED(pGameInstance->Add_Component((_uint)LEVEL::STATIC, wstrName.GetString(), CModel::Create(m_pGraphicDevice, m_pContextDevice, wstrPath.GetString(), Shader_Model, DefaultTechnique, true))))
			return;
		m_listPrototype.AddString(wstrName);
		m_vecModels.push_back(wstrName);
	}
}


void CMeshTab::OnEnChangeObjectname()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	UpdateData(FALSE);
}


void CMeshTab::OnBnClickedForanimationtool()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	int iIndex = m_listPrototype.GetCurSel();

	if (iIndex < 0 || iIndex >= m_listPrototype.GetCount())
		return;

	CMainFrame*	pMain = dynamic_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd());
	CToolView*	pToolView = dynamic_cast<CToolView*>(pMain->GetActiveView());

	auto iter = m_vecModels.begin() + iIndex;

	pToolView->m_pMainTab->m_pAnimationTab->Create_Layer(iter->GetString(), dynamic_cast<CToolObject*>(m_pCurrentObject->Clone((void*)iter->GetString())));


}

void CMeshTab::RadioCtrl(UINT ID)
{
	UpdateData(TRUE);

	switch (ID)
	{
	case Radio_Identity:
		m_bIdentity = true;
		m_bRotateX = false;
		m_bRotateY = false;
		m_bRotateZ = false;
		break;
	case Radio_PivotX:
		m_bIdentity = false;
		m_bRotateX = true;
		m_bRotateY = false;
		m_bRotateZ = false;
		break;
	case Radio_PivotY:
		m_bIdentity = false;
		m_bRotateX = false;
		m_bRotateY = true;
		m_bRotateZ = false;
		break;
	case Radio_PivotZ:
		m_bIdentity = false;
		m_bRotateX = false;
		m_bRotateY = false;
		m_bRotateZ = true;
		break;
	default:
		break;
	}
	UpdateData(FALSE);
}


void CMeshTab::OnEnChangeRotationdegree()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);


	UpdateData(FALSE);
}


void CMeshTab::OnBnClickedIdentity()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	RadioCtrl(Radio_Identity);
}


void CMeshTab::OnBnClickedPivotx()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	RadioCtrl(Radio_PivotX);
}


void CMeshTab::OnBnClickedPivoty()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	RadioCtrl(Radio_PivotY);
}


void CMeshTab::OnBnClickedPivotz()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	RadioCtrl(Radio_PivotZ);
}


void CMeshTab::OnBnClickedLoadallmodel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Load_AllModel(L"../Client/Bin/Data/PrototypeData");
}

void CMeshTab::Load_AllModel(CString strPath)
{
	CFileFind			FileFind;

	BOOL				bContinue = FileFind.FindFile(strPath + L"/*.*");
	CString				strFilter = L".DAT";
	CString				strFolderItem = L"";
	CString				strFileExt = L"";

	while (bContinue)
	{
		bContinue = FileFind.FindNextFile();
		if (!FileFind.IsDots() && FileFind.IsDirectory())
		{
			strFolderItem = FileFind.GetFilePath();
			Load_AllModel(strFolderItem);
		}
		strFolderItem = FileFind.GetFilePath();
		strFileExt = strFolderItem.Mid(strFolderItem.ReverseFind('.'));

		if (!FileFind.IsDots())
		{
			strFileExt.MakeUpper();

			if (FileFind.IsDirectory())
				continue;

			if (lstrcmp(strFileExt, strFilter) != 0)
				continue;

			CString strItem = FileFind.GetFileTitle();

			CGameInstance* pGameInstance = CGameInstance::Get_Instance();

			m_listPrototype.AddString(strItem.GetString());
			m_vecModels.push_back(strItem.GetString());

			m_vecModels.size();
			auto& iter = m_vecModels.begin() + (m_vecModels.size() - 1);
			if (FAILED(pGameInstance->Add_Component((_uint)LEVEL::STATIC, iter->GetString(), CModel::Create(m_pGraphicDevice, m_pContextDevice, strFolderItem.GetString(), Shader_Model, DefaultTechnique, true))))
				continue;

			
		}
	}

	FileFind.Close();
}
