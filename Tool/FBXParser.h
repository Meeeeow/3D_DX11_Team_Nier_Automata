#pragma once

#include "VIBuffer.h"

#include "Animation.h"
#include "HierarchyNode.h"

#include "../Reference/Headers/assimp/scene.h"
#include "../Reference/Headers/assimp/Importer.hpp"
#include "../Reference/Headers/assimp/postprocess.h"

class CFBXParser final : public CVIBuffer
{
public:
	CFBXParser(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual ~CFBXParser() DEFAULT;

public:
	typedef vector<class CMeshContainer*>		MESHCONTAINERS;
	typedef vector<MESHMATERIAL*>				MATERIALS;
	typedef vector<CHierarchyNode*>				HIERARCHYNODE;
	typedef vector<CAnimation*>					ANIMATIONS;
		

public:
	HRESULT						NativeConstruct_Prototype(const char* pModelFilePath, const char* pModelFileName, const wchar_t* pExportDatFilePath);

	HRESULT						MultiThreaFunction();

	HRESULT						Create_MeshContainer();
	HRESULT						Compile_Shader(const _tchar* pShaderFilePath, const char* pTechniqueName);
	HRESULT						Create_Materials(const char* pMeshFilePath);
	HRESULT						Create_SkinnedMesh();
	HRESULT						Create_VIBuffer();
	HRESULT						Create_HierarchyNodes(aiNode* pNode, CHierarchyNode* pParent, _uint iDepth);
	HRESULT						Create_HierarchyNodeMeshContainer();
	HRESULT						Create_Animation();

public:
	CRITICAL_SECTION			Get_CS() { return m_CS; }


private:
	IPGeneral					m_IPGeneral;
	vector<IPMeshContainer*>	m_vecIPMeshContainer;
	vector<IPMaterial*>			m_vecIPMaterial;
	vector<IPHNode*>			m_vecIPHNode;
	vector<IPAnim*>				m_vecIPAnim;
	vector<IPChannel*>			m_vecIPChannel;

	HANDLE						m_hThread = 0;
	CRITICAL_SECTION			m_CS;
	_bool						m_bFinished = false;

	MESHCONTAINERS				m_vecMeshContainers;
	MATERIALS					m_vecMaterials;
	HIERARCHYNODE				m_vecHierarchyNodes;
	ANIMATIONS					m_vecAnimations;

private:
	const aiScene*				m_pScene = nullptr;
	Assimp::Importer			m_Importer;

public:
	static	CFBXParser*			Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice
		, const char* pModelFilePath, const char* pModelFileName, const wchar_t* pExportDatFilePath);
	virtual CComponent*			Clone(void* pArg) override;
	virtual void				Free() override;



};
