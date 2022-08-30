// AnimationTab.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool.h"
#include "AnimationTab.h"
#include "afxdialogex.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "ToolObject.h"
#include "Camera.h"
#include "MainFrm.h"
#include "ToolView.h"
#include "ToolCamera.h"

// CAnimationTab 대화 상자입니다.

IMPLEMENT_DYNAMIC(CAnimationTab, CDialog)

CAnimationTab::CAnimationTab(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_ANIMATIONTAB, pParent)
	, m_PlayTime(0)
	, m_Duration(0)
	, m_PlaySpeedPerSec(0)
	, m_iAnimationIndex(0)
	, m_LinkMinRatio(0)
	, m_LinkMaxRatio(0)
	, m_RatioTime(0)
	, m_BlendMaxTime(0)
	, m_iBlendingLevel(0)
	, m_iRootBoneKeyFrameIndex(0)
	, m_BonePositionX(0)
	, m_BonePositionY(0)
	, m_BonePositionZ(0)
	, m_eDirection(0)
	, m_RootMotionValue(0)
	, m_CollisionMinRatio(0)
	, m_CollisionMaxRatio(0)
{

}

CAnimationTab::~CAnimationTab()
{
	for (auto& iter : m_MapLayer)
	{
		Safe_Release(iter.second);
	}
	/*for (; iter != m_MapLayer.end(); ++iter)
	{
		_tchar* pTag = ((_tchar*)iter->first);
		Safe_Release(iter->second);
		Safe_Delete_Array(pTag);
	}
	*/
	
	Safe_Release(m_pGraphicDevice);
	Safe_Release(m_pContextDevice);
}

void CAnimationTab::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ANIMATIONCOMBOBOX1, m_CharacterComboBox);
	DDX_Control(pDX, IDC_ANIMATIONLIST1, m_AnimationListBox);
	DDX_Text(pDX, IDC_ANIMATIONEDIT1, m_PlayTime);
	DDX_Control(pDX, IDC_ANIMATIONSLIDER1, m_SliderPlayTime);
	DDX_Text(pDX, IDC_ANIMATIONEDIT2, m_Duration);
	DDX_Text(pDX, IDC_ANIMATIONEDIT3, m_PlaySpeedPerSec);
	DDX_Control(pDX, IDC_ANIMATIONSPIN1, m_SpinCtrlPlaySpeed);
	DDX_Control(pDX, IDC_ANIMATIONSPIN2, m_SpinCtrlPlayTime);
	DDX_Control(pDX, IDC_ANIMATIONLIST2, m_BoneListBox);
	DDX_Control(pDX, IDC_ANIMATIONCHECK1, m_CollRenderCheckBox);
	DDX_Text(pDX, IDC_ANIMATIONEDIT5, m_iAnimationIndex);
	DDX_Text(pDX, IDC_ANIMATIONEDIT6, m_LinkMinRatio);
	DDX_Text(pDX, IDC_ANIMATIONEDIT7, m_LinkMaxRatio);
	DDX_Text(pDX, IDC_ANIMATIONEDIT8, m_RatioTime);
	DDX_Text(pDX, IDC_ANIMATIONEDIT10, m_BlendMaxTime);
	DDX_Text(pDX, IDC_ANIMATIONEDIT11, m_iBlendingLevel);
	DDX_Control(pDX, IDC_ANIMATIONCHECK2, m_bNoneUpdatePostion);
	DDX_Text(pDX, EDIT_ROOT_KEY_INDEX, m_iRootBoneKeyFrameIndex);
	DDX_Text(pDX, EDIT_BONE_POS_X, m_BonePositionX);
	DDX_Text(pDX, IDC_ANIMATIONEDIT18, m_BonePositionY);
	DDX_Text(pDX, IDC_ANIMATIONEDIT19, m_BonePositionZ);
	DDX_Text(pDX, IDC_ANIMATIONEDIT20, m_eDirection);
	DDX_Text(pDX, IDC_ANIMATIONEDIT21, m_RootMotionValue);
	DDX_Control(pDX, IDC_ANIMATIONSPIN5, m_SpinCtrlRootMotionValue);
	DDX_Text(pDX, IDC_ANIMATIONEDIT100, m_CollisionMinRatio);
	DDX_Text(pDX, IDC_ANIMATIONEDIT101, m_CollisionMaxRatio);
	DDX_Control(pDX, IDC_ANIMATIONSPIN6, m_SpinCtrlCollisionMinRatio);
	DDX_Control(pDX, IDC_ANIMATIONSPIN7, m_SpinCtrlCollisionMaxRatio);
}


BEGIN_MESSAGE_MAP(CAnimationTab, CDialog)
	ON_CBN_SELCHANGE(IDC_ANIMATIONCOMBOBOX1, &CAnimationTab::OnCbnSelchangeAnimationcombobox)
	ON_LBN_SELCHANGE(IDC_ANIMATIONLIST1, &CAnimationTab::OnLbnSelchangeAnimationlist)
	ON_BN_CLICKED(IDC_ANIMATIONBUTTON1, &CAnimationTab::OnBnClickedButtonPlay)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_ANIMATIONBUTTON2, &CAnimationTab::OnBnClickedAnimationbuttonSTOP)
	ON_EN_CHANGE(IDC_ANIMATIONEDIT3, &CAnimationTab::OnEnChangeAnimationeditPlaySpeedPerSec)
	 ON_NOTIFY(UDN_DELTAPOS, IDC_ANIMATIONSPIN1, &CAnimationTab::OnDeltaposAnimationspinPlaySpeedPerSec)
	ON_BN_CLICKED(IDC_ANIMATIONBUTTON3, &CAnimationTab::OnBnClickedAnimationbuttonSave)
	ON_BN_CLICKED(IDC_ANIMATIONBUTTON4, &CAnimationTab::OnBnClickedAnimationbuttonLoad)
	ON_NOTIFY(UDN_DELTAPOS, IDC_ANIMATIONSPIN2, &CAnimationTab::OnDeltaposAnimationspinPlayTime)
	ON_EN_CHANGE(IDC_ANIMATIONEDIT1, &CAnimationTab::OnEnChangeAnimationeditPlayTime)
	ON_BN_CLICKED(IDC_ANIMATIONBUTTON6, &CAnimationTab::OnBnClickedButtonTimeReset)
	ON_BN_CLICKED(IDC_ANIMATIONBUTTON10, &CAnimationTab::OnBnClickedButtonCamReset)
	ON_BN_CLICKED(IDC_ANIMATIONCHECK1, &CAnimationTab::OnBnClickedAnimationcheckCollRenderCheckBox)
	ON_LBN_SELCHANGE(IDC_ANIMATIONLIST2, &CAnimationTab::OnLbnSelchangeAnimationlistBoneListBox)
	ON_BN_CLICKED(IDC_ANIMATIONBUTTON7, &CAnimationTab::OnBnClickedAnimationbuttonApply)
	ON_EN_CHANGE(IDC_ANIMATIONEDIT8, &CAnimationTab::OnEnChangeAnimationeditTimeRatio)
	ON_BN_CLICKED(IDC_ANIMATIONBUTTON9, &CAnimationTab::OnBnClickedButtonBlendMaxTimeApply)
	ON_EN_CHANGE(IDC_ANIMATIONEDIT10, &CAnimationTab::OnEnChangeAnimationeditBlendMaxTime)
	ON_NOTIFY(UDN_DELTAPOS, IDC_ANIMATIONSPIN3, &CAnimationTab::OnDeltaposAnimationspinBlendingLevel)
	ON_BN_CLICKED(IDC_BUTTON2, &CAnimationTab::OnBnClickedButtonBlendingLevelApply)
	ON_BN_CLICKED(IDC_ANIMATIONCHECK2, &CAnimationTab::OnBnClickedAnimationcheckNoneUpdatePosition)
	ON_EN_CHANGE(IDC_ANIMATIONEDIT20, &CAnimationTab::OnEnChangeAnimationeditDirection)
	ON_NOTIFY(UDN_DELTAPOS, IDC_ANIMATIONSPIN5, &CAnimationTab::OnDeltaposAnimationspinRootMotionValue)
	ON_NOTIFY(UDN_DELTAPOS, IDC_ANIMATIONSPIN6, &CAnimationTab::OnDeltaposAnimationspinCollisionMinRatio)
	ON_NOTIFY(UDN_DELTAPOS, IDC_ANIMATIONSPIN7, &CAnimationTab::OnDeltaposAnimationspinCollisionMaxRatio)
	ON_EN_CHANGE(IDC_ANIMATIONEDIT100, &CAnimationTab::OnEnChangeAnimationeditCollisionMinRatio)
	ON_EN_CHANGE(IDC_ANIMATIONEDIT101, &CAnimationTab::OnEnChangeAnimationeditCollisionMaxRatio)
