// MainTab.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool.h"
#include "MainTab.h"
#include "afxdialogex.h"

#include "MainFrm.h"
#include "ToolView.h"
#include "ToolCamera.h"

#include "ToolModel.h"

#include "CameraTab.h"
#include "MeshTab.h"
#include "TerrainTab.h"
#include "AnimationTab.h"
#include "NavigationTab.h"
#include "EffectTab.h"
#include "EffectMeshTab.h"
#include "EffectMesh2Tab.h"
#include "LightTab.h"
#include "EventColliderTab.h"


// CMainTab 대화 상자입니다.

IMPLEMENT_DYNAMIC(CMainTab, CDialog)

CMainTab::CMainTab(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_MAINTAB, pParent)
	, m_fPosX(0)
	, m_fPosY(0)
	, m_fPosZ(0)
	, m_fScaleX(1)
	, m_fScaleY(1)
	, m_fScaleZ(1)
	, m_fRotX(0)
	, m_fRotY(0)
	, m_fRotZ(0)
	, m_strContainerName(_T(""))
{

}

CMainTab::~CMainTab()
{
	Safe_Delete(m_pCameraTab);
	Safe_Delete(m_pTerrainTab);
	Safe_Delete(m_pAnimationTab);
	Safe_Delete(m_pEffectTab);
	Safe_Delete(m_pEffectMeshTab);
	Safe_Delete(m_pEffectMesh2Tab);
	Safe_Delete(m_pNavigationTab);
	Safe_Delete(m_pMeshTab);
	Safe_Delete(m_pLightTab);

	for (auto map : m_MeshObjectMap)
	{
		for (auto iter : map.second)
		{
			for (auto viter : iter.second)
				Safe_Release(viter);
			iter.second.clear();
		}
		map.second.clear();
	}
	m_MeshObjectMap.clear();

}

void CMainTab::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, TabControl_MainTab, m_tabCtrl);
	DDX_Control(pDX, List_Scene, m_listScene);
	DDX_Control(pDX, Tree_WorldContainer, m_treeWorld);
	DDX_Control(pDX, Spin_Position_X, m_spinCtrlPosX);
	DDX_Text(pDX, Edit_Position_X, m_fPosX);
	DDX_Control(pDX, Spin_Position_Y, m_spinCtrlPosY);
	DDX_Control(pDX, Spin_Position_Z, m_spinCtrlPosZ);
	DDX_Text(pDX, Edit_Position_Y, m_fPosY);
	DDX_Text(pDX, Edit_Position_Z, m_fPosZ);
	DDX_Text(pDX, Edit_Scale_X, m_fScaleX);
	DDX_Text(pDX, Edit_Scale_Y, m_fScaleY);
	DDX_Text(pDX, Edit_Scale_Z, m_fScaleZ);
	DDX_Control(pDX, Spin_Scale_X, m_spinCtrlScaleX);
	DDX_Control(pDX, Spin_Scale_Y, m_spinCtrlScaleY);
	DDX_Control(pDX, Spin_Scale_Z, m_spinCtrlScaleZ);
	DDX_Text(pDX, Edit_Rot_X, m_fRotX);
	DDX_Text(pDX, Edit_Rot_Y, m_fRotY);
	DDX_Text(pDX, Edit_Rot_Z, m_fRotZ);
	DDX_Control(pDX, Spin_Rot_X, m_spinCtrlRotX);
	DDX_Control(pDX, Spin_Rot_Y, m_spinCtrlRotY);
	DDX_Control(pDX, Spin_Rot_Z, m_spinCtrlRotZ);
	DDX_Text(pDX, Edit_Main_ContainerName, m_strContainerName);
}


BEGIN_MESSAGE_MAP(CMainTab, CDialog)
	ON_NOTIFY(TCN_SELCHANGE, TabControl_MainTab, &CMainTab::SELCHANGE_MainTab)
	ON_NOTIFY(TCN_SELCHANGE, TabControl_PrototypeList, &CMainTab::OnTcnSelchangePrototypelist)
	ON_LBN_SELCHANGE(List_Scene, &CMainTab::OnLbnSelchangeScene)
	ON_BN_CLICKED(Button_EditScene, &CMainTab::OnBnClickedEditscene)
	ON_BN_CLICKED(Button_Scene_Save, &CMainTab::OnBnClickedSceneSave)
	ON_BN_CLICKED(Button_Scene_Load, &CMainTab::OnBnClickedSceneLoad)
	ON_BN_CLICKED(Button_Scene_Delete, &CMainTab::OnBnClickedSceneDelete)
	ON_BN_CLICKED(Button_Container, &CMainTab::OnBnClickedContainer)
	ON_BN_CLICKED(Button_DeleteContainer, &CMainTab::OnBnClickedDeletecontainer)
	ON_NOTIFY(TVN_SELCHANGED, Tree_WorldContainer, &CMainTab::OnTvnSelchangedWorldcontainer)
	ON_NOTIFY(NM_CLICK, Tree_WorldContainer, &CMainTab::OnNMClickWorldcontainer)
	ON_NOTIFY(UDN_DELTAPOS, Spin_Position_X, &CMainTab::OnDeltaposPositionX)
	ON_NOTIFY(UDN_DELTAPOS, Spin_Position_Y, &CMainTab::OnDeltaposPositionY)
	ON_NOTIFY(UDN_DELTAPOS, Spin_Position_Z, &CMainTab::OnDeltaposPositionZ)
	ON_NOTIFY(UDN_DELTAPOS, Spin_Scale_X, &CMainTab::OnDeltaposScaleX)
	ON_NOTIFY(UDN_DELTAPOS, Spin_Scale_Y, &CMainTab::OnDeltaposScaleY)
	ON_NOTIFY(UDN_DELTAPOS, Spin_Scale_Z, &CMainTab::OnDeltaposScaleZ)
	ON_NOTIFY(UDN_DELTAPOS, Spin_Rot_X, &CMainTab::OnDeltaposRotX)
	ON_NOTIFY(UDN_DELTAPOS, Spin_Rot_Y, &CMainTab::OnDeltaposRotY)
	ON_NOTIFY(UDN_DELTAPOS, Spin_Rot_Z, &CMainTab::OnDeltaposRotZ)
	ON_EN_CHANGE(Edit_Main_ContainerName, &CMainTab::OnEnChangeMainContainername)
	ON_BN_CLICKED(Button_Main_ChangeName, &CMainTab::OnBnClickedMainChangename)
END_MESSAGE_MAP()


// CMainTab 메시지 처리기입니다.


