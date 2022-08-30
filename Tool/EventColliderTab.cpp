// EventColliderTab.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool.h"
#include "EventColliderTab.h"
#include "afxdialogex.h"

#include "MainFrm.h"
#include "ToolView.h"
#include "Event_Collider.h"

// CEventColliderTab 대화 상자입니다.

IMPLEMENT_DYNAMIC(CEventColliderTab, CDialog)

CEventColliderTab::CEventColliderTab(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_EVENTCOLLIDERTAB, pParent)
	, m_fPosX(0)
	, m_fPosY(0)
	, m_fPosZ(0)
	, m_fPivotX(0)
	, m_fPivotY(0)
	, m_fPivotZ(0)
	, m_fScaleX(0)
	, m_fScaleY(0)
	, m_fScaleZ(0)
	, m_strName(_T(""))
{

}

CEventColliderTab::~CEventColliderTab()
{
	Safe_Release(m_pGraphicDevice);
	Safe_Release(m_pContextDevice);

	auto& iter = m_vecEvent_Collider.begin();
	for (; iter != m_vecEvent_Collider.end();)
	{
		Safe_Release(*iter);
	}
}

void CEventColliderTab::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EVENTEDIT1, m_fPosX);
	DDX_Text(pDX, IDC_EVENTEDIT2, m_fPosY);
	DDX_Text(pDX, IDC_EVENTEDIT3, m_fPosZ);
	DDX_Text(pDX, IDC_EVENTEDIT10, m_fPivotX);
	DDX_Text(pDX, IDC_EVENTEDIT4, m_fPivotY);
	DDX_Text(pDX, IDC_EVENTEDIT5, m_fPivotZ);
	DDX_Text(pDX, IDC_EVENTEDIT9, m_fScaleX);
	DDX_Text(pDX, IDC_EVENTEDIT6, m_fScaleY);
	DDX_Text(pDX, IDC_EVENTEDIT7, m_fScaleZ);
	DDX_Control(pDX, IDC_EVENTSPIN1, m_Spin_PosX);
	DDX_Control(pDX, IDC_EVENTSPIN2, m_Spin_PosY);
	DDX_Control(pDX, IDC_EVENTSPIN3, m_Spin_PosZ);
	DDX_Control(pDX, IDC_EVENTSPIN10, m_Spin_PivotX);
	DDX_Control(pDX, IDC_EVENTSPIN4, m_Spin_PivotY);
	DDX_Control(pDX, IDC_EVENTSPIN5, m_Spin_PivotZ);
	DDX_Control(pDX, IDC_EVENTSPIN9, m_Spin_ScaleX);
	DDX_Control(pDX, IDC_EVENTSPIN6, m_Spin_ScaleY);
	DDX_Control(pDX, IDC_EVENTSPIN7, m_Spin_ScaleZ);
	DDX_Text(pDX, IDC_EVENTEDIT11, m_strName);
	DDX_Control(pDX, IDC_EVENTLIST1, m_EventColiiderListBox);
}

_int CEventColliderTab::Tick(_double dTimeDelta)
{
	for (auto vecEventCollider : m_vecEvent_Collider)
		vecEventCollider->Tick(dTimeDelta);

	return _int();
}

_int CEventColliderTab::LateTick(_double dTimeDelta)
{
	for (auto vecEventCollider : m_vecEvent_Collider)
		vecEventCollider->LateTick(dTimeDelta);

	return _int();
}

_int CEventColliderTab::Picking(_double dTimeDelta)
{
	return _int();
}

void CEventColliderTab::Update_Transform()
{
	_int iSel = m_EventColiiderListBox.GetCurSel();
	if (-1 == iSel)
		return;
		
	CTransform* pTransform = (CTransform*)m_vecEvent_Collider[iSel]->Get_Component(COM_KEY_TRANSFORM);
	if (nullptr == pTransform)
		return;

	_vector vPos = XMVectorSet(m_fPosX, m_fPosY, m_fPosZ, 1.f);

	pTransform->Set_State(CTransform::STATE::POSITION, vPos);

	UpdateData(FALSE);
}