END_MESSAGE_MAP()


void CAnimationTab::Update_Data()
{
	_int iSel = m_AnimationListBox.GetCurSel();
	if (iSel == -1)
		return;

	UpdateData(TRUE);

	auto& iter = find_if(m_MapLayer.begin(), m_MapLayer.end(), CTagFinder(m_pCurModelTag));
	if (iter == m_MapLayer.end())
		return;

	m_PlayTime = ((CToolObject*)iter->second)->Get_PlayTime();
	m_Duration = ((CToolObject*)iter->second)->Get_Duration();
	m_PlaySpeedPerSec = ((CToolObject*)iter->second)->Get_PlaySpeedPerSec();

	UpdateData(FALSE);
}

void CAnimationTab::Update_Slider()
{
	m_SliderPlayTime.SetRange(0, m_Duration);
	
	m_SliderPlayTime.SetRangeMin(0);
	m_SliderPlayTime.SetRangeMax(m_Duration);

	m_SliderPlayTime.SetPos(m_PlayTime);
}

void CAnimationTab::Update_Ratio()
{
	UpdateData(TRUE);

	_int iSel = m_AnimationListBox.GetCurSel();
	if (iSel == -1)
		return;

	auto& iter = find_if(m_MapLayer.begin(), m_MapLayer.end(), CTagFinder(m_pCurModelTag));
	if (iter == m_MapLayer.end())
		return;

	m_RatioTime = m_PlayTime / m_Duration;

	UpdateData(FALSE);
}

void CAnimationTab::Update_RootBoneKeyFrameIndex()
{
	_int iSel = m_AnimationListBox.GetCurSel();
	if (-1 == iSel)
		return;

	_tchar	pszTag[MAX_PATH];
	ZeroMemory(pszTag, sizeof(MAX_PATH));

	m_AnimationListBox.GetText(iSel, pszTag);

	if (nullptr == pszTag)
		return;

	auto& iter = find_if(m_MapLayer.begin(), m_MapLayer.end(), CTagFinder(m_pCurModelTag));
	if (iter == m_MapLayer.end())
		return;

	m_iAnimationIndex = iSel;

	m_iRootBoneKeyFrameIndex = ((CToolObject*)iter->second)->Get_RootBone_CurrentKeyFrameIndex(iSel);

	UpdateData(FALSE);
}

void CAnimationTab::Update_BonePosition()
{
	_int iSel = m_AnimationListBox.GetCurSel();
	if (-1 == iSel)
		return;

	_int iBoneSel = m_BoneListBox.GetCurSel();
	if (-1 == iBoneSel)
		return;

	_tchar	pszTag[MAX_PATH];
	ZeroMemory(pszTag, sizeof(_tchar) * MAX_PATH);

	m_AnimationListBox.GetText(iSel, pszTag);

	if (nullptr == pszTag)
		return;

	auto& iter = find_if(m_MapLayer.begin(), m_MapLayer.end(), CTagFinder(m_pCurModelTag));
	if (iter == m_MapLayer.end())
		return;

	char szTag[MAX_PATH]{};

	ZeroMemory(szTag, sizeof(char) * MAX_PATH);
	ZeroMemory(pszTag, sizeof(_tchar) * MAX_PATH);

	m_BoneListBox.GetText(iBoneSel, pszTag);

	size_t iSize = 0;
	wcstombs_s(&iSize, szTag, pszTag, sizeof(_tchar) * MAX_PATH);

	_float4 vPos{};
	ZeroMemory(&vPos, sizeof(_float4));

	vPos = ((CToolObject*)iter->second)->Get_BonePosition(iSel, szTag);

	m_BonePositionX = vPos.x;
	m_BonePositionY = vPos.y;
	m_BonePositionZ = vPos.z;

	UpdateData(FALSE);
}

// CAnimationTab 메시지 처리기입니다.
BOOL CAnimationTab::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	//m_CharacterComboBox.AddString(L"2B");
	//m_CharacterComboBox.AddString(L"ema000out");
	//m_CharacterComboBox.AddString(L"ema001out");
	//m_CharacterComboBox.AddString(L"ema010out");
	//m_CharacterComboBox.AddString(L"ema011out");
	//m_CharacterComboBox.AddString(L"ema020out");
	//
	//m_CharacterComboBox.AddString(L"em0000out");
	//m_CharacterComboBox.AddString(L"em0010out");
	//m_CharacterComboBox.AddString(L"em0030out");
	//m_CharacterComboBox.AddString(L"em1000out");
	//m_CharacterComboBox.AddString(L"em1080out");
	//m_CharacterComboBox.AddString(L"em1081out");
	//m_CharacterComboBox.AddString(L"em1100out");

	SetTimer(0, 17, NULL);

	m_SliderPlayTime.SetRange(0, m_Duration);

	m_SliderPlayTime.SetRangeMin(0);
	m_SliderPlayTime.SetRangeMax(m_Duration);

	m_SliderPlayTime.SetPos(m_PlayTime);

	m_SpinCtrlPlaySpeed.SetRange(0, 100);
	m_SpinCtrlRootMotionValue.SetRange(0, 100);
	m_SpinCtrlCollisionMinRatio.SetRange(0, 100);
	m_SpinCtrlCollisionMaxRatio.SetRange(0, 100);

	CMainFrame*    pMain = dynamic_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd());
	CToolView*	   pToolView = dynamic_cast<CToolView*>(pMain->GetActiveView());

	m_pGraphicDevice = pToolView->Get_GraphicDevice();
	m_pContextDevice = pToolView->Get_ContextDevice();

	Safe_AddRef(m_pGraphicDevice);
	Safe_AddRef(m_pContextDevice);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

