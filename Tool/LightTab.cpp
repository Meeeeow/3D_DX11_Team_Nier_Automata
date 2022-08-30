// LightTab.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool.h"
#include "LightTab.h"
#include "afxdialogex.h"
#include "GameInstance.h"
#include "MainFrm.h"
#include "ToolView.h"
#include "Light.h"

// CLightTab 대화 상자입니다.

IMPLEMENT_DYNAMIC(CLightTab, CDialog)

CLightTab::CLightTab(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_LIGHTTAB, pParent)
	, m_fLightRed(0)
	, m_fLightBlue(0)
	, m_fLightGreen(0)
	, m_fLightAlpha(0)
	, m_fPoint_X(0)
	, m_fPoint_Y(0)
	, m_fPoint_Z(0)
	, m_fPoint_Radius(0)
	, m_fDir_X(0)
	, m_fDir_Y(0)
	, m_fDir_Z(0)
	, m_fAmbient_Red(0)
	, m_fAmbient_Blue(0)
	, m_fAmbient_Green(0)
	, m_fAmbient_Alpha(0)
	, m_fSpecular_Red(0)
	, m_fSpecular_Blue(0)
	, m_fSpecular_Green(0)
	, m_fSpecular_Alpha(0)
{

}

CLightTab::~CLightTab()
{
	Safe_Release(m_pGraphicDevice);
	Safe_Release(m_pContextDevice);
}

void CLightTab::Init_Light()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();





	auto iter = find_if(m_mapIndex.begin(), m_mapIndex.end(), CTagFinder(m_strLightName));
	if (iter == m_mapIndex.end())
	{
		HTREEITEM hContainer = m_treeCtrlLight.InsertItem(m_strLightName, 1, 2, m_hRootItem, NULL);


		_uint iIndex = 0;
		CString strName;
		strName.Format(L"%s%d", m_strLightName, iIndex);

		m_mapIndex.emplace(m_strLightName, iIndex);
		m_treeCtrlLight.InsertItem(strName, 3, 4, hContainer, NULL);
		m_treeCtrlLight.Expand(m_hRootItem, TVE_EXPAND);
		m_treeCtrlLight.Expand(hContainer, TVE_EXPAND);
	}

	LIGHTDESC tLightDesc;
	tLightDesc.eType = LIGHTDESC::TYPE::DIRECTIONAL;

	tLightDesc.vDiffuse = CProportionConverter()(_ALICEBLUE, 93.f);
	tLightDesc.vAmbient = CProportionConverter()(_ALICEBLUE, 88.f);
	tLightDesc.vSpecular = CProportionConverter()(_ALICEBLUE, 17.f);
	tLightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);

	tLightDesc.bLightOn = true;
	tLightDesc.eState = LIGHTSTATE::ALWAYS;

	if (FAILED(pGameInstance->Add_Light_ForTool(m_pGraphicDevice, m_pContextDevice, m_strLightName, Shader_Viewport, DefaultTechnique, tLightDesc)))
		return;
}

HTREEITEM CLightTab::Get_CurrentWorld()
{
	int iImage = 0;
	int iSelectImage = 0;
	m_treeCtrlLight.GetItemImage(m_treeCtrlLight.GetSelectedItem(), iImage, iSelectImage);
	if (iImage == 0 && iSelectImage == 0)
		return m_treeCtrlLight.GetSelectedItem();

	return NULL;
}

HTREEITEM CLightTab::Get_CurrentContainer()
{
	int iImage = 0;
	int iSelectImage = 0;
	m_treeCtrlLight.GetItemImage(m_treeCtrlLight.GetSelectedItem(), iImage, iSelectImage);
	if (iImage == 1 && iSelectImage == 2)
		return m_treeCtrlLight.GetSelectedItem();

	return NULL;
}

HTREEITEM CLightTab::Get_CurrentItem()
{
	int iImage = 0;
	int iSelectImage = 0;
	m_treeCtrlLight.GetItemImage(m_treeCtrlLight.GetSelectedItem(), iImage, iSelectImage);
	if (iImage == 3 && iSelectImage == 4)
		return m_treeCtrlLight.GetSelectedItem();

	return NULL;
}

