// CameraTab.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool.h"
#include "CameraTab.h"
#include "afxdialogex.h"
#include "ToolCamera.h"
#include "CamObject.h"

#include "MainFrm.h"
#include "ToolView.h"
#include "Tool_CamCollider.h"

// CCameraTab 대화 상자입니다.

IMPLEMENT_DYNAMIC(CCameraTab, CDialog)

CCameraTab::CCameraTab(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_CAMERATAB, pParent)
	, m_fPosX(0)
	, m_fPosY(0)
	, m_fPosZ(0)
	, m_strCameraName(_T(""))
	, m_strPointsName(_T(""))
	, m_fSpeed(0)
	, m_strAtName(_T(""))
	, m_fAtX(0)
	, m_fAtY(0)
	, m_fAtZ(0)
{
	//CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	//m_pToolCam = (CToolCamera*)pGameInstance->Get_ToolCamera();

	//RELEASE_INSTANCE(CGameInstance);
}

CCameraTab::~CCameraTab()
{
	Safe_Release(m_pGraphicDevice);
	Safe_Release(m_pContextDevice);

	auto& iter = m_mapCamera.begin();
	for (; iter != m_mapCamera.end();)
	{
		Safe_Delete(iter->second);
		iter = m_mapCamera.erase(iter);
	}
	
}

void CCameraTab::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CAMERALIST1, m_CamMoveListBox);
	DDX_Control(pDX, IDC_CAMERALIST2, m_MovingPointsListBox);
	DDX_Text(pDX, IDC_CAMERAEDIT1, m_fPosX);
	DDX_Text(pDX, IDC_CAMERAEDIT2, m_fPosY);
	DDX_Text(pDX, IDC_CAMERAEDIT3, m_fPosZ);
	DDX_Text(pDX, IDC_CAMERAEDIT5, m_strCameraName);
	DDX_Text(pDX, IDC_CAMERAEDIT6, m_strPointsName);
	DDX_Text(pDX, IDC_CAMERAEDIT4, m_fSpeed);
	DDX_Control(pDX, IDC_CAMERASPIN2, m_CtrlPositionX);
	DDX_Control(pDX, IDC_CAMERASPIN3, m_CtrlPositionY);
	DDX_Control(pDX, IDC_CAMERASPIN4, m_CtrlPositionZ);
	DDX_Control(pDX, IDC_CAMERASPIN6, m_CtrlSpeed);
	DDX_Control(pDX, IDC_CAMERABUTTON1, m_Radio_MovingPoints);
	DDX_Text(pDX, IDC_CAMERAEDIT8, m_strAtName);
	DDX_Text(pDX, IDC_CAMERAEDIT7, m_fAtX);
	DDX_Text(pDX, IDC_CAMERAEDIT9, m_fAtY);
	DDX_Text(pDX, IDC_CAMERAEDIT10, m_fAtZ);
	DDX_Control(pDX, IDC_CAMERASPIN1, m_CtrlAtPosX);
	DDX_Control(pDX, IDC_CAMERASPIN5, m_CtrlAtPosY);
	DDX_Control(pDX, IDC_CAMERASPIN7, m_CtrlAtPosZ);
	DDX_Control(pDX, IDC_CAMERALIST3, m_AtListBox);
	DDX_Control(pDX, IDC_CAMERACHECK1, m_bCamera_Check);
}

_int CCameraTab::Tick(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (IsWindowVisible() == true)
	{
		if (pGameInstance->Key_Down(DIK_Z))
		{
			if (m_bCamera_Check.GetCheck() == TRUE)
				m_bCamera_Check.SetCheck(FALSE);
			else
				m_bCamera_Check.SetCheck(TRUE);
		}
	}

	if (m_pCurCamObject != nullptr)
	{
		vector<CTool_CamCollider*>* pVecMovePointsCollider = m_pCurCamObject->Get_MovingPoints_ColliderGroup();

		for (auto vecCollider : *pVecMovePointsCollider)
			vecCollider->Tick(dTimeDelta);

		vector<CTool_CamCollider*>* pAtList_Collier = m_pCurCamObject->Get_At_ColliderGroup();

		for (auto vecAtCollider : *pAtList_Collier)
			vecAtCollider->Tick(dTimeDelta);
	}

	return _int();
}