void CMainTab::SELCHANGE_MainTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (TabControl_MainTab == pNMHDR->idFrom)
	{
		int iSelect = m_tabCtrl.GetCurSel();

		switch ((CMainTab::TAB_NUM)iSelect)
		{
		case CMainTab::TAB_NUM::CAMERA:
			m_pCameraTab->ShowWindow(SW_SHOW);
			m_pToolCam->Set_CamTestTrue();
			m_pMeshTab->ShowWindow(SW_HIDE);
			m_pTerrainTab->ShowWindow(SW_HIDE);
			m_pAnimationTab->ShowWindow(SW_HIDE);
			m_pNavigationTab->ShowWindow(SW_HIDE);
			m_pEffectTab->ShowWindow(SW_HIDE);
			m_pEffectMeshTab->ShowWindow(SW_HIDE);
			m_pEffectMesh2Tab->ShowWindow(SW_HIDE);
			m_pLightTab->ShowWindow(SW_HIDE);
			m_pEventColliderTab->ShowWindow(SW_HIDE);
			break;
		case CMainTab::TAB_NUM::MESH:
			m_pCameraTab->ShowWindow(SW_HIDE);
			m_pToolCam->Set_CamTestFalse();
			m_pMeshTab->ShowWindow(SW_SHOW);
			m_pTerrainTab->ShowWindow(SW_HIDE);
			m_pAnimationTab->ShowWindow(SW_HIDE);
			m_pNavigationTab->ShowWindow(SW_HIDE);
			m_pEffectTab->ShowWindow(SW_HIDE);
			m_pEffectMeshTab->ShowWindow(SW_HIDE);
			m_pEffectMesh2Tab->ShowWindow(SW_HIDE);
			m_pLightTab->ShowWindow(SW_HIDE);
			m_pEventColliderTab->ShowWindow(SW_HIDE);
			break;
		case CMainTab::TAB_NUM::TERRAIN:
			m_pCameraTab->ShowWindow(SW_HIDE);
			m_pToolCam->Set_CamTestFalse();
			m_pMeshTab->ShowWindow(SW_HIDE);
			m_pTerrainTab->ShowWindow(SW_SHOW);
			m_pAnimationTab->ShowWindow(SW_HIDE);
			m_pNavigationTab->ShowWindow(SW_HIDE);
			m_pEffectTab->ShowWindow(SW_HIDE);
			m_pEffectMeshTab->ShowWindow(SW_HIDE);
			m_pEffectMesh2Tab->ShowWindow(SW_HIDE);
			m_pLightTab->ShowWindow(SW_HIDE);
			m_pEventColliderTab->ShowWindow(SW_HIDE);
			break;
		case CMainTab::TAB_NUM::ANIMATION:
			m_pCameraTab->ShowWindow(SW_HIDE);
			m_pToolCam->Set_CamTestFalse();
			m_pMeshTab->ShowWindow(SW_HIDE);
			m_pTerrainTab->ShowWindow(SW_HIDE);
			m_pAnimationTab->ShowWindow(SW_SHOW);
			m_pNavigationTab->ShowWindow(SW_HIDE);
			m_pEffectTab->ShowWindow(SW_HIDE);
			m_pEffectMeshTab->ShowWindow(SW_HIDE);
			m_pEffectMesh2Tab->ShowWindow(SW_HIDE);
			m_pLightTab->ShowWindow(SW_HIDE);
			m_pEventColliderTab->ShowWindow(SW_HIDE);
			break;
		case CMainTab::TAB_NUM::NAVIGATION:
			m_pCameraTab->ShowWindow(SW_HIDE);
			m_pToolCam->Set_CamTestFalse();
			m_pMeshTab->ShowWindow(SW_HIDE);
			m_pTerrainTab->ShowWindow(SW_HIDE);
			m_pAnimationTab->ShowWindow(SW_HIDE);
			m_pNavigationTab->ShowWindow(SW_SHOW);
			m_pEffectTab->ShowWindow(SW_HIDE);
			m_pEffectMeshTab->ShowWindow(SW_HIDE);
			m_pEffectMesh2Tab->ShowWindow(SW_HIDE);
			m_pLightTab->ShowWindow(SW_HIDE);
			m_pEventColliderTab->ShowWindow(SW_HIDE);
			break;
		case CMainTab::TAB_NUM::EFFECT:
			m_pCameraTab->ShowWindow(SW_HIDE);
			m_pToolCam->Set_CamTestFalse();
			m_pMeshTab->ShowWindow(SW_HIDE);
			m_pTerrainTab->ShowWindow(SW_HIDE);
			m_pAnimationTab->ShowWindow(SW_HIDE);
			m_pNavigationTab->ShowWindow(SW_HIDE);
			m_pEffectTab->ShowWindow(SW_SHOW);
			m_pEffectMeshTab->ShowWindow(SW_HIDE);
			m_pEffectMesh2Tab->ShowWindow(SW_HIDE);
			m_pLightTab->ShowWindow(SW_HIDE);
			m_pEventColliderTab->ShowWindow(SW_HIDE);
			break;
		case CMainTab::TAB_NUM::EFFECTMESH:
			m_pCameraTab->ShowWindow(SW_HIDE);
			m_pToolCam->Set_CamTestFalse();
			m_pMeshTab->ShowWindow(SW_HIDE);
			m_pTerrainTab->ShowWindow(SW_HIDE);
			m_pAnimationTab->ShowWindow(SW_HIDE);
			m_pNavigationTab->ShowWindow(SW_HIDE);
			m_pEffectTab->ShowWindow(SW_HIDE);
			m_pEffectMeshTab->ShowWindow(SW_SHOW);
			m_pEffectMesh2Tab->ShowWindow(SW_HIDE);
			m_pLightTab->ShowWindow(SW_HIDE);
			m_pEventColliderTab->ShowWindow(SW_HIDE);
			break;
		case CMainTab::TAB_NUM::EFFECTMESH2:
			m_pCameraTab->ShowWindow(SW_HIDE);
			m_pToolCam->Set_CamTestFalse();
			m_pMeshTab->ShowWindow(SW_HIDE);
			m_pTerrainTab->ShowWindow(SW_HIDE);
			m_pAnimationTab->ShowWindow(SW_HIDE);
			m_pNavigationTab->ShowWindow(SW_HIDE);
			m_pEffectTab->ShowWindow(SW_HIDE);
			m_pEffectMeshTab->ShowWindow(SW_HIDE);
			m_pEffectMesh2Tab->ShowWindow(SW_SHOW);
			m_pLightTab->ShowWindow(SW_HIDE);
			m_pEventColliderTab->ShowWindow(SW_HIDE);
			break;
		case CMainTab::TAB_NUM::LIGHTTAB:
			m_pCameraTab->ShowWindow(SW_HIDE);
			m_pToolCam->Set_CamTestFalse();
			m_pMeshTab->ShowWindow(SW_HIDE);
			m_pTerrainTab->ShowWindow(SW_HIDE);
			m_pAnimationTab->ShowWindow(SW_HIDE);
			m_pNavigationTab->ShowWindow(SW_HIDE);
			m_pEffectTab->ShowWindow(SW_HIDE);
			m_pEffectMeshTab->ShowWindow(SW_HIDE);
			m_pEffectMesh2Tab->ShowWindow(SW_HIDE);
			m_pLightTab->ShowWindow(SW_SHOW);
			m_pEventColliderTab->ShowWindow(SW_HIDE);
			break;
		case CMainTab::TAB_NUM::EVENTCOLLIDER:
			m_pCameraTab->ShowWindow(SW_HIDE);
			m_pToolCam->Set_CamTestFalse();
			m_pMeshTab->ShowWindow(SW_HIDE);
			m_pTerrainTab->ShowWindow(SW_HIDE);
			m_pAnimationTab->ShowWindow(SW_HIDE);
			m_pNavigationTab->ShowWindow(SW_HIDE);
			m_pEffectTab->ShowWindow(SW_HIDE);
			m_pEffectMeshTab->ShowWindow(SW_HIDE);
			m_pEffectMesh2Tab->ShowWindow(SW_HIDE);
			m_pLightTab->ShowWindow(SW_HIDE);
			m_pEventColliderTab->ShowWindow(SW_SHOW);
			break;
		default:
			break;
		}
	}
	*pResult = 0;
}