void CEventColliderTab::Update_Data()
{
}

void CEventColliderTab::Update_ColliderDescScale()
{
	_int iSel = m_EventColiiderListBox.GetCurSel();

	if (-1 == iSel)
		return;

	CCollider* pCollider = nullptr;
	pCollider = (CCollider*)m_vecEvent_Collider[iSel]->Get_Component(L"Collider_Com");

	if (pCollider == nullptr)
	{
		return;
	}

	CCollider::CCollider::DESC tColliderDesc = pCollider->Get_ColliderDesc();

	tColliderDesc.vScale.x = m_fScaleX;
	tColliderDesc.vScale.y = m_fScaleY;
	tColliderDesc.vScale.z = m_fScaleZ;
				  
	pCollider->Set_ColliderDesc(tColliderDesc);
}

void CEventColliderTab::Update_ColliderDescPivot()
{
	_int iSel = m_EventColiiderListBox.GetCurSel();

	if (-1 == iSel)
		return;

	CCollider* pCollider = nullptr;
	pCollider = (CCollider*)m_vecEvent_Collider[iSel]->Get_Component(L"Collider_Com");

	if (pCollider == nullptr)
	{
		return;
	}

	CCollider::CCollider::DESC tColliderDesc = pCollider->Get_ColliderDesc();

	tColliderDesc.vPivot.x = m_fPivotX;
	tColliderDesc.vPivot.y = m_fPivotY;
	tColliderDesc.vPivot.z = m_fPivotZ;

	pCollider->Set_ColliderDesc(tColliderDesc);

}


BEGIN_MESSAGE_MAP(CEventColliderTab, CDialog)
	ON_LBN_SELCHANGE(IDC_EVENTLIST1, &CEventColliderTab::OnLbnSelchangeEventlist1)
	ON_BN_CLICKED(IDC_EVENTBUTTON3, &CEventColliderTab::OnBnClickedEventSavebutton)
	ON_BN_CLICKED(IDC_EVENTBUTTON4, &CEventColliderTab::OnBnClickedEventLoadbutton)
	ON_BN_CLICKED(IDC_EVENTBUTTON1, &CEventColliderTab::OnBnClickedEventCreatebutton)
	ON_BN_CLICKED(IDC_EVENTBUTTON2, &CEventColliderTab::OnBnClickedEventDeletebutton)
	ON_EN_CHANGE(IDC_EVENTEDIT1, &CEventColliderTab::OnEnChangeEventPosX)
	ON_EN_CHANGE(IDC_EVENTEDIT2, &CEventColliderTab::OnEnChangeEventPosY)
	ON_EN_CHANGE(IDC_EVENTEDIT3, &CEventColliderTab::OnEnChangeEventPosZ)
	ON_EN_CHANGE(IDC_EVENTEDIT10, &CEventColliderTab::OnEnChangeEventPivotX)
	ON_EN_CHANGE(IDC_EVENTEDIT4, &CEventColliderTab::OnEnChangeEventPivotY)
	ON_EN_CHANGE(IDC_EVENTEDIT5, &CEventColliderTab::OnEnChangeEventPivotZ)
	ON_EN_CHANGE(IDC_EVENTEDIT9, &CEventColliderTab::OnEnChangeEventScaleX)
	ON_EN_CHANGE(IDC_EVENTEDIT6, &CEventColliderTab::OnEnChangeEventScaleY)
	ON_EN_CHANGE(IDC_EVENTEDIT7, &CEventColliderTab::OnEnChangeEventScaleZ)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EVENTSPIN1, &CEventColliderTab::OnDeltaposEventspinPosX)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EVENTSPIN2, &CEventColliderTab::OnDeltaposEventspinPosY)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EVENTSPIN3, &CEventColliderTab::OnDeltaposEventspinPosZ)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EVENTSPIN10, &CEventColliderTab::OnDeltaposEventspinPivotX)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EVENTSPIN4, &CEventColliderTab::OnDeltaposEventspinPivotY)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EVENTSPIN5, &CEventColliderTab::OnDeltaposEventspinPivotZ)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EVENTSPIN9, &CEventColliderTab::OnDeltaposEventspinScaleX)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EVENTSPIN6, &CEventColliderTab::OnDeltaposEventspinScaleY)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EVENTSPIN7, &CEventColliderTab::OnDeltaposEventspinScaleZ)
