// NavigationTab.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool.h"
#include "NavigationTab.h"
#include "afxdialogex.h"
#include "ToolNavigation.h"
#include "MainFrm.h"
#include "ToolView.h"


// CNavigationTab 대화 상자입니다.

IMPLEMENT_DYNAMIC(CNavigationTab, CDialog)

CNavigationTab::CNavigationTab(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_NAVIGATIONTAB, pParent)
	, m_fPoint0_X(0)
	, m_fPoint0_Y(0)
	, m_fPoint0_Z(0)
	, m_fPoint1_X(0)
	, m_fPoint1_Y(0)
	, m_fPoint1_Z(0)
	, m_fPoint2_X(0)
	, m_fPoint2_Y(0)
	, m_fPoint2_Z(0)
	, m_iNavCount(0)
	, m_iCurNavIndex(0)
{

}

CNavigationTab::~CNavigationTab()
{
	Safe_Release(m_pGraphicDevice);
	Safe_Release(m_pContextDevice);
	for (auto Navis : m_vecNavis)
		Safe_Release(Navis);
}

void CNavigationTab::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, List_Navigation, m_listNavigation);
	DDX_Text(pDX, Edit_Point0_X, m_fPoint0_X);
	DDX_Text(pDX, Edit_Point0_Y, m_fPoint0_Y);
	DDX_Text(pDX, Edit_Point0_Z, m_fPoint0_Z);
	DDX_Text(pDX, Edit_Point1_X, m_fPoint1_X);
	DDX_Text(pDX, Edit_Point1_Y, m_fPoint1_Y);
	DDX_Text(pDX, Edit_Point1_Z, m_fPoint1_Z);
	DDX_Text(pDX, Edit_Point2_X, m_fPoint2_X);
	DDX_Text(pDX, Edit_Point2_Y, m_fPoint2_Y);
	DDX_Text(pDX, Edit_Point2_Z, m_fPoint2_Z);
	DDX_Control(pDX, Check_Picking, m_checkPicking);
	DDX_Control(pDX, Check_AutoCreate, m_checkAutoCreate);
	DDX_Radio(pDX, Radio_None, (int&)m_iRadio_Index);
	DDX_Text(pDX, Edit_NavCount, m_iNavCount);
	DDX_Text(pDX, Edit_CurNavCount, m_iCurNavIndex);
}

_int CNavigationTab::Tick(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (IsWindowVisible() == true)
	{
		if (pGameInstance->Key_Down(DIK_Z))
		{
			if (m_checkPicking.GetCheck() == TRUE)
				m_checkPicking.SetCheck(FALSE);
			else
				m_checkPicking.SetCheck(TRUE);
		}
		if (pGameInstance->Key_Down(DIK_X))
		{
			if (m_checkAutoCreate.GetCheck() == TRUE)
				m_checkAutoCreate.SetCheck(FALSE);
			else
				m_checkAutoCreate.SetCheck(TRUE);
		}
		if (pGameInstance->Key_Down(DIK_C))
			CreateNavMesh();

		if (pGameInstance->Key_Down(DIK_R))
			Reset_Navigation();
		if (pGameInstance->Key_Down(DIK_V))
			OnBnClickedNaviDelete();

		if (pGameInstance->Key_Down(DIK_B))
			Render_All();

	}


	return _int();
}

_int CNavigationTab::LateTick(_double dTimeDelta)
{
	for (auto Navis : m_vecNavis)
		Navis->LateTick(dTimeDelta);

	Make_Point();
	

	return _int();
}

_int CNavigationTab::Picking(_double dTimeDelta)
{
	for (auto Navis : m_vecNavis)
		Navis->Picking();

	return 0;
}

void CNavigationTab::Insert_NavDesc(PNMDESC tDesc)
{
	if (m_checkPicking.GetCheck() == false || m_iCurrentPoint == 3)
		return;

	m_listNM.push_back(tDesc);
}

void CNavigationTab::Make_Point()
{
	if (m_listNM.size() == 0 || m_iCurrentPoint == 3)
		return;


	m_listNM.sort([=](PNMDESC t1, PNMDESC t2) {
		return t1.fDistance < t2.fDistance;
	});
	
	m_tInfo.vPoint[m_iCurrentPoint++] = m_listNM.front().vPoint;
	Update_Point();

	m_listNM.clear();
	if (m_iCurrentPoint == 3 && m_checkAutoCreate.GetCheck() == true)
	{
		m_iCurrentPoint = 0;
		m_vecNavis.push_back(CToolNavigation::Create(m_pGraphicDevice, m_pContextDevice, m_tInfo));
		m_listNavigation.AddString(L"NavigationMesh");
		
		for (_uint i = 0; i < 3; ++i)
			m_tInfo.vPoint[i] = _float3(0.f, 0.f, 0.f);
		Update_Point();
	}

}