void CLightTab::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TreeCtrl_Light, m_treeCtrlLight);
	DDX_Text(pDX, Edit_Red, m_fLightRed);
	DDX_Text(pDX, Edit_Blue, m_fLightBlue);
	DDX_Text(pDX, Edit_Green, m_fLightGreen);
	DDX_Text(pDX, Edit_Alpha, m_fLightAlpha);
	DDX_Text(pDX, Edit_LightName, m_strLightName);
	DDX_Text(pDX, Edit_Light_PositionX, m_fPoint_X);
	DDX_Text(pDX, Edit_Light_PositionY, m_fPoint_Y);
	DDX_Text(pDX, Edit_Light_PositionZ, m_fPoint_Z);
	DDX_Text(pDX, Edit_Light_Radius, m_fPoint_Radius);
	DDX_Text(pDX, Edit_Light_DirectionX, m_fDir_X);
	DDX_Text(pDX, Edit_Light_DirectionY, m_fDir_Y);
	DDX_Text(pDX, Edit_Light_DirectionZ, m_fDir_Z);
	DDX_Radio(pDX, Radio_Direction, (int&)m_iRadio_Index);
	DDX_Control(pDX, Check_LightOn, m_checkLightOn);
	DDX_Text(pDX, Edit_Ambient_Red, m_fAmbient_Red);
	DDX_Text(pDX, Edit_Ambient_Blue, m_fAmbient_Blue);
	DDX_Text(pDX, Edit_Ambient_Green, m_fAmbient_Green);
	DDX_Text(pDX, Edit_Ambient_Alpha, m_fAmbient_Alpha);
	DDX_Text(pDX, Edit_Specular_Red, m_fSpecular_Red);
	DDX_Text(pDX, Edit_Specular_Blue, m_fSpecular_Blue);
	DDX_Text(pDX, Edit_Specular_Green, m_fSpecular_Green);
	DDX_Text(pDX, Edit_Specular_Alpha, m_fSpecular_Alpha);
}


BEGIN_MESSAGE_MAP(CLightTab, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TreeCtrl_Light, &CLightTab::OnTvnSelchangedTreectrlLight)
	ON_BN_CLICKED(Button_LightCreate, &CLightTab::OnBnClickedLightcreate)
	ON_EN_CHANGE(Edit_LightName, &CLightTab::OnEnChangeLightname)
	ON_EN_CHANGE(Edit_Light_PositionX, &CLightTab::OnEnChangeLightPositionx)
	ON_EN_CHANGE(Edit_Light_PositionY, &CLightTab::OnEnChangeLightPositiony)
	ON_EN_CHANGE(Edit_Light_PositionZ, &CLightTab::OnEnChangeLightPositionz)
	ON_EN_CHANGE(Edit_Light_Radius, &CLightTab::OnEnChangeLightRadius)
	ON_EN_CHANGE(Edit_Light_DirectionX, &CLightTab::OnEnChangeLightDirectionx)
	ON_EN_CHANGE(Edit_Light_DirectionY, &CLightTab::OnEnChangeLightDirectiony)
	ON_EN_CHANGE(Edit_Light_DirectionZ, &CLightTab::OnEnChangeLightDirectionz)
	ON_EN_CHANGE(Edit_Red, &CLightTab::OnEnChangeRed)
	ON_EN_CHANGE(Edit_Blue, &CLightTab::OnEnChangeBlue)
	ON_EN_CHANGE(Edit_Green, &CLightTab::OnEnChangeGreen)
	ON_EN_CHANGE(Edit_Alpha, &CLightTab::OnEnChangeAlpha)
	ON_BN_CLICKED(Button_LightDelete, &CLightTab::OnBnClickedLightdelete)
	ON_BN_CLICKED(Radio_Direction, &CLightTab::OnBnClickedDirection)
	ON_BN_CLICKED(Radio_Point, &CLightTab::OnBnClickedPoint)
	ON_BN_CLICKED(Radio_SpotLight, &CLightTab::OnBnClickedSpotlight)
	ON_CONTROL_RANGE(BN_CLICKED, Radio_Direction, Radio_SpotLight,&CLightTab::RadioCtrl)
	ON_BN_CLICKED(Check_LightOn, &CLightTab::OnBnClickedLighton)
	ON_EN_CHANGE(Edit_Ambient_Red, &CLightTab::OnEnChangeAmbientRed)
	ON_EN_CHANGE(Edit_Ambient_Blue, &CLightTab::OnEnChangeAmbientBlue)
	ON_EN_CHANGE(Edit_Ambient_Green, &CLightTab::OnEnChangeAmbientGreen)
	ON_EN_CHANGE(Edit_Ambient_Alpha, &CLightTab::OnEnChangeAmbientAlpha)
	ON_EN_CHANGE(Edit_Specular_Red, &CLightTab::OnEnChangeSpecularRed)
	ON_EN_CHANGE(Edit_Specular_Blue, &CLightTab::OnEnChangeSpecularBlue)
	ON_EN_CHANGE(Edit_Specular_Green, &CLightTab::OnEnChangeSpecularGreen)
	ON_EN_CHANGE(Edit_Specular_Alpha, &CLightTab::OnEnChangeSpecularAlpha)
	ON_BN_CLICKED(Button_Save, &CLightTab::OnBnClickedSave)
	ON_BN_CLICKED(Button_Load, &CLightTab::OnBnClickedLoad)
