#include "..\Public\Navigation.h"
#include "Cell.h"
#include "Transform.h"

CNavigation::CNavigation(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CComponent(pGraphicDevice, pContextDevice)
{
	
}

CNavigation::CNavigation(const CNavigation & rhs)
	: CComponent(rhs), m_vecCells(rhs.m_vecCells), m_dTimeDelta(rhs.m_dTimeDelta), m_matWorld(rhs.m_matWorld)
	, m_iSize(rhs.m_iSize)
{
	for (auto& pCell : m_vecCells)
		Safe_AddRef(pCell);
}

HRESULT CNavigation::NativeConstruct_Prototype(const _tchar * pNavDataFilePath)
{
	_ulong				dwByte = 0;
	HANDLE				hFile = CreateFile(pNavDataFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (hFile == INVALID_HANDLE_VALUE)
		return E_FAIL;

	_uint iSize;
	ReadFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < iSize; ++i)
	{
		NAVINFO tNavigationInfo;
		ReadFile(hFile, &tNavigationInfo, sizeof(NAVINFO), &dwByte, nullptr);

	
		CCell*		pCell = CCell::Create(m_pGraphicDevice, m_pContextDevice, tNavigationInfo.vPoint,tNavigationInfo.eOption, (_uint)m_vecCells.size());
		if (pCell == nullptr)
			return E_FAIL;

		m_vecCells.push_back(pCell);
	}
	CloseHandle(hFile);

	if (FAILED(SetUp_Neighbor()))
		return E_FAIL;

	Update(XMMatrixIdentity());

	m_iSize = (_uint)m_vecCells.size();

	return S_OK;
}

HRESULT CNavigation::NativeConstruct(void * pArg)
{
	if (pArg != nullptr)
		m_iCurrentIndex = *(_uint*)pArg;

	return S_OK;
}

HRESULT CNavigation::Update(_fmatrix matWorld)
{
	XMStoreFloat4x4(&m_matWorld, matWorld);

	for (auto& Cell : m_vecCells)
		Cell->Update(matWorld);

	return S_OK;
}

HRESULT CNavigation::SetUp_Neighbor()
{
	for (auto& pSourCell : m_vecCells)
	{
		for (auto& pDestCell : m_vecCells)
		{
			if (pSourCell == pDestCell)
				continue;

			if (pDestCell->Compare(pSourCell->Get_PointInLocal(CCell::POINT::A), pSourCell->Get_PointInLocal(CCell::POINT::B)))
				pSourCell->Set_Neighbor(CCell::LINE::AB, pDestCell);

			if (pDestCell->Compare(pSourCell->Get_PointInLocal(CCell::POINT::B), pSourCell->Get_PointInLocal(CCell::POINT::C)))
				pSourCell->Set_Neighbor(CCell::LINE::BC, pDestCell);

			if (pDestCell->Compare(pSourCell->Get_PointInLocal(CCell::POINT::C), pSourCell->Get_PointInLocal(CCell::POINT::A)))
				pSourCell->Set_Neighbor(CCell::LINE::CA, pDestCell);
		}
	}
	return S_OK;
}

_bool CNavigation::IsOn(_vector& vMovePos, CCell** ppSlideOut)
{
	CCell*		pNeighbor = nullptr;
	if (m_vecCells[m_iCurrentIndex]->IsIn(vMovePos, &pNeighbor, ppSlideOut) == false)
	{
		if (pNeighbor != nullptr)
		{
			while (true)
			{
				if (pNeighbor == nullptr)
					return false;
				if (pNeighbor->IsIn(vMovePos, &pNeighbor, ppSlideOut))
					break;
			}
			m_iCurrentIndex = pNeighbor->Get_Index();
			return true;
		}
		else
			return false;
	}

	return true;
}

_bool CNavigation::IsLowThanCell(_vector & vPos, _vector* pOut)
{
	CCell*		pNeighbor = nullptr;
	if (m_vecCells[m_iCurrentIndex]->IsInForCompare(vPos, pOut, &pNeighbor) == false)
	{
		if (pNeighbor != nullptr)
		{
			while (true)
			{
				if (pNeighbor == nullptr)
					return false;
				if (pNeighbor->IsInForCompare(vPos, pOut, &pNeighbor))
					break;
			}
			m_iCurrentIndex = pNeighbor->Get_Index();
			return true;
		}
		else
			return false;
	}

	return true;
}

_bool CNavigation::IsJump(_vector & vMovePos, CCell ** ppSlideOut)
{
	CCell*		pNeighbor = nullptr;
	if (m_vecCells[m_iCurrentIndex]->IsJump(vMovePos, &pNeighbor, ppSlideOut) == false)
	{
		if (pNeighbor != nullptr)
		{
 			while (true)
			{
				if (pNeighbor == nullptr)
					return false;
				if (pNeighbor->IsJump(vMovePos, &pNeighbor, ppSlideOut))
					break;
			}
			m_iCurrentIndex = pNeighbor->Get_Index();
			return true;
		}
		else
			return false;
	}

	return true;
}

HRESULT CNavigation::Render()
{
	for (auto& Cell : m_vecCells)
		Cell->Render(XMLoadFloat4x4(&m_matWorld));

	return S_OK;
}

HRESULT CNavigation::Culling(_double dTimeDelta)
{
	m_dTimeDelta += dTimeDelta;
	if (m_dTimeDelta >= 1.5)
	{
		for (auto& Cell : m_vecCells)
			Cell->IsCulling();
		m_dTimeDelta = 0.0;
	}

	return S_OK;
}

_uint CNavigation::Find_CurrentCell(const _vector& vPosition)
{
	_uint iSize = (_uint)m_vecCells.size();
	for (_uint i = 0; i < iSize; ++i)
	{
		if (m_vecCells[i]->Check_CurrentCell(vPosition))
			return i;
	}
	return -1;
}

HRESULT CNavigation::Init_Index(const _vector& vPosition)
{
	m_iCurrentIndex = Find_CurrentCell(vPosition);

	if (m_iCurrentIndex < 0)
		return E_FAIL;

	return S_OK;
}

HRESULT CNavigation::Init_Index(const _uint& iIndex)
{
	if (m_iCurrentIndex >= m_vecCells.size())
		return E_FAIL;

	if (m_iCurrentIndex < 0)
		return E_FAIL;

	m_iCurrentIndex = iIndex;

	return S_OK;
}

void CNavigation::Find_NearCell(_vector& vPosition)
{
	list<CELLSORTDESC>	ListCellSort;

	for (auto& pCell : m_vecCells)
	{
		_vector vTempPos = vPosition;

		_vector vCenterPos = Find_CenterPos(pCell);

		vTempPos = XMVectorSetY(vTempPos, XMVectorGetY(vCenterPos));
		
		_vector vDist = XMVector3Length(vCenterPos - vTempPos);

		CELLSORTDESC tDesc{};

		tDesc.fDist = XMVectorGetX(vDist);
		tDesc.pCell = pCell;

		ListCellSort.push_back(tDesc);
	}

	if (0 >= ListCellSort.size())
		return;

	ListCellSort.sort([](CELLSORTDESC& Src, CELLSORTDESC& Dsc) {
		return Src.fDist < Dsc.fDist;
	});

	if (nullptr == ListCellSort.front().pCell)
		return;

	_vector vPos = Find_CenterPos(ListCellSort.front().pCell);

	vPosition = vPos;
	vPosition = XMVectorSetW(vPosition, 1.f);
}

_vector CNavigation::Find_CenterPos(CCell * pCell)
{
	_vector vPointA = pCell->Get_PointInWorld(CCell::POINT::A);
	_vector vPointB = pCell->Get_PointInWorld(CCell::POINT::B);
	_vector vPointC = pCell->Get_PointInWorld(CCell::POINT::C);

	_float fDist = 0.f;

	_vector vCenter = ((vPointB + vPointC) / 2.f);

	_vector vPos = ((vCenter + vPointA) / 2.f);
	vPos = XMVectorSetW(vPos, 1.f);

	return vPos;
}

_vector CNavigation::Find_CenterPos(_uint iIndex)
{
	if (iIndex >= m_vecCells.size())
		return XMVectorZero();

	return Find_CenterPos(m_vecCells[iIndex]);
}

const _float4 & CNavigation::Get_CenterPosition(const _uint & iIndex)
{
	return m_vecCells[iIndex]->Get_CenterPosition();
}

_vector CNavigation::Get_CellPointInWorld(_uint iIndex, CCell::POINT ePoint)
{
	if (iIndex >= m_vecCells.size())
		return XMVectorZero();

	return m_vecCells[iIndex]->Get_PointInWorld(ePoint);
}

CNavigation * CNavigation::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, const _tchar * pNavDataFilePath)
{
	CNavigation* pInstance = new CNavigation(pGraphicDevice, pContextDevice);
	if (FAILED(pInstance->NativeConstruct_Prototype(pNavDataFilePath)))
		Safe_Release(pInstance);

	return pInstance;
}

CComponent * CNavigation::Clone(void * pArg)
{
	CNavigation* pInstance = new CNavigation(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CNavigation::Free()
{
	unsigned long dwRefCnt = 0;
	__super::Free();

	for (auto& Cell : m_vecCells)
		dwRefCnt = Safe_Release(Cell);
	m_vecCells.clear();
}