_int CAnimationTab::Tick(_double TimeDelta)
{
	auto& iter = m_MapLayer.begin();

	for (; iter != m_MapLayer.end(); ++iter)
	{
		if (0 > iter->second->Tick(TimeDelta))
			return -1;
	}

	return 0;
}

_int CAnimationTab::LateTick(_double TimeDelta)
{
	auto& iter = m_MapLayer.begin();

	for (; iter != m_MapLayer.end(); ++iter)
	{
		if (0 > iter->second->LateTick(TimeDelta))
			return -1;
	}

	return 0;
}


void CAnimationTab::OnCbnSelchangeAnimationcombobox()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	_uint iSel = m_CharacterComboBox.GetCurSel();

	if (-1 == iSel)
		return;

	_tchar* szTag = new _tchar[MAX_PATH];
	ZeroMemory(szTag, MAX_PATH * sizeof(_tchar));

	m_CharacterComboBox.GetLBText(iSel, szTag);

	auto& iter = find_if(m_MapLayer.begin(), m_MapLayer.end(), CTagFinder(szTag));

	if (iter == m_MapLayer.end())
		return;
	
	Setting_AnimationList();
	Setting_Render(szTag);
	Setting_BoneList();

	return;
}


void CAnimationTab::OnLbnSelchangeAnimationlist()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	_int iSel = m_AnimationListBox.GetCurSel();
	if (-1 == iSel)
		return;

	_tchar	pszTag[MAX_PATH];
	ZeroMemory(pszTag, sizeof(MAX_PATH));

	m_AnimationListBox.GetText(iSel, pszTag);

	if (nullptr == pszTag)
		return;

	auto& iter = find_if(m_MapLayer.begin(), m_MapLayer.end(), CTagFinder(m_pCurModelTag));
	if (iter == m_MapLayer.end())
		return;

	char	szTag[MAX_PATH];
	ZeroMemory(szTag, sizeof(MAX_PATH));

	size_t iSize = 0;
	wcstombs_s(&iSize, szTag, pszTag, lstrlen(pszTag) * sizeof(_tchar));

	((CToolObject*)(iter->second))->Find_Animation_Initialize(szTag);
	
	auto& iter_begin = m_MapLayer.begin();
	for (; iter_begin != m_MapLayer.end(); ++iter_begin)
	{
		if (iter_begin == iter)
			((CToolObject*)iter->second)->Set_Stop(false);
		else
			((CToolObject*)iter_begin->second)->Set_Stop(true);
	}

	m_iAnimationIndex = iSel;

	m_LinkMinRatio = ((CToolObject*)iter->second)->Get_LinkMinTime(m_iAnimationIndex);
	m_LinkMaxRatio = ((CToolObject*)iter->second)->Get_LinkMaxTime(m_iAnimationIndex);

	m_BlendMaxTime = ((CToolObject*)iter->second)->Get_BlendMaxTime(m_iAnimationIndex);
	m_iBlendingLevel = ((CToolObject*)iter->second)->Get_BlendLevel(m_iAnimationIndex);

	m_RootMotionValue = ((CToolObject*)iter->second)->Get_RootMotionValue(m_iAnimationIndex);
	
	m_CollisionMinRatio = ((CToolObject*)iter->second)->Get_CollisionMinRatio(m_iAnimationIndex);
	m_CollisionMaxRatio = ((CToolObject*)iter->second)->Get_CollisionMaxRatio(m_iAnimationIndex);
	
	if (true == ((CToolObject*)iter->second)->Get_NoneUpdatePos(m_iAnimationIndex))
		m_bNoneUpdatePostion.SetCheck(true);
	else
		m_bNoneUpdatePostion.SetCheck(false);

	DIRECTION eDir = ((CToolObject*)iter->second)->Get_Direction(m_iAnimationIndex);
	
	switch ((_uint)eDir)
	{
	case 0:
		m_eDirection = (_uint)DIRECTION::FORWARD;
		break;
	case 1:
		m_eDirection = (_uint)DIRECTION::BACKWARD;
		break;
	case 2:
		m_eDirection = (_uint)DIRECTION::LEFT;
		break;
	case 3:
		m_eDirection = (_uint)DIRECTION::RIGHT;
		break;
	case 8:
		m_eDirection = (_uint)DIRECTION::DIAGONAL_LOOKUP;
		break;
	default:
		break;
	}

	UpdateData(FALSE);
}


void CAnimationTab::OnBnClickedButtonPlay()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	_int iSel = m_AnimationListBox.GetCurSel();
	if (-1 == iSel)
		return;

	_tchar pszTag[MAX_PATH];
	ZeroMemory(pszTag, sizeof(MAX_PATH));

	m_AnimationListBox.GetText(iSel, pszTag);
	if (nullptr == pszTag)
		return;

	auto& iter = find_if(m_MapLayer.begin(), m_MapLayer.end(), CTagFinder(m_pCurModelTag));
	if (iter == m_MapLayer.end())
		return;

	char szTag[MAX_PATH];
	ZeroMemory(szTag, sizeof(MAX_PATH));

	size_t iSize = 0;
	wcstombs_s(&iSize, szTag, pszTag, lstrlen(pszTag) * sizeof(_tchar));

	auto& iter_begin = m_MapLayer.begin();
	for (; iter_begin != m_MapLayer.end(); ++iter_begin)
	{
		if (iter_begin == iter)
		{
			if ((0.0 == ((CToolObject*)(iter->second))->Get_PlayTime()))
				((CToolObject*)(iter->second))->Initialize_Time();

			((CToolObject*)(iter->second))->Set_Stop(false);
		}
		else
			((CToolObject*)(iter_begin->second))->Set_Stop(true);
	}
}


void CAnimationTab::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	switch (nIDEvent)
	{
	case 0:
		Update_Data();
		Update_Slider();
		Update_Ratio();
		Update_RootBoneKeyFrameIndex();
		Update_BonePosition();
		break;
	default:
		break;
	}

	CDialog::OnTimer(nIDEvent);
}



void CAnimationTab::OnBnClickedAnimationbuttonSTOP()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	_int iSel = m_AnimationListBox.GetCurSel();
	if (-1 == iSel)
		return;

	_tchar pszTag[MAX_PATH];
	ZeroMemory(pszTag, sizeof(MAX_PATH));

	m_AnimationListBox.GetText(iSel, pszTag);
	if (0 == lstrlen(pszTag))
		return;

	auto& iter = find_if(m_MapLayer.begin(), m_MapLayer.end(), CTagFinder(m_pCurModelTag));
	if (iter == m_MapLayer.end())
		return;

	auto& iter_begin = m_MapLayer.begin();
	for (; iter_begin != m_MapLayer.end(); ++iter_begin)
	{
		if (iter_begin == iter)
			((CToolObject*)iter->second)->Set_Stop(true);
		else
			((CToolObject*)iter_begin->second)->Set_Stop(false);
	}
}