END_MESSAGE_MAP()


// CLightTab 메시지 처리기입니다.


void CLightTab::OnTvnSelchangedTreectrlLight(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	HTREEITEM World = Get_CurrentWorld();
	HTREEITEM Container = Get_CurrentContainer();
	HTREEITEM Item = Get_CurrentItem();

	if (World != NULL)
		m_strLightName = m_treeCtrlLight.GetItemText(World);
	else if (Container != NULL)
		m_strLightName = m_treeCtrlLight.GetItemText(Container);
	else if (Item != NULL)
		m_strLightName = m_treeCtrlLight.GetItemText(Item);


	if (Item != NULL)
	{
		HTREEITEM hParent = m_treeCtrlLight.GetParentItem(Item);
		HTREEITEM hChild = m_treeCtrlLight.GetChildItem(hParent);
		_uint i = 0;
		while (true)
		{
			if (hChild == Item)
				break;

			hChild = m_treeCtrlLight.GetNextItem(hChild, TVGN_NEXT);
			++i;
		}


		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		CString strName = m_treeCtrlLight.GetItemText(hParent);

		LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(strName, i);
		if (pLightDesc == nullptr)
			return;
		Change_Info(pLightDesc);
	}

	UpdateData(FALSE);
	*pResult = 0;
}

HRESULT CLightTab::Change_Info(LIGHTDESC * pLightDesc)
{
	m_tLightDesc = *pLightDesc;

	m_fLightRed = m_tLightDesc.vDiffuse.x;
	m_fLightGreen = m_tLightDesc.vDiffuse.y;
	m_fLightBlue = m_tLightDesc.vDiffuse.z;
	m_fLightAlpha = m_tLightDesc.vDiffuse.w;

	m_fAmbient_Red = m_tLightDesc.vAmbient.x;
	m_fAmbient_Green = m_tLightDesc.vAmbient.y;
	m_fAmbient_Blue = m_tLightDesc.vAmbient.z;
	m_fAmbient_Alpha = m_tLightDesc.vAmbient.w;

	m_fSpecular_Red = m_tLightDesc.vSpecular.x;
	m_fSpecular_Green = m_tLightDesc.vSpecular.y;
	m_fSpecular_Blue = m_tLightDesc.vSpecular.z;
	m_fSpecular_Alpha = m_tLightDesc.vSpecular.w;

	m_fPoint_X = m_tLightDesc.vPosition.x;
	m_fPoint_Y = m_tLightDesc.vPosition.y;
	m_fPoint_Z = m_tLightDesc.vPosition.z;
	m_fPoint_Radius = m_tLightDesc.fRange;

	m_fDir_X = m_tLightDesc.vDirection.x;
	m_fDir_Y = m_tLightDesc.vDirection.y;
	m_fDir_Z = m_tLightDesc.vDirection.z;

	return S_OK;
}