void CNavigationTab::Update_Point()
{
	UpdateData(TRUE);

	m_fPoint0_X = m_tInfo.vPoint[0].x; m_fPoint0_Y = m_tInfo.vPoint[0].y; m_fPoint0_Z = m_tInfo.vPoint[0].z;
	m_fPoint1_X = m_tInfo.vPoint[1].x; m_fPoint1_Y = m_tInfo.vPoint[1].y; m_fPoint1_Z = m_tInfo.vPoint[1].z;
	m_fPoint2_X = m_tInfo.vPoint[2].x; m_fPoint2_Y = m_tInfo.vPoint[2].y; m_fPoint2_Z = m_tInfo.vPoint[2].z;
	m_iNavCount = m_vecNavis.size();
	UpdateData(FALSE);
}

void CNavigationTab::Reset_Navigation()
{
	for (_uint i = 0; i < 3; ++i)
		m_tInfo.vPoint[i] = _float3(0.f, 0.f, 0.f);
	m_iCurrentPoint = 0;

	Update_Point();
}

void CNavigationTab::Render_All()
{
	m_bRender_All = !m_bRender_All;

	if (m_bRender_All)
	{
		for (auto& pCell : m_vecNavis)
			pCell->Set_TargetOn();
	}
	else
	{
		for (auto& pCell : m_vecNavis)
			pCell->Set_TargetOff();
	}
}


BEGIN_MESSAGE_MAP(CNavigationTab, CDialog)
	ON_LBN_SELCHANGE(List_Navigation, &CNavigationTab::OnLbnSelchangeNavigation)
	ON_BN_CLICKED(Button_Create, &CNavigationTab::OnBnClickedCreate)
	ON_BN_CLICKED(Button_Save, &CNavigationTab::OnBnClickedSave)
	ON_BN_CLICKED(Button_Load, &CNavigationTab::OnBnClickedLoad)
	ON_BN_CLICKED(Radio_Fall, &CNavigationTab::OnBnClickedFall)
	ON_BN_CLICKED(Radio_None, &CNavigationTab::OnBnClickedNone)
	ON_CONTROL_RANGE(BN_CLICKED, Radio_None, Radio_Fall, &CNavigationTab::RadioCtrl)
	ON_BN_CLICKED(Button_Navi_Delete, &CNavigationTab::OnBnClickedNaviDelete)
	ON_BN_CLICKED(Button_Magical, &CNavigationTab::OnBnClickedMagical)
END_MESSAGE_MAP()


// CNavigationTab 메시지 처리기입니다.


void CNavigationTab::RadioCtrl(UINT id)
{
	UpdateData(TRUE);
	switch (id)
	{
	case Radio_None:
		m_tInfo.eOption = NAVINFO::OPTION::NONE;
		break;
	case Radio_Fall:
		m_tInfo.eOption = NAVINFO::OPTION::FALL;
		break;
	default:
		break;
	}
	UpdateData(FALSE);
}

void CNavigationTab::OnLbnSelchangeNavigation()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int iIndex = m_listNavigation.GetCurSel();
	if (iIndex < 0 || iIndex >= m_listNavigation.GetCount())
		return;

	
	auto Preiter = m_vecNavis.begin() + m_iCurNavIndex;
	auto iter = m_vecNavis.begin() + iIndex;



	for (auto Navigation : m_vecNavis)
	{
		if (Navigation == *iter)
			Navigation->Set_TargetOn();
		else
			Navigation->Set_TargetOff();
	}

	UpdateData(TRUE);
	m_iCurNavIndex = iIndex;
	UpdateData(FALSE);
}


void CNavigationTab::OnBnClickedCreate()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CreateNavMesh();
}


void CNavigationTab::OnBnClickedSave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFileDialog Dlg(FALSE, L"dat", L"*.dat");
	TCHAR szBuf[MAX_PATH]{};
	GetCurrentDirectory(MAX_PATH, szBuf);
	PathRemoveFileSpec(szBuf);
	lstrcat(szBuf, L"\\Client\\Bin\\Data\\NavigationData\\");
	Dlg.m_ofn.lpstrInitialDir = szBuf;
	_ulong dwByte = 0;
	if (IDOK == Dlg.DoModal())
	{
		CString wstrPath = Dlg.GetPathName();
		
		HANDLE hFile = CreateFile(wstrPath.GetBuffer(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (hFile == INVALID_HANDLE_VALUE)
			VOIDMSG("Failed to Save Scene");

		_uint iSize = m_vecNavis.size();
		WriteFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);

		for (auto Navigation : m_vecNavis)
			WriteFile(hFile, &Navigation->Get_NavInfo(), sizeof(NAVINFO), &dwByte, nullptr);

		CloseHandle(hFile);
	}
}


