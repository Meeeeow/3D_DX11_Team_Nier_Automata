#include "../Public/Model.h"
#include "MeshContainer.h"
#include "Texture.h"
#include "Transform.h"
#include "Picking.h"
#include "Navigation.h"
#include "Cell.h"

CModel::CModel(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CVIBuffer(pGraphicDevice, pContextDevice)
{
}

CModel::CModel(const CModel & rhs)
	: CVIBuffer(rhs), m_Materials(rhs.m_Materials), m_PivotMatrix(rhs.m_PivotMatrix), m_IPGeneral(rhs.m_IPGeneral)
	, m_bHaveAnim(rhs.m_bHaveAnim), m_pAnimationPath(rhs.m_pAnimationPath), m_fRadius(rhs.m_fRadius)
	, m_vMiddlePoint(rhs.m_vMiddlePoint)
{
	for (auto& pMeshContainer_Prototype : rhs.m_MeshContainers)
	{
		CMeshContainer*		pMeshContainer = pMeshContainer_Prototype->Clone();
		m_MeshContainers.push_back(pMeshContainer);
	}

	for (auto& pMaterial : m_Materials)
	{
		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; ++j)
		{
			Safe_AddRef(pMaterial->pMaterialTextures[j]);
		}
	}

	// .Dat 파싱해서 만든 컨테이너들은 얕은 복사로 관리하며 원본 객체 삭제시 정리 
	for (auto& pImportMeshContainer : rhs.m_vecIPMeshContainer)
	{
		m_vecIPMeshContainer.push_back(pImportMeshContainer);
	}
	for (auto& pImportMaterial : rhs.m_vecIPMaterial)
	{
		m_vecIPMaterial.push_back(pImportMaterial);
	}
	for (auto& pImportHNode : rhs.m_vecIPHNode)
	{
		m_vecIPHNode.push_back(pImportHNode);
	}
	for (auto& pImportAnim : rhs.m_vecIPAnim)
	{
		m_vecIPAnim.push_back(pImportAnim);
	}
	for (auto& pImportChannel : rhs.m_vecIPChannel)
	{
		m_vecIPChannel.push_back(pImportChannel);
	}
}