void CLightTab::OnBnClickedLightcreate()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	
	HTREEITEM World = Get_CurrentWorld();
	HTREEITEM Container = Get_CurrentContainer();
	HTREEITEM Item = Get_CurrentItem();

	if (World == m_hRootItem)
	{
		auto iter = find_if(m_mapIndex.begin(), m_mapIndex.end(), CTagFinder(m_strLightName));
		if (iter == m_mapIndex.end())
		{
			HTREEITEM	hContainer = m_treeCtrlLight.InsertItem(m_strLightName, 1, 2, World, NULL);


			_uint iIndex = 0;
			CString strName;
			strName.Format(L"%s%d", m_strLightName, iIndex);

			m_mapIndex.emplace(m_strLightName, iIndex);
			m_treeCtrlLight.InsertItem(strName, 3, 4, hContainer, NULL);
			m_treeCtrlLight.Expand(m_hRootItem, TVE_EXPAND);
			m_treeCtrlLight.Expand(hContainer, TVE_EXPAND);
		}
		else
			return;

	}
	else if (World == NULL && Container != NULL)
	{
		auto iter = find_if(m_mapIndex.begin(), m_mapIndex.end(), CTagFinder(m_strLightName));
		if (iter == m_mapIndex.end())
			return;

		CString strName;
		strName.Format(L"%s%d", iter->first, ++(iter->second));
		m_treeCtrlLight.InsertItem(strName, 3, 4, Container, NULL);
	}
	else if (Item != NULL)
		return;

	if (FAILED(pGameInstance->Add_Light_ForTool(m_pGraphicDevice, m_pContextDevice, m_strLightName, Shader_Viewport, DefaultTechnique, m_tLightDesc)))
		return;
}


void CLightTab::OnEnChangeLightname()
{
	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	UpdateData(FALSE);
}


void CLightTab::OnEnChangeLightPositionx()
{
	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	m_tLightDesc.vPosition.x = m_fPoint_X;
	UpdateData(FALSE);
}


void CLightTab::OnEnChangeLightPositiony()
{
	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	m_tLightDesc.vPosition.y = m_fPoint_Y;
	UpdateData(FALSE);
}


void CLightTab::OnEnChangeLightPositionz()
{
	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	m_tLightDesc.vPosition.z = m_fPoint_Z;
	UpdateData(FALSE);
}


void CLightTab::OnEnChangeLightRadius()
{
	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	m_tLightDesc.fRange = m_fPoint_Radius;
	UpdateData(FALSE);
}


void CLightTab::OnEnChangeLightDirectionx()
{
	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	m_tLightDesc.vDirection.x = m_fDir_X;
	UpdateData(FALSE);
}


void CLightTab::OnEnChangeLightDirectiony()
{
	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	m_tLightDesc.vDirection.y = m_fDir_Y;
	UpdateData(FALSE);
}


void CLightTab::OnEnChangeLightDirectionz()
{
	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	m_tLightDesc.vDirection.z = m_fDir_Z;
	UpdateData(FALSE);
}


void CLightTab::OnEnChangeRed()
{
	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	m_tLightDesc.vDiffuse.x = m_fLightRed;
	UpdateData(FALSE);
}


void CLightTab::OnEnChangeBlue()
{
	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	m_tLightDesc.vDiffuse.z = m_fLightBlue;
	UpdateData(FALSE);
}


void CLightTab::OnEnChangeGreen()
{
	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	m_tLightDesc.vDiffuse.y = m_fLightGreen;
	UpdateData(FALSE);
}


void CLightTab::OnEnChangeAlpha()
{
	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	m_tLightDesc.vDiffuse.w = m_fLightAlpha;
	UpdateData(FALSE);
}


void CLightTab::OnBnClickedLightdelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	HTREEITEM			hContainer = Get_CurrentContainer();
	HTREEITEM			hItem = Get_CurrentItem();
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CString strContainerName;

	if (hContainer != NULL)
	{
		strContainerName = m_treeCtrlLight.GetItemText(hContainer);
		pGameInstance->Delete_LightList(strContainerName);
		

		HTREEITEM hNextItem;
		HTREEITEM hDeleteItem = m_treeCtrlLight.GetChildItem(hContainer);
		while (hDeleteItem != NULL)
		{
			hNextItem = m_treeCtrlLight.GetNextItem(hDeleteItem, TVGN_NEXT);
			m_treeCtrlLight.DeleteItem(hDeleteItem);
			hDeleteItem = hNextItem;
		}

	}
	else if (hItem != NULL)
	{
		HTREEITEM hParent = m_treeCtrlLight.GetParentItem(hItem);
		HTREEITEM hChild = m_treeCtrlLight.GetChildItem(hParent);

		strContainerName = m_treeCtrlLight.GetItemText(hParent);

		_uint iIndex = 0;
		while (true)
		{
			if (hChild == hItem)
				break;

			hChild = m_treeCtrlLight.GetNextItem(hChild, TVGN_NEXT);
			++iIndex;
		}

		pGameInstance->Delete_LightIndex(strContainerName, iIndex);

		m_treeCtrlLight.DeleteItem(hChild);
	}

}