void CAnimationTab::OnEnChangeAnimationeditPlaySpeedPerSec()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	auto& iter = find_if(m_MapLayer.begin(), m_MapLayer.end(), CTagFinder(m_pCurModelTag));

	if (iter == m_MapLayer.end())
		return;

	((CToolObject*)iter->second)->Set_PlaySpeedPerSec(m_PlaySpeedPerSec);

	UpdateData(FALSE);
}


void CAnimationTab::OnDeltaposAnimationspinPlaySpeedPerSec(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	m_PlaySpeedPerSec = m_PlaySpeedPerSec + (float)(pNMUpDown->iDelta) / 10.0f;

	auto& iter = find_if(m_MapLayer.begin(), m_MapLayer.end(), CTagFinder(m_pCurModelTag));
	if (iter == m_MapLayer.end())
		return;

	((CToolObject*)iter->second)->Set_PlaySpeedPerSec(m_PlaySpeedPerSec);

	UpdateData(FALSE);
	*pResult = 0;
}


void CAnimationTab::OnBnClickedAnimationbuttonSave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	auto& iter = find_if(m_MapLayer.begin(), m_MapLayer.end(), CTagFinder(m_pCurModelTag));
	if (iter == m_MapLayer.end())
		return;

	CFileDialog Dlg(FALSE, L"ANIMATION", L"*.ANIMATION", 6UL, 0, (CWnd*)0, 0UL, FALSE);

	TCHAR szBuf[MAX_PATH]{};
	GetCurrentDirectory(MAX_PATH, szBuf);
	PathRemoveFileSpec(szBuf);

	lstrcat(szBuf, L"//Data//Animation");
	Dlg.m_ofn.lpstrInitialDir = szBuf;

	if (IDOK == Dlg.DoModal())
	{
		CString wstrPath = Dlg.GetPathName();

		HANDLE hFile = CreateFile(wstrPath, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

		if (INVALID_HANDLE_VALUE == hFile)
			return;

		DWORD dwByte = 0;

		vector<CAnimation*>* vecAnimation = ((CToolObject*)iter->second)->Get_Animation();
		_uint iCount = (*vecAnimation).size();

		_tchar pszTag[MAX_PATH];
		ZeroMemory(pszTag, MAX_PATH);

		lstrcpy(pszTag, m_pCurModelTag);

		WriteFile(hFile, &pszTag, MAX_PATH * sizeof(_tchar), &dwByte, nullptr);
		WriteFile(hFile, &iCount, sizeof(_uint), &dwByte, nullptr);

		for (auto& pAnimation : *vecAnimation)
		{
			char szName[MAX_PATH]{};
			strcpy_s(szName, pAnimation->Get_AnimationName());
			_double PlaySpeedPerSec = pAnimation->Get_PlaySpeedPerSec();

			_double		LinkMinTime = pAnimation->Get_LinkMinRatio();
			_double		LinkMaxTime = pAnimation->Get_LinkMaxRatio();
			_double		BlendMaxTime = pAnimation->Get_BlendMaxTime();
			_uint		iBlendLevel = pAnimation->Get_BlendLevel();
			_bool		NoneUpdatePos = pAnimation->Get_NonUpdatePos();
			DIRECTION   eDirection = pAnimation->Get_Direction();
			_float4		vBeginRootBonePos = pAnimation->Get_BeginRootBone_Position();
			_double		vRootMotionValue = pAnimation->Get_RootMotionValue();
			_double		CollisionMinRatio = pAnimation->Get_CollisionMinRatio();
			_double		CollisionMaxRatio = pAnimation->Get_CollisionMaxRatio();

			WriteFile(hFile, szName, MAX_PATH * sizeof(char), &dwByte, nullptr);
			WriteFile(hFile, &PlaySpeedPerSec, sizeof(_double), &dwByte, nullptr);
			WriteFile(hFile, &LinkMinTime, sizeof(_double), &dwByte, nullptr);
			WriteFile(hFile, &LinkMaxTime, sizeof(_double), &dwByte, nullptr);
			WriteFile(hFile, &BlendMaxTime, sizeof(_double), &dwByte, nullptr);
			WriteFile(hFile, &iBlendLevel, sizeof(_uint), &dwByte, nullptr);
			WriteFile(hFile, &NoneUpdatePos, sizeof(_bool), &dwByte, nullptr);
			WriteFile(hFile, &eDirection, sizeof(DIRECTION), &dwByte, nullptr);
			WriteFile(hFile, &vBeginRootBonePos, sizeof(_float4), &dwByte, nullptr);
			WriteFile(hFile, &vRootMotionValue, sizeof(_double), &dwByte, nullptr);
			WriteFile(hFile, &CollisionMinRatio, sizeof(_double), &dwByte, nullptr);
			WriteFile(hFile, &CollisionMaxRatio, sizeof(_double), &dwByte, nullptr);
		}

		CloseHandle(hFile);
	}
}


