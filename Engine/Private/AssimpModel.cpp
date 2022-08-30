#include "..\Public\AssimpModel.h"
#include "Transform.h"
#include "MeshContainer.h"
#include "Texture.h"

CAssimpModel::CAssimpModel(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
{
}

CAssimpModel::CAssimpModel(const CAssimpModel & rhs)
	: CVIBuffer(rhs)
	, m_pScene(rhs.m_pScene)	
	, m_Materials(rhs.m_Materials)
	, m_PivotMatrix(rhs.m_PivotMatrix)
	, m_pAnimationPath(rhs.m_pAnimationPath)
	//, m_HierarchyNodes(rhs.m_HierarchyNodes)
	//, m_Animations(rhs.m_Animations)
{
	//for (auto& pHierarchyNode : m_HierarchyNodes)
	//	Safe_AddRef(pHierarchyNode);

	//for (auto& pAnimation : m_Animations)
	//	Safe_AddRef(pAnimation);

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
}

_matrix CAssimpModel::Get_BoneMatrix(const _tchar* pNodeTag)
{
	auto	iter = find_if(m_RefNodes.begin(), m_RefNodes.end(), CTagFinder(pNodeTag));
	if (iter == m_RefNodes.end())
		return XMMatrixIdentity();

	_matrix		OffsetMatrix = iter->second->Get_OffsetMatrix();
	_matrix		CombinedMatrix = iter->second->Get_CombinedTransformationMatrix();

	return OffsetMatrix * CombinedMatrix * XMLoadFloat4x4(&m_PivotMatrix);
}

_int CAssimpModel::Find_AnimationIndex(const char* pTag)
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

HRESULT CAssimpModel::NativeConstruct_Prototype(TYPE eMeshType, const char * pModelFilePath, const char * pModelFileName, const _tchar* pShaderFilePath, const char* pTechniqueName, _fmatrix PivotMatrix, const _tchar* pAnimPath)
{
	char		szFullPath[MAX_PATH] = "";

	strcpy_s(szFullPath, pModelFilePath);
	strcat_s(szFullPath, pModelFileName);

	m_pAnimationPath = pAnimPath;

	_uint			iFlag = 0;

	if (eMeshType == TYPE::NONANIM) {
		iFlag = aiProcess_PreTransformVertices | aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_ConvertToLeftHanded;
		m_bAnimation = false;
	}
	else {
		iFlag = aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_ConvertToLeftHanded;
		m_bAnimation = true;
	}

	m_pScene = m_Importer.ReadFile(szFullPath, iFlag);
	if (nullptr == m_pScene)
		return E_FAIL;

	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

	if (FAILED(Create_MeshContainer()))
		return E_FAIL;

	if (FAILED(Compile_Shader(pShaderFilePath, pTechniqueName)))
		return E_FAIL;

	if (FAILED(Create_Materials(pModelFilePath)))
		return E_FAIL;

	if (true == m_pScene->HasAnimations())
	{
		if (FAILED(Create_SkinnedMesh()))
			return E_FAIL;
	}

	if (FAILED(Create_VIBuffer()))
		return E_FAIL;

	return S_OK;
}

HRESULT CAssimpModel::NativeConstruct(void * pArg)
{
	if (false == m_pScene->HasAnimations())
		return S_OK;

	if (FAILED(Create_HierarchyNodes(m_pScene->mRootNode, nullptr, 0)))
		return E_FAIL;

	sort(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [](CHierarchyNode* pSour, CHierarchyNode* pDest)
	{
		return pSour->Get_Depth() < pDest->Get_Depth();
	});

	if (FAILED(SetUp_HierarchyNodeToMeshContainer()))
		return E_FAIL;

	if (nullptr != m_pAnimationPath)
	{ 
		if (FAILED(Create_Animation_FromPath()))
			return E_FAIL;
	}
	else
	{
		if (FAILED(Create_Animation()))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CAssimpModel::NativeConstruct_Prototype_ForTool(TYPE eMeshType, const char * pModelFilePath, const char * pModelFileName, const _tchar * pShaderFilePath, const char * pTechniqueName, _fmatrix PivotMatrix)
{
	char		szFullPath[MAX_PATH] = "";

	strcpy_s(szFullPath, pModelFilePath);
	strcat_s(szFullPath, pModelFileName);
	_uint			iFlag = 0;

	if (eMeshType == TYPE::NONANIM) {
		iFlag = aiProcess_PreTransformVertices | aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_ConvertToLeftHanded;
		m_bAnimation = false;
	}
	else {
		iFlag = aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_ConvertToLeftHanded;
		m_bAnimation = true;
	}

	m_pScene = m_Importer.ReadFile(szFullPath, iFlag );
	if (nullptr == m_pScene)
		return E_FAIL;

	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

	if (FAILED(Create_MeshContainer()))
		return E_FAIL;

	if (FAILED(Compile_Shader(pShaderFilePath, pTechniqueName)))
		return E_FAIL;

	if (FAILED(Create_Materials(pModelFilePath)))
		return E_FAIL;

	if (false == m_pScene->HasAnimations())
	{
		if (FAILED(Create_VIBuffer()))
			return E_FAIL;

		return S_OK;
	}

	if (FAILED(Create_SkinnedMesh()))
		return E_FAIL;

	if (FAILED(Create_VIBuffer()))
		return E_FAIL;

	if (FAILED(Create_HierarchyNodes(m_pScene->mRootNode, nullptr, 0)))
		return E_FAIL;

	sort(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [](CHierarchyNode* pSour, CHierarchyNode* pDest)
	{
		return pSour->Get_Depth() < pDest->Get_Depth();
	});

	if (FAILED(SetUp_HierarchyNodeToMeshContainer()))
		return E_FAIL;

	if (nullptr != m_pAnimationPath)
	{
		if (FAILED(Create_Animation_FromPath()))
			return E_FAIL;
	}
	else
	{
		if (FAILED(Create_Animation()))
			return E_FAIL;
	}

	return S_OK;

	
}

HRESULT CAssimpModel::Render(_uint iMeshIndex, _uint iPassIndex)
{

	if (iPassIndex >= m_vecPassDesc.size())
		return E_FAIL;
	m_pContextDevice->IASetInputLayout(m_vecPassDesc[iPassIndex]->pInputLayout);

	if(false == m_Animations.empty())
	{
		/* 현재 메시(모델x)에 영향을 주는 뼈들의 행렬 집합. */
		_matrix		BoneMatrices[128];
		ZeroMemory(BoneMatrices, sizeof(_matrix) * 128);

		m_MeshContainers[iMeshIndex]->SetUp_CombinedMatrices(BoneMatrices, XMLoadFloat4x4(&m_PivotMatrix));
	
		if (FAILED(SetUp_RawValue("g_tBoneMatrices", BoneMatrices, sizeof(_float4x4) * 128)))
			return E_FAIL;
	}
	else
	{
		_matrix		BoneMatrices[128];
		ZeroMemory(BoneMatrices, sizeof(_matrix) * 128);

		BoneMatrices[0] = XMMatrixIdentity();

		if (FAILED(SetUp_RawValue("g_tBoneMatrices", BoneMatrices, sizeof(_float4x4) * 128)))
			return E_FAIL;
	}
	/* 쉐이터 패스를 시작한다. */
	m_vecPassDesc[iPassIndex]->pEffectPass->Apply(0, m_pContextDevice);

	m_MeshContainers[iMeshIndex]->Render();	

	return S_OK;
}

HRESULT CAssimpModel::Bind_Buffers()
{
	/* 하나의 버퍼를 바인드한다로 가정. */
	/* 다수버퍼를 사용해야하는 경우, 오버라ㅇㅣ드하여 자식에서 호출. */
	_uint		iOffset = 0;

	m_pContextDevice->IASetVertexBuffers(0, 1, &m_pVB, &m_iStride, &iOffset);
	m_pContextDevice->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pContextDevice->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return S_OK;
}

HRESULT CAssimpModel::Compile_Shader(const _tchar * pShaderFilePath, const char * pTechniqueName)
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

HRESULT CAssimpModel::Set_MaterialTexture(_uint iMeshContainerIndex, aiTextureType eTextureType, char * pContantName)
{
	_uint iMaterialIndex = m_MeshContainers[iMeshContainerIndex]->Get_MaterialIndex();

	CTexture*		pTexture = m_Materials[iMaterialIndex]->pMaterialTextures[eTextureType];
	if (nullptr == pTexture)
		return E_FAIL;

	return SetUp_Texture(pContantName, pTexture->Get_SRV());
}

HRESULT CAssimpModel::Create_HierarchyNodes(aiNode* pNode, CHierarchyNode* pParent, _uint iDepth)
{
	_matrix		TransformationMatrix;
	memcpy(&TransformationMatrix, &pNode->mTransformation, sizeof(_matrix));

	CHierarchyNode*		pHierarchyNode = CHierarchyNode::Create(pNode->mName.data, TransformationMatrix, pParent, iDepth++);
	if (nullptr == pHierarchyNode)
		return E_FAIL;

	pHierarchyNode->Reserve_Channels(m_pScene->mNumAnimations);

	m_HierarchyNodes.push_back(pHierarchyNode);

	for (_uint i = 0; i < pNode->mNumChildren; ++i)
	{
		if (FAILED(Create_HierarchyNodes(pNode->mChildren[i], pHierarchyNode, iDepth)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CAssimpModel::Create_SkinnedMesh()
{
	/* 이뼈의 행렬이 영향을 주는 정점에 적용되야할 비율(최대1)*/
	// m_pScene->mMeshes[0]->mBones[0]->mWeights

	for (_uint i = 0; i < m_pScene->mNumMeshes; ++i)
	{
		aiMesh*		pMesh = m_pScene->mMeshes[i];

		CMeshContainer*	pMeshContainer = m_MeshContainers[i];

		for (_uint j = 0; j < pMesh->mNumBones; ++j)
		{
			aiBone*		pBone = pMesh->mBones[j];	

			/* 이 뼈가 영향을 주는 정점의 갯수만큼 순회한다. */
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

HRESULT CAssimpModel::SetUp_HierarchyNodeToMeshContainer()
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

			pHierarchyNode->Set_OffsetMatrix(XMMatrixTranspose(OffsetMatrix));

			pMeshContainer->Add_HierarchyNode(pHierarchyNode);
			Safe_AddRef(pHierarchyNode);		
		}
	}

	return S_OK;
}

HRESULT CAssimpModel::Create_Animation()
{
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
		}

		m_Animations.push_back(pAnimation);
	}	

	return S_OK;
}

HRESULT CAssimpModel::Create_Animation_FromPath()
{
	m_Animations.reserve(m_pScene->mNumAnimations);

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

	for (_uint i = 0; i < m_pScene->mNumAnimations; ++i)
	{
		char   szTag[MAX_PATH];
		ZeroMemory(szTag, MAX_PATH * sizeof(char));

		_double PlaySpeedPerSec = 0.0;

		_double LinkMinRatio = 0.0;
		_double LinkMaxRatio = 0.0; 

		ReadFile(hFile, szTag, MAX_PATH * sizeof(char), &dwByte, nullptr); 
		ReadFile(hFile, &PlaySpeedPerSec, sizeof(_double), &dwByte, nullptr);
		ReadFile(hFile, &LinkMinRatio, sizeof(_double), &dwByte, nullptr);
		ReadFile(hFile, &LinkMaxRatio, sizeof(_double), &dwByte, nullptr);

		_uint a = 0;
		if (0.01 < LinkMinRatio)
			a = 1;

		aiAnimation*	pAnim = m_pScene->mAnimations[i];
		if (nullptr == pAnim)
			return E_FAIL;

		CAnimation*		pAnimation = CAnimation::Create(pAnim->mDuration, pAnim->mTicksPerSecond);
		if (nullptr == pAnimation)
			return E_FAIL;

		if (!strcmp(szTag, pAnim->mName.data))
		{
			pAnimation->Set_PlaySpeedPerSec(PlaySpeedPerSec);
			pAnimation->Set_LinkMinRatio(LinkMinRatio);
			pAnimation->Set_LinkMaxRatio(LinkMaxRatio);
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

		pAnimation->Set_AnimationName(pAnim->mName.data);

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
		}

		m_Animations.push_back(pAnimation);
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CAssimpModel::Add_RefNode(const _tchar* pNodeTag, const char * pBoneName)
{
	CHierarchyNode*	pNode = Find_HierarchyNode(pBoneName);
	if (nullptr == pNode)
		return E_FAIL;

	m_RefNodes.emplace(pNodeTag, pNode);

	Safe_AddRef(pNode);

	return S_OK;
}

HRESULT CAssimpModel::Set_Animation(_uint iAnimIndex, _bool bContinue)
{
	if (iAnimIndex >= m_Animations.size())
		return E_FAIL;

	m_bContinue = bContinue;

	if (!m_bAnimationBlend)
		m_iNextAnimationIndex = iAnimIndex;

	if (m_iCurrentAnimationIndex != m_iNextAnimationIndex)
	{	
		if (!m_bAnimationBlend)
		{
			m_Animations[m_iCurrentAnimationIndex]->Set_Blend(true);
			m_Animations[m_iCurrentAnimationIndex]->Set_NextAnimChannel(m_Animations[m_iNextAnimationIndex]->Get_Channels());
			
			m_Animations[m_iNextAnimationIndex]->Set_OriginUpdate(false);
			m_Animations[m_iNextAnimationIndex]->Initialize_Time();
			m_Animations[m_iNextAnimationIndex]->Initialize_Blend();
			m_Animations[m_iNextAnimationIndex]->Set_RootMotionPos();

			m_bAnimationBlend = true;
		}

		if (0.25 <= m_Animations[m_iCurrentAnimationIndex]->Get_BlendTime())
		{
			m_Animations[m_iCurrentAnimationIndex]->Initialize_Blend();

			m_Animations[m_iCurrentAnimationIndex]->Initialize_Time();
			m_Animations[m_iCurrentAnimationIndex]->Set_OriginUpdate(false);
			m_Animations[m_iCurrentAnimationIndex]->Set_RootMotionPos();

			m_Animations[m_iNextAnimationIndex]->Initialize_Time();
			m_Animations[m_iNextAnimationIndex]->Set_OriginUpdate(false);
			m_Animations[m_iNextAnimationIndex]->Set_RootMotionPos();

			m_vPreRootMotion = _float4(0.f, 0.f, 0.f, 1.f);
			
			m_iCurrentAnimationIndex = m_iNextAnimationIndex;

			m_bAnimationBlend = false;
		}
	}

	return S_OK;
}

HRESULT CAssimpModel::Set_Animation_Tool(_uint iAnimIndex)
{
	if (iAnimIndex >= m_Animations.size())
		return E_FAIL;

		m_iCurrentAnimationIndex = iAnimIndex;

	return S_OK;
}

void CAssimpModel::Update_CombinedTransformationMatrix(_double TimeDelta)
{
	/* 현재애니메이션의 재생상태에 따른 뼈의 TransformationMatrix를 갱신한다. */
	m_Animations[m_iCurrentAnimationIndex]->Update_TransformationMatrix(TimeDelta, m_bContinue);

	/* 뼈의 상속순서대로 행렬을 누적시키낟. */
	for (auto& pHierarchyNode : m_HierarchyNodes)
		pHierarchyNode->Update_CombinedTransformationMatrix(m_iCurrentAnimationIndex);
}

void CAssimpModel::Update_CombinedTransformationMatrixNonContinue_Tool(_double TimeDelta)
{
	m_Animations[m_iCurrentAnimationIndex]->Update_CombinedTransformationMatrixNonContinue_Tool(TimeDelta);

	/* 뼈의 상속순서대로 행렬을 누적시키낟. */
	for (auto& pHierarchyNode : m_HierarchyNodes)
		pHierarchyNode->Update_CombinedTransformationMatrix(m_iCurrentAnimationIndex);
}

const _float4 CAssimpModel::Get_RootMotion() const
{
	_float4 vPos = m_Animations[m_iCurrentAnimationIndex]->Get_RootMotionPos();

	_float4 vResult = _float4(0.f, 0.f, 0.f, 1.f);
	XMStoreFloat4(&vResult, XMVector3TransformCoord(XMLoadFloat4(&vPos), XMLoadFloat4x4(&m_PivotMatrix)));

	return vResult;
}

void CAssimpModel::IsOriginUpdate_Initialize()
{
	if (m_Animations[m_iCurrentAnimationIndex]->Get_OriginUpdate())
	{
		Initialize_RootMotion();
		Set_OriginUpdate(false);
	}
}

const _float4 CAssimpModel::Get_BeginRootBone_Position() const
{
	return m_Animations[m_iCurrentAnimationIndex]->Get_BeginRootBone_Position();
}

void CAssimpModel::Release_RefNode(const _tchar * pTag)
{
	if (nullptr == pTag)
		return;

	auto& iter = find_if(m_RefNodes.begin(), m_RefNodes.end(), CTagFinder(pTag));
	if (iter == m_RefNodes.end())
		return;

	Safe_Release(iter->second);
	m_RefNodes.erase(pTag);
}

void CAssimpModel::Initialize_Time()
{
	m_Animations[m_iCurrentAnimationIndex]->Initialize_Time();
}

const _bool CAssimpModel::IsFinish_Animation() const
{
	return m_Animations[m_iCurrentAnimationIndex]->IsFinish_Animation();
}

void CAssimpModel::Initialize_RootMotion()
{
	m_vPreRootMotion = _float4(0.f, 0.f, 0.f, 1.f);
}

void CAssimpModel::Synchronize_Animation(CTransform * pTransform)
{
	IsOriginUpdate_Initialize();

	_vector vLook = pTransform->Get_State(CTransform::STATE::LOOK);
	_vector vValue = XMVectorSet(0.f, 0.f, 0.f, 1.f);

	vValue = XMLoadFloat4(&Get_PreRootMotion()) - (XMLoadFloat4(&Get_RootMotion()));

	_vector vDist = XMVector3Length(vValue);

	vLook = XMVector3Normalize(vLook);

	vLook *= XMVectorGetX(vDist);

	pTransform->Set_State(CTransform::STATE::POSITION, pTransform->Get_State(CTransform::STATE::POSITION) + vLook);

	Set_PreRootMotion(Get_RootMotion());
}

HRESULT CAssimpModel::Create_MeshContainer()
{
	for (_uint i = 0; i < m_pScene->mNumMeshes; ++i)
	{
		m_iNumVertices += m_pScene->mMeshes[i]->mNumVertices;
		m_iNumPrimitive += m_pScene->mMeshes[i]->mNumFaces;
	}

	/* 모든 메시의 정점들을 하나의 버퍼에 담기위해 정점배열을 한셋트로 구성한다.ㅏ */
	m_pVertices = new VTXMESH[m_iNumVertices];
	ZeroMemory(m_pVertices, sizeof(VTXMESH) * m_iNumVertices);

	/* 모든 메시의 인덱스들을 하나의 버퍼에 담기위해 인ㄷ게스배열을 한셋트로 구성한다.ㅏ */
	m_pIndices = new FACEINDICES32[m_iNumPrimitive];
	ZeroMemory(m_pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);

	_uint	iIndex = 0;
	_uint	iFaceIndex = 0;
	_vector vecMin, vecMax;
	vecMin = vecMax = XMVectorSet(0.f, 0.f, 0.f, 0.f);

	for (_uint i = 0; i < m_pScene->mNumMeshes; ++i)
	{
		aiMesh*		pMesh = m_pScene->mMeshes[i];
		if (nullptr == pMesh)
			return E_FAIL;		

		/* iIndex : 정점배열이 모든 메시의 정점들의 합의 형태로 표현되어있는 상황에서. 
		현재 메시의 정점의 시작 인덱스 */
		/* iFaceIndex : 인덱스배열이 모든 메시의 인ㄷ게스들의 합의 형태로 표현되어있는 상황에서.
		현재 메시의 폴리곤(페이스)의 시작 인덱스 */
		CMeshContainer*		pMeshContainer = CMeshContainer::Create(m_pGraphicDevice, m_pContextDevice, iIndex, pMesh->mNumVertices, iFaceIndex, pMesh->mNumFaces, pMesh->mMaterialIndex);

		for (_uint j = 0; j < pMesh->mNumVertices; ++j)
		{
			memcpy(&((VTXMESH*)m_pVertices)[iIndex].vPosition, &pMesh->mVertices[j], sizeof(_float3));

			if(false == m_pScene->HasAnimations())
				XMStoreFloat3(&((VTXMESH*)m_pVertices)[iIndex].vPosition, XMVector3TransformCoord(XMLoadFloat3(&((VTXMESH*)m_pVertices)[iIndex].vPosition), XMLoadFloat4x4(&m_PivotMatrix)));

			memcpy(&((VTXMESH*)m_pVertices)[iIndex].vNormal, &pMesh->mNormals[j], sizeof(_float3));
			if (false == m_pScene->HasAnimations())
				XMStoreFloat3(&((VTXMESH*)m_pVertices)[iIndex].vNormal, XMVector3TransformNormal(XMLoadFloat3(&((VTXMESH*)m_pVertices)[iIndex].vNormal), XMLoadFloat4x4(&m_PivotMatrix)));

			memcpy(&((VTXMESH*)m_pVertices)[iIndex].vTexUV, &pMesh->mTextureCoords[0][j], sizeof(_float2));					
			memcpy(&((VTXMESH*)m_pVertices)[iIndex].vTangent, &pMesh->mTangents[j], sizeof(_float3));

			vecMax = XMVectorMax(vecMax, XMLoadFloat3(&((VTXMESH*)m_pVertices)[iIndex].vPosition));
			vecMin = XMVectorMin(vecMin, XMLoadFloat3(&((VTXMESH*)m_pVertices)[iIndex].vPosition));

			

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

	XMStoreFloat4(&m_vecMaxPos, vecMax);
	XMStoreFloat4(&m_vecMinPos, vecMin);

	return S_OK;
}

HRESULT CAssimpModel::Create_VIBuffer()
{
	m_iStride = sizeof(VTXMESH);

	ZeroMemory(&m_tVB_Desc, sizeof(D3D11_BUFFER_DESC));

	m_tVB_Desc.ByteWidth = m_iStride * m_iNumVertices;
	m_tVB_Desc.Usage = D3D11_USAGE_IMMUTABLE;
	m_tVB_Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_tVB_Desc.CPUAccessFlags = 0;
	m_tVB_Desc.MiscFlags = 0;
	m_tVB_Desc.StructureByteStride = m_iStride;

	m_eRenderType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	ZeroMemory(&m_tVB_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_tVB_SubResourceData.pSysMem = m_pVertices;


	if (FAILED(CVIBuffer::Create_VertexBuffer()))
		return E_FAIL;

	m_iNumIndicesPerPT = 3; /* 도형하나그릴때 필요한 인덱스 갯수. */
	m_iFaceIndexSize = sizeof(FACEINDICES32);

	m_eIndexFormat = DXGI_FORMAT_R32_UINT;

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

HRESULT CAssimpModel::Create_Materials(const char * pMeshFilePath)
{
	_uint	iNumMaterials = m_pScene->mNumMaterials;

	m_Materials.reserve(iNumMaterials);

	for (_uint i = 0; i < iNumMaterials; ++i)
	{
		aiMaterial*		pMaterial = m_pScene->mMaterials[i];
		if (nullptr == pMaterial)
			return E_FAIL;		
		
		MESHMATERIAL*	pMeshMaterial = new MESHMATERIAL;
		ZeroMemory(pMeshMaterial, sizeof(MESHMATERIAL));

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

			if (0 == strlen(szExt))
				strcpy_s(szExt, ".dds");

			strcat_s(szFullPath, szExt);

			_tchar		szMaterialPath[MAX_PATH] = TEXT("");

			MultiByteToWideChar(CP_ACP, 0, szFullPath, (_int)strlen(szFullPath), szMaterialPath, 256);

			CTexture*		pTexture = CTexture::Create(m_pGraphicDevice, m_pContextDevice, szMaterialPath);
			if (nullptr == pTexture)
				return E_FAIL;

			pMeshMaterial->pMaterialTextures[j] = pTexture;
		}

		m_Materials.push_back(pMeshMaterial);
	}

	return S_OK;
}

CHierarchyNode * CAssimpModel::Find_HierarchyNode(const char * pBoneName)
{
	auto	iter = find_if(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [&](CHierarchyNode* pNode)
	{
		return !strcmp(pNode->Get_Name(), pBoneName);	
	});

	if (iter == m_HierarchyNodes.end())
		return nullptr;

	return *iter;
}

CAssimpModel * CAssimpModel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, TYPE eMeshType, const char * pModelFilePath, const char * pModelFileName, const _tchar* pShaderFilePath, const char* pTechniqueName, _fmatrix PivotMatrix, const _tchar* pAnimPath)
{
	CAssimpModel* pInstance = new CAssimpModel(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(eMeshType, pModelFilePath, pModelFileName, pShaderFilePath, pTechniqueName, PivotMatrix, pAnimPath)))
	{
		MSGBOX("Failed To Creating CAssimpModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CAssimpModel * CAssimpModel::Create_ForTool(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, TYPE eMeshType, const char * pModelFilePath, const char * pModelFileName, const _tchar * pShaderFilePath, const char * pTechniqueName, _fmatrix PivotMatrix)
{
	CAssimpModel* pInstance = new CAssimpModel(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype_ForTool(eMeshType, pModelFilePath, pModelFileName, pShaderFilePath, pTechniqueName, PivotMatrix)))
		Safe_Release(pInstance);

	return pInstance;
}


CComponent * CAssimpModel::Clone(void * pArg)
{
	CAssimpModel* pInstance = new CAssimpModel(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed To Cloning CAssimpModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAssimpModel::Free()
{
	__super::Free();

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