_int CCameraTab::LateTick(_double dTimeDelta)
{
	if (m_pCurCamObject != nullptr)
	{
		vector<CTool_CamCollider*>* pVecMovePointsCollider = m_pCurCamObject->Get_MovingPoints_ColliderGroup();

		for (auto vecCollider : *pVecMovePointsCollider)
			vecCollider->LateTick(dTimeDelta);

		vector<CTool_CamCollider*>* pAtList_Collier = m_pCurCamObject->Get_At_ColliderGroup();

		for (auto vecAtCollider : *pAtList_Collier)
			vecAtCollider->LateTick(dTimeDelta);
	}

	return _int();
}

_int CCameraTab::Picking(_double dTimeDelta)
{
	return _int();
}


BEGIN_MESSAGE_MAP(CCameraTab, CDialog)
	ON_BN_CLICKED(IDC_CAMERABUTTON7, &CCameraTab::OnBnClickedPlay)
	ON_BN_CLICKED(IDC_CAMERABUTTON8, &CCameraTab::OnBnClickedStop)
	ON_BN_CLICKED(IDC_CAMERABUTTON9, &CCameraTab::OnBnClickedReset)
	ON_NOTIFY(UDN_DELTAPOS, IDC_CAMERASPIN2, &CCameraTab::OnDeltaposSpinX)
	ON_NOTIFY(UDN_DELTAPOS, IDC_CAMERASPIN3, &CCameraTab::OnDeltaposSpinY)
	ON_NOTIFY(UDN_DELTAPOS, IDC_CAMERASPIN4, &CCameraTab::OnDeltaposSpinZ)
	ON_NOTIFY(UDN_DELTAPOS, IDC_CAMERASPIN6, &CCameraTab::OnDeltaposSpinSpeed)
	ON_BN_CLICKED(IDC_CAMERABUTTON1, &CCameraTab::OnBnClickedSave)
	ON_BN_CLICKED(IDC_CAMERABUTTON2, &CCameraTab::OnBnClickedLoad)
	ON_BN_CLICKED(IDC_CAMERABUTTON3, &CCameraTab::OnBnClickedButtonMoveListCreate)
	ON_BN_CLICKED(IDC_CAMERABUTTON4, &CCameraTab::OnBnClickedButtonMoveListDelete)
	ON_BN_CLICKED(IDC_CAMERABUTTON5, &CCameraTab::OnBnClickedButtonMovingPointsCreate)
	ON_BN_CLICKED(IDC_CAMERABUTTON6, &CCameraTab::OnBnClickedButtonMovingPointsDelete)
	ON_BN_CLICKED(IDC_CAMERARADIO1, &CCameraTab::OnBnClickedMovingPoints)
	ON_BN_CLICKED(IDC_CAMERABUTTON10, &CCameraTab::OnBnClickedButtonAtDelete)
	ON_EN_CHANGE(IDC_CAMERAEDIT1, &CCameraTab::OnEnChangePosX)
	ON_EN_CHANGE(IDC_CAMERAEDIT2, &CCameraTab::OnEnChangePosY)
	ON_EN_CHANGE(IDC_CAMERAEDIT3, &CCameraTab::OnEnChangePosZ)
	ON_EN_CHANGE(IDC_CAMERAEDIT4, &CCameraTab::OnEnChangeSpeed)
	ON_NOTIFY(UDN_DELTAPOS, IDC_CAMERASPIN1, &CCameraTab::OnDeltaposSpinAtX)
	ON_NOTIFY(UDN_DELTAPOS, IDC_CAMERASPIN5, &CCameraTab::OnDeltaposSpinAtY)
	ON_NOTIFY(UDN_DELTAPOS, IDC_CAMERASPIN7, &CCameraTab::OnDeltaposSpinAtZ)
	ON_BN_CLICKED(IDC_CAMERABUTTON11, &CCameraTab::OnBnClickedButtonAtCreate)
	ON_LBN_SELCHANGE(IDC_CAMERALIST1, &CCameraTab::OnLbnSelchangeCameraMoveList)
	ON_LBN_SELCHANGE(IDC_CAMERALIST2, &CCameraTab::OnLbnSelchangeMovingPoints)
	ON_LBN_SELCHANGE(IDC_CAMERALIST3, &CCameraTab::OnLbnSelchangeAtListBox)
	ON_EN_CHANGE(IDC_CAMERAEDIT7, &CCameraTab::OnEnChangeAtX)
	ON_EN_CHANGE(IDC_CAMERAEDIT9, &CCameraTab::OnEnChangeAtY)
	ON_EN_CHANGE(IDC_CAMERAEDIT10, &CCameraTab::OnEnChangeAtZ)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CCameraTab 메시지 처리기입니다.