BOOL CMainTab::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRect		tRect;
	CRect		tTabRect;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CMainFrame*    pMain = dynamic_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd());
	CToolView*     pToolView = dynamic_cast<CToolView*>(pMain->GetActiveView());
	m_pToolCam = pToolView->m_pCamera;

	RELEASE_INSTANCE(CGameInstance);
	
	m_tabCtrl.InsertItem((_int)TAB_NUM::CAMERA, L"Camera");
	m_tabCtrl.InsertItem((_int)TAB_NUM::MESH, L"Mesh");
	m_tabCtrl.InsertItem((_int)TAB_NUM::TERRAIN, L"Terrain");
	m_tabCtrl.InsertItem((_int)TAB_NUM::ANIMATION, L"Animation");
	m_tabCtrl.InsertItem((_int)TAB_NUM::NAVIGATION, L"Navigation");
	m_tabCtrl.InsertItem((_int)TAB_NUM::EFFECT, L"Effect");
	m_tabCtrl.InsertItem((_int)TAB_NUM::EFFECTMESH, L"EffectMesh");
	m_tabCtrl.InsertItem((_int)TAB_NUM::EFFECTMESH2, L"EffectMesh2");
	m_tabCtrl.InsertItem((_int)TAB_NUM::LIGHTTAB, L"LightTab");
	m_tabCtrl.InsertItem((_int)TAB_NUM::EVENTCOLLIDER, L"EventCollider");

	m_tabCtrl.SetCurSel((_int)TAB_NUM::CAMERA);
	m_tabCtrl.GetWindowRect(&tRect);
	m_tabCtrl.GetItemRect((_int)TAB_NUM::CAMERA, &tTabRect);

	m_pCameraTab = new CCameraTab;
	m_pCameraTab->Create(IDD_CAMERATAB, &m_tabCtrl);
	m_pCameraTab->MoveWindow(0, tTabRect.Height(), tRect.Width(), tRect.Height());
	m_pCameraTab->ShowWindow(SW_SHOW);
	m_pToolCam->Set_CamTestTrue();

	m_pMeshTab = new CMeshTab;
	m_pMeshTab->Create(IDD_MESHTAB, &m_tabCtrl);
	m_pMeshTab->MoveWindow(0, tTabRect.Height(), tRect.Width(), tRect.Height());
	m_pMeshTab->ShowWindow(SW_HIDE);

	m_pTerrainTab= new CTerrainTab;
	m_pTerrainTab->Create(IDD_TERRAINTAB, &m_tabCtrl);
	m_pTerrainTab->MoveWindow(0, tTabRect.Height(), tRect.Width(), tRect.Height());
	m_pTerrainTab->ShowWindow(SW_HIDE);

	m_pAnimationTab = new CAnimationTab;
	m_pAnimationTab->Create(IDD_ANIMATIONTAB, &m_tabCtrl);
	m_pAnimationTab->MoveWindow(0, tTabRect.Height(), tRect.Width(), tRect.Height());
	m_pAnimationTab->ShowWindow(SW_HIDE);

	m_pNavigationTab = new CNavigationTab;
	m_pNavigationTab->Create(IDD_NAVIGATIONTAB, &m_tabCtrl);
	m_pNavigationTab->MoveWindow(0, tTabRect.Height(), tRect.Width(), tRect.Height());
	m_pNavigationTab->ShowWindow(SW_HIDE);

	m_pEffectTab = new CEffectTab;
	m_pEffectTab->Create(IDD_EFFECTTAB, &m_tabCtrl);
	m_pEffectTab->MoveWindow(0, tTabRect.Height(), tRect.Width(), tRect.Height());
	m_pEffectTab->ShowWindow(SW_HIDE);

	m_pEffectMeshTab = new CEffectMeshTab;
	m_pEffectMeshTab->Create(IDD_EFFECTMESHTAB, &m_tabCtrl);
	m_pEffectMeshTab->MoveWindow(0, tTabRect.Height(), tRect.Width(), tRect.Height());
	m_pEffectMeshTab->ShowWindow(SW_HIDE);

	m_pEffectMesh2Tab = new CEffectMesh2Tab;
	m_pEffectMesh2Tab->Create(IDD_EFFECTMESH2TAB, &m_tabCtrl);
	m_pEffectMesh2Tab->MoveWindow(0, tTabRect.Height(), tRect.Width(), tRect.Height());
	m_pEffectMesh2Tab->ShowWindow(SW_HIDE);


	m_pLightTab = new CLightTab;
	m_pLightTab->Create(IDD_LIGHTTAB, &m_tabCtrl);
	m_pLightTab->MoveWindow(0, tTabRect.Height(), tRect.Width(), tRect.Height());
	m_pLightTab->ShowWindow(SW_HIDE);

	m_pEventColliderTab = new CEventColliderTab;
	m_pEventColliderTab->Create(IDD_EVENTCOLLIDERTAB, &m_tabCtrl);
	m_pEventColliderTab->MoveWindow(0, tTabRect.Height(), tRect.Width(), tRect.Height());
	m_pEventColliderTab->ShowWindow(SW_HIDE);

	// 아이콘 생성
	CBitmap bmp;
	bmp.LoadBitmap(IDB_CONTAINER);

	m_imagelist.DeleteImageList();
	m_imagelist.Create(16, 16, ILC_COLOR16 | ILC_MASK, 0, 0);
	m_imagelist.Add(&bmp, RGB(255, 0, 255));
	    
	m_treeWorld.SetImageList(&m_imagelist, TVSIL_NORMAL);


	m_spinCtrlPosX.SetRange(-500000, 500000);
	m_spinCtrlPosY.SetRange(-500000, 500000);
	m_spinCtrlPosZ.SetRange(-500000, 500000);

	m_spinCtrlScaleX.SetRange(0, 100);
	m_spinCtrlScaleY.SetRange(0, 100);
	m_spinCtrlScaleZ.SetRange(0, 100);


	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

_bool CMainTab::Set_SceneName(CString strSceneName)
{
	int iIndex = m_listScene.FindStringExact(-1, strSceneName);
	if (iIndex != -1)
		return false;

	m_listScene.AddString(strSceneName);
	Add_World(strSceneName);
	return true;
}

void CMainTab::Add_World(CString strSceneName)
{
	HTREEITEM	hWorldTree = m_treeWorld.InsertItem(strSceneName, 0, 0, NULL, NULL);
	m_treeWorld.SetCheck(hWorldTree);
	m_strCurrentWorld = strSceneName;
}


_int CMainTab::Tick(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CurrentWorldTick(dTimeDelta);
	m_pMeshTab->Tick(dTimeDelta);
	m_pNavigationTab->Tick(dTimeDelta);
	m_pAnimationTab->Tick(dTimeDelta);
	m_pCameraTab->Tick(dTimeDelta);
	m_pEventColliderTab->Tick(dTimeDelta);
	return _int();
}

