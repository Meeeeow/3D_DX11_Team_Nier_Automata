#pragma once

#include "VIBuffer.h"
#include "Animation.h"
#include "HierarchyNode.h"


/*
FBX to Client ±âº» Èå¸§
1. CExportModel
: fbx -> assimp -> data structure -> binary
2. CImportModel
: binary -> data structure -> Instance
*/

BEGIN(Engine)

class ENGINE_DLL CJussimpModel final : public CVIBuffer
{
public:
	enum class TYPE : _uint
	{
		NONANIM = 0,
		ANIM = 1,
		NONE = 2
	};

private:
	CJussimpModel(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CJussimpModel(const CJussimpModel& rhs);
	virtual ~CJussimpModel() = default;

public:
	static	CJussimpModel*	Create(ID3D11Device * pDevice
							, ID3D11DeviceContext * pDeviceContext
							, TYPE eMeshType
							, const char * pModelFilePath
							, const char * pModelFileName
							, const _tchar* pShaderFilePath
							, const char* pTechniqueName
							, _fmatrix PivotMatrix
							);
	static	CJussimpModel*	Create_ForTool(ID3D11Device * pDevice
							, ID3D11DeviceContext * pDeviceContext
							, TYPE eMeshType
							, const char * pModelFilePath
							, const char * pModelFileName
							, const _tchar* pShaderFilePath
							, const char* pTechniqueName
							, const wchar_t* pExportDatFilePath
							, _fmatrix PivotMatrix = XMMatrixIdentity()
							);
	virtual HRESULT			NativeConstruct_Prototype(TYPE eMeshType
							, const char* pModelFilePath
							, const char* pModelFileName
							, const _tchar* pShaderFilePath
							, const char* pTechniqueName
							, _fmatrix PivotMatrix
							);
	virtual CComponent*		Clone(void* pArg) override;
	virtual HRESULT			NativeConstruct(void* pArg) override;
	virtual HRESULT			NativeConstruct_Prototype_ForTool(TYPE eMeshType
							, const char* pModelFilePath
							, const char* pModelFileName
							, const _tchar* pShaderFilePath
							, const char* pTechniqueName
							, const wchar_t* pExportDatFilePath
							, _fmatrix PivotMatrix
							);
	virtual void			Free() override;

private:
	// For NativeConstruct_Prototype
	HRESULT Create_MeshContainer();
	HRESULT Compile_Shader(const _tchar* pShaderFilePath, const char* pTechniqueName);
	HRESULT Create_Materials(const char* pMeshFilePath);
	HRESULT Create_SkinnedMesh();
	HRESULT Create_VIBuffer();

	// For NativeConstruct
	HRESULT Create_HierarchyNodes(aiNode* pNode, CHierarchyNode* pParent, _uint iDepth);
	HRESULT SetUp_HierarchyNodeToMeshContainer();
	HRESULT Create_Animation();

public:
	// For Export
	HRESULT	Export_Parameter(const wchar_t* pDatName);
	HRESULT	Free_Param();

private:
	CAnimation*		Find_Animation(const char* pAnimName);
	CHierarchyNode* Find_HierarchyNode(const char* pBoneName);

public:
	// For Client
	HRESULT			Bind_Buffers();
	HRESULT			Set_Animation(_uint iAnimIndex);
	_uint			Get_NumMeshContainers() const { return (_uint)m_MeshContainers.size(); }
	HRESULT			Set_MaterialTexture(_uint iMeshContainerIndex, aiTextureType eTextureType, char* pContantName);
	void			Update_CombinedTransformationMatrix(_double TimeDelta);
	virtual HRESULT Render(_uint iMeshIndex, _uint iPassIndex);

private:
	// For Create
	const aiScene*		m_pScene = nullptr;
	Assimp::Importer	m_Importer;
	
	IPGeneral					m_IPGeneral;
	//IPVertex					m_IPVertex;
	vector<IPMeshContainer*>	m_vecIPMeshContainer;
	vector<IPMaterial*>			m_vecIPMaterial;
	vector<IPHNode*>			m_vecIPHNode;
	vector<IPAnim*>				m_vecIPAnim;
	vector<IPChannel*>			m_vecIPChannel;

	// For Instance
	_float4x4			m_PivotMatrix;
	_uint				m_iCurrentAnimationIndex = 0;

	vector<class CMeshContainer*>				m_MeshContainers;
	typedef vector<class CMeshContainer*>		MESHCONTAINERS;

	vector<MESHMATERIAL*>				m_Materials;
	typedef vector<MESHMATERIAL*>		MATERIALS;

	vector<CHierarchyNode*>				m_HierarchyNodes;
	typedef vector<CHierarchyNode*>		HIERARCHYNODE;

	vector<CAnimation*>					m_Animations;
	typedef vector<CAnimation*>			ANIMATIONS;

	unordered_map<const _tchar*, CHierarchyNode*>				m_RefNodes;
	typedef unordered_map<const _tchar*, CHierarchyNode*>		REFNODES;

	// For Animation
	_bool				m_bContinue = false;
};

END