void CCameraTab::OnBnClickedPlay()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_pToolCam == nullptr)
		return;

	m_pToolCam->Set_CamPlayTrue();
}


void CCameraTab::OnBnClickedStop()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (m_pToolCam == nullptr)
		return;

	m_pToolCam->Set_CamPlayFalse();
}


void CCameraTab::OnBnClickedReset()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (m_pToolCam == nullptr)
		return;

	m_pToolCam->Set_CamResetTrue();
}


void CCameraTab::OnDeltaposSpinX(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_fPosX = m_fPosX - (float)(pNMUpDown->iDelta) / 10.f;
	Update_Data_PointsBox();
	UpdateData(FALSE);
	*pResult = 0;
}


void CCameraTab::OnDeltaposSpinY(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_fPosY = m_fPosY - (float)(pNMUpDown->iDelta) / 10.f;
	Update_Data_PointsBox();
	UpdateData(FALSE);
	*pResult = 0;
}


void CCameraTab::OnDeltaposSpinZ(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_fPosZ = m_fPosZ - (float)(pNMUpDown->iDelta) / 10.f;
	Update_Data_PointsBox();
	UpdateData(FALSE);
	*pResult = 0;
}


void CCameraTab::OnDeltaposSpinSpeed(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_fSpeed = m_fSpeed - (float)(pNMUpDown->iDelta) / 10.f;
	Update_Data_Speed();
	UpdateData(FALSE);
	*pResult = 0;
}


void CCameraTab::OnBnClickedSave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFileDialog Dlg(FALSE, L"dat", L"*.dat", 6UL, 0, (CWnd*)0, 0UL, FALSE);

	TCHAR szBuff[MAX_PATH]{};
	GetCurrentDirectory(MAX_PATH, szBuff);
	PathRemoveFileSpec(szBuff);

	lstrcat(szBuff, L"//Data//Location");
	Dlg.m_ofn.lpstrInitialDir = szBuff;

	if (IDOK == Dlg.DoModal())
	{
		CString wstrPath = Dlg.GetPathName();

		HANDLE hFile = CreateFile(wstrPath, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

		if (INVALID_HANDLE_VALUE == hFile)
			return;

		DWORD dwByte = 0;
		_uint iCount = (_uint)m_mapCamera.size();
		_uint iIndex = 0;

		WriteFile(hFile, &iCount, sizeof(_uint), &dwByte, nullptr);

		for (auto& iter : m_mapCamera)
		{
			_tchar pszBuff[MAX_PATH];
			ZeroMemory(pszBuff, sizeof(_tchar) * MAX_PATH);

			lstrcpy(pszBuff, iter.first.GetString());

			WriteFile(hFile, pszBuff, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);

			vector<_float4>* vVecMove = iter.second->Get_MovingPoints();

			_uint iSize = (_uint)(*vVecMove).size();
			WriteFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);

			for (auto& pVector : (*vVecMove))
				WriteFile(hFile, &pVector, sizeof(_float4), &dwByte, nullptr);

			vector<_float4>* vVecAt = iter.second->Get_At();

			iSize = (*vVecAt).size();
			WriteFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);

			for (auto& pVecAt : (*vVecAt))
				WriteFile(hFile, &pVecAt, sizeof(_float4), &dwByte, nullptr);

			_float fPositionSpeed = iter.second->Get_Speed();
			WriteFile(hFile, &fPositionSpeed, sizeof(_float), &dwByte, nullptr);

			_uint iNumEye = iter.second->Get_NumPoints();
			WriteFile(hFile, &iNumEye, sizeof(_uint), &dwByte, nullptr);

			_uint iNumAt = iter.second->Get_NumAt();
			WriteFile(hFile, &iNumAt, sizeof(_uint), &dwByte, nullptr);

		}

		CloseHandle(hFile);
	}
}