void CAnimationTab::OnBnClickedAnimationbuttonLoad()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFileDialog Dlg(TRUE, L"ANIMATION", L"*.ANIMATION", 6UL, 0, (CWnd*)0, 0UL, FALSE);

	TCHAR szBuf[MAX_PATH]{};
	GetCurrentDirectory(MAX_PATH, szBuf);
	PathRemoveFileSpec(szBuf);

	lstrcat(szBuf, L"//Data//Animation");
	Dlg.m_ofn.lpstrInitialDir = szBuf;

	if (IDOK == Dlg.DoModal())
	{
		CString wstrPath = Dlg.GetPathName();

		HANDLE hFile = CreateFile(wstrPath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

		if (INVALID_HANDLE_VALUE == hFile)
			return;

		DWORD dwByte = 0;
		_uint iCount = 0;

		_tchar pszTag[MAX_PATH];
		ZeroMemory(pszTag, MAX_PATH);

		ReadFile(hFile, pszTag, MAX_PATH * sizeof(_tchar), &dwByte, nullptr);

		if (lstrcmp(m_pCurModelTag, pszTag))
		{
			_tchar szMessage[MAX_PATH];
			ZeroMemory(szMessage, MAX_PATH * sizeof(_tchar));

			lstrcpy(szMessage, m_pCurModelTag);
			lstrcat(szMessage, L" 에 해당하는 파일이 아닙니다.");

			AfxMessageBox(szMessage, MB_OK);
			CloseHandle(hFile);

			return;
		}

		auto& iter = find_if(m_MapLayer.begin(), m_MapLayer.end(), CTagFinder(m_pCurModelTag));
		if (iter == m_MapLayer.end())
		{
			CloseHandle(hFile);
			return;
		}

		ReadFile(hFile, &iCount, sizeof(_uint), &dwByte, nullptr);

		UpdateData(TRUE);

		for (_uint i = 0; i < iCount; ++i)
		{
			char szName[MAX_PATH]{};
			_double PlaySpeedPerSec = 0.0;

			_double LinkMinRatio = 0.0;
			_double LinkMaxRatio = 0.0;
			_double BlendMaxTime = 0.0;
			_uint	iBlendLevel = 0;
			_bool NoneUpdatePos = false;
			DIRECTION	 eDirection = DIRECTION::FORWARD;
			_float4 vRootBonePos = _float4(0.f, 0.f, 0.f, 0.f);
			_double RootMotionValue = 0.0;
			_double CollisionMinRatio = 0.0;
			_double CollisionMaxRatio = 0.0;

			ReadFile(hFile, szName, MAX_PATH* sizeof(char), &dwByte, nullptr);
			ReadFile(hFile, &PlaySpeedPerSec, sizeof(_double), &dwByte, nullptr);
			ReadFile(hFile, &LinkMinRatio, sizeof(_double), &dwByte, nullptr);
			ReadFile(hFile, &LinkMaxRatio, sizeof(_double), &dwByte, nullptr);
			ReadFile(hFile, &BlendMaxTime, sizeof(_double), &dwByte, nullptr);
			ReadFile(hFile, &iBlendLevel, sizeof(_uint), &dwByte, nullptr);
			ReadFile(hFile, &NoneUpdatePos, sizeof(_bool), &dwByte, nullptr);
			ReadFile(hFile, &eDirection, sizeof(DIRECTION), &dwByte, nullptr);
			ReadFile(hFile, &vRootBonePos, sizeof(_float4), &dwByte, nullptr);
			ReadFile(hFile, &RootMotionValue, sizeof(_double), &dwByte, nullptr);
			ReadFile(hFile, &CollisionMinRatio, sizeof(_double), &dwByte, nullptr);
			ReadFile(hFile, &CollisionMaxRatio, sizeof(_double), &dwByte, nullptr);

			vector<CAnimation*>* vecAnimation = ((CToolObject*)iter->second)->Get_Animation();
			(*vecAnimation)[i]->Set_PlaySpeedPerSec(PlaySpeedPerSec);
			(*vecAnimation)[i]->Set_LinkMinRatio(LinkMinRatio);
			(*vecAnimation)[i]->Set_LinkMaxRatio(LinkMaxRatio);
			(*vecAnimation)[i]->Set_BlendMaxTime(BlendMaxTime);
			(*vecAnimation)[i]->Set_BlendLevel(iBlendLevel);
			(*vecAnimation)[i]->Set_NonUpdatePos(NoneUpdatePos);
			(*vecAnimation)[i]->Set_Direction(eDirection);
			(*vecAnimation)[i]->Set_BeginRootBone_Position(vRootBonePos);
			(*vecAnimation)[i]->Set_RootMotionValue(RootMotionValue);
			(*vecAnimation)[i]->Set_CollisionMinRatio(CollisionMinRatio);
			(*vecAnimation)[i]->Set_CollisionMaxRatio(CollisionMaxRatio);
		}
		CloseHandle(hFile);
	}
}


void CAnimationTab::OnDeltaposAnimationspinPlayTime(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	UpdateData(TRUE);
	m_PlayTime = m_PlayTime + (float)(pNMUpDown->iDelta) / 10.0f;

	auto& iter = find_if(m_MapLayer.begin(), m_MapLayer.end(), CTagFinder(m_pCurModelTag));
	if (iter == m_MapLayer.end())
		return;

	((CToolObject*)iter->second)->Set_PlayTime(m_PlayTime);

	UpdateData(FALSE);
	*pResult = 0;
}

void CAnimationTab::OnEnChangeAnimationeditPlayTime()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	if (0 > m_PlayTime)
		return;

	auto& iter = find_if(m_MapLayer.begin(), m_MapLayer.end(), CTagFinder(m_pCurModelTag));
	if (iter == m_MapLayer.end())
		return;

	((CToolObject*)iter->second)->Set_PlayTime(m_PlayTime);

	UpdateData(FALSE);
}

void CAnimationTab::OnBnClickedButtonTimeReset()
{
	_int iSel = m_AnimationListBox.GetCurSel();
	if (-1 == iSel)
		return;

	_tchar pszTag[MAX_PATH];
	ZeroMemory(pszTag, sizeof(MAX_PATH));

	m_AnimationListBox.GetText(iSel, pszTag);
	if (0 == lstrlen(pszTag))
		return;

	auto& iter = find_if(m_MapLayer.begin(), m_MapLayer.end(), CTagFinder(m_pCurModelTag));
	if (iter == m_MapLayer.end())
		return;

	((CToolObject*)iter->second)->Initialize_Time();
}

