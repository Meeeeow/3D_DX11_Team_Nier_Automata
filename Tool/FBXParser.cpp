#include "stdafx.h"
#include "FBXParser.h"
#include "Texture.h"

CFBXParser::CFBXParser(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CVIBuffer(pGraphicDevice, pContextDevice)
{
}

_uint APIENTRY MakeDatFile(LPVOID pArg) 
{
	CFBXParser* pParser = (CFBXParser*)pArg;
	EnterCriticalSection(&pParser->Get_CS());

	HRESULT hr = 0;
	hr = pParser->MultiThreaFunction();

	if (FAILED(hr))
		FAILMSG("Failed to MultiThreadFunction");

	LeaveCriticalSection(&pParser->Get_CS());
	return S_OK;
}


HRESULT CFBXParser::NativeConstruct_Prototype(const char * pModelFilePath, const char * pModelFileName, const wchar_t * pExportDatFilePath)
{
	char		szFullPath[MAX_PATH] = "";

	strcpy_s(szFullPath, pModelFilePath);
	strcat_s(szFullPath, pModelFileName);

	m_pScene = m_Importer.ReadFile(szFullPath, aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);
	if (m_pScene == nullptr)
		return E_FAIL;

	if(m_pScene->HasAnimations() == false)
		m_pScene = m_Importer.ReadFile(szFullPath, aiProcess_PreTransformVertices | aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);

	if (m_pScene == nullptr)
		return E_FAIL;

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, MakeDatFile, this, 0, nullptr);

	if (m_hThread == 0)
		return E_FAIL;

	WaitForSingleObject(m_hThread, INFINITE);
	

	return S_OK;
}

HRESULT CFBXParser::MultiThreaFunction()
{
	if (FAILED(Create_MeshContainer()))
		return E_FAIL;



	return S_OK;
}

HRESULT CFBXParser::Create_MeshContainer()
{
	return S_OK;
}

HRESULT CFBXParser::Create_Materials(const char * pMeshFilePath)
{
	return S_OK;
}

HRESULT CFBXParser::Create_SkinnedMesh()
{
	return S_OK;
}

HRESULT CFBXParser::Create_VIBuffer()
{
	return S_OK;
}

HRESULT CFBXParser::Create_HierarchyNodes(aiNode * pNode, CHierarchyNode * pParent, _uint iDepth)
{
	return S_OK;
}

HRESULT CFBXParser::Create_HierarchyNodeMeshContainer()
{
	return S_OK;
}

HRESULT CFBXParser::Create_Animation()
{
	return S_OK;
}

CFBXParser * CFBXParser::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, const char * pModelFilePath, const char * pModelFileName, const wchar_t * pExportDatFilePath)
{
	CFBXParser* pInstance = new CFBXParser(pGraphicDevice, pContextDevice);
	if (FAILED(pInstance->NativeConstruct_Prototype(pModelFilePath, pModelFileName, pExportDatFilePath)))
		Safe_Release(pInstance);

	return pInstance;
}

CComponent * CFBXParser::Clone(void * pArg)
{
	return nullptr;
}

void CFBXParser::Free()
{
	DeleteObject(m_hThread);
	CloseHandle(m_hThread);
	DeleteCriticalSection(&m_CS);
}