void CCameraTab::OnBnClickedLoad()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFileDialog Dlg(TRUE, L"dat", L"*dat", 6UL, 0, (CWnd*)0, 0UL, FALSE);

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

		auto& iter = m_mapCamera.begin();
		for (; iter != m_mapCamera.end();)
			Safe_Delete(iter->second);
		m_mapCamera.clear();
		_int iListSize = m_CamMoveListBox.GetCount();

		if (iListSize > 0)
		{
			for (int i = 0; i < iListSize; ++i)
				m_CamMoveListBox.DeleteString(0);
		}

		iListSize = m_MovingPointsListBox.GetCount();
		
		if (iListSize > 0)
		{
			for (int i = 0; i < iListSize; ++i)
				m_MovingPointsListBox.DeleteString(0);
		}

		iListSize = m_AtListBox.GetCount();

		if (iListSize > 0)
		{
			for (int i = 0; i < iListSize; ++i)
				m_AtListBox.DeleteString(0);
		}

		iCount = 0;
		ReadFile(hFile, &iCount, sizeof(_uint), &dwByte, nullptr);

		UpdateData(TRUE);

		for (_uint i = 0; i < iCount; ++i)
		{
			_tchar szBuff[MAX_PATH];
			ZeroMemory(szBuff, sizeof(_tchar) * MAX_PATH);

			ReadFile(hFile, szBuff, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
			CString StrBuff;
			StrBuff.SetString(szBuff);

			_uint iSize = 0;
			ReadFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);

			vector<_float4> vecMovePoints;
			vector<CTool_CamCollider*> vecMovePointsCollider;
			for (_uint i = 0; i < iSize; ++i)
			{
				_float4 vPos;
				ReadFile(hFile, &vPos, sizeof(_float4), &dwByte, nullptr);
				vecMovePoints.push_back(vPos);
				_vector vPosition = XMLoadFloat4(&vPos);
				vecMovePointsCollider.push_back(CTool_CamCollider::Create(m_pGraphicDevice, m_pContextDevice, &vPosition));
				
			}

			_uint iSize2 = 0;
			ReadFile(hFile, &iSize2, sizeof(_uint), &dwByte, nullptr);

			vector<_float4> vecAtPoints;
			vector<CTool_CamCollider*> vecAtPointsCollider;
			for (_uint j = 0; j < iSize2; ++j)
			{
				_float4 vAtPos;
				ReadFile(hFile, &vAtPos, sizeof(_float4), &dwByte, nullptr);
				vecAtPoints.push_back(vAtPos);
				_vector vAtPosition = XMLoadFloat4(&vAtPos);
				vecAtPointsCollider.push_back(CTool_CamCollider::Create(m_pGraphicDevice, m_pContextDevice, &vAtPosition));
			}
			_float fPositionSpeed = 0.f;
			ReadFile(hFile, &fPositionSpeed, sizeof(_float), &dwByte, nullptr);
			_uint iNumEye = 0;
			ReadFile(hFile, &iNumEye, sizeof(_uint), &dwByte, nullptr);
			_uint iNumAt = 0;
			ReadFile(hFile, &iNumAt, sizeof(_uint), &dwByte, nullptr);
			
			CCamObject* pCamObject = new CCamObject(m_pGraphicDevice,m_pContextDevice);
			pCamObject->Set_vecMovePoints(vecMovePoints);
			pCamObject->Set_vecAt(vecAtPoints);
			pCamObject->Set_Speed(fPositionSpeed);
			pCamObject->Set_NumPoints(iNumEye);
			pCamObject->Set_NumAt(iNumAt);
			pCamObject->Set_vecMovePoints_Collider(vecMovePointsCollider);
			pCamObject->Set_vecAt_ColliderGroup(vecAtPointsCollider);

			m_mapCamera.emplace(StrBuff, pCamObject);
			m_CamMoveListBox.AddString(StrBuff);
		}

		UpdateData(FALSE);
		CloseHandle(hFile);
	}
	UpdateData(TRUE);
	UpdateData(FALSE);
}


