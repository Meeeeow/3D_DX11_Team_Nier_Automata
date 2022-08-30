#pragma once
#ifndef __ENGINE_TEXTURE_H__
#define __ENGINE_TEXTURE_H__

#include "Component.h"
BEGIN(Engine)
class ENGINE_DLL CTexture final : public CComponent
{
public:
	typedef	vector<ID3D11ShaderResourceView*>		TEXTURES;
	typedef	vector<DirectX::TexMetadata>			METADATAS;
	typedef ID3D11ShaderResourceView				SRV;
	typedef ID3D11Resource							RESOURCE;

private:
	CTexture(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	explicit CTexture(const CTexture& rhs);
	virtual ~CTexture() DEFAULT;

public:
	virtual HRESULT					NativeConstruct_Prototype(const _tchar* pTextureFilePath, _uint iIndex);
	virtual HRESULT					NativeConstruct(void* pArg) override;

	SRV*						Get_SRV(_uint iIndex = 0) {	return m_vecTextures[iIndex];	}
	const DirectX::TexMetadata&	Get_TexMetaData(_uint iIndex = 0) const { return m_vecMetaDatas[iIndex]; }

private:
	TEXTURES						m_vecTextures;
	METADATAS						m_vecMetaDatas;

public:
	static	CTexture*				Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice
		, const _tchar* pTextureFilePath, _uint iNumTextures = 1);
	virtual CComponent*				Clone(void* pArg) override;
	virtual void					Free() override;

};
END
#endif // !__ENGINE_TEXTURE_H__