_int CMainTab::LateTick(_double dTimeDelta)
{
	CurrentWorldLateTick(dTimeDelta);
	m_pMeshTab->LateTick(dTimeDelta);
	m_pAnimationTab->LateTick(dTimeDelta);
	m_pNavigationTab->LateTick(dTimeDelta);
	m_pCameraTab->LateTick(dTimeDelta);
	m_pEventColliderTab->LateTick(dTimeDelta);
	return _int();
}

_int CMainTab::CurrentWorldTick(_double dTimeDelta)
{
	auto map = find_if(m_MeshObjectMap.begin(), m_MeshObjectMap.end(), CTagFinder(m_strCurrentWorld));
	if (map == m_MeshObjectMap.end())
		return 0;

	if (m_treeWorld.GetCheck(m_treeWorld.GetRootItem()) == false)
		return 0;

	for (auto iter : map->second)
	{
		for (auto vector : iter.second)
		{
			HTREEITEM hContiner = Find_ItemUsingString(iter.first, m_treeWorld.GetChildItem(m_treeWorld.GetRootItem()));
			CString strDebugContainer = m_treeWorld.GetItemText(hContiner);
			if (hContiner == NULL)
				continue;

			if (m_treeWorld.GetCheck(hContiner) == false)
				continue;

			HTREEITEM hItem = Find_ItemUsingData((DWORD_PTR)vector, hContiner);
			CString strDebugItem  = m_treeWorld.GetItemText(hItem);

			if (hItem == NULL || m_treeWorld.GetCheck(hItem) == FALSE)
				continue;

			vector->Tick(dTimeDelta);
		}
	}

	return _int();
}

_int CMainTab::CurrentWorldLateTick(_double dTimeDelta)
{
	auto map = find_if(m_MeshObjectMap.begin(), m_MeshObjectMap.end(), CTagFinder(m_strCurrentWorld));
	if (map == m_MeshObjectMap.end())
		return 0;

	if (m_treeWorld.GetCheck(m_treeWorld.GetRootItem()) == false)
		return 0;

	for (auto iter : map->second)
	{
		for (auto vector : iter.second)
		{
			HTREEITEM hContiner = Find_ItemUsingString(iter.first, m_treeWorld.GetChildItem(m_treeWorld.GetRootItem()));
			if (hContiner == NULL)
				continue;

			if (m_treeWorld.GetCheck(hContiner) == false)
				continue;

			HTREEITEM hItem = Find_ItemUsingData((DWORD_PTR)vector, hContiner);
			if (hItem == NULL || m_treeWorld.GetCheck(hItem) == FALSE)
				continue;

			vector->LateTick(dTimeDelta);
		}
	}

	return _int();
}

HRESULT CMainTab::Render()
{
	m_pMeshTab->Render();
	return S_OK;
}

_int CMainTab::Picking(_double dTimeDelta)
{
	auto map = find_if(m_MeshObjectMap.begin(), m_MeshObjectMap.end(), CTagFinder(m_strCurrentWorld));
	if (map == m_MeshObjectMap.end())
		return 0;

	if (m_treeWorld.GetCheck(m_treeWorld.GetRootItem()) == false)
		return 0;

	for (auto iter : map->second)
	{
		for (auto vector : iter.second)
		{
			HTREEITEM hContiner = Find_ItemUsingString(iter.first, m_treeWorld.GetChildItem(m_treeWorld.GetRootItem()));
			if (hContiner == NULL)
				continue;

			if (m_treeWorld.GetCheck(hContiner) == false)
				continue;

			HTREEITEM hItem = Find_ItemUsingData((DWORD_PTR)vector, hContiner);
			if (hItem == NULL || m_treeWorld.GetCheck(hItem) == FALSE)
				continue;

			dynamic_cast<CToolModel*>(vector)->Picking();
		}
	}
	return _int();
}


HRESULT CMainTab::Insert_MeshObjectToMap(CToolModel * pToolModel, CString strObjectName)
{
	auto iter = find_if(m_MeshObjectMap.begin(), m_MeshObjectMap.end(), CTagFinder(m_strCurrentWorld));

	if (iter != m_MeshObjectMap.end())
	{
		HTREEITEM hContainer = Find_CurrentContainer();
		if (hContainer == NULL)
		{
			MSGBOX("오브젝트는 컨테이너에 넣어주세요!");
			Safe_Release(pToolModel);
			return S_OK;
		}
		
		auto iterVector = find_if(iter->second.begin(), iter->second.end(), 
			CTagFinder(m_treeWorld.GetItemText(hContainer)));

		if (iterVector != iter->second.end())
		{
			if (Insert_ObjectIndex(pToolModel, strObjectName) == true)
			{
				pToolModel->Set_ModelObjectTag(strObjectName);
				iterVector->second.push_back(pToolModel);
			}
		}
		else
		{
			CONTAINERS	mapContainer;
			vector<CToolModel*> vecModel;
			
			if (Insert_ObjectIndex(pToolModel, strObjectName) == true)
			{
				pToolModel->Set_ModelObjectTag(strObjectName);
				vecModel.push_back(pToolModel);
				mapContainer.emplace(m_strCurrentContainer, vecModel);
				m_MeshObjectMap.emplace(m_strCurrentWorld, mapContainer);
			}
		}
	}
	else
	{
		CONTAINERS mapContainer;
		vector<CToolModel*> vecModel;

		if (Insert_ObjectIndex(pToolModel, strObjectName))
		{
			pToolModel->Set_ModelObjectTag(strObjectName);
			vecModel.push_back(pToolModel);
			mapContainer.emplace(m_strCurrentContainer, vecModel);
			m_MeshObjectMap.emplace(m_strCurrentWorld, mapContainer);
		}
	}



	return S_OK;
}

_bool CMainTab::Insert_ObjectIndex(CToolModel * pToolModel, CString strObjectName)
{
	auto map = find_if(m_mapCounter.begin(), m_mapCounter.end(), CTagFinder(strObjectName));
	if (map == m_mapCounter.end())
	{
		HTREEITEM hCurrentContainer = Find_CurrentContainer();
		if (hCurrentContainer == NULL)
			return false;

		CString strName;

		CString strMapTag = strObjectName;
		_uint iIndex = 0;

		m_mapCounter.emplace(strMapTag, iIndex);

		strName.Format(TEXT("%s%d"), strMapTag, iIndex);
		
		HTREEITEM hItem = m_treeWorld.InsertItem(strName, 3, 4, hCurrentContainer, NULL);
		m_treeWorld.SetItemData(hItem, (DWORD_PTR)pToolModel);
		m_treeWorld.Expand(hCurrentContainer, TVE_EXPAND);
	}
	else
	{
		HTREEITEM hCurrentContainer = Find_CurrentContainer();
		if (hCurrentContainer == NULL)
			return false;

		CString strMapTag;
		strMapTag.Format(TEXT("%s%d"), map->first, ++(map->second));
		HTREEITEM hItem = m_treeWorld.InsertItem(strMapTag, 3, 4, hCurrentContainer, NULL);
		m_treeWorld.SetItemData(hItem, (DWORD_PTR)pToolModel);
		m_treeWorld.Expand(hCurrentContainer, TVE_EXPAND);
	}

	return true;
}

