// TerrainTab.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Tool.h"
#include "TerrainTab.h"
#include "afxdialogex.h"
#include "GameInstance.h"
#include "Terrain.h"
#include "MainFrm.h"
#include "ToolView.h"
#include "MainTab.h"

// CTerrainTab ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CTerrainTab, CDialog)

CTerrainTab::CTerrainTab(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_TERRAINTAB, pParent)
	, m_iVerticesX(0)
	, m_iVerticesZ(0)
{

}

CTerrainTab::~CTerrainTab()
{
}

void CTerrainTab::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TERRAINCHECK1, m_WireFrameCheckBox);
	DDX_Text(pDX, IDC_TERRAINEDIT1, m_iVerticesX);
	DDX_Text(pDX, IDC_TERRAINEDIT2, m_iVerticesZ);
	DDX_Control(pDX, IDC_TERRAINRADIO3, m_Radio_UpButton);
	DDX_Control(pDX, IDC_TERRAINRADIO5, m_Radio_DownButton);
	DDX_Control(pDX, IDC_TERRAINSLIDER3, m_SliderRange);
	DDX_Control(pDX, IDC_TERRAINRADIO1, m_RadioCircle);
	DDX_Control(pDX, IDC_TERRAINLIST1, m_SplattingListBox);
}


BEGIN_MESSAGE_MAP(CTerrainTab, CDialog)
	ON_BN_CLICKED(IDC_TERRAINCHECK1, &CTerrainTab::OnBnClickedWireFrameCheckBox)
	ON_BN_CLICKED(IDC_TERRAINBUTTON1, &CTerrainTab::OnBnClickedTerrainResetButton)
	ON_BN_CLICKED(IDC_TERRAINRADIO3, &CTerrainTab::OnBnClickedTerrain_UpButton)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_TERRAINSLIDER3, &CTerrainTab::OnNMReleasedcaptureTerrainsliderRange)
	ON_BN_CLICKED(IDC_TERRAINRADIO1, &CTerrainTab::OnBnClickedTerrainradioCircle)
	ON_BN_CLICKED(IDC_TERRAINRADIO2, &CTerrainTab::OnBnClickedTerrainradioRectangle)
	ON_BN_CLICKED(IDC_TERRAINRADIO5, &CTerrainTab::OnBnClickedTerrainRadioDown)
END_MESSAGE_MAP()


// CTerrainTab �޽��� ó�����Դϴ�.


BOOL CTerrainTab::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	
	m_SliderRange.SetRange(0, 20);

	m_SliderRange.SetRangeMin(0);
	m_SliderRange.SetRangeMax(20);

	m_SliderRange.SetTicFreq(1);

	m_SliderRange.SetLineSize(1);

	m_SplattingListBox.AddString(PROTO_KEY_TEXTURE_TERRAIN);


	return TRUE;  // return TRUE unless you set the focus to a control
				  // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

HRESULT CTerrainTab::Create_Terrain()
{
	if (nullptr != m_pTerrain)
	{
		Safe_Release(m_pTerrain);
		m_pTerrain = nullptr;
	}

	UpdateData(TRUE);

	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	_uint iNumVertices[2] = { (_uint)m_iVerticesX, (_uint)m_iVerticesZ };


	if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr((_uint)LEVEL::STATIC, PROTO_KEY_TERRAIN, TEXT("Layer_Terrain"), (CGameObject**)&m_pTerrain, iNumVertices)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	m_pTerrain->Set_Dir(true);
	m_pTerrain->Set_Circle(true);
	m_pTerrain->Set_Picking(true);

	m_Radio_UpButton.SetCheck(BST_CHECKED);
	m_RadioCircle.SetCheck(BST_CHECKED);

	RELEASE_INSTANCE(CGameInstance);
	UpdateData(FALSE);
	
	return S_OK;
}



void CTerrainTab::OnBnClickedWireFrameCheckBox()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if (BST_CHECKED == m_WireFrameCheckBox.GetCheck())
		m_pTerrain->Set_Pass(1);
	else
		m_pTerrain->Set_Pass(0);
}


void CTerrainTab::OnBnClickedTerrainResetButton()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	if (FAILED(Create_Terrain()))
		return;
}


void CTerrainTab::OnBnClickedTerrain_UpButton()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if (BST_CHECKED == m_Radio_UpButton.GetCheck())
	{
		if (nullptr == m_pTerrain)
			return;

		m_pTerrain->Set_Dir(true);
		m_pTerrain->Set_Picking(true);
	}
}


void CTerrainTab::OnNMReleasedcaptureTerrainsliderRange(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if (nullptr == m_pTerrain)
		return;

	m_pTerrain->Set_Range(m_SliderRange.GetPos());
	*pResult = 0;
}


void CTerrainTab::OnBnClickedTerrainradioCircle()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if (nullptr == m_pTerrain)
		return;

	m_pTerrain->Set_Circle(true);
}


void CTerrainTab::OnBnClickedTerrainradioRectangle()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if (nullptr == m_pTerrain)
		return;

	m_pTerrain->Set_Rectangle(true);
}


void CTerrainTab::OnBnClickedTerrainRadioDown()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if (nullptr == m_pTerrain)
		return;

	m_pTerrain->Set_Dir(false);
	m_pTerrain->Set_Picking(true);
}
