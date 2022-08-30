#include "..\Public\CustomFonts.h"


CCustomFonts::CCustomFonts(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: m_pGraphicDevice(pGraphicDevice), m_pContextDevice(pContextDevice)
{
	Safe_AddRef(m_pGraphicDevice);
	Safe_AddRef(m_pContextDevice);
}

HRESULT CCustomFonts::NativeConstruct(const _tchar * pFontPath)
{
	m_pSpriteBatch = new SpriteBatch(m_pContextDevice);
	m_pSpriteFont = new SpriteFont(m_pGraphicDevice, pFontPath);


	return S_OK;
}

HRESULT CCustomFonts::Render(const _tchar * pText, _float2 vPosition, _fvector vColor)
{
	if (nullptr == m_pSpriteBatch ||
		nullptr == m_pSpriteFont)
		return E_FAIL;

	m_pSpriteBatch->Begin();

	m_pSpriteFont->DrawString(m_pSpriteBatch, pText, vPosition, vColor);

	m_pSpriteBatch->End();

	return S_OK;
}

CCustomFonts * CCustomFonts::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, const _tchar * pFontPath)
{
	CCustomFonts* pGameInstance = new CCustomFonts(pGraphicDevice, pContextDevice);
	if (FAILED(pGameInstance->NativeConstruct(pFontPath)))
		Safe_Release(pGameInstance);

	return pGameInstance;
}

void CCustomFonts::Free()
{
	Safe_Delete(m_pSpriteBatch);
	Safe_Delete(m_pSpriteFont);
}
