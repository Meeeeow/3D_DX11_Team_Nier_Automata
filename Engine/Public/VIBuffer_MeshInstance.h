#pragma once
#include "VIBuffer_Instance.h"
#include "Engine_Struct.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_MeshInstance :
	public CVIBuffer_Instance
{
public:
	struct MESHINST_DESC 
	{
		_tchar		szMeshProto[MAX_PATH] = L"";
		_int		iNumber = 0;
	};

private:
	MESHINST_DESC		m_tDesc;
private:
	HRESULT Init_Buffer();


private:
	explicit CVIBuffer_MeshInstance(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CVIBuffer_MeshInstance(const CVIBuffer_MeshInstance& rhs);
	virtual ~CVIBuffer_MeshInstance() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const _tchar* pShaderFilePath, const char* pTechniqueName);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual HRESULT Render(_uint iPassIndex);

public:
	void Update(_double TimeDelta);
	void Init();
	void Set_Desc(const POINTINST_DESC& _tDesc);
	void Update_ByDesc();



private:
	_float3*	m_pDirection = nullptr;
	_float3*	m_pPosition = nullptr;
	_float*		m_pLifeTime = nullptr;
	_float*		m_pSpeed = nullptr;
	_int*		m_pCurrentIndex = nullptr;

private:
	_float*	m_pSavedLifeTime = nullptr;

public:
	static CVIBuffer_MeshInstance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pShaderFilePath, const char* pTechniqueName);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;

};

END