HRESULT CMainTab::Load_Container(HTREEITEM hParent, CString strContainerName, HTREEITEM& hContainer)
{
	hContainer = m_treeWorld.InsertItem(strContainerName, 1, 2, hParent, NULL);
	m_treeWorld.Expand(hParent, TVE_EXPAND);

	CString WorldTag = m_treeWorld.GetItemText(m_treeWorld.GetRootItem());

	auto iter = find_if(m_MeshObjectMap.begin(), m_MeshObjectMap.end(), CTagFinder(WorldTag));
	if (iter != m_MeshObjectMap.end())
	{
		vector<CToolModel*> vecModels;
		iter->second.emplace(strContainerName, vecModels);
	}
	else
	{
		CONTAINERS tContainers;
		vector<CToolModel*> vecModels;

		tContainers.emplace(strContainerName, vecModels);
		m_MeshObjectMap.emplace(WorldTag, tContainers);
	}

	return S_OK;
}

HRESULT CMainTab::Load_Object(CToolModel * pToolModel, HTREEITEM hParent, CString strItemName)
{

	// Insert To World Map Container ///////////////////////////////////////////////////////
	auto iter = find_if(m_MeshObjectMap.begin(), m_MeshObjectMap.end(), CTagFinder(m_strCurrentWorld));
	if (iter != m_MeshObjectMap.end())
	{
		CString strContainerName = m_treeWorld.GetItemText(hParent);
		auto iterVector = find_if(iter->second.begin(), iter->second.end(), CTagFinder(strContainerName));
		if (iterVector != iter->second.end())
			iterVector->second.push_back(pToolModel);
		else
		{
			vector<CToolModel*> vecModel;
			vecModel.push_back(pToolModel);

			iter->second.emplace(strContainerName, vecModel);
		}
	}
	else
	{
		CONTAINERS mapContainer;
		vector<CToolModel*> vecModel;
		vecModel.push_back(pToolModel);
		mapContainer.emplace(m_strCurrentContainer, vecModel);
		m_MeshObjectMap.emplace(m_strCurrentWorld, mapContainer);
	}
	////////////////////////////////////////////////////////////////////////////////////////

	// Insert To Tree Control //////////////////////////////////////////////////////////////
	auto map = find_if(m_mapCounter.begin(), m_mapCounter.end(), CTagFinder(strItemName));
	if (map == m_mapCounter.end())
	{
		CString strName;
		_uint iIndex = 0;
		m_mapCounter.emplace(strItemName, iIndex);
		strName.Format(TEXT("%s%d"), strItemName, iIndex);
		HTREEITEM hItem = m_treeWorld.InsertItem(strName, 3, 4, hParent, NULL);
		m_treeWorld.SetItemData(hItem, (DWORD_PTR)pToolModel);
		m_treeWorld.Expand(hParent, TVE_EXPAND);
	}
	else
	{
		CString strName;
		strName.Format(TEXT("%s%d"), map->first, ++(map->second));
		HTREEITEM hItem = m_treeWorld.InsertItem(strName, 3, 4, hParent, NULL);
		m_treeWorld.SetItemData(hItem, (DWORD_PTR)pToolModel);
		m_treeWorld.Expand(hParent, TVE_EXPAND);
	}
	////////////////////////////////////////////////////////////////////////////////////////

	return S_OK;
}

HTREEITEM CMainTab::Find_CurrentContainer()
{
	int iImage = 0;
	int iSelectImage = 0;
	m_treeWorld.GetItemImage(m_treeWorld.GetSelectedItem(), iImage, iSelectImage);
	if (iImage == 1 && iSelectImage == 2)
		return m_treeWorld.GetSelectedItem();

	return NULL;
}

HTREEITEM CMainTab::Find_ItemUsingString(CString str, HTREEITEM hContainer)
{
	HTREEITEM hFindItem, hSibling, hChild;
	hFindItem = hSibling = hChild = NULL;

	if (str.Compare(m_treeWorld.GetItemText(hContainer)) == 0)
		hFindItem = hContainer;
	else
	{
		hChild = m_treeWorld.GetChildItem(hContainer);
		if (hChild != NULL)
			hFindItem = Find_ItemUsingString(str, hChild);

		hSibling = m_treeWorld.GetNextSiblingItem(hContainer);
		if (hFindItem == NULL && hSibling != NULL)
			hFindItem = Find_ItemUsingString(str, hSibling);

	}
	return hFindItem;
}

HTREEITEM CMainTab::Find_ItemUsingData(DWORD_PTR ptr, HTREEITEM hContainer)
{
	HTREEITEM hFindItem, hSibling, hChild;
	hFindItem = hSibling = hChild = NULL;

	if (m_treeWorld.GetItemData(hContainer) == ptr)
		hFindItem = hContainer;
	else
	{
		hChild = m_treeWorld.GetChildItem(hContainer);
		if (hChild != NULL)
			hFindItem = Find_ItemUsingData(ptr, hChild);

		hSibling = m_treeWorld.GetNextSiblingItem(hContainer);
		if (hFindItem == NULL && hSibling != NULL)
			hFindItem = Find_ItemUsingData(ptr, hSibling);

	}
	return hFindItem;
}

void CMainTab::Check_ChildItem(HTREEITEM hParent)
{
	HTREEITEM hChildItem = m_treeWorld.GetChildItem(hParent);
	while (hChildItem != NULL)
	{
		m_treeWorld.SetCheck(hChildItem, TRUE);
		if (m_treeWorld.ItemHasChildren(hChildItem) == TRUE)
			Check_ChildItem(hChildItem);

		hChildItem = m_treeWorld.GetNextItem(hChildItem, TVGN_NEXT);
	}
}

void CMainTab::UnCheck_ChildItem(HTREEITEM hParent)
{
	HTREEITEM hChildItem = m_treeWorld.GetChildItem(hParent);
	while (hChildItem != NULL)
	{
		m_treeWorld.SetCheck(hChildItem, FALSE);
		if (m_treeWorld.ItemHasChildren(hChildItem) == TRUE)
			UnCheck_ChildItem(hChildItem);

		hChildItem = m_treeWorld.GetNextItem(hChildItem, TVGN_NEXT);
	}
}


void CMainTab::OnTcnSelchangePrototypelist(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


void CMainTab::OnLbnSelchangeScene()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int iIndex = m_listScene.GetCurSel();
	
	if (iIndex >= m_listScene.GetCount() || iIndex < 0)
		return;

	m_listScene.GetText(iIndex, m_strSceneName);
}