void CCameraTab::OnBnClickedButtonMoveListCreate()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	auto& iter = find_if(m_mapCamera.begin(), m_mapCamera.end(), CTagFinder(m_strCameraName));
	
	if (iter == m_mapCamera.end())
	{
		CCamObject* pCamObject = nullptr;
		pCamObject = new CCamObject(m_pGraphicDevice, m_pContextDevice);

		m_mapCamera.emplace(m_strCameraName, pCamObject);
		m_CamMoveListBox.AddString(m_strCameraName);
	}
	else
		return;

	UpdateData(FALSE);
}


void CCameraTab::OnBnClickedButtonMoveListDelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
}


void CCameraTab::OnBnClickedButtonMovingPointsCreate()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	if (m_pCurCamObject == nullptr)
		return;

	int iNumPoints = m_pCurCamObject->Get_NumPoints();

	_tchar szBuff[MAX_PATH]{};
	lstrcpy(szBuff, L"Point_");

	_tchar szNum[MAX_PATH]{};
	swprintf_s(szNum, L"%d", iNumPoints);

	lstrcat(szBuff,szNum);

	m_pCurCamObject->Set_Points_Plus();

	_vector vPos = { 3.f , 2.f , 1.f , 1.f };

	_float4 vPosition;
	XMStoreFloat4(&vPosition, vPos);

	m_pCurCamObject->Get_MovingPoints()->push_back(vPosition);
	m_pCurCamObject->Get_MovingPoints_ColliderGroup()->push_back(CTool_CamCollider::Create(m_pGraphicDevice, m_pContextDevice, &vPos));

	m_MovingPointsListBox.AddString(szBuff);

	UpdateData(FALSE);
}


void CCameraTab::OnBnClickedButtonMovingPointsDelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	if (m_pCurCamObject == nullptr)
		return;

	_int iSel = m_MovingPointsListBox.GetCurSel();

	if (iSel == -1)
		return;

	vector<_float4>* pVecMovePointsList = m_pCurCamObject->Get_MovingPoints();
	vector<CTool_CamCollider*>* pVecMovePointsList_Collider = m_pCurCamObject->Get_MovingPoints_ColliderGroup();

	pVecMovePointsList->erase(pVecMovePointsList->begin() + iSel);
	pVecMovePointsList_Collider->erase(pVecMovePointsList_Collider->begin() + iSel);

	m_pCurCamObject->Set_Points_Minus();

	UpdateData(FALSE);
}


void CCameraTab::OnBnClickedMovingPoints()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CCameraTab::OnBnClickedButtonAtDelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	if (m_pCurCamObject == nullptr)
		return;

	_int iSel = m_AtListBox.GetCurSel();

	if (iSel == -1)
		return;

	vector<_float4>* pAtList = m_pCurCamObject->Get_At();
	vector<CTool_CamCollider*>* pAtList_Collier = m_pCurCamObject->Get_At_ColliderGroup();

	pAtList->erase(pAtList->begin() + iSel);
	pAtList_Collier->erase(pAtList_Collier->begin() + iSel);

	m_pCurCamObject->Set_NumAt_Minus();

	UpdateData(FALSE);
}

