#include "..\Public\Texture.h"

CTexture::CTexture(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CComponent(pGraphicDevice, pContextDevice)
{

}

CTexture::CTexture(const CTexture & rhs)
	: CComponent(rhs), m_vecTextures(rhs.m_vecTextures), m_vecMetaDatas(rhs.m_vecMetaDatas)
{
	for (auto& Texture : m_vecTextures)
		Safe_AddRef(Texture);
}

HRESULT CTexture::NativeConstruct_Prototype(const _tchar * pTextureFilePath, _uint iIndex)
{
	_tchar					szExt[MAX_PATH] = TEXT("");
	_tchar					szFileName[MAX_PATH] = TEXT("");
	_wsplitpath_s(pTextureFilePath, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

	DirectX::ScratchImage	CImage;

	HRESULT					hr;
	_tchar					szFilePath[MAX_PATH] = TEXT("");

	for (_uint i = 0; i < iIndex; ++i)
	{
		DirectX::TexMetadata MetaData;

		int iResult = wsprintf(szFilePath, pTextureFilePath, i);

		if (lstrcmp(szExt, TEXT(".dds")) == 0)
			hr = DirectX::LoadFromDDSFile(szFilePath, DDS_FLAGS_NONE, &MetaData, CImage);
		else if (lstrcmp(szExt, TEXT(".tga")) == 0)
			hr = DirectX::LoadFromTGAFile(szFilePath, &MetaData, CImage);
		else
			hr = DirectX::LoadFromWICFile(szFilePath, WIC_FLAGS_NONE, &MetaData, CImage);

		if (FAILED(hr))
		{
			/*_tchar			szFailMSG[MAX_PATH] = TEXT("");
			wsprintf(szFailMSG, L"%s File Create Failed", szFileName);
			MessageBox(NULL, szFailMSG, L"Texture Create Failed", MB_OK);*/
			return E_FAIL;
		}

		RESOURCE*		pResource = nullptr;
		if (FAILED(DirectX::CreateTexture(m_pGraphicDevice, CImage.GetImages(), CImage.GetImageCount(), CImage.GetMetadata(), &pResource)))
			FAILMSG("Fail CreateTexture");

		SRV*			pSRV = nullptr;
		if (FAILED(m_pGraphicDevice->CreateShaderResourceView(pResource, nullptr, &pSRV)))
			FAILMSG("Fail CreateShaderResourceView");

		m_vecTextures.emplace_back(pSRV);
		m_vecMetaDatas.emplace_back(MetaData);

		Safe_Release(pResource);
		CImage.Release();
	}

	return S_OK;
}

HRESULT CTexture::NativeConstruct(void * pArg)
{
	return S_OK;
}

CTexture * CTexture::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, const _tchar * pTextureFilePath, _uint iNumTextures)
{
	CTexture* pInstance = new CTexture(pGraphicDevice, pContextDevice);
	if (FAILED(pInstance->NativeConstruct_Prototype(pTextureFilePath, iNumTextures)))
		Safe_Release(pInstance);

	return pInstance;
}

CComponent * CTexture::Clone(void * pArg)
{
	CTexture* pInstance = new CTexture(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CTexture::Free()
{
	__super::Free();

	for (auto& Texture : m_vecTextures)
		Safe_Release(Texture);
	m_vecTextures.clear();
}