BOOL CLightTab::OnInitDialog()
{
	CDialog::OnInitDialog();
	CBitmap bmp;
	bmp.LoadBitmap(IDB_BITMAP);

	m_imagelist.DeleteImageList();
	m_imagelist.Create(16, 16, ILC_COLOR16 | ILC_MASK, 0, 0);
	m_imagelist.Add(&bmp, RGB(255, 0, 255));

	m_treeCtrlLight.SetImageList(&m_imagelist, TVSIL_NORMAL);

	m_hRootItem = m_treeCtrlLight.InsertItem(L"World", 0, 0, NULL, NULL);

	m_checkLightOn.SetCheck(TRUE);

	CMainFrame*	pMain = dynamic_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd());
	CToolView*	pToolView = dynamic_cast<CToolView*>(pMain->GetActiveView());

	m_pGraphicDevice = pToolView->Get_GraphicDevice();
	Safe_AddRef(m_pGraphicDevice);
	m_pContextDevice = pToolView->Get_ContextDevice();
	Safe_AddRef(m_pContextDevice);

	m_tLightDesc.vPosition.w = 1.f;

 	Init_Light();


	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CLightTab::RadioCtrl(UINT ID)
{
	switch (ID)
	{
	case Radio_Direction:
		m_tLightDesc.eType = LIGHTDESC::TYPE::DIRECTIONAL;
		break;
	case Radio_Point:
		m_tLightDesc.eType = LIGHTDESC::TYPE::POINT;
		break;
	case Radio_SpotLight:
		m_tLightDesc.eType = LIGHTDESC::TYPE::SPOT;
		break;
	default:
		break;
	}
}


void CLightTab::OnBnClickedDirection()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	RadioCtrl(Radio_Direction);
}


void CLightTab::OnBnClickedPoint()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	RadioCtrl(Radio_Point);
}


void CLightTab::OnBnClickedSpotlight()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	RadioCtrl(Radio_SpotLight);
}


void CLightTab::OnBnClickedLighton()
{
	// Client Load시 불을 밝힐 것 인지 체크합니다.
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_checkLightOn.GetCheck() == TRUE)
		m_tLightDesc.bLightOn = true;
	else
		m_tLightDesc.bLightOn = false;
}


void CLightTab::OnEnChangeAmbientRed()
{
	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	m_tLightDesc.vAmbient.x = m_fAmbient_Red;
	UpdateData(FALSE);
}


void CLightTab::OnEnChangeAmbientBlue()
{
	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	m_tLightDesc.vAmbient.z = m_fAmbient_Blue;
	UpdateData(FALSE);
}


void CLightTab::OnEnChangeAmbientGreen()
{
	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	m_tLightDesc.vAmbient.y = m_fAmbient_Green;
	UpdateData(FALSE);
}


void CLightTab::OnEnChangeAmbientAlpha()
{
	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	m_tLightDesc.vAmbient.w = m_fAmbient_Alpha;
	UpdateData(FALSE);
}


void CLightTab::OnEnChangeSpecularRed()
{
	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	m_tLightDesc.vSpecular.x = m_fSpecular_Red;
	UpdateData(FALSE);
}


void CLightTab::OnEnChangeSpecularBlue()
{
	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	m_tLightDesc.vSpecular.z = m_fSpecular_Blue;
	UpdateData(FALSE);
}


void CLightTab::OnEnChangeSpecularGreen()
{
	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	m_tLightDesc.vSpecular.y = m_fSpecular_Blue;
	UpdateData(FALSE);
}


void CLightTab::OnEnChangeSpecularAlpha()
{
	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	m_tLightDesc.vSpecular.w = m_fSpecular_Alpha;
	UpdateData(FALSE);
}


