// EffectMesh2Tab.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool.h"
#include "EffectMesh2Tab.h"
#include "afxdialogex.h"
#include "ToolView.h"
#include "ToolDefines.h"
#include "MainFrm.h"
#include "MainTab.h"
#include "EffectMeshTab.h"
// CEffectMesh2Tab 대화 상자입니다.

IMPLEMENT_DYNAMIC(CEffectMesh2Tab, CDialog)

CEffectMesh2Tab::CEffectMesh2Tab(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_EFFECTMESH2TAB, pParent)
{

}

CEffectMesh2Tab::~CEffectMesh2Tab()
{
	for (auto& pIter : m_listShaderName)
		Safe_Delete(pIter);
	m_listShaderName.clear();
}

void CEffectMesh2Tab::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EFFECTMESH2_SHADER_LIST, m_ShaderList);
	DDX_Control(pDX, IDC_EFFECTMESH2_ADDED_SHADER_LIST, m_AddedShaderList);
}


BEGIN_MESSAGE_MAP(CEffectMesh2Tab, CDialog)
	ON_LBN_SELCHANGE(IDC_EFFECTMESH2_SHADER_LIST, &CEffectMesh2Tab::OnLbnSelchangeEffectmesh2ShaderList)
	ON_BN_CLICKED(IDC_MESHEFFECT2_ADD_SHADER_BUTTON, &CEffectMesh2Tab::OnBnClickedMesheffect2AddShaderButton)
	ON_BN_CLICKED(IDC_EFFECTMESH2_DELETE_SHADER_BUTTON, &CEffectMesh2Tab::OnBnClickedEffectmesh2DeleteShaderButton)
	ON_LBN_SELCHANGE(IDC_EFFECTMESH2_ADDED_SHADER_LIST, &CEffectMesh2Tab::OnLbnSelchangeEffectmesh2AddedShaderList)
END_MESSAGE_MAP()


// CEffectMesh2Tab 메시지 처리기입니다.




BOOL CEffectMesh2Tab::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_ShaderList.AddString(L"Dissolve");
	m_ShaderList.AddString(L"Glow");
	m_ShaderList.AddString(L"Distortion");
	m_ShaderList.AddString(L"Blur");
	m_ShaderList.AddString(L"Trail");

	return TRUE;  
}


void CEffectMesh2Tab::OnLbnSelchangeEffectmesh2ShaderList()
{
	int iIndex = m_ShaderList.GetCurSel();
	if (iIndex >= m_ShaderList.GetCount() || iIndex < 0)
		return;
	m_ShaderList.GetText(iIndex, m_strShaderListSelected);
}


void CEffectMesh2Tab::OnBnClickedMesheffect2AddShaderButton()
{
	if (m_strShaderListSelected.GetLength() < 1) return;

	if (LB_ERR == m_AddedShaderList.FindString(0, m_strShaderListSelected))
	{
		m_AddedShaderList.AddString(m_strShaderListSelected);
		TCHAR* szName = (TCHAR*)(LPCTSTR)m_strShaderListSelected;
		SHADERNAME* tShaderName = new SHADERNAME;
		_tcscpy_s(tShaderName->szName, szName);
		m_listShaderName.emplace_back(tShaderName);
	}

}


void CEffectMesh2Tab::OnBnClickedEffectmesh2DeleteShaderButton()
{
	if (m_strAddedShaderListSelected.GetLength() < 1) return;

	_int iResult = m_AddedShaderList.FindString(0, m_strAddedShaderListSelected);

	if (LB_ERR == iResult)
		return;
	m_AddedShaderList.DeleteString(iResult);

	TCHAR* szName = (TCHAR*)(LPCTSTR)m_strAddedShaderListSelected;

	list<SHADERNAME*>::iterator iter;
	for (iter = m_listShaderName.begin(); iter != m_listShaderName.end(); ++iter)
	{
		if (!_tcscmp((*iter)->szName, szName))
		{
			break;
		}
	}
	Safe_Delete(*iter);
	m_listShaderName.erase(iter);
}


void CEffectMesh2Tab::OnLbnSelchangeEffectmesh2AddedShaderList()
{
	int iIndex = m_AddedShaderList.GetCurSel();
	if (iIndex >= m_AddedShaderList.GetCount() || iIndex < 0)
		return;
	m_AddedShaderList.GetText(iIndex, m_strAddedShaderListSelected);
}