END_MESSAGE_MAP()


// CEventColliderTab 메시지 처리기입니다.


void CEventColliderTab::OnLbnSelchangeEventlist1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	_int iSel = m_EventColiiderListBox.GetCurSel();
	if (-1 == iSel)
		return;
	
	CTransform* pTransform = (CTransform*)m_vecEvent_Collider[iSel]->Get_Component(COM_KEY_TRANSFORM);
	if (nullptr == pTransform)
		return;

	_float4 vPos{};

	XMStoreFloat4(&vPos, pTransform->Get_State(CTransform::STATE::POSITION));

	m_fPosX = vPos.x;
	m_fPosY = vPos.y;
	m_fPosZ = vPos.z;

	CCollider* pCollider = (CCollider*)m_vecEvent_Collider[iSel]->Get_Component(L"Collider_Com");
	if (nullptr == pCollider)
		return;

	CCollider::DESC tColliderDesc = pCollider->Get_ColliderDesc();

	m_fScaleX = tColliderDesc.vScale.x;
	m_fScaleY = tColliderDesc.vScale.y;
	m_fScaleZ = tColliderDesc.vScale.z;

	m_fPivotX = tColliderDesc.vPivot.x;
	m_fPivotY = tColliderDesc.vPivot.y;
	m_fPivotZ = tColliderDesc.vPivot.z;

	UpdateData(FALSE);

}


void CEventColliderTab::OnBnClickedEventSavebutton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFileDialog Dlg(FALSE, L"dat", L"*.dat", 6UL, 0, (CWnd*)0, 0UL, FALSE);

	TCHAR szBuf[MAX_PATH]{};
	GetCurrentDirectory(MAX_PATH, szBuf);
	PathRemoveFileSpec(szBuf);

	lstrcat(szBuf, L"//Data//Location");
	Dlg.m_ofn.lpstrInitialDir = szBuf;

	if (IDOK == Dlg.DoModal())
	{
		CString wstrPath = Dlg.GetPathName();

		HANDLE hFile = CreateFile(wstrPath, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

		if (INVALID_HANDLE_VALUE == hFile)
			return;

		DWORD dwByte = 0;
		_uint iCount = m_vecEvent_Collider.size();
		_uint iIndex = 0;

		WriteFile(hFile, &iCount, sizeof(_uint), &dwByte, nullptr);

		for (auto& pEventCollider : m_vecEvent_Collider)
		{
			CString strName{};
			_tchar szName[MAX_PATH]{};

			strName = pEventCollider->Get_Name();
			lstrcpy(szName, strName);

			WriteFile(hFile, szName, sizeof(_tchar)*MAX_PATH, &dwByte, nullptr);

			CTransform* pTransform = (CTransform*)pEventCollider->Get_Component(COM_KEY_TRANSFORM);
			if (nullptr == pTransform)
				return;

			CCollider* pCollider = (CCollider*)pEventCollider->Get_Component(L"Collider_Com");
			if (nullptr == pCollider)
				return;

			CCollider::DESC tColliderDesc = pCollider->Get_ColliderDesc();

			_float4 vPos;
			XMStoreFloat4(&vPos, pTransform->Get_State(CTransform::STATE::POSITION));

			WriteFile(hFile, &vPos, sizeof(_float4), &dwByte, nullptr);
			WriteFile(hFile, &tColliderDesc, sizeof(CCollider::DESC), &dwByte, nullptr);

			++iIndex;

		}

		CloseHandle(hFile);
	}
}