void CLightTab::OnBnClickedSave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CGameInstance* pGameInstnace = CGameInstance::Get_Instance();
	CFileDialog Dlg(FALSE, L"dat", L"*.dat");
	TCHAR szBuf[MAX_PATH]{};
	GetCurrentDirectory(MAX_PATH, szBuf);
	PathRemoveFileSpec(szBuf);
	lstrcat(szBuf, L"\\Client\\Bin\\Data\\LightData\\");
	Dlg.m_ofn.lpstrInitialDir = szBuf;
	if (IDOK == Dlg.DoModal())
	{
		CString wstrPath = Dlg.GetPathName();

		HANDLE hFile = CreateFile(wstrPath.GetString(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (hFile == INVALID_HANDLE_VALUE)
			return;

		_ulong dwByte = 0;

		HTREEITEM hContainer = m_treeCtrlLight.GetChildItem(m_hRootItem);


		while (true)
		{
			if (hContainer == NULL)
				break;

			wchar_t tagLightLayer[32];
			lstrcpy(tagLightLayer, m_treeCtrlLight.GetItemText(hContainer));
			WriteFile(hFile, tagLightLayer, sizeof(wchar_t) * 32, &dwByte, nullptr);

			list<CLight*>* pLightList = pGameInstnace->Get_LightList(m_treeCtrlLight.GetItemText(hContainer));
			_uint iSize = pLightList->size();
			WriteFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);
			for (_uint i = 0; i < iSize; ++i)
			{
				LIGHTDESC* pLightDesc = pGameInstnace->Get_LightDesc(tagLightLayer, i);
				WriteFile(hFile, pLightDesc, sizeof(LIGHTDESC), &dwByte, nullptr);
			}
			hContainer = m_treeCtrlLight.GetNextItem(hContainer, TVGN_NEXT);
		}

	}
}


void CLightTab::OnBnClickedLoad()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CFileDialog Dlg(TRUE, L"dat", L"*.dat");
	TCHAR szBuf[MAX_PATH]{};
	GetCurrentDirectory(MAX_PATH, szBuf);
	PathRemoveFileSpec(szBuf);
	lstrcat(szBuf, L"\\Client\\Bin\\Data\\LightData\\");
	Dlg.m_ofn.lpstrInitialDir = szBuf;
	if (IDOK == Dlg.DoModal())
	{
		CString wstrPath = Dlg.GetPathName();

		HANDLE	hFile = CreateFile(wstrPath.GetString(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

		if (INVALID_HANDLE_VALUE == hFile)
			return;

		_ulong	dwByte = 0;
		while(true)
		{
			wchar_t pLayerTag[32];
			ReadFile(hFile, pLayerTag, sizeof(wchar_t) * 32, &dwByte, nullptr);

			if (dwByte <= 0)
				break;

			_uint iSize = 0;
			ReadFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);

			for (_uint i = 0; i < iSize; ++i)
			{
				LIGHTDESC tLightDesc;
				ReadFile(hFile, &tLightDesc, sizeof(LIGHTDESC), &dwByte, nullptr);
				LoadObject(pLayerTag, tLightDesc);
				auto iter = find_if(m_mapIndex.begin(), m_mapIndex.end(), CTagFinder(pLayerTag));
				if(iter == m_mapIndex.end())
					pGameInstance->Add_Light_ForTool(m_pGraphicDevice, m_pContextDevice, pLayerTag, Shader_Viewport, DefaultTechnique, tLightDesc);
				else
					pGameInstance->Add_Light_ForTool(m_pGraphicDevice, m_pContextDevice, iter->first, Shader_Viewport, DefaultTechnique, tLightDesc);
			}
		}
		CloseHandle(hFile);
	}
}

void CLightTab::LoadObject(CString tag, LIGHTDESC tLightDesc)
{
	auto iter = find_if(m_mapIndex.begin(), m_mapIndex.end(), CTagFinder(tag));
	if (iter == m_mapIndex.end())
	{
		_uint iIndex = 0;
		m_mapIndex.emplace(tag, iIndex);

		HTREEITEM hContainer = m_treeCtrlLight.InsertItem(tag, 1, 2, m_hRootItem, NULL);
		
		CString strName;
		strName.Format(L"%s%d", tag, iIndex);

		m_treeCtrlLight.InsertItem(strName, 3, 4, hContainer, NULL);
	}
	else
	{
		HTREEITEM hContainer = m_treeCtrlLight.GetChildItem(m_hRootItem);

		while (true)
		{
			CString strName;
			strName = m_treeCtrlLight.GetItemText(hContainer);

			if (tag.Compare(strName) == 0)
				break;

			hContainer = m_treeCtrlLight.GetNextItem(hContainer, TVGN_NEXT);
		}

		CString strItemName;
		strItemName.Format(L"%s%d", iter->first, ++(iter->second));

		m_treeCtrlLight.InsertItem(strItemName, 3, 4, hContainer, NULL);
	}

}
