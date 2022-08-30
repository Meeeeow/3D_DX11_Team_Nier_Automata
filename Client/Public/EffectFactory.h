#pragma once
#include "Base.h"
#include "Client_Defines.h"
BEGIN(Engine)
class CGameInstance;
END


BEGIN(Client)
class CEffectFactory : public CBase
{
	DECLARE_SINGLETON(CEffectFactory)

private:
	CGameInstance*	m_pGameInstance = nullptr;

private:
	vector<TEXNAMEPROTOPATH*>		m_vecTexProtoPath;
	vector<MODELPATHNAME*>			m_vecModelPathName;

private:
	_pGraphicDevice					m_pGraphicDevice = nullptr;
	_pContextDevice					m_pContextDevice = nullptr;

private:
	unordered_map<const _tchar*, EFFECT_LOAD_INFO*> m_mapLoadedEffectInfo;


public:
	HRESULT Load_Effect_Component_Prototypes();

private:
	void Load_Effect_Component_FromData(const _tchar * szPath);
	void Load_Effect_Component_FromData_ToMap(const _tchar * szPath);


public:
	CGameObject* Load_Effect(const _tchar * szPath, EFFECT_PACKET* pPacket, const _tchar* szLayer = L"Layer_Effect");
	CGameObject* Load_EffectBullet(const _tchar * szPath, EFFECT_PACKET* pPacket, const _tchar* pTag = L"MonsterAttack");
	CGameObject* Load_EffectProto(const _tchar * szPath, EFFECT_PACKET* pPacket, const _tchar* szProto, const _tchar* szLayer = L"Layer_Effect");

	void Load_Effect_FromData(const _tchar * szPath, EffectParticleGroupDescription& tGroupDesc);
	void Load_Effect_FromMap(const _tchar * szPath, EffectParticleGroupDescription& tGroupDesc);


public:
	HRESULT NativeConstruct(_pGraphicDevice _pGraphicDevice, _pContextDevice _pContextDevice);

private:
	explicit CEffectFactory();

public:
	virtual	void Free() override;
};

END