void CCameraTab::Update_Data_PointsBox()
{
	_int iSel = m_MovingPointsListBox.GetCurSel();
	if (-1 == iSel)
		return;

	if (m_pCurCamObject == nullptr)
		return;

	_vector vPos = XMVectorSet(m_fPosX, m_fPosY, m_fPosZ, 1.f);

	_float4 vPosition;
	XMStoreFloat4(&vPosition, vPos);

	vector<_float4>* pVecMovePointsList = m_pCurCamObject->Get_MovingPoints();
	vector<CTool_CamCollider*>* pVecMovePointsCollider = m_pCurCamObject->Get_MovingPoints_ColliderGroup();

	((*pVecMovePointsList)[iSel]) = vPosition;
	CTransform* pTransform = (CTransform*)((*pVecMovePointsCollider)[iSel])->Get_Component(COM_KEY_TRANSFORM);
	pTransform->Set_State(CTransform::STATE::POSITION, vPos);

	UpdateData(FALSE);
	
}

void CCameraTab::Update_Data_AtBox()
{
	_int iSel = m_AtListBox.GetCurSel();
	if (-1 == iSel)
		return;

	if (m_pCurCamObject == nullptr)
		return;

	_vector vPos = XMVectorSet(m_fAtX, m_fAtY, m_fAtZ, 1.f);

	_float4 vPosition;
	XMStoreFloat4(&vPosition, vPos);

	vector<_float4>* pVecAtList = m_pCurCamObject->Get_At();
	vector<CTool_CamCollider*>* pVecAtPointsCollider = m_pCurCamObject->Get_At_ColliderGroup();

	((*pVecAtList)[iSel]) = vPosition;
	CTransform* pTransform = (CTransform*)((*pVecAtPointsCollider)[iSel])->Get_Component(COM_KEY_TRANSFORM);
	pTransform->Set_State(CTransform::STATE::POSITION, vPos);

	UpdateData(FALSE);
}

void CCameraTab::Update_Data_Speed()
{
	_int iSel = m_CamMoveListBox.GetCurSel();
	if (-1 == iSel)
		return;

	if (m_pCurCamObject == nullptr)
		return;

	m_pCurCamObject->Set_Speed(m_fSpeed);

	UpdateData(FALSE);
}

void CCameraTab::Update_Data()
{
	if (m_pToolCam == nullptr)
		return;

	m_pToolCam->Set_CamObject(m_pCurCamObject);
}


void CCameraTab::OnEnChangePosX()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.

	UpdateData(TRUE);
	Update_Data_PointsBox();
	UpdateData(FALSE);
}


void CCameraTab::OnEnChangePosY()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	Update_Data_PointsBox();
	UpdateData(FALSE);
}


void CCameraTab::OnEnChangePosZ()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	Update_Data_PointsBox();
	UpdateData(FALSE);
}


void CCameraTab::OnEnChangeSpeed()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	Update_Data_Speed();
	UpdateData(FALSE);
}


void CCameraTab::OnDeltaposSpinAtX(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_fAtX = m_fAtX - (float)(pNMUpDown->iDelta) / 10.f;
	Update_Data_AtBox();
	UpdateData(FALSE);
	*pResult = 0;
}


void CCameraTab::OnDeltaposSpinAtY(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_fAtY = m_fAtY - (float)(pNMUpDown->iDelta) / 10.f;
	Update_Data_AtBox();
	UpdateData(FALSE);
	*pResult = 0;
}


void CCameraTab::OnDeltaposSpinAtZ(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_fAtZ = m_fAtZ - (float)(pNMUpDown->iDelta) / 10.f;
	Update_Data_AtBox();
	UpdateData(FALSE);
	*pResult = 0;
}


void CCameraTab::OnBnClickedButtonAtCreate()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	if (m_pCurCamObject == nullptr)
		return;

	int iNumPoints = m_pCurCamObject->Get_NumAt();

	_tchar szBuff[MAX_PATH]{};
	lstrcpy(szBuff, L"At_");

	_tchar szNum[MAX_PATH]{};
	swprintf_s(szNum, L"%d", iNumPoints);

	lstrcat(szBuff, szNum);

	m_pCurCamObject->Set_NumAt_Plus();

	_vector vPos = { 1.f , 1.f , 1.f , 1.f };

	_float4 vPosition;
	XMStoreFloat4(&vPosition, vPos);

	m_pCurCamObject->Get_At()->push_back(vPosition);
	m_pCurCamObject->Get_At_ColliderGroup()->push_back(CTool_CamCollider::Create(m_pGraphicDevice, m_pContextDevice, &vPos));
	m_AtListBox.AddString(szBuff);

	UpdateData(FALSE);
}