void CAnimationTab::Create_Prototype_Layer(const _tchar * pTag)
{
	if (nullptr == pTag)
		return;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CGameObject* pGameObject = nullptr;

	if (false == pGameInstance->IsFind_Prototype(pTag))
	{
		_matrix PivotMatrix = XMMatrixIdentity();

		if (!lstrcmp(L"Proto_Model_2B", pTag))
		{
			if (FAILED(pGameInstance->Add_Component((_uint)LEVEL::STATIC, pTag, CAssimpModel::Create(m_pGraphicDevice, m_pContextDevice, CAssimpModel::TYPE::ANIM, "../Client/Bin/Resources/Meshes/2B_Full/", "2B_Full.fbx", Shader_Model, "DefaultTechnique", PivotMatrix))))
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
		}
		else if (!lstrcmp(L"Proto_Model_EMA000OUT", pTag))
		{
			if (FAILED(pGameInstance->Add_Component((_uint)LEVEL::STATIC, pTag, CAssimpModel::Create(m_pGraphicDevice, m_pContextDevice, CAssimpModel::TYPE::ANIM, "../Client/Bin/Resources/Meshes/ema000out/", "ema000out.fbx", Shader_Model, "DefaultTechnique", PivotMatrix))))
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
		}

		else if (!lstrcmp(L"Proto_Model_EMA001OUT", pTag))
		{
			if (FAILED(pGameInstance->Add_Component((_uint)LEVEL::STATIC, pTag, CAssimpModel::Create(m_pGraphicDevice, m_pContextDevice, CAssimpModel::TYPE::ANIM, "../Client/Bin/Resources/Meshes/ema001out/", "ema001out.fbx", Shader_Model, "DefaultTechnique", PivotMatrix))))
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
		}

		else if (!lstrcmp(L"Proto_Model_EMA010OUT", pTag))
		{
			if (FAILED(pGameInstance->Add_Component((_uint)LEVEL::STATIC, pTag, CAssimpModel::Create(m_pGraphicDevice, m_pContextDevice, CAssimpModel::TYPE::ANIM, "../Client/Bin/Resources/Meshes/ema010out/", "ema010out.fbx", Shader_Model, "DefaultTechnique", PivotMatrix))))
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
		}

		else if (!lstrcmp(L"Proto_Model_EMA011OUT", pTag))
		{
			if (FAILED(pGameInstance->Add_Component((_uint)LEVEL::STATIC, pTag, CAssimpModel::Create(m_pGraphicDevice, m_pContextDevice, CAssimpModel::TYPE::ANIM, "../Client/Bin/Resources/Meshes/ema011out/", "ema011out.fbx", Shader_Model, "DefaultTechnique", PivotMatrix))))
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
		}

		else if (!lstrcmp(L"Proto_Model_EMA020OUT", pTag))
		{
			if (FAILED(pGameInstance->Add_Component((_uint)LEVEL::STATIC, pTag, CAssimpModel::Create(m_pGraphicDevice, m_pContextDevice, CAssimpModel::TYPE::ANIM, "../Client/Bin/Resources/Meshes/ema020out/", "ema020out.fbx", Shader_Model, "DefaultTechnique", PivotMatrix))))
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
		}

		else if (!lstrcmp(L"Proto_Model_EM0000OUT", pTag))
		{
			if (FAILED(pGameInstance->Add_Component((_uint)LEVEL::STATIC, pTag, CAssimpModel::Create(m_pGraphicDevice, m_pContextDevice, CAssimpModel::TYPE::ANIM, "../Client/Bin/Resources/Meshes/em0000out/", "em0000out.fbx", Shader_Model, "DefaultTechnique", PivotMatrix))))
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
		}
		else if (!lstrcmp(L"Proto_Model_EM0010OUT", pTag))
		{
			if (FAILED(pGameInstance->Add_Component((_uint)LEVEL::STATIC, pTag, CAssimpModel::Create(m_pGraphicDevice, m_pContextDevice, CAssimpModel::TYPE::ANIM, "../Client/Bin/Resources/Meshes/em0010out/", "em0010out.fbx", Shader_Model, "DefaultTechnique", PivotMatrix))))
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
		}
		else if (!lstrcmp(L"Proto_Model_EM0030OUT", pTag))
		{
			if (FAILED(pGameInstance->Add_Component((_uint)LEVEL::STATIC, pTag, CAssimpModel::Create(m_pGraphicDevice, m_pContextDevice, CAssimpModel::TYPE::ANIM, "../Client/Bin/Resources/Meshes/em0030out/", "em0030out.fbx", Shader_Model, "DefaultTechnique", PivotMatrix))))
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
		}
		else if (!lstrcmp(L"Proto_Model_EM1000OUT", pTag))
		{
			if (FAILED(pGameInstance->Add_Component((_uint)LEVEL::STATIC, pTag, CAssimpModel::Create(m_pGraphicDevice, m_pContextDevice, CAssimpModel::TYPE::ANIM, "../Client/Bin/Resources/Meshes/em1000out/", "em1000out.fbx", Shader_Model, "DefaultTechnique", PivotMatrix))))
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
		}

		else if (!lstrcmp(L"Proto_Model_EM1080OUT", pTag))
		{
			if (FAILED(pGameInstance->Add_Component((_uint)LEVEL::STATIC, pTag, CAssimpModel::Create(m_pGraphicDevice, m_pContextDevice, CAssimpModel::TYPE::ANIM, "../Client/Bin/Resources/Meshes/em1080out/", "em1080out.fbx", Shader_Model, "DefaultTechnique", PivotMatrix))))
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
		}

		else if (!lstrcmp(L"Proto_Model_EM1081OUT", pTag))
		{
			if (FAILED(pGameInstance->Add_Component((_uint)LEVEL::STATIC, pTag, CAssimpModel::Create(m_pGraphicDevice, m_pContextDevice, CAssimpModel::TYPE::ANIM, "../Client/Bin/Resources/Meshes/em1081out/", "em1081out.fbx", Shader_Model, "DefaultTechnique", PivotMatrix))))
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
		}

		else if (!lstrcmp(L"Proto_Model_EM1100OUT", pTag))
		{
			if (FAILED(pGameInstance->Add_Component((_uint)LEVEL::STATIC, pTag, CAssimpModel::Create(m_pGraphicDevice, m_pContextDevice, CAssimpModel::TYPE::ANIM, "../Client/Bin/Resources/Meshes/em1100out/", "em1100out.fbx", Shader_Model, "DefaultTechnique", PivotMatrix))))
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
		}
	}

	if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr((_uint)LEVEL::STATIC, PROTO_KEY_TOOLOBJECT, L"Layer_Object", &pGameObject, (void*)pTag)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	if (m_pCurModelTag)
	{
		auto& iter = find_if(m_MapLayer.begin(), m_MapLayer.end(), CTagFinder(m_pCurModelTag));
		if (iter == m_MapLayer.end())
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
			
		((CToolObject*)iter->second)->Set_Stop(true);
		((CToolObject*)iter->second)->Initialize_Time();
		((CToolObject*)iter->second)->Set_CollRender(false);
	}

	m_CollRenderCheckBox.SetCheck(BST_UNCHECKED);

	m_pCurModelTag = pTag;
	m_MapLayer.emplace(pTag, pGameObject);

	CModel* pModel = (CModel*)pGameObject->Get_Component(L"Com_Model");
	if (nullptr == pModel)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	vector<CAnimation*>* vecAnimation = pModel->Get_Animation();

	char szName[MAX_PATH];
	_tchar tszName[MAX_PATH];


	_int iCounts = m_AnimationListBox.GetCount();

	if (iCounts != -1)
	{
		for (_uint i = 0; i < iCounts; ++i)
			m_AnimationListBox.DeleteString(0);
	}

	for (auto& pAnim : *vecAnimation)
	{
		ZeroMemory(szName, sizeof(MAX_PATH));
		ZeroMemory(tszName, sizeof(MAX_PATH));

		strcpy_s(szName, pAnim->Get_AnimationName());

		size_t iCount = 0;
		mbstowcs_s(&iCount, tszName, szName, strlen(szName) * sizeof(char));

		m_AnimationListBox.AddString(tszName);
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CAnimationTab::Setting_Render(const _tchar * pTag)
{
	auto& iter = find_if(m_MapLayer.begin(), m_MapLayer.end(), CTagFinder(pTag));
	if (iter == m_MapLayer.end())
		return;

	auto& iter_begin = m_MapLayer.begin();
	for (; iter_begin != m_MapLayer.end(); ++iter_begin)
	{
		if (iter_begin == iter)
			((CToolObject*)iter_begin->second)->Set_RenderStop(false);
		else
			((CToolObject*)iter_begin->second)->Set_RenderStop(true);
	}

	m_pCurModelTag = pTag;

	CModel* pModel = (CModel*)((CToolObject*)(iter->second))->Get_Component(L"Com_Model");
	if (nullptr == pModel)
		return;

	vector<CAnimation*>* vecAnimation = pModel->Get_Animation();

	char szName[MAX_PATH];
	_tchar tszName[MAX_PATH];


	_int iCounts = m_AnimationListBox.GetCount();

	if (iCounts != -1)
	{
		for (_uint i = 0; i < iCounts; ++i)
			m_AnimationListBox.DeleteString(0);
	}

	for (auto& pAnim : *vecAnimation)
	{
		ZeroMemory(szName, sizeof(MAX_PATH));
		ZeroMemory(tszName, sizeof(MAX_PATH));

		strcpy_s(szName, pAnim->Get_AnimationName());

		size_t iCount = 0;
		mbstowcs_s(&iCount, tszName, szName, strlen(szName) * sizeof(char));

		m_AnimationListBox.AddString(tszName);
	}
}


void CAnimationTab::OnBnClickedButtonCamReset()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame*    pMain = dynamic_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd());
	CToolView*     pToolView = dynamic_cast<CToolView*>(pMain->GetActiveView());

	CCamera::DESC tCameraDesc;

	tCameraDesc.fAspect = g_iWinCX / (_float)g_iWinCY;
	tCameraDesc.fFar = 500.f;
	tCameraDesc.fNear = 0.2f;
	tCameraDesc.vEye = _float3(0.f, 1.f, -2.f);
	tCameraDesc.vAt = _float3(0.f, 0.f, 0.f);
	tCameraDesc.tTransformDesc.dSpeedPerSec = 10.f;
	tCameraDesc.tTransformDesc.dRotationPerSec = 2.f;

	pToolView->m_pCamera->Set_CameraDesc(tCameraDesc);
	pToolView->m_pCamera->Set_CamTransformDesc(tCameraDesc.tTransformDesc);
}

