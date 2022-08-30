#pragma once
#ifndef __ENGINE_FONT_MANAGER_H__
#define __ENGINE_FONT_MANAGER_H__

#include "CustomFonts.h"

BEGIN(Engine)
class CFontManager final : public CBase
{
	DECLARE_SINGLETON(CFontManager)

private:
	explicit CFontManager();
	virtual ~CFontManager() DEFAULT;

private:
	typedef unordered_map<const _tchar*, CCustomFonts*>		FONTS;
	
public:
	HRESULT						Add_Font(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, const _tchar* pFontTag, const _tchar* pFontFilePath);
	HRESULT						Render_Font(const _tchar* pFontTag, const _tchar* pText, _float2 vPosition, _fvector vColor);

public:
	CCustomFonts*				Find_Font(const _tchar* pFontTag);

private:
	FONTS						m_mapFonts;

public:
	virtual void				Free() override;

};

END
#endif // !__ENGINE_FONT_MANAGER_H__