void CEventColliderTab::OnBnClickedEventLoadbutton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFileDialog Dlg(TRUE, L"dat", L"*.dat", 6UL, 0, (CWnd*)0, 0UL, FALSE);

	TCHAR szBuf[MAX_PATH]{};
	GetCurrentDirectory(MAX_PATH, szBuf);
	PathRemoveFileSpec(szBuf);

	lstrcat(szBuf, L"//Data//Location");
	Dlg.m_ofn.lpstrInitialDir = szBuf;

	if (IDOK == Dlg.DoModal())
	{
		CString wstrPath = Dlg.GetPathName();

		HANDLE hFile = CreateFile(wstrPath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

		if (INVALID_HANDLE_VALUE == hFile)
			return;

		DWORD dwByte = 0;
		_uint iCount = 0;

		_int iSelect = m_EventColiiderListBox.GetCount();

		for (_uint i = 0; i < iSelect; ++i)
			m_EventColiiderListBox.DeleteString(0);

		for (auto& pEventCollider : m_vecEvent_Collider)
			Safe_Delete(pEventCollider);

		m_vecEvent_Collider.clear();

		ReadFile(hFile, &iCount, sizeof(_uint), &dwByte, nullptr);

		UpdateData(TRUE);

		for (_uint i = 0; i < iCount; ++i)
		{
			_tchar		szName[MAX_PATH]{};
			_float4		vPos{};
			CCollider::DESC		tColliderDesc{};

			ReadFile(hFile, szName, sizeof(_tchar)*MAX_PATH, &dwByte, nullptr);
			ReadFile(hFile, &vPos, sizeof(_float4), &dwByte, nullptr);
			ReadFile(hFile, &tColliderDesc, sizeof(CCollider::DESC), &dwByte, nullptr);

			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

			CGameObject* pEvent_Collider = nullptr;

			if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr((_uint)LEVEL::STATIC, L"Prototype_GameObject_EventCollider", L"Layer_Event_Collider", &pEvent_Collider)))
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}

			((CEvent_Collider*)pEvent_Collider)->Set_Name(szName);

			CTransform* pTransform = (CTransform*)pEvent_Collider->Get_Component(COM_KEY_TRANSFORM);
			if (nullptr == pTransform)
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}

			CCollider* pCollider = (CCollider*)pEvent_Collider->Get_Component(L"Collider_Com");
			if (nullptr == pCollider)
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}

			pTransform->Set_State(CTransform::STATE::POSITION, XMLoadFloat4(&vPos));
			pCollider->Set_ColliderDesc(tColliderDesc);

			m_vecEvent_Collider.push_back((CEvent_Collider*)pEvent_Collider);
			m_EventColiiderListBox.AddString(szName);

			RELEASE_INSTANCE(CGameInstance);
		}
		UpdateData(FALSE);
		CloseHandle(hFile);
	}
	UpdateData(TRUE);
	UpdateData(FALSE);
}


void CEventColliderTab::OnBnClickedEventCreatebutton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CEvent_Collider* pEvent_Collider = nullptr;
	if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr((_uint)LEVEL::STATIC, L"Prototype_GameObject_EventCollider", L"Layer_Event_Collider", (CGameObject**)&pEvent_Collider)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}


	m_vecEvent_Collider.push_back(pEvent_Collider);
	pEvent_Collider->Set_Name(m_strName);
	m_EventColiiderListBox.AddString(m_strName);

	RELEASE_INSTANCE(CGameInstance);

	UpdateData(FALSE);
}


void CEventColliderTab::OnBnClickedEventDeletebutton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CEventColliderTab::OnEnChangeEventPosX()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.

	UpdateData(TRUE);
	Update_Transform();
	UpdateData(FALSE);
}


void CEventColliderTab::OnEnChangeEventPosY()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.

	UpdateData(TRUE);
	Update_Transform();
	UpdateData(FALSE);
}


