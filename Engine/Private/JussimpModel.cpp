#include "..\Public\JussimpModel.h"
#include "MeshContainer.h"
#include "Texture.h"

CJussimpModel::CJussimpModel(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
{
}

CJussimpModel::CJussimpModel(const CJussimpModel & rhs)
	: CVIBuffer(rhs)
	, m_pScene(rhs.m_pScene)
	, m_Materials(rhs.m_Materials)
	, m_PivotMatrix(rhs.m_PivotMatrix)
	, m_IPGeneral(rhs.m_IPGeneral)
	//, m_IPVertex(rhs.m_IPVertex)
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

	for (auto& pImportMeshContainer : rhs.m_vecIPMeshContainer)
	{
		m_vecIPMeshContainer.push_back(pImportMeshContainer);
	}
	for (auto& pImportMaterial : rhs.m_vecIPMaterial)
	{
		m_vecIPMaterial.push_back(pImportMaterial);
	}
}

CJussimpModel * CJussimpModel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, TYPE eMeshType, const char * pModelFilePath, const char * pModelFileName, const _tchar* pShaderFilePath, const char* pTechniqueName, _fmatrix PivotMatrix)
{
	CJussimpModel* pInstance = new CJussimpModel(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(eMeshType, pModelFilePath, pModelFileName, pShaderFilePath, pTechniqueName, PivotMatrix)))
	{
		MSGBOX("Failed To Creating CJussimpModel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CJussimpModel * CJussimpModel::Create_ForTool(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, TYPE eMeshType, const char * pModelFilePath, const char * pModelFileName, const _tchar * pShaderFilePath, const char * pTechniqueName, const wchar_t* pExportDatFilePath, _fmatrix PivotMatrix)
{
	CJussimpModel* pInstance = new CJussimpModel(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype_ForTool(eMeshType, pModelFilePath, pModelFileName, pShaderFilePath, pTechniqueName, pExportDatFilePath,PivotMatrix)))
		Safe_Release(pInstance);

	return pInstance;
}

HRESULT CJussimpModel::NativeConstruct_Prototype(TYPE eMeshType, const char * pModelFilePath, const char * pModelFileName, const _tchar * pShaderFilePath, const char * pTechniqueName, _fmatrix PivotMatrix)
{
	char		szFullPath[MAX_PATH] = "";

	strcpy_s(szFullPath, pModelFilePath);
	strcat_s(szFullPath, pModelFileName);

	_uint			iFlag = 0;

	if (eMeshType == TYPE::NONANIM)
		iFlag = aiProcess_PreTransformVertices | aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_ConvertToLeftHanded;
	else
		iFlag = aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_ConvertToLeftHanded;

	m_pScene = m_Importer.ReadFile(szFullPath, iFlag);
	if (nullptr == m_pScene)
		return E_FAIL;

	ZeroMemory(&m_IPGeneral, sizeof(IPGeneral));

	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

	if (FAILED(Create_MeshContainer()))
	{
		return E_FAIL;
	}
	if (FAILED(Compile_Shader(pShaderFilePath, pTechniqueName)))
	{
		return E_FAIL;
	}
	if (FAILED(Create_Materials(pModelFilePath)))
	{
		return E_FAIL;
	}
	if (true == m_pScene->HasAnimations())
	{
		m_IPGeneral.iMeshType = (_uint)TYPE::ANIM;
		if (FAILED(Create_SkinnedMesh()))
		{
			return E_FAIL;
		}
	}
	else
	{
		m_IPGeneral.iMeshType = (_uint)TYPE::NONANIM;
	}
	if (FAILED(Create_VIBuffer()))
	{
		return E_FAIL;
	}
	return S_OK;
}

CComponent * CJussimpModel::Clone(void * pArg)
{
	CJussimpModel* pInstance = new CJussimpModel(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed To Cloning CJussimpModel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

HRESULT CJussimpModel::NativeConstruct(void * pArg)
{
	if (false == m_pScene->HasAnimations())
	{
		return S_OK;
	}
	if (FAILED(Create_HierarchyNodes(m_pScene->mRootNode, nullptr, 0)))
	{
		return E_FAIL;
	}

	sort(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [](CHierarchyNode* pSour, CHierarchyNode* pDest)
	{
		return pSour->Get_Depth() < pDest->Get_Depth();
	});

	if (FAILED(SetUp_HierarchyNodeToMeshContainer()))
	{
		return E_FAIL;
	}
	if (FAILED(Create_Animation()))
	{
		return E_FAIL;
	}

	// 반드시 Create_Animation() 호출 이후에 호출되어야함.
	if (FAILED(Export_Parameter(L"../Model.dat")))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CJussimpModel::NativeConstruct_Prototype_ForTool(TYPE eMeshType, const char * pModelFilePath, const char * pModelFileName, const _tchar * pShaderFilePath, const char * pTechniqueName, const wchar_t* pExportDatFilePath, _fmatrix PivotMatrix)
{
	char		szFullPath[MAX_PATH] = "";

	strcpy_s(szFullPath, pModelFilePath);
	strcat_s(szFullPath, pModelFileName);

	_uint			iFlag = 0;

	if (eMeshType == TYPE::NONANIM)
		iFlag = aiProcess_PreTransformVertices | aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_ConvertToLeftHanded;
	else
		iFlag = aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_ConvertToLeftHanded;

	m_pScene = m_Importer.ReadFile(szFullPath, iFlag);
	if (nullptr == m_pScene)
		return E_FAIL;

	ZeroMemory(&m_IPGeneral, sizeof(IPGeneral));

	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

	if (FAILED(Create_MeshContainer()))
	{
		return E_FAIL;
	}
	if (FAILED(Compile_Shader(pShaderFilePath, pTechniqueName)))
	{
		return E_FAIL;
	}
	if (FAILED(Create_Materials(pModelFilePath)))
	{
		return E_FAIL;
	}
	if (true == m_pScene->HasAnimations())
	{
		m_IPGeneral.iMeshType = (_uint)TYPE::ANIM;
		if (FAILED(Create_SkinnedMesh()))
		{
			return E_FAIL;
		}
	}
	else
	{
		m_IPGeneral.iMeshType = (_uint)TYPE::NONANIM;
	}
	if (FAILED(Create_VIBuffer()))
	{
		return E_FAIL;
	}
	if (false == m_pScene->HasAnimations())
	{
		if (FAILED(Export_Parameter(pExportDatFilePath)))
		{
			return E_FAIL;
		}
		return S_OK;
	}
	if (FAILED(Create_HierarchyNodes(m_pScene->mRootNode, nullptr, 0)))
	{
		return E_FAIL;
	}

	sort(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [](CHierarchyNode* pSour, CHierarchyNode* pDest)
	{
		return pSour->Get_Depth() < pDest->Get_Depth();
	});

	if (FAILED(SetUp_HierarchyNodeToMeshContainer()))
	{
		return E_FAIL;
	}
	if (FAILED(Create_Animation()))
	{
		return E_FAIL;
	}

	// 반드시 Create_Animation() 호출 이후에 호출되어야함.
	if (FAILED(Export_Parameter(pExportDatFilePath)))
	{
		return E_FAIL;
	}
	return S_OK;
}

void CJussimpModel::Free()
{
	__super::Free();

	Free_Param();

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

}

HRESULT CJussimpModel::Create_MeshContainer()
{
	for (_uint i = 0; i < m_pScene->mNumMeshes; ++i)
	{
		m_iNumVertices += m_pScene->mMeshes[i]->mNumVertices;
		m_iNumPrimitive += m_pScene->mMeshes[i]->mNumFaces;
	}

	m_pVertices = new VTXMESH[m_iNumVertices];
	ZeroMemory(m_pVertices, sizeof(VTXMESH) * m_iNumVertices);

	m_pIndices = new FACEINDICES32[m_iNumPrimitive];
	ZeroMemory(m_pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);

	_uint	iIndex = 0;
	_uint	iFaceIndex = 0;
	_uint	iNumMeshes = 0;

	// For.Export
	iNumMeshes = m_pScene->mNumMeshes;
	m_IPGeneral.iNumVertices = m_iNumVertices;
	m_IPGeneral.iNumIndices = m_iNumPrimitive;
	m_IPGeneral.iNumMeshes = iNumMeshes;

	m_vecIPMeshContainer.reserve(iNumMeshes);

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		aiMesh*		pMesh = m_pScene->mMeshes[i];
		if (nullptr == pMesh)
			return E_FAIL;

		CMeshContainer*		pMeshContainer = CMeshContainer::Create(m_pGraphicDevice, m_pContextDevice, iIndex, pMesh->mNumVertices, iFaceIndex, pMesh->mNumFaces, pMesh->mMaterialIndex);

		IPMeshContainer*	pIPMC = new IPMeshContainer;
		ZeroMemory(&pIPMC->iNumVertices, sizeof(_uint));
		ZeroMemory(&pIPMC->iStartVertexIndex, sizeof(_uint));
		ZeroMemory(&pIPMC->iNumFaces, sizeof(_uint));
		ZeroMemory(&pIPMC->iStartFaceIndex, sizeof(_uint));
		ZeroMemory(&pIPMC->iMaterialIndex, sizeof(_uint));
		ZeroMemory(&pIPMC->iNumBones, sizeof(_uint));

		pIPMC->iNumVertices = pMesh->mNumVertices;
		pIPMC->iStartVertexIndex = iIndex;
		pIPMC->iNumFaces = pMesh->mNumFaces;
		pIPMC->iStartFaceIndex = iFaceIndex;
		pIPMC->iMaterialIndex = pMesh->mMaterialIndex;
		pIPMC->iNumBones = pMesh->mNumBones;
		pIPMC->vecBoneName.reserve(pIPMC->iNumBones);

		for (_uint k = 0; k < pIPMC->iNumBones; ++k)
		{
			char*	pName = new char[MAX_PATH];

			ZeroMemory(pName, sizeof(char) * MAX_PATH);

			strcpy_s(pName, MAX_PATH, pMesh->mBones[k]->mName.data);

			pIPMC->vecBoneName.push_back(pName);
		}
		m_vecIPMeshContainer.push_back(pIPMC);

		for (_uint j = 0; j < pMesh->mNumVertices; ++j)
		{
			memcpy(&((VTXMESH*)m_pVertices)[iIndex].vPosition, &pMesh->mVertices[j], sizeof(_float3));

			if (false == m_pScene->HasAnimations())
				XMStoreFloat3(&((VTXMESH*)m_pVertices)[iIndex].vPosition, XMVector3TransformCoord(XMLoadFloat3(&((VTXMESH*)m_pVertices)[iIndex].vPosition), XMLoadFloat4x4(&m_PivotMatrix)));

			memcpy(&((VTXMESH*)m_pVertices)[iIndex].vNormal, &pMesh->mNormals[j], sizeof(_float3));
			if (false == m_pScene->HasAnimations())
				XMStoreFloat3(&((VTXMESH*)m_pVertices)[iIndex].vNormal, XMVector3TransformNormal(XMLoadFloat3(&((VTXMESH*)m_pVertices)[iIndex].vNormal), XMLoadFloat4x4(&m_PivotMatrix)));


			
			memcpy(&((VTXMESH*)m_pVertices)[iIndex].vTexUV, &pMesh->mTextureCoords[0][j], sizeof(_float2));
			memcpy(&((VTXMESH*)m_pVertices)[iIndex].vTangent, &pMesh->mTangents[j], sizeof(_float3));
			++iIndex;
		}

		for (_uint j = 0; j < pMesh->mNumFaces; ++j)
		{
			((FACEINDICES32*)m_pIndices)[iFaceIndex]._1 = pMesh->mFaces[j].mIndices[0];
			((FACEINDICES32*)m_pIndices)[iFaceIndex]._2 = pMesh->mFaces[j].mIndices[1];
			((FACEINDICES32*)m_pIndices)[iFaceIndex]._3 = pMesh->mFaces[j].mIndices[2];
			++iFaceIndex;
		}
		m_MeshContainers.push_back(pMeshContainer);
	}

	return S_OK;
}

HRESULT CJussimpModel::Compile_Shader(const _tchar * pShaderFilePath, const char * pTechniqueName)
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

HRESULT CJussimpModel::Create_Materials(const char * pMeshFilePath)
{
	_uint	iNumMaterials = m_pScene->mNumMaterials;

	m_IPGeneral.iNumMaterials = iNumMaterials;

	m_Materials.reserve(iNumMaterials);

	for (_uint i = 0; i < iNumMaterials; ++i)
	{
		aiMaterial*		pMaterial = m_pScene->mMaterials[i];
		if (nullptr == pMaterial)
			return E_FAIL;

		MESHMATERIAL*	pMeshMaterial = new MESHMATERIAL;
		ZeroMemory(pMeshMaterial, sizeof(MESHMATERIAL));

		IPMaterial*		pIPM = new IPMaterial;

		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; ++j)
		{
			aiString			StrTexturePath;

			if (FAILED(pMaterial->GetTexture(aiTextureType(j), 0, &StrTexturePath)))
			{
				continue;
			}

			char		szFileName[MAX_PATH] = "";
			char		szExt[MAX_PATH] = "";

			_splitpath_s(StrTexturePath.data, nullptr, 0, nullptr, 0, szFileName, 256, szExt, 256);

			char		szFullPath[MAX_PATH] = "";
			strcpy_s(szFullPath, pMeshFilePath);
			strcat_s(szFullPath, szFileName);
			strcat_s(szFullPath, szExt);

			_tchar		szMaterialPath[MAX_PATH] = TEXT("");

			MultiByteToWideChar(CP_ACP, 0, szFullPath, (_int)strlen(szFullPath), szMaterialPath, 256);

			//CTexture*		pTexture = CTexture::Create(m_pGraphicDevice, m_pContextDevice, szMaterialPath);
			//if (nullptr == pTexture)
			//	return E_FAIL;

			//pMeshMaterial->pMaterialTextures[j] = pTexture;

			// For.Export
			IPMaterial::TEXDESC	pTexDesc;

			char	szExportFileRelativePath[MAX_PATH] = "";

			for (int k = 0; k < MAX_PATH; ++k)
			{
				if (szFullPath[k] == 'c')
				{
					if (!(strncmp("client", &szFullPath[k], 6)))
					{
						strcpy_s(szExportFileRelativePath, "..\\");
						strncat_s(szExportFileRelativePath, &szFullPath[k], MAX_PATH - k);
					}
				}
				else if (szFullPath[k] == 'C')
				{
					if (!(strncmp("Client", &szFullPath[k], 6)))
					{
						strcpy_s(szExportFileRelativePath, "..\\");
						strncat_s(szExportFileRelativePath, &szFullPath[k], MAX_PATH - k);
					}
				}
			}

			pTexDesc.iTextureType = j;
			strcpy_s(pTexDesc.szMaterialTexturePath, MAX_PATH, szExportFileRelativePath);
			//strcpy_s(pTexDesc.szMaterialTexturePath, MAX_PATH, szFullPath);

			pIPM->vecTexDesc.push_back(pTexDesc);
		}
		m_vecIPMaterial.push_back(pIPM);

		m_Materials.push_back(pMeshMaterial);
	}

	return S_OK;
}

HRESULT CJussimpModel::Create_SkinnedMesh()
{
	for (_uint i = 0; i < m_pScene->mNumMeshes; ++i)
	{
		aiMesh*		pMesh = m_pScene->mMeshes[i];

		CMeshContainer*	pMeshContainer = m_MeshContainers[i];

		for (_uint j = 0; j < pMesh->mNumBones; ++j)
		{
			aiBone*		pBone = pMesh->mBones[j];

			for (_uint k = 0; k < pBone->mNumWeights; ++k)
			{
				_uint iVertexIndex = pMeshContainer->Get_StartVertexIndex() + pBone->mWeights[k].mVertexId;

				if (0.0f == ((VTXMESH*)m_pVertices)[iVertexIndex].vBlendWeight.x)
				{
					((VTXMESH*)m_pVertices)[iVertexIndex].vBlendIndex.x = j;
					((VTXMESH*)m_pVertices)[iVertexIndex].vBlendWeight.x = pBone->mWeights[k].mWeight;
				}

				else if (0.0f == ((VTXMESH*)m_pVertices)[iVertexIndex].vBlendWeight.y)
				{
					((VTXMESH*)m_pVertices)[iVertexIndex].vBlendIndex.y = j;
					((VTXMESH*)m_pVertices)[iVertexIndex].vBlendWeight.y = pBone->mWeights[k].mWeight;
				}

				else if (0.0f == ((VTXMESH*)m_pVertices)[iVertexIndex].vBlendWeight.z)
				{
					((VTXMESH*)m_pVertices)[iVertexIndex].vBlendIndex.z = j;
					((VTXMESH*)m_pVertices)[iVertexIndex].vBlendWeight.z = pBone->mWeights[k].mWeight;
				}
				else
				{
					((VTXMESH*)m_pVertices)[iVertexIndex].vBlendIndex.w = j;
					((VTXMESH*)m_pVertices)[iVertexIndex].vBlendWeight.w = pBone->mWeights[k].mWeight;
				}
			}
		}
	}
	return S_OK;
}

HRESULT CJussimpModel::Create_VIBuffer()
{
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

HRESULT CJussimpModel::Create_HierarchyNodes(aiNode * pNode, CHierarchyNode * pParent, _uint iDepth)
{
	_matrix		TransformationMatrix;
	memcpy(&TransformationMatrix, &pNode->mTransformation, sizeof(_matrix));

	CHierarchyNode*		pHierarchyNode = CHierarchyNode::Create(pNode->mName.data, TransformationMatrix, pParent, iDepth);
	if (nullptr == pHierarchyNode)
		return E_FAIL;

	// For Export
	IPHNode*	pIPHNode = new IPHNode;
	const char*	szParentsName = nullptr;

	ZeroMemory(pIPHNode, sizeof(IPHNode));

	if (nullptr == pParent)
	{
		szParentsName = "";
	}
	else
	{
		szParentsName = pParent->Get_Name();
	}

	strcpy_s(pIPHNode->szParentName, szParentsName);
	strcpy_s(pIPHNode->szName, pNode->mName.data);
	memcpy(&pIPHNode->TransformationMatrix, &pNode->mTransformation, sizeof(_matrix));
	pIPHNode->OffsetMatrix = XMMatrixIdentity();
	pIPHNode->iDepth = iDepth++;
	pIPHNode->iNumChildren = pNode->mNumChildren;
	// pIPHNode->OffSetMatrix는 SetUp_HierarchyNodeToMeshContainer() 에서 세팅

	pHierarchyNode->Reserve_Channels(m_pScene->mNumAnimations);

	m_IPGeneral.iNumHierachyNodes++;	// 세이브 로드에 필요한 param

	m_vecIPHNode.push_back(pIPHNode);

	m_HierarchyNodes.push_back(pHierarchyNode);

	for (_uint i = 0; i < pNode->mNumChildren; ++i)
	{
		if (FAILED(Create_HierarchyNodes(pNode->mChildren[i], pHierarchyNode, iDepth)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CJussimpModel::SetUp_HierarchyNodeToMeshContainer()
{
	for (_uint i = 0; i < m_pScene->mNumMeshes; ++i)
	{
		aiMesh*		pMesh = m_pScene->mMeshes[i];

		CMeshContainer*	pMeshContainer = m_MeshContainers[i];

		for (_uint j = 0; j < pMesh->mNumBones; ++j)
		{
			aiBone*		pBone = pMesh->mBones[j];

			CHierarchyNode*		pHierarchyNode = Find_HierarchyNode(pBone->mName.data);
			if (nullptr == pHierarchyNode)
				return E_FAIL;

			_matrix		OffsetMatrix;
			memcpy(&OffsetMatrix, &pBone->mOffsetMatrix, sizeof(_matrix));

			OffsetMatrix = XMMatrixTranspose(OffsetMatrix);

			///////////////////////////////////////////////////////////////////////

			// 1. pMesh->pBone->mName.data와 동일한 IPHNode를 찾는다.
			// 2. 해당 IPHNode->OffsetMatrix에 Bone->mOffsetMatrix를 memcpy() 한다.

			auto	iter = find_if(m_vecIPHNode.begin(), m_vecIPHNode.end(), [&](IPHNode* pNode)
			{
				return !strcmp(pNode->szName, pBone->mName.data);
			});
			if (iter == m_vecIPHNode.end())
			{
				return E_FAIL;
			}
			memcpy(&(*iter)->OffsetMatrix, &OffsetMatrix, sizeof(_matrix));

			///////////////////////////////////////////////////////////////////////

			pHierarchyNode->Set_OffsetMatrix(OffsetMatrix);
			pMeshContainer->Add_HierarchyNode(pHierarchyNode);
			Safe_AddRef(pHierarchyNode);
		}
	}
	return S_OK;
}

HRESULT CJussimpModel::Create_Animation()
{
	m_IPGeneral.iNumAnimations = m_pScene->mNumAnimations;

	m_Animations.reserve(m_pScene->mNumAnimations);

	for (_uint i = 0; i < m_pScene->mNumAnimations; ++i)
	{
		aiAnimation*	pAnim = m_pScene->mAnimations[i];
		if (nullptr == pAnim)
			return E_FAIL;

		CAnimation*		pAnimation = CAnimation::Create(pAnim->mDuration, pAnim->mTicksPerSecond);
		if (nullptr == pAnimation)
			return E_FAIL;

		pAnimation->Set_AnimationName(pAnim->mName.data);

		// For.Export
		IPAnim*	pIPAnim = new IPAnim;
		ZeroMemory(pIPAnim, sizeof(IPAnim));

		strcpy_s(pIPAnim->szName, pAnim->mName.data);
		pIPAnim->iDuration = pAnim->mDuration;
		pIPAnim->iTicksPerSecond = pAnim->mTicksPerSecond;
		pIPAnim->iNumChannels = pAnim->mNumChannels;

		m_vecIPAnim.push_back(pIPAnim);

		/////////////////////////////////////////

		/* 현재 애니메이션의 상태를 표현하기위한 뼈들. */
		for (_uint j = 0; j < pAnim->mNumChannels; ++j)
		{
			aiNodeAnim*		pNodeAnim = pAnim->mChannels[j];

			CChannel*		pChannel = CChannel::Create(pNodeAnim->mNodeName.data);
			if (nullptr == pChannel)
				return E_FAIL;

			CHierarchyNode*	pHierarchyNode = Find_HierarchyNode(pNodeAnim->mNodeName.data);
			if (nullptr == pHierarchyNode)
				return E_FAIL;

			/* KeyFrame : 하나의 애니메이션을 표현하기위한 구분동작. */
			_uint	iNumKeyFrames = max(pAnim->mChannels[j]->mNumScalingKeys, pAnim->mChannels[j]->mNumRotationKeys);
			iNumKeyFrames = max(iNumKeyFrames, pAnim->mChannels[j]->mNumPositionKeys);

			_vector		vScale, vRotation, vPosition;
			vScale = XMVectorSet(1.f, 1.f, 1.f, 0.f);
			vRotation = XMVectorZero();
			vPosition = XMVectorSet(0.f, 0.f, 0.f, 1.f);

			for (_uint k = 0; k < iNumKeyFrames; ++k)
			{
				KEYFRAME*		pKeyFrame = new KEYFRAME;
				ZeroMemory(pKeyFrame, sizeof(KEYFRAME));

				if (pAnim->mChannels[j]->mNumScalingKeys > k)
				{
					memcpy(&vScale, &pNodeAnim->mScalingKeys[k].mValue, sizeof(_float3));
					pKeyFrame->dTime = pNodeAnim->mScalingKeys[k].mTime;
				}

				if (pAnim->mChannels[j]->mNumRotationKeys > k)
				{
					// memcpy(&vRotation, &pNodeAnim->mRotationKeys[k].mValue , sizeof(_float4));
					vRotation = XMVectorSetX(vRotation, pNodeAnim->mRotationKeys[k].mValue.x);
					vRotation = XMVectorSetY(vRotation, pNodeAnim->mRotationKeys[k].mValue.y);
					vRotation = XMVectorSetZ(vRotation, pNodeAnim->mRotationKeys[k].mValue.z);
					vRotation = XMVectorSetW(vRotation, pNodeAnim->mRotationKeys[k].mValue.w);

					pKeyFrame->dTime = pNodeAnim->mRotationKeys[k].mTime;
				}

				if (pAnim->mChannels[j]->mNumPositionKeys > k)
				{
					memcpy(&vPosition, &pNodeAnim->mPositionKeys[k].mValue, sizeof(_float3));
					vPosition = XMVectorSetW(vPosition, 1.f);

					pKeyFrame->dTime = pNodeAnim->mPositionKeys[k].mTime;
				}

				XMStoreFloat4(&pKeyFrame->vScale, vScale);
				XMStoreFloat4(&pKeyFrame->vRotation, vRotation);
				XMStoreFloat4(&pKeyFrame->vPosition, vPosition);

				pChannel->Add_KeyFrame(pKeyFrame);
			}

			pAnimation->Add_Channel(pChannel);
			pHierarchyNode->Add_Channel(i, pChannel);


			// For Export
			IPChannel*	pIPChannel = new IPChannel;
			ZeroMemory(pIPChannel->szTargetName, sizeof(char) * MAX_PATH);
			ZeroMemory(pIPChannel->szName, sizeof(char) * MAX_PATH);

			strcpy_s(pIPChannel->szTargetName, pAnim->mName.data);		// param for pAnim->Add_Channel()
			strcpy_s(pIPChannel->szName, pNodeAnim->mNodeName.data);	// param for pHNode->Add_Channel()

			vector<KEYFRAME*> pVecFrames = pChannel->Get_KeyFrames();

			size_t vecSize = pVecFrames.size();

			pIPChannel->vecKeyFrames.reserve(vecSize);

			for (size_t k = 0; k < vecSize; ++k)
			{
				KEYFRAME*	pFrame = new KEYFRAME;

				ZeroMemory(pFrame, sizeof(KEYFRAME));

				memcpy(pFrame, pVecFrames[k], sizeof(KEYFRAME));

				pIPChannel->vecKeyFrames.push_back(pFrame);
			}
			m_vecIPChannel.push_back(pIPChannel);
		}
		m_Animations.push_back(pAnimation);

	}

	return S_OK;
}

HRESULT CJussimpModel::Export_Parameter(const wchar_t* pDatName)
{
	// For.Export
	//m_IPVertex.pVertices = new VTXMESH[m_iNumVertices];

	//ZeroMemory(m_IPVertex.pVertices, sizeof(VTXMESH) * m_iNumVertices);

	//memcpy(m_IPVertex.pVertices, m_pVertices, sizeof(VTXMESH) * m_iNumVertices);

	//m_IPVertex.pIndices = new FACEINDICES32[m_iNumPrimitive];

	//ZeroMemory(m_IPVertex.pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);

	//memcpy(m_IPVertex.pIndices, m_pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);

	HANDLE	hFile = nullptr;

	hFile = CreateFile(pDatName, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	//hFile = CreateFile(TEXT("../Model.dat"), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		return E_FAIL;
	}

	// For Debug
	m_IPGeneral;
	//m_IPVertex;
	m_vecIPMeshContainer;
	m_vecIPMaterial;
	m_vecIPHNode;
	m_vecIPAnim;
	m_vecIPChannel;

	DWORD	dwByte = 0;
	WriteFile(hFile, &m_PivotMatrix, sizeof(_matrix), &dwByte, nullptr);

	WriteFile(hFile, &m_IPGeneral, sizeof(IPGeneral), &dwByte, nullptr);
	
	WriteFile(hFile, m_pVertices, sizeof(VTXMESH) * m_iNumVertices, &dwByte, nullptr);
	WriteFile(hFile, m_pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive, &dwByte, nullptr);

	//WriteFile(hFile, m_IPVertex.pVertices, sizeof(VTXMESH) * m_iNumVertices, &dwByte, nullptr);
	//WriteFile(hFile, m_IPVertex.pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive, &dwByte, nullptr);

	//Safe_Delete_Array(m_IPVertex.pVertices);
	//Safe_Delete_Array(m_IPVertex.pIndices);

	for (auto& pIPMC : m_vecIPMeshContainer)
	{
		WriteFile(hFile, &pIPMC->iNumVertices, sizeof(_uint), &dwByte, nullptr);
		WriteFile(hFile, &pIPMC->iStartFaceIndex, sizeof(_uint), &dwByte, nullptr);
		WriteFile(hFile, &pIPMC->iNumFaces, sizeof(_uint), &dwByte, nullptr);
		WriteFile(hFile, &pIPMC->iStartVertexIndex, sizeof(_uint), &dwByte, nullptr);
		WriteFile(hFile, &pIPMC->iMaterialIndex, sizeof(_uint), &dwByte, nullptr);
		WriteFile(hFile, &pIPMC->iNumBones, sizeof(_uint), &dwByte, nullptr);

		for (auto& pBoneName : pIPMC->vecBoneName)
		{
			DWORD	nameLen = (DWORD)strlen(pBoneName);

			WriteFile(hFile, &nameLen, sizeof(DWORD), &dwByte, nullptr);
			WriteFile(hFile, pBoneName, sizeof(char) * nameLen, &dwByte, nullptr);
		}
	}
	for (auto& pIPMat : m_vecIPMaterial)
	{
		DWORD iCount = (DWORD)pIPMat->vecTexDesc.size();

		WriteFile(hFile, &iCount, sizeof(DWORD), &dwByte, nullptr);

		for (DWORD i = 0; i < iCount; ++i)
		{
			//WriteFile(hFile, pIPMat->vecTexDesc[i], sizeof(IPMaterial::TEXDESC), &dwByte, nullptr);
			WriteFile(hFile, &(pIPMat->vecTexDesc)[i], sizeof(IPMaterial::TEXDESC), &dwByte, nullptr);
		}

		//for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
		//{
		//	//if (nullptr == pIPMat->pMaterialTextures[i])
		//	//{
		//	//	continue;
		//	//}

		//	DWORD	nameLen = (DWORD)strlen(pIPMat->pMaterialTextures[i]->szMaterialTexturePath);

		//	WriteFile(hFile, &nameLen, sizeof(DWORD), &dwByte, nullptr);
		//	WriteFile(hFile, pIPMat->pMaterialTextures[i]->szMaterialTexturePath, sizeof(char) * nameLen, &dwByte, nullptr);

		//	int check = 0;
		//}
		int check = 0;
	}

	for (auto& pIPHNode : m_vecIPHNode)
	{
		DWORD		nameLen = 0;

		nameLen = (DWORD)strlen(pIPHNode->szParentName);
		WriteFile(hFile, &nameLen, sizeof(DWORD), &dwByte, nullptr);
		WriteFile(hFile, pIPHNode->szParentName, sizeof(char) * nameLen, &dwByte, nullptr);

		nameLen = (DWORD)strlen(pIPHNode->szName);
		WriteFile(hFile, &nameLen, sizeof(DWORD), &dwByte, nullptr);
		WriteFile(hFile, pIPHNode->szName, sizeof(char) * nameLen, &dwByte, nullptr);

		WriteFile(hFile, &pIPHNode->TransformationMatrix, sizeof(_matrix), &dwByte, nullptr);
		WriteFile(hFile, &pIPHNode->OffsetMatrix, sizeof(_matrix), &dwByte, nullptr);
		WriteFile(hFile, &pIPHNode->iDepth, sizeof(_uint), &dwByte, nullptr);
		WriteFile(hFile, &pIPHNode->iNumChildren, sizeof(_uint), &dwByte, nullptr);
	}
	for (auto& pIPAnim : m_vecIPAnim)
	{
		WriteFile(hFile, &pIPAnim->iDuration, sizeof(_double), &dwByte, nullptr);
		WriteFile(hFile, &pIPAnim->iTicksPerSecond, sizeof(_double), &dwByte, nullptr);
		WriteFile(hFile, &pIPAnim->iNumChannels, sizeof(_uint), &dwByte, nullptr);

		DWORD		nameLen = 0;

		nameLen = (DWORD)strlen(pIPAnim->szName);
		WriteFile(hFile, &nameLen, sizeof(DWORD), &dwByte, nullptr);
		WriteFile(hFile, pIPAnim->szName, sizeof(char) * nameLen, &dwByte, nullptr);
	}

	_uint iNumChannel = (_uint)m_vecIPChannel.size();
	WriteFile(hFile, &iNumChannel, sizeof(_uint), &dwByte, nullptr);

	for (auto& pIPChan : m_vecIPChannel)
	{
		DWORD		nameLen = 0;

		nameLen = (DWORD)strlen(pIPChan->szName);
		WriteFile(hFile, &nameLen, sizeof(DWORD), &dwByte, nullptr);
		WriteFile(hFile, &pIPChan->szName, sizeof(char) * nameLen, &dwByte, nullptr);

		nameLen = (DWORD)strlen(pIPChan->szTargetName);
		WriteFile(hFile, &nameLen, sizeof(DWORD), &dwByte, nullptr);
		WriteFile(hFile, &pIPChan->szTargetName, sizeof(char) * nameLen, &dwByte, nullptr);

		DWORD	iNumKeyFrame = (DWORD)pIPChan->vecKeyFrames.size();
		WriteFile(hFile, &iNumKeyFrame, sizeof(DWORD), &dwByte, nullptr);

		for (auto& pKeyFrame : pIPChan->vecKeyFrames)
		{
			WriteFile(hFile, pKeyFrame, sizeof(KEYFRAME), &dwByte, nullptr);
		}
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CJussimpModel::Free_Param()
{
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

	if (false == m_bCloned)
	{
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
	}
	return S_OK;
}

CAnimation * CJussimpModel::Find_Animation(const char * pAnimName)
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

CHierarchyNode * CJussimpModel::Find_HierarchyNode(const char * pBoneName)
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

HRESULT CJussimpModel::Bind_Buffers()
{
	_uint		iOffset = 0;

	m_pContextDevice->IASetVertexBuffers(0, 1, &m_pVB, &m_iStride, &iOffset);
	m_pContextDevice->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pContextDevice->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return S_OK;
}
HRESULT CJussimpModel::Set_Animation(_uint iAnimIndex)
{
	if (iAnimIndex >= m_Animations.size())
		return E_FAIL;

	m_iCurrentAnimationIndex = iAnimIndex;

	return S_OK;
}

HRESULT CJussimpModel::Set_MaterialTexture(_uint iMeshContainerIndex, aiTextureType eTextureType, char * pContantName)
{
	_uint iMaterialIndex = m_MeshContainers[iMeshContainerIndex]->Get_MaterialIndex();

	CTexture*		pTexture = m_Materials[iMaterialIndex]->pMaterialTextures[eTextureType];
	if (nullptr == pTexture)
		return E_FAIL;

	return SetUp_Texture(pContantName, pTexture->Get_SRV(0));
}

void CJussimpModel::Update_CombinedTransformationMatrix(_double TimeDelta)
{
	CAnimation*	pCurrentAnim = nullptr;

	pCurrentAnim = m_Animations[m_iCurrentAnimationIndex];

	if (nullptr == pCurrentAnim)
	{
		MSGBOX("There is no Animation");
		return;
	}

	pCurrentAnim->Update_TransformationMatrix(TimeDelta, m_bContinue);

	for (auto& pHierarchyNode : m_HierarchyNodes)
	{
		pHierarchyNode->Update_CombinedTransformationMatrix(m_iCurrentAnimationIndex);
	}
}

HRESULT CJussimpModel::Render(_uint iMeshIndex, _uint iPassIndex)
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

		if (FAILED(SetUp_RawValue("g_BoneMatrices", BoneMatrices, sizeof(_float4x4) * 128)))
		{
			return E_FAIL;
		}
	}

	m_vecPassDesc[iPassIndex]->pEffectPass->Apply(0, m_pContextDevice);

	m_MeshContainers[iMeshIndex]->Render();

	return S_OK;
}