void CNavigationTab::OnBnClickedLoad()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFileDialog Dlg(TRUE, L"dat", L"*.dat");
	TCHAR szBuf[MAX_PATH]{};
	GetCurrentDirectory(MAX_PATH, szBuf);
	PathRemoveFileSpec(szBuf);
	lstrcat(szBuf, L"\\Client\\Bin\\Data\\NavigationData\\");
	Dlg.m_ofn.lpstrInitialDir = szBuf;
	if (IDOK == Dlg.DoModal())
	{
		CString wstrPath = Dlg.GetPathName();
		_ulong dwByte = 0;
		HANDLE hFile = CreateFile(wstrPath.GetBuffer(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		_uint iSize = 0;

		ReadFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);

		for (_uint i = 0; i < iSize; ++i)
		{
			NAVINFO tInfo;
			ReadFile(hFile, &tInfo, sizeof(NAVINFO), &dwByte, nullptr);
			m_vecNavis.push_back(CToolNavigation::Create(m_pGraphicDevice, m_pContextDevice, tInfo));
			m_listNavigation.AddString(L"NavigationMesh");
		}
		CloseHandle(hFile);
	}
	UpdateData(TRUE);
	m_iNavCount = m_vecNavis.size();
	UpdateData(FALSE);
}

void CNavigationTab::CreateNavMesh()
{
	if (m_checkAutoCreate.GetCheck() == TRUE || m_iCurrentPoint != 3)
		return;

	m_iCurrentPoint = 0;
	m_vecNavis.push_back(CToolNavigation::Create(m_pGraphicDevice, m_pContextDevice, m_tInfo));
	m_listNavigation.AddString(L"NavigationMesh");

	for (_uint i = 0; i < 3; ++i)
		m_tInfo.vPoint[i] = _float3(0.f, 0.f, 0.f);
	Update_Point();
}

void CNavigationTab::OnBnClickedFall()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	RadioCtrl(Radio_Fall);
}


void CNavigationTab::OnBnClickedNone()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	RadioCtrl(Radio_None);
}


BOOL CNavigationTab::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	CMainFrame*    pMain = dynamic_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd());
	CToolView*	   pToolView = dynamic_cast<CToolView*>(pMain->GetActiveView());

	m_pGraphicDevice = pToolView->Get_GraphicDevice();
	m_pContextDevice = pToolView->Get_ContextDevice();

	Safe_AddRef(m_pGraphicDevice);
	Safe_AddRef(m_pContextDevice);


	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CNavigationTab::OnBnClickedNaviDelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	_uint iSelect = m_listNavigation.GetCurSel();
	if (iSelect < 0 || iSelect >= m_listNavigation.GetCount())
		return;


	m_listNavigation.DeleteString(iSelect);
	auto iter = m_vecNavis.begin() + iSelect;

	m_vecNavis.erase(iter);
}


void CNavigationTab::OnBnClickedMagical()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	TCHAR szBuf[MAX_PATH]{};
	GetCurrentDirectory(MAX_PATH, szBuf);
	PathRemoveFileSpec(szBuf);
	lstrcat(szBuf, L"\\Client\\Bin\\Data\\NavigationData\\");

	CString FilePath, datFileName;
	datFileName = L"DriveThrough";
	FilePath = szBuf + datFileName + L".dat";

	DWORD dwByte = 0;

	HANDLE hFile = CreateFile(FilePath.GetString(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

	
	NAVINFO tInfo1, tInfo2;
	tInfo1.eOption = NAVINFO::OPTION::NONE;
	tInfo2.eOption = NAVINFO::OPTION::NONE;
	_uint iSize = 320;
	WriteFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < 40; ++i)
	{
		for (_uint j = 0; j < 4; ++j)
		{
			tInfo1.vPoint[0] = _float3(-0.92f + (0.4825f * j)		, 1.4f, -0.76f + (0.17875f * (i + 1)));
			tInfo1.vPoint[1] = _float3(-0.92f + (0.4825f * (j + 1))	, 1.4f, -0.76f + (0.17875f * i));
			tInfo1.vPoint[2] = _float3(-0.92f + (0.4825f * j)		, 1.4f, -0.76f + (0.17875f * i));

			tInfo2.vPoint[0] = _float3(-0.92f + (0.4825f * j)		, 1.4f, -0.76f + (0.17875f * (i + 1)));
			tInfo2.vPoint[1] = _float3(-0.92f + (0.4825f * (j + 1))	, 1.4f, -0.76f + (0.17875f * (i + 1)));
			tInfo2.vPoint[2] = _float3(-0.92f + (0.4825f * (j + 1))	, 1.4f, -0.76f + (0.17875f * i));

			WriteFile(hFile, &tInfo1, sizeof(NAVINFO), &dwByte, nullptr);
			WriteFile(hFile, &tInfo2, sizeof(NAVINFO), &dwByte, nullptr);
		}
	}

	CloseHandle(hFile);
}