void CCameraTab::OnLbnSelchangeCameraMoveList()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	int iSel = m_CamMoveListBox.GetCurSel();
	if (-1 == iSel)
		return;

	CString strName;
	m_CamMoveListBox.GetText(iSel,strName);

	auto& iter = find_if(m_mapCamera.begin(), m_mapCamera.end(), CTagFinder(strName));
	m_pCurCamObject = iter->second;

	_uint iNum = m_MovingPointsListBox.GetCount();

	for (int i = 0; i < iNum; ++i)
	{
		m_MovingPointsListBox.DeleteString(0);
	}

	_uint iNum3 = m_AtListBox.GetCount();

	for (int k = 0; k < iNum3; ++k)
	{
		m_AtListBox.DeleteString(0);
	}

	_uint iNum2 = m_pCurCamObject->Get_NumPoints();

	for (int j = 0; j < iNum2; ++j)
	{
		_tchar szBuff[MAX_PATH]{};
		lstrcpy(szBuff, L"Point_");

		_tchar szNum[MAX_PATH]{};
		swprintf_s(szNum, L"%d", j);

		lstrcat(szBuff, szNum);

		m_MovingPointsListBox.AddString(szBuff);
	}

	_uint iNum4 = m_pCurCamObject->Get_NumAt();

	for (int q = 0; q < iNum4; ++q)
	{
		_tchar szBuff[MAX_PATH]{};
		lstrcpy(szBuff, L"At_");

		_tchar szNum[MAX_PATH]{};
		swprintf_s(szNum, L"%d", q);

		lstrcat(szBuff, szNum);

		m_AtListBox.AddString(szBuff);
	}

	m_fSpeed = m_pCurCamObject->Get_Speed();

	UpdateData(FALSE);

}


void CCameraTab::OnLbnSelchangeMovingPoints()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	UpdateData(TRUE);

	if (m_pCurCamObject == nullptr)
		return;

	int iSel = m_MovingPointsListBox.GetCurSel();

	if (iSel == -1)
		return;

	vector<_float4>* pVecMovePointsList = m_pCurCamObject->Get_MovingPoints();

	_float4 vSelPoint = ((*pVecMovePointsList)[iSel]);

	m_fPosX = vSelPoint.x;
	m_fPosY = vSelPoint.y;
	m_fPosZ = vSelPoint.z;

	UpdateData(FALSE);

}


void CCameraTab::OnLbnSelchangeAtListBox()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	if (m_pCurCamObject == nullptr)
		return;

	int iSel = m_AtListBox.GetCurSel();

	if (iSel == -1)
		return;

	vector<_float4>* pVecAtList = m_pCurCamObject->Get_At();

	_float4 vSelAt = ((*pVecAtList)[iSel]);

	m_fAtX = vSelAt.x;
	m_fAtY = vSelAt.y;
	m_fAtZ = vSelAt.z;

	UpdateData(FALSE);
}


void CCameraTab::OnEnChangeAtX()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	Update_Data_AtBox();
	UpdateData(FALSE);
}


void CCameraTab::OnEnChangeAtY()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	Update_Data_AtBox();
	UpdateData(FALSE);
}


void CCameraTab::OnEnChangeAtZ()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	Update_Data_AtBox();
	UpdateData(FALSE);
}


BOOL CCameraTab::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	
	CMainFrame*    pMain = dynamic_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd());
	CToolView*     pToolView = dynamic_cast<CToolView*>(pMain->GetActiveView());

	m_pToolCam = pToolView->m_pCamera;
	m_pGraphicDevice = pToolView->Get_GraphicDevice();
	m_pContextDevice = pToolView->Get_ContextDevice();

	Safe_AddRef(m_pGraphicDevice);
	Safe_AddRef(m_pContextDevice);

	SetTimer(0, 17, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CCameraTab::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	switch (nIDEvent)
	{
	case 0:
		Update_Data();
		break;
	default:
		break;
	}

	CDialog::OnTimer(nIDEvent);
}