// Scene Create
void CMainTab::OnBnClickedEditscene()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFileDialog Dlg(FALSE, L"dat", L"*.dat");
	TCHAR szBuf[MAX_PATH]{};
	GetCurrentDirectory(MAX_PATH, szBuf);
	PathRemoveFileSpec(szBuf);
	lstrcat(szBuf, L"\\Client\\Bin\\Data\\SceneData");
	Dlg.m_ofn.lpstrInitialDir = szBuf;
	if (IDOK == Dlg.DoModal())
	{
		CString wstrPath = Dlg.GetPathName();
		if (Set_SceneName(Dlg.GetFileTitle()) == false)
			return;
		
		HANDLE hFile = CreateFile(wstrPath.GetString(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

		if (INVALID_HANDLE_VALUE == hFile)
			return;

		CloseHandle(hFile);
	}
} 


void CMainTab::OnBnClickedSceneSave()
{
	TCHAR szBuf[MAX_PATH]{};
	GetCurrentDirectory(MAX_PATH, szBuf);
	PathRemoveFileSpec(szBuf);
	lstrcat(szBuf, L"\\Client\\Bin\\Data\\SceneData\\");

	int iIndex = m_listScene.GetCurSel();
	CString FilePath, datFileName;
	m_listScene.GetText(iIndex, datFileName);

	FilePath = szBuf + datFileName + L".dat";

	HANDLE hFile = CreateFile(FilePath.GetString(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (hFile == INVALID_HANDLE_VALUE)
		VOIDMSG("Failed to Save Scene");

	_ulong dwByte = 0;

	SCENE_INFO tSceneInfo;
	_uint iSize = 0;

	// Tree Control's Root Item ( = defulat FileName )
	HTREEITEM hRoot = m_treeWorld.GetRootItem();
	CString strRoot = m_treeWorld.GetItemText(hRoot);

	auto CurrentMap = find_if(m_MeshObjectMap.begin(), m_MeshObjectMap.end(), CTagFinder(m_strCurrentWorld));

	// First Record Struct (Parent Name, TreeName) + Size.
	wcscpy_s(tSceneInfo.szParentName, L"Root");
	wcscpy_s(tSceneInfo.szSceneName, strRoot);
	iSize = (_uint)CurrentMap->second.size();

	// Recording Root's Struct + Size
	WriteFile(hFile, &tSceneInfo, sizeof(SCENE_INFO), &dwByte, nullptr);
	WriteFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);

	// Roop For Recording Containers
	for (auto Scene : CurrentMap->second)
	{
		HTREEITEM hRoopContainer = m_treeWorld.GetRootItem();
		// Struct for Recording Container
		wcscpy_s(tSceneInfo.szSceneName, Scene.first);

		// Find Current Container's Parent Item
		hRoopContainer = Find_ItemUsingString(tSceneInfo.szSceneName, hRoopContainer);

		if (hRoopContainer == NULL)
			continue;


		HTREEITEM hParent = m_treeWorld.GetParentItem(hRoopContainer);

		wcscpy_s(tSceneInfo.szParentName, m_treeWorld.GetItemText(hParent));

		// Recording Containers Info
		WriteFile(hFile, &tSceneInfo, sizeof(SCENE_INFO), &dwByte, nullptr);
		iSize = (_uint)Scene.second.size();
		WriteFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);
		for (auto vector : Scene.second)
		{
			MO_INFO tObjInfo = vector->Get_SaveData();
			WriteFile(hFile, &tObjInfo, sizeof(MO_INFO), &dwByte, nullptr);
		}
	}
	CloseHandle(hFile);
}


void CMainTab::OnBnClickedSceneLoad()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFileDialog Dlg(TRUE, L"dat", L"*.dat");
	TCHAR szBuf[MAX_PATH]{};
	GetCurrentDirectory(MAX_PATH, szBuf);
	PathRemoveFileSpec(szBuf);
	lstrcat(szBuf, L"\\Client\\Bin\\Data\\SceneData\\");
	Dlg.m_ofn.lpstrInitialDir = szBuf;
	if (IDOK == Dlg.DoModal())
	{
		CString wstrPath = Dlg.GetPathName();



		HANDLE	hFile = CreateFile(wstrPath.GetString(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

		if (INVALID_HANDLE_VALUE == hFile)
			return;

		_ulong	dwByte = 0;

		SCENE_INFO	tSceneName;
		_uint		iContainerSize = 0;
		ReadFile(hFile, &tSceneName, sizeof(SCENE_INFO), &dwByte, nullptr);
		ReadFile(hFile, &iContainerSize, sizeof(_uint), &dwByte, nullptr);

		if (Set_SceneName(tSceneName.szSceneName) == false)
			return;

		for (_uint i = 0; i < iContainerSize; ++i)
		{
			SCENE_INFO	tContainerName;
			_uint		iVectorSize = 0;
			ReadFile(hFile, &tContainerName, sizeof(SCENE_INFO), &dwByte, nullptr);
			ReadFile(hFile, &iVectorSize, sizeof(_uint), &dwByte, nullptr);
			HTREEITEM hParent = Find_ItemUsingString(tContainerName.szParentName, m_treeWorld.GetRootItem());
			HTREEITEM hContainer = NULL;
			Load_Container(hParent, tContainerName.szSceneName, hContainer);

			for (_uint j = 0; j < iVectorSize; ++j)
			{
				MO_INFO tModelObjectInfo;
				ReadFile(hFile, &tModelObjectInfo, sizeof(MO_INFO), &dwByte, nullptr);
				CToolModel* pToolModel = m_pMeshTab->Find_Object(tModelObjectInfo);
				pToolModel->Set_LoadData(tModelObjectInfo);

				if (pToolModel == nullptr)
					continue;

				Load_Object(pToolModel, hContainer, tModelObjectInfo.szModelObjectName);
			}
		}
		CloseHandle(hFile);
	}
}




void CMainTab::OnBnClickedSceneDelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int iIndex = m_listScene.GetCurSel();

	if (iIndex >= m_listScene.GetCount() || iIndex < 0)
		return;



}


void CMainTab::OnBnClickedContainer()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	HTREEITEM hRoot = m_treeWorld.GetSelectedItem();
	if (hRoot == NULL)
		return;

	UpdateData(TRUE);
	CString strContainerName;
	strContainerName.Format(TEXT("Container_%d"), m_iCurrentIndex++);

	HTREEITEM hContainer = m_treeWorld.InsertItem(strContainerName,1,2, hRoot, NULL);
	m_strCurrentContainer = strContainerName;
	m_treeWorld.SetCheck(hContainer);

	m_treeWorld.Expand(hRoot, TVE_EXPAND);
	
	vector<CToolModel*> vecContainer;

	auto iter = find_if(m_MeshObjectMap.begin(), m_MeshObjectMap.end(), CTagFinder(m_strCurrentWorld));
	if (iter != m_MeshObjectMap.end())
	{
		iter->second.emplace(strContainerName, vecContainer);
	}
	else
	{
		CONTAINERS mapContainer;
		mapContainer.emplace(strContainerName, vecContainer);

		m_MeshObjectMap.emplace(m_strCurrentWorld, mapContainer);
	}


	UpdateData(FALSE);
}

