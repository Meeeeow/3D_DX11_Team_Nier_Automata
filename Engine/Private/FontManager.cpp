#include "..\Public\FontManager.h"

IMPLEMENT_SINGLETON(CFontManager)

CFontManager::CFontManager()
{
}

HRESULT CFontManager::Add_Font(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, const _tchar * pFontTag, const _tchar * pFontFilePath)
{
	if (Find_Font(pFontTag) != nullptr)
		return E_FAIL;

	CCustomFonts*		pFont = CCustomFonts::Create(pGraphicDevice, pContextDevice, pFontFilePath);
	if (pFont == nullptr)
		return E_FAIL;

	m_mapFonts.emplace(pFontTag, pFont);

	return S_OK;
}

HRESULT CFontManager::Render_Font(const _tchar * pFontTag, const _tchar * pText, _float2 vPosition, _fvector vColor)
{
	CCustomFonts*		pFont = Find_Font(pFontTag);

	if (nullptr == pFont)
		return E_FAIL;

	return pFont->Render(pText, vPosition, vColor);
}

CCustomFonts * CFontManager::Find_Font(const _tchar * pFontTag)
{
	auto& iter = find_if(m_mapFonts.begin(), m_mapFonts.end(), CTagFinder(pFontTag));

	if (iter == m_mapFonts.end())
		return nullptr;

	return iter->second;
}

void CFontManager::Free()
{
	for (auto& Font : m_mapFonts)
		Safe_Release(Font.second);

	m_mapFonts.clear();
}
