#pragma once
#ifndef __ENGINE_CUSTOM_FONTS_H__
#define __ENGINE_CUSTOM_FONTS_H__

#include "Base.h"
BEGIN(Engine)
class CCustomFonts final : public CBase
{
private:
	CCustomFonts(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual ~CCustomFonts() DEFAULT;

public:
	HRESULT							NativeConstruct(const _tchar* pFontPath);
	HRESULT							Render(const _tchar* pText, _float2 vPosition, _fvector vColor);

private:
	_pGraphicDevice					m_pGraphicDevice	= nullptr;
	_pContextDevice					m_pContextDevice	= nullptr;
	SpriteBatch*					m_pSpriteBatch		= nullptr;
	SpriteFont*						m_pSpriteFont		= nullptr;

public:
	static	CCustomFonts*			Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice
		, const _tchar* pFontPath);
	virtual void					Free() override;

};
END
#endif // !__ENGINE_CUSTOM_FONTS_H__