void CAnimationTab::Setting_BoneList()
{
	auto& iter = find_if(m_MapLayer.begin(), m_MapLayer.end(), CTagFinder(m_pCurModelTag));
	if (iter == m_MapLayer.end())
		return;

	vector<CHierarchyNode*>* vecHierarchyNode = ((CToolObject*)iter->second)->Get_HierarchyNode();

	if (nullptr == vecHierarchyNode)
		return;

	if (0 >= (*vecHierarchyNode).size())
		return;

	_int iCount = m_BoneListBox.GetCount();

	if (0 < iCount)
	{
		for (_uint i = 0; i < iCount; ++i)
			m_BoneListBox.DeleteString(0);
	}

	for (auto& pHierarchyNode : *vecHierarchyNode)
	{
		_tchar szName[MAX_PATH];
		ZeroMemory(szName, MAX_PATH * sizeof(_tchar));

		size_t iSize = 0;
		mbstowcs_s(&iSize, szName, pHierarchyNode->Get_Name(), MAX_PATH * sizeof(char));
		m_BoneListBox.AddString(szName);
	}
}

void CAnimationTab::Setting_AnimationList()
{
	if (m_pCurModelTag)
	{
		auto& iter = find_if(m_MapLayer.begin(), m_MapLayer.end(), CTagFinder(m_pCurModelTag));
		if (iter == m_MapLayer.end())
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		((CToolObject*)iter->second)->Set_Stop(true);
		((CToolObject*)iter->second)->Initialize_Time();
		((CToolObject*)iter->second)->Set_CollRender(false);
	}

	m_CollRenderCheckBox.SetCheck(BST_UNCHECKED);
}

void CAnimationTab::Create_Layer(const _tchar * pTag, CToolObject * pObject)
{
	if (nullptr == pTag || nullptr == pObject)
		return;

	if (m_pCurModelTag)
	{
		auto& iter = find_if(m_MapLayer.begin(), m_MapLayer.end(), CTagFinder(m_pCurModelTag));
		if (iter == m_MapLayer.end())
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		((CToolObject*)iter->second)->Set_Stop(true);
		((CToolObject*)iter->second)->Initialize_Time();
		((CToolObject*)iter->second)->Set_CollRender(false);
	}

	m_CollRenderCheckBox.SetCheck(BST_UNCHECKED);

	m_pCurModelTag = pTag;
	m_MapLayer.emplace(pTag, pObject);

	m_CharacterComboBox.AddString(m_pCurModelTag);

	_uint iComboCount = m_CharacterComboBox.GetCount();
	m_CharacterComboBox.SetCurSel((iComboCount - 1));

	CModel* pModel = (CModel*)pObject->Get_Component(L"Com_Model");
	if (nullptr == pModel)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	vector<CAnimation*>* vecAnimation = pModel->Get_Animation();

	char szName[MAX_PATH];
	_tchar tszName[MAX_PATH];


	_int iCounts = m_AnimationListBox.GetCount();

	if (iCounts != -1)
	{
		for (_uint i = 0; i < iCounts; ++i)
			m_AnimationListBox.DeleteString(0);
	}

	for (auto& pAnim : *vecAnimation)
	{
		ZeroMemory(szName, sizeof(MAX_PATH));
		ZeroMemory(tszName, sizeof(MAX_PATH));

		strcpy_s(szName, pAnim->Get_AnimationName());

		size_t iCount = 0;
		mbstowcs_s(&iCount, tszName, szName, strlen(szName) * sizeof(char));

		m_AnimationListBox.AddString(tszName);
	}

	Setting_BoneList();

	RELEASE_INSTANCE(CGameInstance);
}


void CAnimationTab::OnBnClickedAnimationcheckCollRenderCheckBox()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	_bool bCheck = false;

	if (BST_CHECKED == m_CollRenderCheckBox.GetCheck())
		bCheck = true;
	else if (BST_UNCHECKED == m_CollRenderCheckBox.GetCheck())
		bCheck = false;

	auto& iter = find_if(m_MapLayer.begin(), m_MapLayer.end(), CTagFinder(m_pCurModelTag));
	if (iter == m_MapLayer.end())
		return;

	((CToolObject*)iter->second)->Set_CollRender(bCheck);
}


void CAnimationTab::OnLbnSelchangeAnimationlistBoneListBox()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	_int iSel = m_BoneListBox.GetCurSel();
	if (-1 == iSel)
		return;

	_tchar	szBone[MAX_PATH];
	ZeroMemory(szBone, MAX_PATH * sizeof(_tchar));

	m_BoneListBox.GetText(iSel, szBone);

	auto& iter = find_if(m_MapLayer.begin(), m_MapLayer.end(), CTagFinder(m_pCurModelTag));
	if (iter == m_MapLayer.end())
		return;

	size_t iSize = 0;

	char szBoneName[MAX_PATH];
	ZeroMemory(szBoneName, MAX_PATH * sizeof(char));

	wcstombs_s(&iSize, szBoneName, szBone, MAX_PATH * sizeof(_tchar));

	((CToolObject*)iter->second)->Set_CurBoneName(szBoneName);
}

void CAnimationTab::OnBnClickedAnimationbuttonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	_int iSel = m_AnimationListBox.GetCurSel();
	if (-1 == iSel)
		return;

	_tchar	pszTag[MAX_PATH];
	ZeroMemory(pszTag, sizeof(MAX_PATH));

	m_AnimationListBox.GetText(iSel, pszTag);

	if (nullptr == pszTag)
		return;

	auto& iter = find_if(m_MapLayer.begin(), m_MapLayer.end(), CTagFinder(m_pCurModelTag));
	if (iter == m_MapLayer.end())
		return;

	size_t iSize = 0;

	char szName[MAX_PATH]{};
	ZeroMemory(szName, MAX_PATH * sizeof(char));

	wcstombs_s(&iSize, szName, pszTag, MAX_PATH * sizeof(_tchar));

	_int iIndex = ((CToolObject*)iter->second)->Find_Animation(szName);
	if (0 > iIndex)
		return;

	((CToolObject*)iter->second)->Set_LinkMinRatio(iIndex, m_LinkMinRatio);
	((CToolObject*)iter->second)->Set_LinkMaxRatio(iIndex, m_LinkMaxRatio);

	UpdateData(FALSE);
}