//
//void CMainTab::OnTvnBeginlabeleditWorldcontainer(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
//	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
//	CEdit* pEdit = m_treeWorld.GetEditControl();
//	
//	if (pEdit)
//	{
//		CString str;
//		pEdit->GetWindowText(str);
//		m_treeWorld.SetItemText(pTVDispInfo->item.hItem, str);
//	}
//	*pResult = 0;
//}
//
//
//void CMainTab::OnTvnEndlabeleditWorldcontainer(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
//	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
//	CEdit* pEdit = m_treeWorld.GetEditControl();
//
//	if (pEdit)
//	{
//		CString str;
//		pEdit->GetWindowText(str);
//		auto hContainer = find_if(m_vecContainer.begin(), m_vecContainer.end(), [=](HTREEITEM hTreeItem) {
//			return !lstrcmp(m_strCurrentContainer, m_treeWorld.GetItemText(hTreeItem));
//		});
//
//		if (hContainer == m_vecContainer.end())
//			return;
//
//
//		if (str.GetLength() > 0)
//		{
//			m_treeWorld.SetItemText(*hContainer, str);
//			m_treeWorld.SetItemText(pTVDispInfo->item.hItem, str);
//			auto CurrentWorld = find_if(m_MeshObjectMap.begin(), m_MeshObjectMap.end(), CTagFinder(m_strCurrentWorld));
//
//			auto CurrentContainer = find_if(CurrentWorld->second.begin(), CurrentWorld->second.end(), CTagFinder(m_strCurrentContainer));
//			if (CurrentContainer != CurrentWorld->second.end())
//			{
//				CONTAINERS mapContainer;
//				vector<CToolModel*> vecModels; 
//
//				swap(CurrentContainer->second, vecModels);
//				 CurrentWorld->second.emplace(str, vecModels);
//
//				CurrentWorld->second.erase(CurrentContainer);
//			}
//			m_strCurrentContainer = str;
//		}
//	}
//	*pResult = 0;
//}


void CMainTab::OnBnClickedDeletecontainer()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	HTREEITEM hSelectItem = m_treeWorld.GetSelectedItem();
	if (hSelectItem == m_treeWorld.GetRootItem())
	{
		MSGBOX("월드는 삭제할 수 없습니다.");
		return;
	}

	if (m_treeWorld.ItemHasChildren(hSelectItem) == TRUE)
	{
		auto World = find_if(m_MeshObjectMap.begin(), m_MeshObjectMap.end(), CTagFinder(m_strCurrentWorld));
		auto map = find_if( World->second.begin(), World->second.end(), CTagFinder(m_treeWorld.GetItemText(hSelectItem)));

		World->second.erase(map->first);

		HTREEITEM hNextItem;
		HTREEITEM hDeleteItem = m_treeWorld.GetChildItem(hSelectItem);
		while (hDeleteItem != NULL)
		{
			hNextItem = m_treeWorld.GetNextItem(hDeleteItem, TVGN_NEXT);
			m_treeWorld.DeleteItem(hDeleteItem);
			hDeleteItem = hNextItem;
		}
		
		m_treeWorld.DeleteItem(hSelectItem);
	}
	else
	{
		HTREEITEM hParent = m_treeWorld.GetParentItem(hSelectItem);
		auto World = find_if(m_MeshObjectMap.begin(), m_MeshObjectMap.end(), CTagFinder(m_strCurrentWorld));
		auto map = find_if(World->second.begin(), World->second.end(), CTagFinder(m_treeWorld.GetItemText(hParent)));
		
		vector<CToolModel*>& vecModel = map->second;
		DWORD_PTR pData = m_treeWorld.GetItemData(hSelectItem);
		int iIndex = 0;
		for (auto vector : vecModel)
		{
			if ((DWORD_PTR)vector == pData)
			{
				Safe_Release(vector);
				vecModel.erase(vecModel.begin() + iIndex);

				break;
			}
			++iIndex;
		}
		m_treeWorld.DeleteItem(hSelectItem);
	}
}


void CMainTab::OnTvnSelchangedWorldcontainer(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	HTREEITEM hItem = pNMTreeView->itemNew.hItem;
	m_dptrCurrentItem = m_treeWorld.GetItemData(hItem);
	if (m_dptrCurrentItem != 0)
	{
		HTREEITEM hParent = m_treeWorld.GetParentItem(hItem);
		auto World = find_if(m_MeshObjectMap.begin(), m_MeshObjectMap.end(), CTagFinder(m_strCurrentWorld));
		auto map = find_if(World->second.begin(), World->second.end(), CTagFinder(m_treeWorld.GetItemText(hParent)));

		vector<CToolModel*>& vecModel = map->second;
		DWORD_PTR pItemData = m_treeWorld.GetItemData(hItem);
		for (auto vector : vecModel)
		{
			if ((DWORD_PTR)vector == pItemData)
			{
				_float4 vPosition;
				XMStoreFloat4(&vPosition, vector->Get_Position());

				_float4 vScale;
				XMStoreFloat4(&vScale, vector->Get_Scale());

				UpdateData(TRUE);
				m_fPosX = vPosition.x; m_fPosY = vPosition.y; m_fPosZ = vPosition.z;
				m_fScaleX = vScale.x; m_fScaleY = vScale.y; m_fScaleZ = vScale.z;
				m_strContainerName = m_treeWorld.GetItemText(hItem);
				UpdateData(FALSE);
				break;
			}
		}
	}
	else
	{
		UpdateData(TRUE);
		m_fPosX = 0.f; m_fPosY = 0.f; m_fPosZ = 0.f;
		m_fScaleX = 1.f; m_fScaleY = 1.f; m_fScaleZ = 1.f;
		m_strContainerName = m_treeWorld.GetItemText(hItem);
		UpdateData(FALSE);
	}

	*pResult = 0;
}

void CMainTab::OnNMClickWorldcontainer(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	CPoint	pt;

	UINT	nFlags = 0;

	GetCursorPos(&pt);

	::ScreenToClient(m_treeWorld.m_hWnd, &pt);

	HTREEITEM hItem = m_treeWorld.HitTest(pt, &nFlags);
	int iImage = 0;
	int iClickImage = 0;
	m_treeWorld.GetItemImage(hItem, iImage, iClickImage);

	if (iImage == 3 && iClickImage == 4)
		return;

	if (hItem != NULL && (nFlags & TVHT_ONITEMSTATEICON))
	{
		if (m_treeWorld.GetCheck(hItem) == TRUE)
			UnCheck_ChildItem(hItem);
		else
			Check_ChildItem(hItem);
	}
}


void CMainTab::OnDeltaposPositionX(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	m_fPosX = m_fPosX + (_float)pNMUpDown->iDelta / 10.f;
	UpdateData(FALSE);


	HTREEITEM hItem = m_treeWorld.GetSelectedItem();
	if (m_treeWorld.ItemHasChildren(hItem) == TRUE)
		Add_MoveTargetX();
	else
		Move_Target();
	*pResult = 0;
}


void CMainTab::OnDeltaposPositionY(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	m_fPosY = m_fPosY + (_float)pNMUpDown->iDelta / 10.f;
	UpdateData(FALSE);

	HTREEITEM hItem = m_treeWorld.GetSelectedItem();
	if (m_treeWorld.ItemHasChildren(hItem) == TRUE)
		Add_MoveTargetY();
	else
		Move_Target();
	*pResult = 0;
}


void CMainTab::OnDeltaposPositionZ(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	m_fPosZ = m_fPosZ + (_float)pNMUpDown->iDelta / 10.f;
	UpdateData(FALSE);

	HTREEITEM hItem = m_treeWorld.GetSelectedItem();
	if (m_treeWorld.ItemHasChildren(hItem) == TRUE)
		Add_MoveTargetZ();
	else
		Move_Target();
	*pResult = 0;
}