void CEventColliderTab::OnEnChangeEventPosZ()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.

	UpdateData(TRUE);
	Update_Transform();
	UpdateData(FALSE);
}


void CEventColliderTab::OnEnChangeEventPivotX()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.

	UpdateData(TRUE);
	Update_ColliderDescPivot();
	UpdateData(FALSE);
}


void CEventColliderTab::OnEnChangeEventPivotY()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	Update_ColliderDescPivot();
	UpdateData(FALSE);
}


void CEventColliderTab::OnEnChangeEventPivotZ()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	Update_ColliderDescPivot();
	UpdateData(FALSE);
}


void CEventColliderTab::OnEnChangeEventScaleX()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	Update_ColliderDescScale();
	UpdateData(FALSE);
}


void CEventColliderTab::OnEnChangeEventScaleY()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	Update_ColliderDescScale();
	UpdateData(FALSE);
}


void CEventColliderTab::OnEnChangeEventScaleZ()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	Update_ColliderDescScale();
	UpdateData(FALSE);
}


void CEventColliderTab::OnDeltaposEventspinPosX(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_fPosX = m_fPosX - (float)(pNMUpDown->iDelta) / 10.f;
	Update_Transform();
	UpdateData(FALSE);
	*pResult = 0;
}


void CEventColliderTab::OnDeltaposEventspinPosY(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_fPosY = m_fPosY - (float)(pNMUpDown->iDelta) / 10.f;
	Update_Transform();
	UpdateData(FALSE);
	*pResult = 0;
}


void CEventColliderTab::OnDeltaposEventspinPosZ(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_fPosZ = m_fPosZ - (float)(pNMUpDown->iDelta) / 10.f;
	Update_Transform();
	UpdateData(FALSE);
	*pResult = 0;
}


void CEventColliderTab::OnDeltaposEventspinPivotX(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_fPivotX = m_fPivotX - (float)(pNMUpDown->iDelta) / 10.f;
	Update_ColliderDescPivot();
	UpdateData(FALSE);
	*pResult = 0;
}


void CEventColliderTab::OnDeltaposEventspinPivotY(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_fPivotY = m_fPivotY - (float)(pNMUpDown->iDelta) / 10.f;
	Update_ColliderDescPivot();
	UpdateData(FALSE);
	*pResult = 0;
}


void CEventColliderTab::OnDeltaposEventspinPivotZ(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_fPivotZ = m_fPivotZ - (float)(pNMUpDown->iDelta) / 10.f;
	Update_ColliderDescPivot();
	UpdateData(FALSE);
	*pResult = 0;
}


void CEventColliderTab::OnDeltaposEventspinScaleX(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_fScaleX = m_fScaleX - (float)(pNMUpDown->iDelta) / 10.f;
	Update_ColliderDescScale();
	UpdateData(FALSE);
	*pResult = 0;
}


void CEventColliderTab::OnDeltaposEventspinScaleY(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_fScaleY = m_fScaleY - (float)(pNMUpDown->iDelta) / 10.f;
	Update_ColliderDescScale();
	UpdateData(FALSE);
	*pResult = 0;
}


void CEventColliderTab::OnDeltaposEventspinScaleZ(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_fScaleZ = m_fScaleZ - (float)(pNMUpDown->iDelta) / 10.f;
	Update_ColliderDescScale();
	UpdateData(FALSE);
	*pResult = 0;
}


BOOL CEventColliderTab::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	CMainFrame*    pMain = dynamic_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd());
	CToolView*     pToolView = dynamic_cast<CToolView*>(pMain->GetActiveView());

	//m_pToolCam = pToolView->m_pCamera;
	m_pGraphicDevice = pToolView->Get_GraphicDevice();
	m_pContextDevice = pToolView->Get_ContextDevice();

	Safe_AddRef(m_pGraphicDevice);
	Safe_AddRef(m_pContextDevice);

	SetTimer(0, 17, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