void CAnimationTab::OnEnChangeAnimationeditTimeRatio()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CAnimationTab::OnBnClickedButtonBlendMaxTimeApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	_int iSel = m_AnimationListBox.GetCurSel();
	if (-1 == iSel)
		return;

	_tchar	pszTag[MAX_PATH];
	ZeroMemory(pszTag, sizeof(MAX_PATH));

	m_AnimationListBox.GetText(iSel, pszTag);

	if (nullptr == pszTag)
		return;

	auto& iter = find_if(m_MapLayer.begin(), m_MapLayer.end(), CTagFinder(m_pCurModelTag));
	if (iter == m_MapLayer.end())
		return;

	size_t iSize = 0;

	char szName[MAX_PATH]{};
	ZeroMemory(szName, MAX_PATH * sizeof(char));

	wcstombs_s(&iSize, szName, pszTag, MAX_PATH * sizeof(_tchar));

	_int iIndex = ((CToolObject*)iter->second)->Find_Animation(szName);
	if (0 > iIndex)
		return;

	((CToolObject*)iter->second)->Set_BlendMaxTime(iIndex, m_BlendMaxTime);

	UpdateData(FALSE);
}


void CAnimationTab::OnEnChangeAnimationeditBlendMaxTime()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	UpdateData(FALSE);
}


void CAnimationTab::OnDeltaposAnimationspinBlendingLevel(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	UpdateData(TRUE);
	m_iBlendingLevel = m_iBlendingLevel + 1;
	UpdateData(FALSE);
	*pResult = 0;
}


void CAnimationTab::OnBnClickedButtonBlendingLevelApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	_int iSel = m_AnimationListBox.GetCurSel();
	if (-1 == iSel)
		return;

	_tchar	pszTag[MAX_PATH];
	ZeroMemory(pszTag, sizeof(MAX_PATH));

	m_AnimationListBox.GetText(iSel, pszTag);

	if (nullptr == pszTag)
		return;

	auto& iter = find_if(m_MapLayer.begin(), m_MapLayer.end(), CTagFinder(m_pCurModelTag));
	if (iter == m_MapLayer.end())
		return;

	size_t iSize = 0;

	char szName[MAX_PATH]{};
	ZeroMemory(szName, MAX_PATH * sizeof(char));

	wcstombs_s(&iSize, szName, pszTag, MAX_PATH * sizeof(_tchar));

	_int iIndex = ((CToolObject*)iter->second)->Find_Animation(szName);
	if (0 > iIndex)
		return;

	((CToolObject*)iter->second)->Set_BlendLevel(iIndex, m_iBlendingLevel);

	UpdateData(FALSE);
}


void CAnimationTab::OnBnClickedAnimationcheckNoneUpdatePosition()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	_int iSel = m_AnimationListBox.GetCurSel();
	if (-1 == iSel)
		return;

	_tchar	pszTag[MAX_PATH];
	ZeroMemory(pszTag, sizeof(MAX_PATH));

	m_AnimationListBox.GetText(iSel, pszTag);

	if (nullptr == pszTag)
		return;

	auto& iter = find_if(m_MapLayer.begin(), m_MapLayer.end(), CTagFinder(m_pCurModelTag));
	if (iter == m_MapLayer.end())
		return;

	size_t iSize = 0;

	char szName[MAX_PATH]{};
	ZeroMemory(szName, MAX_PATH * sizeof(char));

	wcstombs_s(&iSize, szName, pszTag, MAX_PATH * sizeof(_tchar));

	_int iIndex = ((CToolObject*)iter->second)->Find_Animation(szName);
	if (0 > iIndex)
		return;

	
	if (BST_CHECKED == m_bNoneUpdatePostion.GetCheck())
		((CToolObject*)iter->second)->Set_NoneUpdatePos(iIndex, true);
	else
		((CToolObject*)iter->second)->Set_NoneUpdatePos(iIndex, false);

	UpdateData(FALSE);

}

void CAnimationTab::OnEnChangeAnimationeditDirection()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	_int iSel = m_AnimationListBox.GetCurSel();
	if (-1 == iSel)
		return;

	_tchar	pszTag[MAX_PATH];
	ZeroMemory(pszTag, sizeof(MAX_PATH));

	m_AnimationListBox.GetText(iSel, pszTag);

	if (nullptr == pszTag)
		return;

	auto& iter = find_if(m_MapLayer.begin(), m_MapLayer.end(), CTagFinder(m_pCurModelTag));
	if (iter == m_MapLayer.end())
		return;

	size_t iSize = 0;

	char szName[MAX_PATH]{};
	ZeroMemory(szName, MAX_PATH * sizeof(char));

	wcstombs_s(&iSize, szName, pszTag, MAX_PATH * sizeof(_tchar));

	_int iIndex = ((CToolObject*)iter->second)->Find_Animation(szName);
	if (0 > iIndex)
		return;

	((CToolObject*)iter->second)->Set_Direction(iIndex, (DIRECTION)m_eDirection);

	UpdateData(FALSE);
}


void CAnimationTab::OnDeltaposAnimationspinRootMotionValue(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	m_RootMotionValue = m_RootMotionValue + (float)(pNMUpDown->iDelta) / 10.0f;

	auto& iter = find_if(m_MapLayer.begin(), m_MapLayer.end(), CTagFinder(m_pCurModelTag));
	if (iter == m_MapLayer.end())
		return;

	((CToolObject*)iter->second)->Set_RootMotionValue(m_RootMotionValue);

	UpdateData(FALSE);
	*pResult = 0;
}


void CAnimationTab::OnDeltaposAnimationspinCollisionMinRatio(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	UpdateData(TRUE);
	m_CollisionMinRatio = m_CollisionMinRatio + (float)(pNMUpDown->iDelta) / 10.0f;

	auto& iter = find_if(m_MapLayer.begin(), m_MapLayer.end(), CTagFinder(m_pCurModelTag));
	if (iter == m_MapLayer.end())
		return;

	((CToolObject*)iter->second)->Set_CollisionMinRatio(m_CollisionMinRatio);

	UpdateData(FALSE);
	*pResult = 0;
}


void CAnimationTab::OnDeltaposAnimationspinCollisionMaxRatio(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	UpdateData(TRUE);
	m_CollisionMaxRatio = m_CollisionMaxRatio + (float)(pNMUpDown->iDelta) / 10.0f;

	auto& iter = find_if(m_MapLayer.begin(), m_MapLayer.end(), CTagFinder(m_pCurModelTag));
	if (iter == m_MapLayer.end())
		return;

	((CToolObject*)iter->second)->Set_CollisionMaxRatio(m_CollisionMaxRatio);

	UpdateData(FALSE);
	*pResult = 0;
}


void CAnimationTab::OnEnChangeAnimationeditCollisionMinRatio()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	auto& iter = find_if(m_MapLayer.begin(), m_MapLayer.end(), CTagFinder(m_pCurModelTag));

	if (iter == m_MapLayer.end())
		return;

	((CToolObject*)iter->second)->Set_CollisionMinRatio(m_CollisionMinRatio);

	UpdateData(FALSE);
}


void CAnimationTab::OnEnChangeAnimationeditCollisionMaxRatio()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	auto& iter = find_if(m_MapLayer.begin(), m_MapLayer.end(), CTagFinder(m_pCurModelTag));

	if (iter == m_MapLayer.end())
		return;

	((CToolObject*)iter->second)->Set_CollisionMaxRatio(m_CollisionMaxRatio);

	UpdateData(FALSE);
}
