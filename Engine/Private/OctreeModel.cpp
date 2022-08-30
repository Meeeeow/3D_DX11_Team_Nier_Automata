#include "..\Public\OctreeModel.h"
#include "GameInstance.h"
#include "OcMeshContainer.h"
#include "Texture.h"

COctreeModel::COctreeModel(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CVIBuffer(pGraphicDevice, pContextDevice)
{

}

COctreeModel::COctreeModel(const COctreeModel & rhs)
	: CVIBuffer(rhs), m_Materials(rhs.m_Materials), m_PivotMatrix(rhs.m_PivotMatrix), m_IPGeneral(rhs.m_IPGeneral)
{
	for (auto& pMeshContainer_Prototype : rhs.m_MeshContainers)
	{
		COcMeshContainer*	pMeshContainer = pMeshContainer_Prototype->Clone();
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

HRESULT COctreeModel::NativeConstruct_Prototype(const _tchar * pDatFilePath, const _tchar * pShaderFilePath, const char * pTechniqueName)
{
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
	if (FAILED(Create_Materials()))
	{
		return E_FAIL;
	}
	if (FAILED(Create_VIBuffer()))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT COctreeModel::NativeConstruct(void * pArg)
{
	if (0 == m_IPGeneral.iNumAnimations)
	{
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
	if (FAILED(Create_Animation()))
	{
		return E_FAIL;
	}


	return S_OK;
}

HRESULT COctreeModel::Import_Parameter(const _tchar * pDatFilePath)
{
	HANDLE	hFile = nullptr;

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

	CloseHandle(hFile);

	return S_OK;
}

HRESULT COctreeModel::Free_Param()
{
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

HRESULT COctreeModel::Create_MeshContainer()
{
	for (_uint i = 0; i < m_IPGeneral.iNumMeshes; ++i)
	{
		IPMeshContainer*	pImportMC = m_vecIPMeshContainer[i];

		COcMeshContainer*	pMeshContainer = COcMeshContainer::Create(m_pGraphicDevice, m_pContextDevice
			, pImportMC->iStartVertexIndex, pImportMC->iNumVertices
			, pImportMC->iStartFaceIndex, pImportMC->iNumFaces, pImportMC->iMaterialIndex);

		if (nullptr == pMeshContainer)
		{
			return E_FAIL;
		}
		m_MeshContainers.push_back(pMeshContainer);
	}
	return S_OK;
}

HRESULT COctreeModel::Compile_Shader(const _tchar * pShaderFilePath, const char * pTechniqueName)
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

HRESULT COctreeModel::Create_Materials()
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

			wstring wstr(szMaterialPath);
			wstring::size_type wstrRemover;
			wstrRemover = wstr.find(L"Client\\", 0);
			wstr.erase(wstrRemover, 7);
			lstrcpy(szMaterialPath, wstr.c_str());

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

HRESULT COctreeModel::Create_VIBuffer()
{
	m_iNumVertices = m_IPGeneral.iNumVertices;
	m_iNumPrimitive = m_IPGeneral.iNumIndices;

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

HRESULT COctreeModel::Create_HierarchyNodes()
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

HRESULT COctreeModel::SetUp_HierarchyNodeToMeshContainer()
{
	for (_uint i = 0; i < m_IPGeneral.iNumMeshes; ++i)
	{
		IPMeshContainer*	pImportMeshCon = m_vecIPMeshContainer[i];

		COcMeshContainer*	pMeshContainer = m_MeshContainers[i];

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

HRESULT COctreeModel::Create_Animation()
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

CHierarchyNode * COctreeModel::Find_HierarchyNode(const char * pBoneName)
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

vector<CHierarchyNode*>* COctreeModel::Get_HierarchyNode()
{
	if (0 >= m_HierarchyNodes.size())
		return nullptr;

	return &m_HierarchyNodes;
}

HRESULT COctreeModel::Add_RefNode(const _tchar * pNodeTag, const char * pBoneName)
{
	CHierarchyNode*	pNode = Find_HierarchyNode(pBoneName);
	if (nullptr == pNode)
		return E_FAIL;

	m_RefNodes.emplace(pNodeTag, pNode);

	Safe_AddRef(pNode);

	return S_OK;
}

HRESULT COctreeModel::Bind_Buffers()
{
	_uint		iOffset = 0;

	m_pContextDevice->IASetVertexBuffers(0, 1, &m_pVB, &m_iStride, &iOffset);
	m_pContextDevice->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pContextDevice->IASetPrimitiveTopology(m_eRenderType);

	return S_OK;
}

HRESULT COctreeModel::Set_Animation(_uint iAnimIndex, _bool bContinue)
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

HRESULT COctreeModel::Set_MaterialTexture(_uint iMeshContainerIndex, aiTextureType eTextureType, char * pContantName)
{
	_uint iMaterialIndex = m_MeshContainers[iMeshContainerIndex]->Get_MaterialIndex();

	CTexture*		pTexture = m_Materials[iMaterialIndex]->pMaterialTextures[eTextureType];

	if (nullptr == pTexture)
	{
		return E_FAIL;
	}
	return SetUp_Texture(pContantName, pTexture->Get_SRV(0));
}

void COctreeModel::Update_CombinedTransformationMatrix(_double TimeDelta)
{
	m_Animations[m_iCurrentAnimationIndex]->Update_TransformationMatrix(TimeDelta, m_bContinue);

	for (auto& pHierarchyNode : m_HierarchyNodes)
		pHierarchyNode->Update_CombinedTransformationMatrix(m_iCurrentAnimationIndex);
}

void COctreeModel::Culling()
{
	for (auto& iter : m_MeshContainers)
		iter->Culling();
}

void COctreeModel::Culling_Additional()
{
	for (auto& iter : m_MeshContainers)
	{
		if (iter->Get_Culling() == false)
			continue;

		iter->Culling();
	}
}

HRESULT COctreeModel::Render(_uint iMeshIndex, _uint iPassIndex)
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
	}
	m_vecPassDesc[iPassIndex]->pEffectPass->Apply(0, m_pContextDevice);

	m_MeshContainers[iMeshIndex]->Render();

	return S_OK;
}

void COctreeModel::Reset_Culling()
{
	for (auto& pMesh : m_MeshContainers)
		pMesh->Set_Culling();
}

void COctreeModel::SetUp_BoundingBox(const _matrix & matWorld)
{
	_uint iSize = (_uint)m_vecIPMeshContainer.size();

	for (_uint i = 0; i < iSize; ++i)
	{
		_uint iStartVertexIndex = m_vecIPMeshContainer[i]->iStartVertexIndex;;
		_uint iEndVertexIndex = m_vecIPMeshContainer[i]->iStartVertexIndex + m_vecIPMeshContainer[i]->iNumVertices;

		_vector vMaxPoint = XMVectorZero(), vMinPoint = XMVectorZero();

		Compute_BoundingBoxPoint(iStartVertexIndex, iEndVertexIndex, vMaxPoint, vMinPoint, matWorld);

		m_MeshContainers[i]->SetUp_BoundingBoxPoint(vMaxPoint, vMinPoint);
	}

	_vector vModelMaxPoint = XMVectorZero(), vModelMinPoint = XMVectorZero();

	Compute_BoundingBoxPoint(0, m_iNumVertices, vModelMaxPoint, vModelMinPoint, matWorld);

	XMStoreFloat3(&m_vMinPoint, vModelMinPoint);
	XMStoreFloat3(&m_vMaxPoint, vModelMaxPoint);

	_float3 vCenter, vExtents;
	XMStoreFloat3(&vCenter, (vModelMinPoint + vModelMaxPoint) * 0.5f);
	XMStoreFloat3(&vExtents, (vModelMaxPoint - vModelMinPoint) * 0.5f);

	m_vMiddlePoint = vCenter;
	m_fRadius = XMVectorGetX(XMVector3Length(XMLoadFloat3(&vExtents)));
	m_pBoundingBox = new BoundingBox(vCenter, vExtents);

}


void COctreeModel::Compute_BoundingBoxPoint(const _uint & iStartVertexIndex, const _uint & iEndVertexIndex, _vector & vMaxPoint, _vector & vMinPoint, const _matrix & matWorld)
{
	for (_uint i = iStartVertexIndex; i < iEndVertexIndex; ++i)
	{
		vMinPoint = XMVectorMin(vMinPoint, XMLoadFloat3(&((VTXMESH*)m_pVertices)[i].vPosition));
		vMaxPoint = XMVectorMax(vMaxPoint, XMLoadFloat3(&((VTXMESH*)m_pVertices)[i].vPosition));
	}

	vMinPoint = XMVector4Transform(vMinPoint, matWorld);
	vMaxPoint = XMVector4Transform(vMaxPoint, matWorld);

}

_float3 COctreeModel::Get_MiddlePoint()
{
	return m_vMiddlePoint;
}

_float3 COctreeModel::Get_MinPoint()
{
	return m_vMinPoint;
}

_float3 COctreeModel::Get_MaxPoint()
{
	return m_vMaxPoint;
}

_float COctreeModel::Get_Radius()
{
	return m_fRadius;
}

BoundingBox * COctreeModel::Get_BoundingBox()
{
	return m_pBoundingBox;
}

COctreeModel * COctreeModel::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, const _tchar * pDatFilePath, const _tchar * pShaderFilePath, const char * pTechniqueName)
{
	COctreeModel* pInstance = new COctreeModel(pGraphicDevice, pContextDevice);
	if (FAILED(pInstance->NativeConstruct_Prototype(
		pDatFilePath, pShaderFilePath, pTechniqueName)))
		Safe_Release(pInstance);

	return pInstance;
}

COctreeModel * COctreeModel::Clone(void * pArg)
{
	COctreeModel* pInstance = new COctreeModel(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void COctreeModel::Free()
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

	if (m_bCloned)
	{
		Safe_Delete(m_pBoundingBox);
	}

}
