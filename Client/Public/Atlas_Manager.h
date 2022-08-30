#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)

class CAtlas_Manager final : public CBase
{
	DECLARE_SINGLETON(CAtlas_Manager)

public:
	typedef struct tagRawData
	{
		_tchar*	pCategory;
		_tchar*	pElement;
		_float	fLeft;
		_float	fRight;
		_float	fTop;
		_float	fBottom;
	}RAWDATA;

	enum class CATEGORY : _uint
	{
		CORE		= 0,
		HUD			= 1,
		BACKGROUND	= 2,
		TRIGRID		= 3,
		CURSOR		= 4,
		DAMAGE_A	= 5,
		DAMAGE_B	= 6,
		TARGET		= 7,
		FONT		= 8,
		CIRCLE		= 9,
		DECO		= 10,
		INDICATOR_B	= 11,
		INDICATOR_R = 12,
		CANVAS		= 13,
		LOADING		= 14,
		WEAPON		= 15,
		H_MSG		= 16,
		IRON		= 17,
		IRON2		= 18,
		COUNT		= 19
	};

private:
	const static RAWDATA	m_AtlasRawData[];
	const static int		m_AtlasRawDataCount;

private:
	explicit CAtlas_Manager();
	virtual ~CAtlas_Manager() = default;

public:
	virtual void		Free() override;
	HRESULT				NativeConstruct();

private:
	HRESULT		Reserve_Container(_uint iNumCategory);
	HRESULT		Parse_DataFromDat();
	HRESULT		Parse_DataFromHeader();

public:
	HRESULT		Add_AtlasDesc(_uint iCategoryIndex, const _tchar* pElemTag, ATLASDESC* pAtlasDesc);
	ATLASDESC*	Get_AtlasDesc(_uint iCategoryIndex, const _tchar* pElemTag);

private:
	typedef unordered_map<const _tchar*, ATLASDESC*>	ATLASMAP;

	ATLASMAP*	m_pAtlases = nullptr;
	_uint		m_iNumCategories;
};

END