void CMainTab::Move_Target()
{
	HTREEITEM hItem = m_treeWorld.GetSelectedItem();
	HTREEITEM hParent = m_treeWorld.GetParentItem(hItem);
	auto World = find_if(m_MeshObjectMap.begin(), m_MeshObjectMap.end(), CTagFinder(m_strCurrentWorld));
	auto map = find_if(World->second.begin(), World->second.end(), CTagFinder(m_treeWorld.GetItemText(hParent)));

	vector<CToolModel*>& vecModel = map->second;
	DWORD_PTR pItemData = m_treeWorld.GetItemData(hItem);
	int iIndex = 0;
	for (auto vector : vecModel)
	{
		if ((DWORD_PTR)vector == pItemData)
		{
			vector->Set_Position(XMVectorSet(m_fPosX, m_fPosY, m_fPosZ, 1.f));
			break;
		}
		++iIndex;
	}
}

void CMainTab::Add_MoveTargetX()
{
	HTREEITEM hItem = m_treeWorld.GetSelectedItem();
	if (m_treeWorld.GetRootItem() == hItem)
		return;

	auto World = find_if(m_MeshObjectMap.begin(), m_MeshObjectMap.end(), CTagFinder(m_strCurrentWorld));
	auto map = find_if(World->second.begin(), World->second.end(), CTagFinder(m_treeWorld.GetItemText(hItem)));

	vector<CToolModel*>& vecModel = map->second;
	for (auto vector : vecModel)
	{
		vector->Add_Position(XMVectorSet(m_fPosX, 0, 0, 1));
	}
}

void CMainTab::Add_MoveTargetY()
{
	HTREEITEM hItem = m_treeWorld.GetSelectedItem();
	if (m_treeWorld.GetRootItem() == hItem)
		return;

	auto World = find_if(m_MeshObjectMap.begin(), m_MeshObjectMap.end(), CTagFinder(m_strCurrentWorld));
	auto map = find_if(World->second.begin(), World->second.end(), CTagFinder(m_treeWorld.GetItemText(hItem)));

	vector<CToolModel*>& vecModel = map->second;
	for (auto vector : vecModel)
	{
		vector->Add_Position(XMVectorSet(0, m_fPosY, 0, 1));
	}
}

void CMainTab::Add_MoveTargetZ()
{
	HTREEITEM hItem = m_treeWorld.GetSelectedItem();
	if (m_treeWorld.GetRootItem() == hItem)
		return;

	auto World = find_if(m_MeshObjectMap.begin(), m_MeshObjectMap.end(), CTagFinder(m_strCurrentWorld));
	auto map = find_if(World->second.begin(), World->second.end(), CTagFinder(m_treeWorld.GetItemText(hItem)));

	vector<CToolModel*>& vecModel = map->second;
	for (auto vector : vecModel)
	{
		vector->Add_Position(XMVectorSet(0, 0, m_fPosZ, 1));
	}
}


void CMainTab::OnDeltaposScaleX(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	m_fScaleX = m_fScaleX + (_float)pNMUpDown->iDelta / 1000.f;
	UpdateData(FALSE);


	Scaling_Target();
	*pResult = 0;
}


void CMainTab::OnDeltaposScaleY(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	m_fScaleY = m_fScaleY + (_float)pNMUpDown->iDelta / 1000.f;
	UpdateData(FALSE);

	Scaling_Target();
	*pResult = 0;
}


void CMainTab::OnDeltaposScaleZ(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	m_fScaleZ = m_fScaleZ + (_float)pNMUpDown->iDelta / 1000.f;
	UpdateData(FALSE);

	Scaling_Target();
	*pResult = 0;
}

void CMainTab::Scaling_Target()
{
	HTREEITEM hItem = m_treeWorld.GetSelectedItem();
	if (m_treeWorld.ItemHasChildren(hItem) == TRUE)
		return;

	HTREEITEM hParent = m_treeWorld.GetParentItem(hItem);
	auto World = find_if(m_MeshObjectMap.begin(), m_MeshObjectMap.end(), CTagFinder(m_strCurrentWorld));
	auto map = find_if(World->second.begin(), World->second.end(), CTagFinder(m_treeWorld.GetItemText(hParent)));

	vector<CToolModel*>& vecModel = map->second;
	DWORD_PTR pItemData = m_treeWorld.GetItemData(hItem);
	int iIndex = 0;
	for (auto vector : vecModel)
	{
		if ((DWORD_PTR)vector == pItemData)
		{
			vector->Set_Scale(XMVectorSet(m_fScaleX, m_fScaleY, m_fScaleZ, 1.f));
			break;
		}
		++iIndex;
	}
}


void CMainTab::OnDeltaposRotX(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	m_fRotX = m_fRotX + (_float)pNMUpDown->iDelta / 1.f;
	UpdateData(FALSE);

	Rotation_Target();
	*pResult = 0;
}


void CMainTab::OnDeltaposRotY(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	m_fRotY = m_fRotY + (_float)pNMUpDown->iDelta / 1.f;
	UpdateData(FALSE);

	Rotation_Target();
	*pResult = 0;
}


void CMainTab::OnDeltaposRotZ(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	m_fRotZ = m_fRotZ + (_float)pNMUpDown->iDelta / 1.f;
	UpdateData(FALSE);

	Rotation_Target();
	*pResult = 0;
}

void CMainTab::Rotation_Target()
{
	HTREEITEM hItem = m_treeWorld.GetSelectedItem();
	if (m_treeWorld.ItemHasChildren(hItem) == TRUE)
		return;

	HTREEITEM hParent = m_treeWorld.GetParentItem(hItem);
	auto World = find_if(m_MeshObjectMap.begin(), m_MeshObjectMap.end(), CTagFinder(m_strCurrentWorld));
	auto map = find_if(World->second.begin(), World->second.end(), CTagFinder(m_treeWorld.GetItemText(hParent)));

	vector<CToolModel*>& vecModel = map->second;
	DWORD_PTR pItemData = m_treeWorld.GetItemData(hItem);
	int iIndex = 0;
	for (auto vector : vecModel)
	{
		if ((DWORD_PTR)vector == pItemData)
		{
			vector->Set_Rotation(_float3(m_fRotX, m_fRotY, m_fRotZ));
			break;
		}
		++iIndex;
	}
}


void CMainTab::OnEnChangeMainContainername()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CMainTab::OnBnClickedMainChangename()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	HTREEITEM hChangeItem = m_treeWorld.GetSelectedItem();

	auto World = find_if(m_MeshObjectMap.begin(), m_MeshObjectMap.end(), CTagFinder(m_strCurrentWorld));
	auto Map = find_if(World->second.begin(), World->second.end(), CTagFinder(m_treeWorld.GetItemText(hChangeItem)));

	CONTAINERS mapNewContainer;
	vector<CToolModel*> vecNew;

	swap(Map->second, vecNew);

	World->second.erase(m_treeWorld.GetItemText(hChangeItem));
	m_treeWorld.SetItemText(hChangeItem, m_strContainerName);
	World->second.emplace(m_strContainerName, vecNew);

	UpdateData(FALSE);
}