CModel * CModel::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, const _tchar * pDatFilePath, const _tchar * pShaderFilePath, const char * pTechniqueName, _bool bMFC, const _tchar * pAnimPath)
{
	CModel* pInstance = new CModel(pGraphicDevice, pContextDevice);

	if (FAILED(pInstance->NativeConstruct_Prototype(pDatFilePath, pShaderFilePath, pTechniqueName, bMFC, pAnimPath)))
	{
		MSGBOX("Failed To Creating CModel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CModel * CModel::DynamicCreate(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, const _tchar * pDatFilePath, const _tchar * pShaderFilePath, const char * pTechniqueName)
{
	CModel* pInstance = new CModel(pGraphicDevice, pContextDevice);
	if (FAILED(pInstance->NativeConstruct_DynamicPrototype(pDatFilePath, pShaderFilePath, pTechniqueName)))
		Safe_Release(pInstance);

	return pInstance;
}

HRESULT CModel::NativeConstruct_Prototype(const _tchar * pDatFilePath, const _tchar * pShaderFilePath, const char * pTechniqueName, _bool bMFC, const _tchar* pAnimPath)
{
	 m_pAnimationPath = pAnimPath;

	if (FAILED(Import_Parameter(pDatFilePath)))
	{
		return E_FAIL;
	}
	if (FAILED(Create_MeshContainer()))
	{
		return E_FAIL;
	}
	if (FAILED(Compile_Shader(pShaderFilePath, pTechniqueName)))
	{
		return E_FAIL;
	}
	if (FAILED(Create_Materials(bMFC)))
	{
		return E_FAIL;
	}
	if (FAILED(Create_VIBuffer()))
	{
		return E_FAIL;
	}


	//XMStoreFloat4x4(&m_PivotMatrix, XMMatrixRotationAxis(XMVectorSet(0.0f, 1.f, 0.f, 0.f), XMConvertToRadians(180.0f)));

	// ImportModel에서는 Create_SkinnedMesh()가 필요없음!!

	return S_OK;
}

HRESULT CModel::NativeConstruct_DynamicPrototype(const _tchar * pDatFilePath, const _tchar * pShaderFilePath, const char * pTechniqueName)
{
	m_pAnimationPath = nullptr;

	if (FAILED(Import_Parameter(pDatFilePath)))
	{
		return E_FAIL;
	}
	if (FAILED(Create_MeshContainer()))
	{
		return E_FAIL;
	}
	if (FAILED(Compile_Shader(pShaderFilePath, pTechniqueName)))
	{
		return E_FAIL;
	}
	if (FAILED(Create_Materials(false)))
	{
		return E_FAIL;
	}
	if (FAILED(Create_DynamicVIBuffer()))
	{
		return E_FAIL;
	}
	return S_OK;
}

CComponent * CModel::Clone(void * pArg)
{
	CModel* pInstance = new CModel(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed To Cloning CModel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

HRESULT CModel::NativeConstruct(void * pArg)
{
	if (0 == m_IPGeneral.iNumAnimations)
	{
		Compute_MiddlePoint();
		return S_OK;
	}
	if (FAILED(Create_HierarchyNodes()))
	{
		return E_FAIL;
	}
	if (FAILED(SetUp_HierarchyNodeToMeshContainer()))
	{
		return E_FAIL;
	}
	if (nullptr != m_pAnimationPath)
	{
		if (FAILED(Create_Animation_FromPath()))
			return E_FAIL;
	}
	else
	{
		if (FAILED(Create_Animation()))
		{
			return E_FAIL;
		}
	}

	Compute_MiddlePoint();

	return S_OK;
}

void CModel::Free()
{
	for (auto& pMaterial : m_Materials)
	{
		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; ++j)
		{
			Safe_Release(pMaterial->pMaterialTextures[j]);
		}
		if (false == m_bCloned)
			Safe_Delete(pMaterial);
	}

	m_Materials.clear();

	for (auto& Pair : m_RefNodes)
		Safe_Release(Pair.second);
	m_RefNodes.clear();

	for (auto& pHierarchyNode : m_HierarchyNodes)
		Safe_Release(pHierarchyNode);
	m_HierarchyNodes.clear();

	for (auto& pAnimation : m_Animations)
		Safe_Release(pAnimation);
	m_Animations.clear();

	for (auto& pMeshContainer : m_MeshContainers)
		Safe_Release(pMeshContainer);
	m_MeshContainers.clear();

	Free_Param();

	__super::Free();
}

HRESULT CModel::Import_Parameter(const _tchar * pDatFilePath)
{
	HANDLE	hFile;

	//hFile = CreateFile(pDatFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	hFile = CreateFile(pDatFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		return E_FAIL;
	}

	DWORD	dwByte = 0;
	_matrix matPivot;
	ReadFile(hFile, &matPivot, sizeof(_matrix), &dwByte, nullptr);
	XMStoreFloat4x4(&m_PivotMatrix, matPivot);
	ReadFile(hFile, &m_IPGeneral, sizeof(IPGeneral), &dwByte, nullptr);
	if (m_IPGeneral.iNumAnimations == 0)
		m_bHaveAnim = false;
	else
		m_bHaveAnim = true;

	//m_IPGeneral;

	//m_IPVertex.pVertices = new VTXMESH[m_IPGeneral.iNumVertices];
	//ZeroMemory(m_IPVertex.pVertices, sizeof(VTXMESH) * m_IPGeneral.iNumVertices);

	//m_IPVertex.pIndices = new FACEINDICES32[m_IPGeneral.iNumIndices];
	//ZeroMemory(m_IPVertex.pIndices, sizeof(FACEINDICES32) * m_IPGeneral.iNumIndices);

	//ReadFile(hFile, m_IPVertex.pVertices, sizeof(VTXMESH) * m_IPGeneral.iNumVertices, &dwByte, nullptr);
	//ReadFile(hFile, m_IPVertex.pIndices, sizeof(FACEINDICES32) * m_IPGeneral.iNumIndices, &dwByte, nullptr);

	m_pVertices = new VTXMESH[m_IPGeneral.iNumVertices];

	ZeroMemory(m_pVertices, sizeof(VTXMESH) * m_IPGeneral.iNumVertices);

	m_pIndices = new FACEINDICES32[m_IPGeneral.iNumIndices];

	ZeroMemory(m_pIndices, sizeof(FACEINDICES32) * m_IPGeneral.iNumIndices);

	ReadFile(hFile, m_pVertices, sizeof(VTXMESH) * m_IPGeneral.iNumVertices, &dwByte, nullptr);
	ReadFile(hFile, m_pIndices, sizeof(FACEINDICES32) * m_IPGeneral.iNumIndices, &dwByte, nullptr);


	//m_IPVertex;

	for (_uint i = 0; i < m_IPGeneral.iNumMeshes; ++i)
	{
		IPMeshContainer*	pIPMC = new IPMeshContainer;
		ZeroMemory(&pIPMC->iNumVertices, sizeof(_uint));
		ZeroMemory(&pIPMC->iStartVertexIndex, sizeof(_uint));
		ZeroMemory(&pIPMC->iNumFaces, sizeof(_uint));
		ZeroMemory(&pIPMC->iStartFaceIndex, sizeof(_uint));
		ZeroMemory(&pIPMC->iMaterialIndex, sizeof(_uint));
		ZeroMemory(&pIPMC->iNumBones, sizeof(_uint));

		ReadFile(hFile, &pIPMC->iNumVertices, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &pIPMC->iStartFaceIndex, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &pIPMC->iNumFaces, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &pIPMC->iStartVertexIndex, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &pIPMC->iMaterialIndex, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &pIPMC->iNumBones, sizeof(_uint), &dwByte, nullptr);

		for (_uint j = 0; j < pIPMC->iNumBones; ++j)
		{
			DWORD	nameLen = 0;

			ReadFile(hFile, &nameLen, sizeof(DWORD), &dwByte, nullptr);

			char*	pBoneName = new char[nameLen + 1];

			ZeroMemory(pBoneName, sizeof(char) * nameLen + 1);

			ReadFile(hFile, pBoneName, sizeof(char) * nameLen, &dwByte, nullptr);

			pIPMC->vecBoneName.push_back(pBoneName);
		}
		m_vecIPMeshContainer.push_back(pIPMC);
	}

	//m_vecIPMeshContainer;

	for (_uint i = 0; i < m_IPGeneral.iNumMaterials; ++i)
	{
		IPMaterial*	pIPM = new IPMaterial;

		DWORD iCount = 0;

		ReadFile(hFile, &iCount, sizeof(DWORD), &dwByte, nullptr);

		for (DWORD j = 0; j < iCount; ++j)
		{
			IPMaterial::TEXDESC	pTexDesc;

			ReadFile(hFile, &pTexDesc, sizeof(IPMaterial::TEXDESC), &dwByte, nullptr);

 			pIPM->vecTexDesc.push_back(pTexDesc);
		}
		m_vecIPMaterial.push_back(pIPM);
	}

	//m_vecIPMaterial;

	for (_uint i = 0; i < m_IPGeneral.iNumHierachyNodes; ++i)
	{
		DWORD		nameLen = 0;
		IPHNode*	pIPHNode = new IPHNode;
		ZeroMemory(pIPHNode, sizeof(IPHNode));

		ReadFile(hFile, &nameLen, sizeof(DWORD), &dwByte, nullptr);
		ReadFile(hFile, pIPHNode->szParentName, sizeof(char) * nameLen, &dwByte, nullptr);

		ReadFile(hFile, &nameLen, sizeof(DWORD), &dwByte, nullptr);
		ReadFile(hFile, pIPHNode->szName, sizeof(char) * nameLen, &dwByte, nullptr);

		ReadFile(hFile, &pIPHNode->TransformationMatrix, sizeof(_matrix), &dwByte, nullptr);
		ReadFile(hFile, &pIPHNode->OffsetMatrix, sizeof(_matrix), &dwByte, nullptr);
		ReadFile(hFile, &pIPHNode->iDepth, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &pIPHNode->iNumChildren, sizeof(_uint), &dwByte, nullptr);

		m_vecIPHNode.push_back(pIPHNode);
	}

	//m_vecIPHNode;

	for (_uint i = 0; i < m_IPGeneral.iNumAnimations; ++i)
	{
		IPAnim*		pIPAnim = new IPAnim;
		ZeroMemory(pIPAnim, sizeof(IPAnim));

		ReadFile(hFile, &pIPAnim->iDuration, sizeof(_double), &dwByte, nullptr);
		ReadFile(hFile, &pIPAnim->iTicksPerSecond, sizeof(_double), &dwByte, nullptr);
		ReadFile(hFile, &pIPAnim->iNumChannels, sizeof(_uint), &dwByte, nullptr);

		DWORD		nameLen = 0;

		ReadFile(hFile, &nameLen, sizeof(DWORD), &dwByte, nullptr);
		ReadFile(hFile, pIPAnim->szName, sizeof(char) * nameLen, &dwByte, nullptr);

		m_vecIPAnim.push_back(pIPAnim);
	}

	//m_vecIPAnim;

	_uint iNumChannel = 0;
	ReadFile(hFile, &iNumChannel, sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < iNumChannel; ++i)
	{
		DWORD		nameLen = 0;
		IPChannel*	pIPChan = new IPChannel;
		ZeroMemory(pIPChan->szTargetName, sizeof(char) * MAX_PATH);
		ZeroMemory(pIPChan->szName, sizeof(char) * MAX_PATH);

		ReadFile(hFile, &nameLen, sizeof(DWORD), &dwByte, nullptr);
		ReadFile(hFile, pIPChan->szName, sizeof(char) * nameLen, &dwByte, nullptr);

		ReadFile(hFile, &nameLen, sizeof(DWORD), &dwByte, nullptr);
		ReadFile(hFile, pIPChan->szTargetName, sizeof(char) * nameLen, &dwByte, nullptr);

		DWORD	iNumKeyFrame = 0;
		ReadFile(hFile, &iNumKeyFrame, sizeof(DWORD), &dwByte, nullptr);

		for (DWORD j = 0; j < iNumKeyFrame; ++j)
		{
			KEYFRAME*	pKeyFrame = new KEYFRAME;

			ZeroMemory(pKeyFrame, sizeof(KEYFRAME));

			ReadFile(hFile, pKeyFrame, sizeof(KEYFRAME), &dwByte, nullptr);

			pIPChan->vecKeyFrames.push_back(pKeyFrame);
		}
		m_vecIPChannel.push_back(pIPChan);
	}

	//m_vecIPChannel;

	// For Debug
	//m_IPGeneral;
	//m_IPVertex;
	//m_vecIPMeshContainer;
	//m_vecIPMaterial;
	//m_vecIPHNode;
	//m_vecIPAnim;
	//m_vecIPChannel;

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CModel::Free_Param()
{
	if (false == m_bCloned)
	{
		//Safe_Delete_Array(m_IPVertex.pVertices);
		//Safe_Delete_Array(m_IPVertex.pIndices);

		for (auto& pIPMC : m_vecIPMeshContainer)
		{
			for (auto& pName : pIPMC->vecBoneName)
			{
				Safe_Delete_Array(pName);
			}
			Safe_Delete(pIPMC);
		}
		m_vecIPMeshContainer.clear();

		for (auto& pIPM : m_vecIPMaterial)
		{
			pIPM->vecTexDesc.clear();
			Safe_Delete(pIPM);
		}
		m_vecIPMaterial.clear();

		for (auto& pIPH : m_vecIPHNode)
		{
			Safe_Delete(pIPH);
		}
		m_vecIPHNode.clear();

		for (auto& pIPA : m_vecIPAnim)
		{
			Safe_Delete(pIPA);
		}
		m_vecIPAnim.clear();

		for (auto& pIPC : m_vecIPChannel)
		{
			for (auto& pKeyFrame : pIPC->vecKeyFrames)
			{
				Safe_Delete(pKeyFrame);
			}
			Safe_Delete(pIPC);
		}
		m_vecIPChannel.clear();
	}
	return S_OK;
}

HRESULT CModel::Create_MeshContainer()
{
	for (_uint i = 0; i < m_IPGeneral.iNumMeshes; ++i)
	{
		IPMeshContainer*	pImportMC = m_vecIPMeshContainer[i];

		CMeshContainer*		pMeshContainer = CMeshContainer::Create(m_pGraphicDevice, m_pContextDevice, pImportMC->iStartVertexIndex, pImportMC->iNumVertices, pImportMC->iStartFaceIndex, pImportMC->iNumFaces, pImportMC->iMaterialIndex);

		if (nullptr == pMeshContainer)
		{
			return E_FAIL;
		}
		m_MeshContainers.push_back(pMeshContainer);
	}
	return S_OK;
}

HRESULT CModel::Compile_Shader(const _tchar * pShaderFilePath, const char * pTechniqueName)
{
	D3D11_INPUT_ELEMENT_DESC		ElementDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDINDEX", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 60, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	if (FAILED(CVIBuffer::Compile_Shader(ElementDesc, 6, pShaderFilePath, pTechniqueName)))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::Create_Materials(_bool bMFC)
{
	for (_uint i = 0; i < m_IPGeneral.iNumMaterials; ++i)
	{
		IPMaterial*		pImportMat = m_vecIPMaterial[i];

		MESHMATERIAL*	pMeshMaterial = new MESHMATERIAL;
		ZeroMemory(pMeshMaterial, sizeof(MESHMATERIAL));

		_uint iTexCount = (_uint)pImportMat->vecTexDesc.size();

		for (_uint j = 0; j < iTexCount; ++j)
		{
			_tchar		szMaterialPath[MAX_PATH] = TEXT("");

			MultiByteToWideChar(CP_ACP, 0
				, pImportMat->vecTexDesc[j].szMaterialTexturePath
				, (_int)strlen(pImportMat->vecTexDesc[j].szMaterialTexturePath)
				, szMaterialPath, 256);

			if (bMFC == false)
			{
				wstring wstr(szMaterialPath);
				wstring::size_type wstrRemover;
				wstrRemover = wstr.find(L"Client\\", 0);
				wstr.erase(wstrRemover, 7);
				lstrcpy(szMaterialPath, wstr.c_str());
			}

			CTexture*		pTexture = CTexture::Create(m_pGraphicDevice, m_pContextDevice, szMaterialPath);

			if (nullptr == pTexture)
			{
				continue;
			}

			pMeshMaterial->pMaterialTextures[pImportMat->vecTexDesc[j].iTextureType] = pTexture;
		}
		m_Materials.push_back(pMeshMaterial);
	}
	return S_OK;
}

HRESULT CModel::Create_VIBuffer()
{
	m_iNumVertices = m_IPGeneral.iNumVertices;
	m_iNumPrimitive = m_IPGeneral.iNumIndices;

	// Copy Vertices & Indices 

	//m_pVertices = new VTXMESH[m_iNumVertices];

	//ZeroMemory(m_pVertices, sizeof(VTXMESH) * m_iNumVertices);

	//memcpy(m_pVertices, m_IPVertex.pVertices, sizeof(VTXMESH) * m_iNumVertices);

	//m_pIndices = new FACEINDICES32[m_iNumPrimitive];

	//ZeroMemory(m_pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);

	//memcpy(m_pIndices, m_IPVertex.pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);

	// Setup VIBuffer

	m_iStride = sizeof(VTXMESH);

	ZeroMemory(&m_tVB_Desc, sizeof(D3D11_BUFFER_DESC));

	m_tVB_Desc.ByteWidth = m_iStride * m_iNumVertices;
	m_tVB_Desc.Usage = D3D11_USAGE_IMMUTABLE;
	m_tVB_Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_tVB_Desc.CPUAccessFlags = 0;
	m_tVB_Desc.MiscFlags = 0;
	m_tVB_Desc.StructureByteStride = m_iStride;

	ZeroMemory(&m_tVB_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_tVB_SubResourceData.pSysMem = m_pVertices;

	if (FAILED(CVIBuffer::Create_VertexBuffer()))
		return E_FAIL;

	m_iNumIndicesPerPT = 3; /* 도형하나그릴때 필요한 인덱스 갯수. */
	m_iFaceIndexSize = sizeof(FACEINDICES32);

	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eRenderType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	ZeroMemory(&m_tIB_Desc, sizeof(D3D11_BUFFER_DESC));

	m_tIB_Desc.ByteWidth = m_iFaceIndexSize * m_iNumPrimitive;
	m_tIB_Desc.Usage = D3D11_USAGE_IMMUTABLE;
	m_tIB_Desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_tIB_Desc.CPUAccessFlags = 0;
	m_tIB_Desc.MiscFlags = 0;

	ZeroMemory(&m_tIB_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_tIB_SubResourceData.pSysMem = m_pIndices;

	if (FAILED(CVIBuffer::Create_IndexBuffer()))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::Create_DynamicVIBuffer()
{
	m_iNumVertices = m_IPGeneral.iNumVertices;
	m_iNumPrimitive = m_IPGeneral.iNumIndices;

	// Copy Vertices & Indices 

	//m_pVertices = new VTXMESH[m_iNumVertices];

	//ZeroMemory(m_pVertices, sizeof(VTXMESH) * m_iNumVertices);

	//memcpy(m_pVertices, m_IPVertex.pVertices, sizeof(VTXMESH) * m_iNumVertices);

	//m_pIndices = new FACEINDICES32[m_iNumPrimitive];

	//ZeroMemory(m_pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);

	//memcpy(m_pIndices, m_IPVertex.pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);

	// Setup VIBuffer

	m_iStride = sizeof(VTXMESH);

	ZeroMemory(&m_tVB_Desc, sizeof(D3D11_BUFFER_DESC));

	m_tVB_Desc.ByteWidth = m_iStride * m_iNumVertices;
	m_tVB_Desc.Usage = D3D11_USAGE_IMMUTABLE;
	m_tVB_Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_tVB_Desc.CPUAccessFlags = 0;
	m_tVB_Desc.MiscFlags = 0;
	m_tVB_Desc.StructureByteStride = m_iStride;

	ZeroMemory(&m_tVB_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_tVB_SubResourceData.pSysMem = m_pVertices;

	if (FAILED(CVIBuffer::Create_VertexBuffer()))
		return E_FAIL;

	m_iNumIndicesPerPT = 3; /* 도형하나그릴때 필요한 인덱스 갯수. */
	m_iFaceIndexSize = sizeof(FACEINDICES32);

	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eRenderType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	ZeroMemory(&m_tIB_Desc, sizeof(D3D11_BUFFER_DESC));

	m_tIB_Desc.ByteWidth = m_iFaceIndexSize * m_iNumPrimitive;
	m_tIB_Desc.Usage = D3D11_USAGE_DYNAMIC;
	m_tIB_Desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_tIB_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_tIB_Desc.MiscFlags = 0;

	ZeroMemory(&m_tIB_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_tIB_SubResourceData.pSysMem = m_pIndices;

	if (FAILED(CVIBuffer::Create_IndexBuffer()))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::Create_HierarchyNodes()
{
	// Import HNode from .dat
	for (_uint i = 0; i < m_IPGeneral.iNumHierachyNodes; ++i)
	{
		IPHNode*	pImportNode = m_vecIPHNode[i];

		CHierarchyNode*		pNode = CHierarchyNode::Create_ByDat(
			pImportNode->szName,
			pImportNode->TransformationMatrix,
			pImportNode->OffsetMatrix,
			pImportNode->szParentName,
			pImportNode->iDepth
		);

		if (nullptr == pNode)
		{
			return E_FAIL;
		}

		pNode->Reserve_Channels(m_IPGeneral.iNumAnimations);

		m_HierarchyNodes.push_back(pNode);
	}

	// Set HNode Parent
	for (_uint i = 0; i < m_HierarchyNodes.size(); ++i)
	{
		const char*	pTargetNodeName = m_HierarchyNodes[i]->Get_TargetName();

		for (_uint j = 0; j < m_HierarchyNodes.size(); ++j)
		{
			const char* pNodeName = m_HierarchyNodes[j]->Get_Name();

			if (!strcmp(pNodeName, pTargetNodeName))
			{
				m_HierarchyNodes[i]->Set_Parent(m_HierarchyNodes[j]);
			}
		}
	}

	// Sorting HNode Depth
	sort(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [](CHierarchyNode* pSour, CHierarchyNode* pDest)
	{
		return pSour->Get_Depth() < pDest->Get_Depth();
	});

	return S_OK;
}

HRESULT CModel::SetUp_HierarchyNodeToMeshContainer()
{
	for (_uint i = 0; i < m_IPGeneral.iNumMeshes; ++i)
	{
		IPMeshContainer*	pImportMeshCon = m_vecIPMeshContainer[i];

		CMeshContainer*	pMeshContainer = m_MeshContainers[i];

		for (_uint j = 0; j < pImportMeshCon->iNumBones; ++j)
		{
			CHierarchyNode* pHierachyNode = Find_HierarchyNode(pImportMeshCon->vecBoneName[j]);

			if (nullptr == pHierachyNode)
				E_FAIL;

			// _matrix	OffsetMatrix;
			// memcpy(&OffsetMatrix, &pHierachyNode->Get_OffsetMatrix(), sizeof(_matrix));
			// pHierachyNode->Set_OffsetMatrix(XMMatrixTranspose(OffsetMatrix));
			// Export 시에 .dat 파일 생성하면서 오프셋은 이미 설정해줌

			pMeshContainer->Add_HierarchyNode(pHierachyNode);

			Safe_AddRef(pHierachyNode);
		}
	}
	return S_OK;
}

HRESULT CModel::Create_Animation()
{
	_uint iTotalChannelIndex = 0;

	for (_uint i = 0; i < m_IPGeneral.iNumAnimations; ++i)
	{
		IPAnim*	pImportAnim = m_vecIPAnim[i];

		CAnimation*	pAnim = CAnimation::Create(pImportAnim->iDuration, pImportAnim->iTicksPerSecond);

		if (nullptr == pAnim)
			return E_FAIL;

		pAnim->Set_AnimationName(pImportAnim->szName);

		for (_uint j = 0; j < pImportAnim->iNumChannels; ++j)
		{
			IPChannel*	pImportChannel = m_vecIPChannel[iTotalChannelIndex];

			CChannel*	pChannel = CChannel::Create(pImportChannel->szName);

			if (nullptr == pChannel)
				return E_FAIL;

			CHierarchyNode*	pHNode = Find_HierarchyNode(pImportChannel->szName);

			if (nullptr == pHNode)
				return E_FAIL;

			_uint iKeyFrameCount = (_uint)pImportChannel->vecKeyFrames.size();

			for (_uint k = 0; k < iKeyFrameCount; ++k)
			{
				KEYFRAME* pKeyFrame = new KEYFRAME;

				memcpy(pKeyFrame, pImportChannel->vecKeyFrames[k], sizeof(KEYFRAME));

				pChannel->Add_KeyFrame(pKeyFrame);
			}
			pAnim->Add_Channel(pChannel);
			pHNode->Add_Channel(i, pChannel);

			iTotalChannelIndex++;
		}
		m_Animations.push_back(pAnim);
	}
	return S_OK;
}

HRESULT CModel::Create_Animation_FromPath()
{
	HANDLE hFile = CreateFile(m_pAnimationPath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		_tchar szPath[MAX_PATH];
		ZeroMemory(szPath, lstrlen(szPath) * sizeof(_tchar));

		lstrcpy(szPath, m_pAnimationPath);
		lstrcat(szPath, L" 경로에 있는 파일을 읽어들일 수 없습니다.");

		MessageBoxW(nullptr, szPath, L"ERROR", MB_OK);
		return E_FAIL;
	}

	DWORD dwByte = 0;
	_uint iCount = 0;

	_tchar pszTag[MAX_PATH];
	ZeroMemory(pszTag, MAX_PATH * sizeof(_tchar));

	ReadFile(hFile, pszTag, MAX_PATH * sizeof(_tchar), &dwByte, nullptr);
	ReadFile(hFile, &iCount, sizeof(_uint), &dwByte, nullptr);

	_uint iTotalChannelIndex = 0;

	for (_uint i = 0; i < m_IPGeneral.iNumAnimations; ++i)
	{
		char   szTag[MAX_PATH];
		ZeroMemory(szTag, MAX_PATH * sizeof(char));

		_double PlaySpeedPerSec = 0.0;

		_double LinkMinRatio = 0.0;
		_double LinkMaxRatio = 0.0;
		_double BlendMaxTime = 0.0;
		_uint	iBlendLevel = 0;
		_bool	bNoneUpdatePos = false;
		DIRECTION   eDirection = DIRECTION::FORWARD;
		_float4 vBeginRootBonePos = _float4(0.f, 0.f, 0.f, 1.f);
		_double RootMotionValue = 0.0;
		_double CollisionMinRatio = 0.0;
		_double CollisionMaxRatio = 0.0;

		ReadFile(hFile, szTag, MAX_PATH * sizeof(char), &dwByte, nullptr);
		ReadFile(hFile, &PlaySpeedPerSec, sizeof(_double), &dwByte, nullptr);
		ReadFile(hFile, &LinkMinRatio, sizeof(_double), &dwByte, nullptr);
		ReadFile(hFile, &LinkMaxRatio, sizeof(_double), &dwByte, nullptr);
		ReadFile(hFile, &BlendMaxTime, sizeof(_double), &dwByte, nullptr);
		ReadFile(hFile, &iBlendLevel, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &bNoneUpdatePos, sizeof(_bool), &dwByte, nullptr);
		ReadFile(hFile, &eDirection, sizeof(DIRECTION), &dwByte, nullptr);
		ReadFile(hFile, &vBeginRootBonePos, sizeof(_float4), &dwByte, nullptr);
		ReadFile(hFile, &RootMotionValue, sizeof(_double), &dwByte, nullptr);
		ReadFile(hFile, &CollisionMinRatio, sizeof(_double), &dwByte, nullptr);
		ReadFile(hFile, &CollisionMaxRatio, sizeof(_double), &dwByte, nullptr);

		IPAnim*	pImportAnim = m_vecIPAnim[i];

		CAnimation*	pAnim = CAnimation::Create(pImportAnim->iDuration, pImportAnim->iTicksPerSecond);

		if (nullptr == pAnim)
			return E_FAIL;

#ifdef _DEBUG
		if (!strcmp(szTag, pImportAnim->szName))
		{
			pAnim->Set_PlaySpeedPerSec(PlaySpeedPerSec);
			pAnim->Set_LinkMinRatio(LinkMinRatio);
			pAnim->Set_LinkMaxRatio(LinkMaxRatio);
			pAnim->Set_BlendMaxTime(BlendMaxTime);
			pAnim->Set_BlendLevel(iBlendLevel);
			pAnim->Set_NonUpdatePos(bNoneUpdatePos);
			pAnim->Set_Direction(eDirection);
			pAnim->Set_BeginRootBone_Position(vBeginRootBonePos);
			pAnim->Set_RootMotionValue(RootMotionValue);
			pAnim->Set_CollisionMinRatio(CollisionMinRatio);
			pAnim->Set_CollisionMaxRatio(CollisionMaxRatio);
		}
		else
		{
			_tchar szPath[MAX_PATH];
			ZeroMemory(szPath, lstrlen(szPath) * sizeof(_tchar));

			lstrcpy(szPath, m_pAnimationPath);
			lstrcat(szPath, L" 경로에 있는 파일의 정보와 현재 모델의 애니메이션의 정보가 다릅니다.");

			MessageBoxW(nullptr, szPath, L"ERROR", MB_OK);
			return E_FAIL;
		}
#else
		pAnim->Set_PlaySpeedPerSec(PlaySpeedPerSec);
		pAnim->Set_LinkMinRatio(LinkMinRatio);
		pAnim->Set_LinkMaxRatio(LinkMaxRatio);
		pAnim->Set_BlendMaxTime(BlendMaxTime);
		pAnim->Set_BlendLevel(iBlendLevel);
		pAnim->Set_NonUpdatePos(bNoneUpdatePos);
		pAnim->Set_Direction(eDirection);
		pAnim->Set_BeginRootBone_Position(vBeginRootBonePos);
		pAnim->Set_RootMotionValue(RootMotionValue);
		pAnim->Set_CollisionMinRatio(CollisionMinRatio);
		pAnim->Set_CollisionMaxRatio(CollisionMaxRatio);
#endif
		pAnim->Set_AnimationName(pImportAnim->szName);

		for (_uint j = 0; j < pImportAnim->iNumChannels; ++j)
		{
			IPChannel*	pImportChannel = m_vecIPChannel[iTotalChannelIndex];

			CChannel*	pChannel = CChannel::Create(pImportChannel->szName);

			if (nullptr == pChannel)
				return E_FAIL;

			CHierarchyNode*	pHNode = Find_HierarchyNode(pImportChannel->szName);

			if (nullptr == pHNode)
				return E_FAIL;

			_uint iKeyFrameCount = (_uint)pImportChannel->vecKeyFrames.size();

			for (_uint k = 0; k < iKeyFrameCount; ++k)
			{
				KEYFRAME* pKeyFrame = new KEYFRAME;

				memcpy(pKeyFrame, pImportChannel->vecKeyFrames[k], sizeof(KEYFRAME));

				pChannel->Add_KeyFrame(pKeyFrame);
			}
			pAnim->Add_Channel(pChannel);
			pHNode->Add_Channel(i, pChannel);

			iTotalChannelIndex++;
		}
		m_Animations.push_back(pAnim);
	}

	CloseHandle(hFile);

	return S_OK;
}

CAnimation * CModel::Find_Animation(const char * pAnimName)
{
	auto	iter = find_if(m_Animations.begin(), m_Animations.end(), [&](CAnimation* pNode)
	{
		return !strcmp(pNode->Get_AnimationName(), pAnimName);
	});

	if (iter == m_Animations.end())
	{
		return nullptr;
	}
	return *iter;
}

CHierarchyNode * CModel::Find_HierarchyNode(const char * pBoneName)
{
	auto	iter = find_if(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [&](CHierarchyNode* pNode)
	{
		return !strcmp(pNode->Get_Name(), pBoneName);
	});

	if (iter == m_HierarchyNodes.end())
	{
		return nullptr;
	}
	return *iter;
}

HRESULT CModel::Bind_Buffers()
{
	_uint		iOffset = 0;

	m_pContextDevice->IASetVertexBuffers(0, 1, &m_pVB, &m_iStride, &iOffset);
	m_pContextDevice->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pContextDevice->IASetPrimitiveTopology(m_eRenderType);

	return S_OK;
}

HRESULT CModel::Set_Animation(_uint iAnimIndex, _bool bContinue)
{
	if (iAnimIndex >= m_Animations.size())
		return E_FAIL;

	m_bContinue = bContinue;

	if (!m_bAnimationBlend)
		m_iNextAnimationIndex = iAnimIndex;

	m_bBlendFinishMoment = false;								// Initialize each time to check the moment the blending is finished
	
	if (m_iCurrentAnimationIndex != m_iNextAnimationIndex)
	{
		if (m_bNoneBlend)
		{
			m_vPreRootMotion = m_Animations[m_iNextAnimationIndex]->Get_BeginRootBone_Position_NonFind();

			//
			m_Animations[m_iNextAnimationIndex]->Initialize_Time();
			m_Animations[m_iNextAnimationIndex]->Set_OriginUpdate(false);
			m_Animations[m_iNextAnimationIndex]->Set_RootMotionPos();

			m_Animations[m_iCurrentAnimationIndex]->Initialize_Time();
			m_Animations[m_iCurrentAnimationIndex]->Set_OriginUpdate(false);
			m_Animations[m_iCurrentAnimationIndex]->Set_RootMotionPos();
			//

			m_iCurrentAnimationIndex = m_iNextAnimationIndex;
			m_bAnimationBlend = false;

			m_bNoneBlend = false;

			return S_OK;
		}

		if (!m_bAnimationBlend)
		{
			m_Animations[m_iCurrentAnimationIndex]->Set_Blend(true);
			m_Animations[m_iCurrentAnimationIndex]->Set_NextAnimChannel(m_Animations[m_iNextAnimationIndex]->Get_Channels());
			m_Animations[m_iCurrentAnimationIndex]->Set_NextBlendLevel(m_Animations[m_iNextAnimationIndex]->Get_BlendLevel());

			m_Animations[m_iNextAnimationIndex]->Set_OriginUpdate(false);
			m_Animations[m_iNextAnimationIndex]->Initialize_Time();
			m_Animations[m_iNextAnimationIndex]->Initialize_Blend();
			m_Animations[m_iNextAnimationIndex]->Set_RootMotionPos();

			m_bAnimationBlend = true;
		}

		if (m_Animations[m_iNextAnimationIndex]->Get_BlendMaxTime() <= m_Animations[m_iCurrentAnimationIndex]->Get_BlendTime())
		{
			m_Animations[m_iCurrentAnimationIndex]->Initialize_Blend();

			m_Animations[m_iCurrentAnimationIndex]->Initialize_Time();
			m_Animations[m_iCurrentAnimationIndex]->Set_OriginUpdate(false);
			m_Animations[m_iCurrentAnimationIndex]->Set_RootMotionPos();

			m_Animations[m_iNextAnimationIndex]->Initialize_Time();
			m_Animations[m_iNextAnimationIndex]->Set_OriginUpdate(false);
			m_Animations[m_iNextAnimationIndex]->Set_RootMotionPos();

			m_vPreRootMotion = m_Animations[m_iNextAnimationIndex]->Get_BeginRootBone_Position();

			m_iCurrentAnimationIndex = m_iNextAnimationIndex;

			m_bAnimationBlend = false;
			
			m_bBlendFinishMoment = true;					 // The moment the blending is over
		}
	}

	return S_OK;
}

HRESULT CModel::Set_MaterialTexture(_uint iMeshContainerIndex, aiTextureType eTextureType, char * pContantName)
{
	_uint iMaterialIndex = m_MeshContainers[iMeshContainerIndex]->Get_MaterialIndex();

	CTexture*		pTexture = m_Materials[iMaterialIndex]->pMaterialTextures[eTextureType];

	if (nullptr == pTexture)
	{
		return E_FAIL;
	}
	return SetUp_Texture(pContantName, pTexture->Get_SRV(0));
}

void CModel::Update_CombinedTransformationMatrix(_double TimeDelta)
{
	m_Animations[m_iCurrentAnimationIndex]->Update_TransformationMatrix(TimeDelta, m_bContinue);

	/* 뼈의 상속순서대로 행렬을 누적시키낟. */
	for (auto& pHierarchyNode : m_HierarchyNodes)
		pHierarchyNode->Update_CombinedTransformationMatrix(m_iCurrentAnimationIndex);
}

HRESULT CModel::Render(_uint iMeshIndex, _uint iPassIndex)
{
	if (iPassIndex >= m_vecPassDesc.size())
	{
		return E_FAIL;
	}
	m_pContextDevice->IASetInputLayout(m_vecPassDesc[iPassIndex]->pInputLayout);

	if (false == m_Animations.empty())
	{
		_matrix		BoneMatrices[128];
		ZeroMemory(BoneMatrices, sizeof(_matrix) * 128);

		m_MeshContainers[iMeshIndex]->SetUp_CombinedMatrices(BoneMatrices, XMLoadFloat4x4(&m_PivotMatrix));
		if (FAILED(SetUp_RawValue("g_tBoneMatrices", BoneMatrices, sizeof(_float4x4) * 128)))
		{
			return E_FAIL;
		}

		if (m_bIsSaveBoneMatrices)
		{
			memcpy(&m_tBoneMatrices, BoneMatrices, sizeof(_float4x4) * 128);
		}

	}
	m_vecPassDesc[iPassIndex]->pEffectPass->Apply(0, m_pContextDevice);

	m_MeshContainers[iMeshIndex]->Render();

	return S_OK;
}

HRESULT CModel::Render_WithPreAnimation(_uint iMeshIndex, _uint iPassIndex)
{
	if (iPassIndex >= m_vecPassDesc.size())
	{
		return E_FAIL;
	}
	m_pContextDevice->IASetInputLayout(m_vecPassDesc[iPassIndex]->pInputLayout);

	if (false == m_Animations.empty())
	{
		_matrix		BoneMatrices[128];
		_matrix		PreBoneMatrices[128];
		ZeroMemory(BoneMatrices, sizeof(_matrix) * 128);
		ZeroMemory(PreBoneMatrices, sizeof(_matrix) * 128);

		m_MeshContainers[iMeshIndex]->SetUp_CombineBlurMatrices(BoneMatrices, PreBoneMatrices, XMLoadFloat4x4(&m_PivotMatrix));

		if (FAILED(SetUp_RawValue("g_tBoneMatrices", BoneMatrices, sizeof(_float4x4) * 128)))
		{
			return E_FAIL;
		}
		if (FAILED(SetUp_RawValue("g_tPreBoneMatrices", PreBoneMatrices, sizeof(_float4x4) * 128)))
		{
			return E_FAIL;
		}
	}
	m_vecPassDesc[iPassIndex]->pEffectPass->Apply(0, m_pContextDevice);

	m_MeshContainers[iMeshIndex]->Render();

	return S_OK;
}

void CModel::Set_OffsetMatrix(const _tchar * pTag, _matrix OffSetMatrix)
{
	auto& iter = find_if(m_RefNodes.begin(), m_RefNodes.end(), CTagFinder(pTag));
	if (iter == m_RefNodes.end())
		return;

	iter->second->Set_OffsetMatrix(OffSetMatrix);
}

_matrix CModel::Get_OffsetMatrix(const _tchar* pTag)
{
	auto& iter = find_if(m_RefNodes.begin(), m_RefNodes.end(), CTagFinder(pTag));
	if (iter == m_RefNodes.end())
		return XMMatrixIdentity();

	return iter->second->Get_OffsetMatrix();
}

HRESULT CModel::Set_Animation_Tool(_uint iAnimIndex)
{
	if (iAnimIndex >= m_Animations.size())
		return E_FAIL;

	m_iCurrentAnimationIndex = iAnimIndex;

	return S_OK;
}

void CModel::Update_CombinedTransformationMatrixNonContinue_Tool(_double TimeDelta)
{
	m_Animations[m_iCurrentAnimationIndex]->Update_CombinedTransformationMatrixNonContinue_Tool(TimeDelta);

	for (auto& pHierarchyNode : m_HierarchyNodes)
		pHierarchyNode->Update_CombinedTransformationMatrix(m_iCurrentAnimationIndex);
}

_bool CModel::Picking(_float3 * pOut, _matrix matWorld, _float & fDist)
{
	CPicking* pPicking = CPicking::Get_Instance();
	struct tagPointDesc {
		_float3 vPos;
		_float	fDistance;
	};
	list<tagPointDesc> PointList;

	_float3 vClickedPosition;
	_float	vClickedDistance;

	_uint iIndex = 0;
	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		_float3	iIndices[] =
		{
			((VTXMESH*)m_pVertices)[i].vPosition,
			((VTXMESH*)m_pVertices)[++i].vPosition,
			((VTXMESH*)m_pVertices)[++i].vPosition
		};

		for (_uint j = 0; j<3; ++j)
			XMStoreFloat3(&iIndices[j], XMVector3TransformCoord(XMLoadFloat3(&iIndices[j]), matWorld));

		if (pPicking->Picking(XMLoadFloat3(&iIndices[0]), XMLoadFloat3(&iIndices[1]), XMLoadFloat3(&iIndices[2]), vClickedDistance))
		{

			_vector vPivot = pPicking->Get_Pivot();
			_vector vRay = pPicking->Get_Ray();
			XMStoreFloat3(&vClickedPosition, vPivot + vRay * vClickedDistance);

			tagPointDesc tPointDesc;
			tPointDesc.fDistance = vClickedDistance;
			tPointDesc.vPos = vClickedPosition;

			PointList.push_back(tPointDesc);
		}
	}
	if (PointList.size() != 0)
	{
		PointList.sort([](tagPointDesc t1, tagPointDesc t2) {
			return t1.fDistance < t2.fDistance;
		});

		*pOut = PointList.front().vPos;
		fDist = PointList.front().fDistance;
		return true;

	}
	return false;
}

_int CModel::Find_AnimationIndex(const char * pTag)
{
	if (0 == m_Animations.size())
		return -1;

	_uint iIndex = 0;
	for (auto& pAnim : m_Animations)
	{
		if (!strcmp(pTag, pAnim->Get_AnimationName()))
			return iIndex;

		++iIndex;
	}

	return -1;
}

const _float4 CModel::Get_RootMotion() const
{
	_float4 vPos = m_Animations[m_iCurrentAnimationIndex]->Get_RootMotionPos();

	_float4 vResult = _float4(0.f, 0.f, 0.f, 1.f);
	XMStoreFloat4(&vResult, XMVector3TransformCoord(XMLoadFloat4(&vPos), XMLoadFloat4x4(&m_PivotMatrix)));

	return vResult;
}

_matrix CModel::Get_BoneMatrix(const _tchar * pNodeTag)
{
	auto	iter = find_if(m_RefNodes.begin(), m_RefNodes.end(), CTagFinder(pNodeTag));
	if (iter == m_RefNodes.end())
		return XMMatrixIdentity();

	_matrix		OffsetMatrix = iter->second->Get_OffsetMatrix();
	_matrix		CombinedMatrix = iter->second->Get_CombinedTransformationMatrix();

	return OffsetMatrix * CombinedMatrix * XMLoadFloat4x4(&m_PivotMatrix);
}

_matrix CModel::Get_BoneMatrixNonPivot(const _tchar * pNodeTag)
{
	auto	iter = find_if(m_RefNodes.begin(), m_RefNodes.end(), CTagFinder(pNodeTag));
	if (iter == m_RefNodes.end())
		return XMMatrixIdentity();

	_matrix		OffsetMatrix = iter->second->Get_OffsetMatrix();
	_matrix		CombinedMatrix = iter->second->Get_CombinedTransformationMatrix();

	return OffsetMatrix * CombinedMatrix;
}

const _float4 CModel::Get_BeginRootBone_Position() const
{
	return m_Animations[m_iCurrentAnimationIndex]->Get_BeginRootBone_Position();
}

const _double CModel::Get_BlendMaxTime_WithIndex(_uint iIndex)
{
	if (iIndex >= m_Animations.size())
		return 0.0;

	return m_Animations[iIndex]->Get_BlendMaxTime();
}

const _double CModel::Get_RootMotionValue() const
{
	return m_Animations[m_iCurrentAnimationIndex]->Get_RootMotionValue();
}

const _double CModel::Get_RootMotionValue(_uint iIndex) const
{
	return m_Animations[iIndex]->Get_RootMotionValue();
}

const _bool CModel::Check_BlendFinishMoment() const
{
	return m_bBlendFinishMoment;
}

const _matrix CModel::Get_MeshContainer_Root(const _int & _iMeshIndex, const char* szTag) const
{
	const CHierarchyNode * pHierarchyNode = m_MeshContainers[_iMeshIndex]->Get_HierarchyNode(szTag);
	if (pHierarchyNode)
	{
		return pHierarchyNode->Get_CombinedTransformationMatrix();
	}
	return _matrix();
}

const XMUINT4 & CModel::Get_MaxBlendIndex()
{
	return ((VTXMESH*)m_pVertices)[m_iMaxIndex].vBlendIndex;
}

const _float4 & CModel::Get_MaxBlendWeight()
{
	return ((VTXMESH*)m_pVertices)[m_iMaxIndex].vBlendWeight;
}

const XMUINT4 & CModel::Get_MinBlendIndex()
{
	return ((VTXMESH*)m_pVertices)[m_iMinIndex].vBlendIndex;
}

const _float4 & CModel::Get_MinBlendWeight()
{
	return ((VTXMESH*)m_pVertices)[m_iMinIndex].vBlendWeight;
}

vector<CHierarchyNode*>* CModel::Get_HierarchyNode()
{
	if(0 >= m_HierarchyNodes.size())
		return nullptr;

	return &m_HierarchyNodes;
}

HRESULT CModel::Add_RefNode(const _tchar * pNodeTag, const char * pBoneName)
{
	CHierarchyNode*	pNode = Find_HierarchyNode(pBoneName);
	if (nullptr == pNode)
		return E_FAIL;

	m_RefNodes.emplace(pNodeTag, pNode);

	Safe_AddRef(pNode);

	return S_OK;
}

void CModel::Set_RootMotionValue(_double dValue)
{
	m_Animations[m_iCurrentAnimationIndex]->Set_RootMotionValue(dValue);
}

void CModel::Set_RootMotionValue(_uint iIndex, _double dValue)
{
	if (iIndex >= m_Animations.size())
		return;

	m_Animations[iIndex]->Set_RootMotionValue(dValue);
}

void CModel::Release_RefNode(const _tchar * pTag)
{
	if (nullptr == pTag)
		return;

	auto& iter = find_if(m_RefNodes.begin(), m_RefNodes.end(), CTagFinder(pTag));
	if (iter == m_RefNodes.end())
		return;

	Safe_Release(iter->second);
	m_RefNodes.erase(pTag);
}

void CModel::IsOriginUpdate_Initialize()
{
	if (m_Animations[m_iCurrentAnimationIndex]->Get_OriginUpdate())
	{
		Initialize_RootMotion();
		Set_OriginUpdate(false);
	}
}

void CModel::Initialize_Time()
{
	m_Animations[m_iCurrentAnimationIndex]->Initialize_Time();
}

void CModel::Initialize_Time(_uint iIndex)
{
	m_Animations[iIndex]->Initialize_Time();
	m_Animations[iIndex]->Initialize_OriginUpdate();
}

void CModel::Initialize_RootMotion()
{
	m_vPreRootMotion = m_Animations[m_iCurrentAnimationIndex]->Get_BeginRootBone_Position_NonFind();
}

void CModel::Initialize_OriginUpdate()
{
	m_Animations[m_iCurrentAnimationIndex]->Initialize_OriginUpdate();
}

void CModel::Initialize_Blend()
{
	m_bAnimationBlend = false;
	m_Animations[m_iCurrentAnimationIndex]->Initialize_Blend();
}

void CModel::Immediately_Change(_uint iAnimIndex)
{
	m_iNextAnimationIndex = iAnimIndex;

	m_vPreRootMotion = m_Animations[m_iNextAnimationIndex]->Get_BeginRootBone_Position_NonFind();

	m_Animations[m_iNextAnimationIndex]->Initialize_Time();
	m_Animations[m_iNextAnimationIndex]->Set_OriginUpdate(false);
	m_Animations[m_iNextAnimationIndex]->Set_RootMotionPos();

	m_Animations[m_iCurrentAnimationIndex]->Initialize_Time();
	m_Animations[m_iCurrentAnimationIndex]->Set_OriginUpdate(false);
	m_Animations[m_iCurrentAnimationIndex]->Set_RootMotionPos();

	m_iCurrentAnimationIndex = m_iNextAnimationIndex;
	m_bAnimationBlend = false;
}

void CModel::Synchronize_Animation(CTransform * pTransform, class CNavigation* pNavigation, _double dTimeDelta)
{
	pTransform->Set_OldMatrix();

	IsOriginUpdate_Initialize();

	_vector vLook = pTransform->Get_State(CTransform::STATE::LOOK);
	_vector vValue = XMVectorSet(0.f, 0.f, 0.f, 1.f);

	vValue = ((XMLoadFloat4(&Get_RootMotion()) - XMLoadFloat4(&Get_PreRootMotion())) * (_float)m_Animations[m_iCurrentAnimationIndex]->Get_RootMotionValue());

	_vector vDist = XMVector3Length(vValue);

	vLook = XMVector3Normalize(vLook);

	_vector vPrePos = pTransform->Get_State(CTransform::STATE::POSITION);

	if (!m_Animations[m_iCurrentAnimationIndex]->Get_NonUpdatePos())
	{
		// 원래 
		if (DIRECTION::BACKWARD == m_Animations[m_iCurrentAnimationIndex]->Get_Direction())
		{
			vLook *= -1.f;
			vLook *= XMVectorGetX(vDist);

			pTransform->Set_State(CTransform::STATE::POSITION, pTransform->Get_State(CTransform::STATE::POSITION) + vLook);
		}
		else if (DIRECTION::LEFT == m_Animations[m_iCurrentAnimationIndex]->Get_Direction())
		{
			_vector vRight = pTransform->Get_State(CTransform::STATE::RIGHT);
			vRight = XMVector3Normalize(vRight);

			vRight *= -1.f;
			vRight *= XMVectorGetX(vDist);

			pTransform->Set_State(CTransform::STATE::POSITION, pTransform->Get_State(CTransform::STATE::POSITION) + vRight);
		}
		else if (DIRECTION::RIGHT == m_Animations[m_iCurrentAnimationIndex]->Get_Direction())
		{
			_vector vRight = pTransform->Get_State(CTransform::STATE::RIGHT);
			vRight = XMVector3Normalize(vRight);
			vRight *= XMVectorGetX(vDist);

			pTransform->Set_State(CTransform::STATE::POSITION, pTransform->Get_State(CTransform::STATE::POSITION) + vRight);
		}
		else if (DIRECTION::DIAGONAL_LOOKUP == m_Animations[m_iCurrentAnimationIndex]->Get_Direction())
		{
			_vector vLook = XMVector3Normalize(pTransform->Get_State(CTransform::STATE::LOOK));
			_vector vUp = XMVector3Normalize(pTransform->Get_State(CTransform::STATE::UP));

			_vector vDiagonalLookUp = XMVector3Normalize(vLook + vUp);
			vDiagonalLookUp *= XMVectorGetX(vDist);

			pTransform->Set_State(CTransform::STATE::POSITION, pTransform->Get_State(CTransform::STATE::POSITION) + vDiagonalLookUp);
		}
		else
		{
			vLook *= XMVectorGetX(vDist);
			pTransform->Set_State(CTransform::STATE::POSITION, pTransform->Get_State(CTransform::STATE::POSITION) + vLook);
		}

	}
	_vector vPosition = pTransform->Get_State(CTransform::STATE::POSITION);

	if (pNavigation)
	{
		CCell* pSlideCell = nullptr;
		if (pNavigation->IsOn(vPosition, &pSlideCell))
		{
			if (!pTransform->IsJump())
			{
				if (!XMVector4Equal(vPosition, XMVectorZero()))
				{
					_float fY = 0.f;
					if (pTransform->IsLowThanCell(pNavigation, &fY))
					{
						vPosition = XMVectorSetY(vPosition, fY);
						pTransform->Set_State(CTransform::STATE::POSITION, vPosition);
					}
					else
					{
						pTransform->Set_State(CTransform::STATE::POSITION, vPosition);
					}
				}
			}
		}
		else
		{
			if(nullptr == pSlideCell)
				pTransform->Set_State(CTransform::STATE::POSITION, vPrePos);
			else
			{
				_vector vNormal = pSlideCell->IsSlide(vPrePos);
				_vector vOldLook = pTransform->Get_State(CTransform::STATE::LOOK);
				_vector vSlideVector = XMVector3Normalize(XMVector3Normalize(vOldLook) - XMVector3Dot(vOldLook, vNormal) * vNormal);
				_float fAngle = XMVectorGetX(XMVector3Dot(vSlideVector, XMVector3Normalize(vOldLook)));

				_vector vSlidePos = vPrePos;
				vSlidePos += XMVector3Normalize(vSlideVector) * fAngle * (_float)dTimeDelta * 5.f;
				if (pNavigation->IsOn(vSlidePos, &pSlideCell))
				{
					if (!pTransform->IsJump())
					{
						if (!XMVector4Equal(vSlidePos, XMVectorZero()))
						{
							pTransform->Set_State(CTransform::STATE::POSITION, vSlidePos);
							_float fY = 0.f;
							if (pTransform->IsLowThanCell(pNavigation, &fY))
							{
								vSlidePos = XMVectorSetY(vSlidePos, fY);
								pTransform->Set_State(CTransform::STATE::POSITION, vSlidePos);
							}
						}
					}
				}
				else
					pTransform->Set_State(CTransform::STATE::POSITION, vPrePos);
			}

		}
	}

	Set_PreRootMotion(Get_RootMotion());
}

void CModel::Synchronize_Animation_WithOtherLook(CTransform * pTransform, CNavigation * pNavigation, _vector vOtherLook, _double dTimeDelta)
{
	IsOriginUpdate_Initialize();

	_vector vLook = vOtherLook;
	_vector vValue = XMVectorSet(0.f, 0.f, 0.f, 1.f);

	vValue = ((XMLoadFloat4(&Get_RootMotion()) - XMLoadFloat4(&Get_PreRootMotion())) * static_cast<_float>(m_Animations[m_iCurrentAnimationIndex]->Get_RootMotionValue()));

	_vector vDist = XMVector3Length(vValue);

	vLook = XMVector3Normalize(vLook);

	_vector vPrePos = pTransform->Get_State(CTransform::STATE::POSITION);

	if (!m_Animations[m_iCurrentAnimationIndex]->Get_NonUpdatePos())
	{
		vLook *= XMVectorGetX(vDist);
		pTransform->Set_State(CTransform::STATE::POSITION, pTransform->Get_State(CTransform::STATE::POSITION) + vLook);
	}
	_vector vPosition = pTransform->Get_State(CTransform::STATE::POSITION);

	if (pNavigation)
	{
		CCell* pSlideCell = nullptr;
		if (pNavigation->IsOn(vPosition, &pSlideCell))
		{
			if (!pTransform->IsJump())
			{
				if (!XMVector4Equal(vPosition, XMVectorZero()))
				{
					_float fY = 0.f;
					if (pTransform->IsLowThanCell(pNavigation, &fY))
					{
						vPosition = XMVectorSetY(vPosition, fY);
						pTransform->Set_State(CTransform::STATE::POSITION, vPosition);
					}
					else
					{
						pTransform->Set_State(CTransform::STATE::POSITION, vPosition);
					}
				}
			}
		}
		else
		{
			if (nullptr == pSlideCell)
				pTransform->Set_State(CTransform::STATE::POSITION, vPrePos);
			else
			{
				_vector vNormal = pSlideCell->IsSlide(vPrePos);
				_vector vOldLook = pTransform->Get_State(CTransform::STATE::LOOK);
				_vector vSlideVector = XMVector3Normalize(XMVector3Normalize(vOldLook) - XMVector3Dot(vOldLook, vNormal) * vNormal);
				_float fAngle = XMVectorGetX(XMVector3Dot(vSlideVector, XMVector3Normalize(vOldLook)));

				_vector vSlidePos = vPrePos;
				vSlidePos += XMVector3Normalize(vSlideVector) * fAngle * (_float)dTimeDelta * 5.f;
				if (pNavigation->IsOn(vSlidePos, &pSlideCell))
				{
					if (!pTransform->IsJump())
					{
						if (!XMVector4Equal(vSlidePos, XMVectorZero()))
						{
							pTransform->Set_State(CTransform::STATE::POSITION, vSlidePos);
							_float fY = 0.f;
							if (pTransform->IsLowThanCell(pNavigation, &fY))
							{
								vSlidePos = XMVectorSetY(vSlidePos, fY);
								pTransform->Set_State(CTransform::STATE::POSITION, vSlidePos);
							}
						}
					}
				}
				else
					pTransform->Set_State(CTransform::STATE::POSITION, vPrePos);
			}

		}
	}

	Set_PreRootMotion(Get_RootMotion());
}

void CModel::Synchronize_Animation_For_Coaster(CTransform * pTransform, CNavigation * pNavigation, _double dTimeDelta)
{
	IsOriginUpdate_Initialize();

	_vector vLook = pTransform->Get_State(CTransform::STATE::LOOK);
	_vector vValue = XMVectorSet(0.f, 0.f, 0.f, 1.f);

	vValue = ((XMLoadFloat4(&Get_RootMotion()) - XMLoadFloat4(&Get_PreRootMotion())) * static_cast<_float>(m_Animations[m_iCurrentAnimationIndex]->Get_RootMotionValue()));

	_vector vDist = XMVector3Length(vValue);

	vLook = XMVector3Normalize(vLook);

	_vector vPrePos = pTransform->Get_State(CTransform::STATE::POSITION);

	if (!m_Animations[m_iCurrentAnimationIndex]->Get_NonUpdatePos())
	{
		//롤코용
		vValue = ((XMLoadFloat4(&Get_PreRootMotion()) - XMLoadFloat4(&Get_RootMotion())) * static_cast<_float>(m_Animations[m_iCurrentAnimationIndex]->Get_RootMotionValue()));
		vValue = XMVectorSetY(vValue, XMVectorGetY(vValue) * -1.f);

		XMStoreFloat3(&m_vValue, vValue);
		pTransform->Set_State(CTransform::STATE::POSITION, vPrePos + vValue);

		vValue = XMVector3Normalize(vValue);
		XMStoreFloat4(&m_vecLook, vValue);
	}
	_vector vPosition = pTransform->Get_State(CTransform::STATE::POSITION);

	if (pNavigation)
	{
		CCell* pSlideCell = nullptr;
		if (pNavigation->IsOn(vPosition, &pSlideCell))
		{
			if (!pTransform->IsJump())
			{
				if (!XMVector4Equal(vPosition, XMVectorZero()))
				{
					_float fY = 0.f;
					if (pTransform->IsLowThanCell(pNavigation, &fY))
					{
						vPosition = XMVectorSetY(vPosition, fY);
						pTransform->Set_State(CTransform::STATE::POSITION, vPosition);
					}
					else
					{
						pTransform->Set_State(CTransform::STATE::POSITION, vPosition);
					}
				}
			}
		}
		else
		{
			if (nullptr == pSlideCell)
				pTransform->Set_State(CTransform::STATE::POSITION, vPrePos);
			else
			{
				_vector vNormal = pSlideCell->IsSlide(vPrePos);
				_vector vOldLook = pTransform->Get_State(CTransform::STATE::LOOK);
				_vector vSlideVector = XMVector3Normalize(XMVector3Normalize(vOldLook) - XMVector3Dot(vOldLook, vNormal) * vNormal);
				_float fAngle = XMVectorGetX(XMVector3Dot(vSlideVector, XMVector3Normalize(vOldLook)));

				_vector vSlidePos = vPrePos;
				vSlidePos += XMVector3Normalize(vSlideVector) * fAngle * static_cast<_float>(dTimeDelta);
				if (pNavigation->IsOn(vSlidePos, &pSlideCell))
				{
					if (!pTransform->IsJump())
					{
						if (!XMVector4Equal(vSlidePos, XMVectorZero()))
						{
							pTransform->Set_State(CTransform::STATE::POSITION, vSlidePos);
							_float fY = 0.f;
							if (pTransform->IsLowThanCell(pNavigation, &fY))
							{
								vSlidePos = XMVectorSetY(vSlidePos, fY);
								pTransform->Set_State(CTransform::STATE::POSITION, vSlidePos);
							}
						}
					}
				}
				else
					pTransform->Set_State(CTransform::STATE::POSITION, vPrePos);
			}

		}
	}

	Set_PreRootMotion(Get_RootMotion());
}

const _bool CModel::IsFinish_Animation() const
{
	return m_Animations[m_iCurrentAnimationIndex]->IsFinish_Animation();
}

HRESULT CModel::Compute_MiddlePoint()
{
	_vector vMinPoint = XMVectorSet(0.f, 0.f, 0.f, 0.f), vMaxPoint = XMVectorSet(0.f, 0.f, 0.f, 0.f);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		vMinPoint = XMVectorMin(vMinPoint, XMLoadFloat3(&((VTXMESH*)m_pVertices)[i].vPosition));
		vMaxPoint = XMVectorMax(vMaxPoint, XMLoadFloat3(&((VTXMESH*)m_pVertices)[i].vPosition));
	}
	XMStoreFloat3(&m_vMinPoint, vMinPoint);
	XMStoreFloat3(&m_vMaxPoint, vMaxPoint);
	XMStoreFloat3(&m_vMiddlePoint, (vMaxPoint + vMinPoint) / 2.f);
	m_fRadius = XMVectorGetX(XMVector4Length((vMaxPoint - vMinPoint) / 2.f));

	return S_OK;

	//_vector vMinPoint = XMVectorSet(0.f, 0.f, 0.f, 0.f), vMaxPoint = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	//
	//for (_uint i = 0; i < m_iNumVertices; ++i)
	//{
	//	_vector vTempMinPoint = vMinPoint;
	//	_vector vTempMaxPoint = vMaxPoint;
	//
	//	vTempMinPoint = XMVectorMin(vMinPoint, XMLoadFloat3(&((VTXMESH*)m_pVertices)[i].vPosition));
	//	vTempMaxPoint = XMVectorMax(vMaxPoint, XMLoadFloat3(&((VTXMESH*)m_pVertices)[i].vPosition));
	//	// different
	//	if (0 != XMVectorGetX(XMVector4Length(XMVectorEqual(vMinPoint, vTempMinPoint))))
	//	{
	//		m_iMinIndex = i;
	//		vMinPoint = vTempMinPoint;
	//	}
	//	// different
	//	if (0 != XMVectorGetX(XMVector4Length(XMVectorEqual(vMaxPoint, vTempMaxPoint))))
	//	{
	//		m_iMaxIndex = i;
	//		vMaxPoint = vTempMaxPoint;
	//	}
	//}
	//XMStoreFloat3(&m_vMinPoint, vMinPoint);
	//XMStoreFloat3(&m_vMaxPoint, vMaxPoint);
	//XMStoreFloat3(&m_vMiddlePoint, (vMaxPoint + vMinPoint) / 2.f);
	//m_fRadius = XMVectorGetX(XMVector4Length((vMaxPoint - vMinPoint) / 2.f));

	//return S_OK;
}

HRESULT CModel::Compute_LongestTwoPoint()
{
	if (m_iNumVertices < 3)
	{
		assert(false);
	}

	_vector vFirstPoint = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	_vector vSecondPoint = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	_vector vTestPoint = XMVectorZero();

	vFirstPoint = XMVectorSetW(XMLoadFloat3(&((VTXMESH*)m_pVertices)[0].vPosition), 1.f);
	vSecondPoint = XMVectorSetW(XMLoadFloat3(&((VTXMESH*)m_pVertices)[1].vPosition), 1.f);
	m_iMaxIndex = 0;
	m_iMinIndex = 1;
	_float fFirstTestLength = 0.f;
	_float fFirstSecondLength = 0.f;
	_float fSecondTestLength = 0.f;
	for (_uint i = 2; i < m_iNumVertices; ++i)
	{
		vTestPoint = XMVectorSetW(XMLoadFloat3(&((VTXMESH*)m_pVertices)[i].vPosition), 1.f);
		fFirstTestLength = XMVectorGetX(XMVector4Length(vFirstPoint - vTestPoint));
		fFirstSecondLength = XMVectorGetX(XMVector4Length(vFirstPoint - vSecondPoint));
		fSecondTestLength = XMVectorGetX(XMVector4Length(vSecondPoint - vTestPoint));

		// First, Test is Longest
		if (fFirstTestLength > fFirstSecondLength && fFirstTestLength > fSecondTestLength)
		{
			memcpy(&vSecondPoint, &vTestPoint, sizeof(_vector));
			m_iMinIndex = i;
			continue;
		}
		// First, Second is Longest
		else if (fFirstSecondLength > fFirstTestLength && fFirstSecondLength > fSecondTestLength)
		{
			continue;
		}
		// Second, Test is Longest
		else if (fSecondTestLength > fFirstTestLength && fSecondTestLength > fFirstSecondLength)
		{
			vFirstPoint = vSecondPoint;
			memcpy(&vSecondPoint, &vTestPoint, sizeof(_vector));

			m_iMaxIndex = m_iMinIndex;
			m_iMinIndex = i;
			continue;
		}


	}

	XMStoreFloat3(&m_vFarPoint1, vFirstPoint);
	XMStoreFloat3(&m_vFarPoint2, vSecondPoint);


	return S_OK;
}

_float3 CModel::Get_MiddlePoint()
{
	return m_vMiddlePoint;
}

_float3 CModel::Get_MinPoint()
{
	return m_vMinPoint;
}

_float3 CModel::Get_MaxPoint()
{
	return m_vMaxPoint;
}

_float CModel::Get_Radius()
{
	return m_fRadius;
}
