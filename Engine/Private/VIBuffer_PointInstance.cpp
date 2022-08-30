#include "..\Public\VIBuffer_PointInstance.h"
#include "RandomManager.h"

HRESULT CVIBuffer_PointInstance::Init_Buffer()
{
	m_iStride = sizeof(VTXPOINT);
	m_iNumVertices = 1;

	ZeroMemory(&m_tVB_Desc, sizeof(D3D11_BUFFER_DESC));

	m_tVB_Desc.ByteWidth = m_iStride * m_iNumVertices;
	m_tVB_Desc.Usage = D3D11_USAGE_IMMUTABLE;
	m_tVB_Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_tVB_Desc.CPUAccessFlags = 0;
	m_tVB_Desc.MiscFlags = 0;
	m_tVB_Desc.StructureByteStride = m_iStride;

	m_pVertices = new VTXPOINT;
	ZeroMemory(m_pVertices, sizeof(VTXPOINT));

	((VTXPOINT*)m_pVertices)[0].vPosition = _float3(0.0f, 0.0f, 0.f);
	((VTXPOINT*)m_pVertices)[0].fSize = m_tDesc.fPSize;

	m_tVB_SubResourceData.pSysMem = m_pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	m_iNumPrimitive = m_iNumInstance;
	m_iNumIndicesPerPT = 1; /* 도형하나그릴때 필요한 인덱스 갯수. */
	m_iFaceIndexSize = sizeof(_ushort);

	m_eIndexFormat = DXGI_FORMAT_R16_UINT;

	ZeroMemory(&m_tIB_Desc, sizeof(D3D11_BUFFER_DESC));

	m_tIB_Desc.ByteWidth = m_iFaceIndexSize * m_iNumPrimitive;
	m_tIB_Desc.Usage = D3D11_USAGE_IMMUTABLE;
	m_tIB_Desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_tIB_Desc.CPUAccessFlags = 0;
	m_tIB_Desc.MiscFlags = 0;

	m_pIndices = new _ushort[m_iNumPrimitive];
	ZeroMemory(m_pIndices, sizeof(_ushort) * m_iNumPrimitive);

	m_tIB_SubResourceData.pSysMem = m_pIndices;

	if (FAILED(Create_IndexBuffer()))
		return E_FAIL;


	m_iInstanceStride = sizeof(VTXMATRIX_UV_FADE_DIR);
	ZeroMemory(&m_VBInstanceDesc, sizeof(D3D11_BUFFER_DESC));

	m_VBInstanceDesc.ByteWidth = m_iInstanceStride * m_iNumInstance;
	m_VBInstanceDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_VBInstanceDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBInstanceDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_VBInstanceDesc.MiscFlags = 0;
	m_VBInstanceDesc.StructureByteStride = m_iInstanceStride;

	m_pInstanceVertices = new VTXMATRIX_UV_FADE_DIR[m_iNumInstance];
	ZeroMemory(m_pInstanceVertices, sizeof(VTXMATRIX_UV_FADE_DIR) * m_iNumInstance);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		((VTXMATRIX_UV_FADE_DIR*)m_pInstanceVertices)[i].vRight = _float4(1.f, 0.f, 0.f, 0.f);
		((VTXMATRIX_UV_FADE_DIR*)m_pInstanceVertices)[i].vUp = _float4(0.f, 1.f, 0.f, 0.f);
		((VTXMATRIX_UV_FADE_DIR*)m_pInstanceVertices)[i].vLook = _float4(0.f, 0.f, 1.f, 0.f);
		((VTXMATRIX_UV_FADE_DIR*)m_pInstanceVertices)[i].vPosition = _float4(m_pPosition[i].x, m_pPosition[i].y, m_pPosition[i].z, 1.f);
		((VTXMATRIX_UV_FADE_DIR*)m_pInstanceVertices)[i].vUV = _float4(0.f, 0.f, 0.f, 0.f);
		((VTXMATRIX_UV_FADE_DIR*)m_pInstanceVertices)[i].vUV.z = 1.f / m_tDesc.iWidth;
		((VTXMATRIX_UV_FADE_DIR*)m_pInstanceVertices)[i].vUV.w = 1.f / m_tDesc.iHeight;
		((VTXMATRIX_UV_FADE_DIR*)m_pInstanceVertices)[i].vFade.x = static_cast<_float>(m_tDesc.bFadeIn);
		((VTXMATRIX_UV_FADE_DIR*)m_pInstanceVertices)[i].vFade.y = 0.f;
		((VTXMATRIX_UV_FADE_DIR*)m_pInstanceVertices)[i].vFade.z = static_cast<_float>(m_tDesc.bFadeOut);
		((VTXMATRIX_UV_FADE_DIR*)m_pInstanceVertices)[i].vFade.w = 1.f;
		((VTXMATRIX_UV_FADE_DIR*)m_pInstanceVertices)[i].vDirection = _float4(m_pDirection[i].x, m_pDirection[i].y, m_pDirection[i].z, 0.f);
	}

	m_VBInstanceSubResourceData.pSysMem = m_pInstanceVertices;

	if (FAILED(Create_VertexInstanceBuffer()))
		return E_FAIL;

	Safe_Delete_Array(m_pInstanceVertices);

	Safe_Delete_Array(m_pIndices);
	Safe_Delete_Array(m_pVertices);



	return S_OK;
}

CVIBuffer_PointInstance::CVIBuffer_PointInstance(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer_Instance(pDevice, pDeviceContext)
{
	
}

CVIBuffer_PointInstance::CVIBuffer_PointInstance(const CVIBuffer_PointInstance & rhs)
	: CVIBuffer_Instance(rhs)
{
	
}

HRESULT CVIBuffer_PointInstance::NativeConstruct_Prototype(const _tchar* pShaderFilePath, const char* pTechniqueName)
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;



	D3D11_INPUT_ELEMENT_DESC			ElementDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "PSIZE", 0, DXGI_FORMAT_R32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },

		{ "TEXCOORD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD", 4, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD", 5, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD", 6, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 80, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD", 7, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 96, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
	};

	if (FAILED(__super::Compile_Shader(ElementDesc, 9, pShaderFilePath, pTechniqueName)))
		return E_FAIL;





	return S_OK;
}

HRESULT CVIBuffer_PointInstance::NativeConstruct(void * pArg)
{
	POINTINST_DESC* pDesc = (POINTINST_DESC*)pArg;
	m_tDesc = *pDesc;

	m_iNumInstance = m_tDesc.iNumber;



	m_pPosition = new _float3[m_iNumInstance];
	m_pDirection = new _float3[m_iNumInstance];
	m_pSpeed = new _float[m_iNumInstance];
	m_pLifeTime = new _float[m_iNumInstance];
	m_pCurrentIndex = new _int[m_iNumInstance];
	m_pSpriteElapsedTime = new _float[m_iNumInstance];
	m_pSavedLifeTime = new _float[m_iNumInstance];
	ZeroMemory(m_pPosition, sizeof(_float3) * m_iNumInstance);
	ZeroMemory(m_pDirection, sizeof(_float3) * m_iNumInstance);
	ZeroMemory(m_pSpeed, sizeof(_float) * m_iNumInstance);
	ZeroMemory(m_pLifeTime, sizeof(_float) * m_iNumInstance);
	ZeroMemory(m_pCurrentIndex, sizeof(_int) * m_iNumInstance);
	ZeroMemory(m_pSpriteElapsedTime, sizeof(_float) * m_iNumInstance);
	ZeroMemory(m_pSavedLifeTime, sizeof(_float) * m_iNumInstance);


	Update_ByDesc();

	if (FAILED(Init_Buffer()))
	{
		return E_FAIL;
	}

	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_PointInstance::Render(_uint iPassIndex)
{
	if (iPassIndex >= m_vecPassDesc.size())
		return E_FAIL;

	ID3D11Buffer*		pVertexBuffers[] = {
		m_pVB,
		m_pVBInstance
	};

	_uint				iStrides[] = {
		m_iStride,
		m_iInstanceStride
	};

	_uint		iOffsets[] = {
		0,
		0
	};

	m_pContextDevice->IASetVertexBuffers(0, 2, pVertexBuffers, iStrides, iOffsets);
	m_pContextDevice->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pContextDevice->IASetInputLayout(m_vecPassDesc[iPassIndex]->pInputLayout);
	m_pContextDevice->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

	/* 쉐이터 패스를 시작한다. */
	m_vecPassDesc[iPassIndex]->pEffectPass->Apply(0, m_pContextDevice);

	m_pContextDevice->DrawIndexedInstanced(m_iNumIndicesPerPT, m_iNumInstance, 0, 0, 0);

	return S_OK;
}

void CVIBuffer_PointInstance::Update(_double TimeDelta)
{
	if (nullptr == m_pContextDevice)
		return;

	D3D11_MAPPED_SUBRESOURCE		SubResourceData;

	m_pContextDevice->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResourceData);
	int iCount = 0;
	for (_uint i = 0; i < m_iNumInstance; ++i)
	{


		m_pLifeTime[i] += (_float)TimeDelta;
		if (m_pLifeTime[i] > 0.f)
		{
			((VTXMATRIX_UV_FADE_DIR*)SubResourceData.pData)[i].vPosition.x += m_pDirection[i].x * m_pSpeed[i] * (_float)TimeDelta;
			((VTXMATRIX_UV_FADE_DIR*)SubResourceData.pData)[i].vPosition.y += m_pDirection[i].y * m_pSpeed[i] * (_float)TimeDelta;
			((VTXMATRIX_UV_FADE_DIR*)SubResourceData.pData)[i].vPosition.z += m_pDirection[i].z * m_pSpeed[i] * (_float)TimeDelta;

			// Sprite Animation
			if (m_tDesc.iWidth != 1 || m_tDesc.iHeight != 1)
			{
				m_pSpriteElapsedTime[i] += (_float)TimeDelta;
				if (m_pSpriteElapsedTime[i] > m_tDesc.fSpriteInterval)
				{
					m_pSpriteElapsedTime[i] = 0.f;
					if (++m_pCurrentIndex[i] >= m_tDesc.iWidth * m_tDesc.iHeight)
					{
						m_pCurrentIndex[i] = 0;
					}
				}
			}

			// Calculate UV with CurrentIndex;
			if (m_tDesc.iWidth != 0 && m_tDesc.iHeight)
			{
				((VTXMATRIX_UV_FADE_DIR*)SubResourceData.pData)[i].vUV.x = 1.f / m_tDesc.iWidth * (m_pCurrentIndex[i] % m_tDesc.iWidth);
				((VTXMATRIX_UV_FADE_DIR*)SubResourceData.pData)[i].vUV.y = 1.f / m_tDesc.iHeight * (static_cast<_int>(m_pCurrentIndex[i] / m_tDesc.iWidth));
			}


			// Caculate Fade
			if (((VTXMATRIX_UV_FADE_DIR*)SubResourceData.pData)[i].vFade.x)
			{
				if (m_pLifeTime[i] < m_tDesc.fFadeInTime)
				{
					((VTXMATRIX_UV_FADE_DIR*)SubResourceData.pData)[i].vFade.y += 1.f / m_tDesc.fFadeInTime * (_float)TimeDelta;
				}
			}
			if (((VTXMATRIX_UV_FADE_DIR*)SubResourceData.pData)[i].vFade.z)
			{
				if (m_pLifeTime[i] > m_tDesc.fLifeTime - m_tDesc.fFadeOutTime)
				{
					((VTXMATRIX_UV_FADE_DIR*)SubResourceData.pData)[i].vFade.w -= 1.f / m_tDesc.fFadeOutTime * (_float)TimeDelta;
					//((VTXMATRIX_UV_FADE_DIR*)SubResourceData.pData)[i].vFade.w -= 1.f / (m_tDesc.fFadeOutEndTime - (m_tDesc.fLifeTime - m_tDesc.fFadeOutTime)) * (_float)TimeDelta;
					

				}
			}

			//// Update Direction
			//((VTXMATRIX_UV_FADE_DIR*)SubResourceData.pData)[i].vDirection.x = m_pDirection[i].x;
			//((VTXMATRIX_UV_FADE_DIR*)SubResourceData.pData)[i].vDirection.y = m_pDirection[i].y;
			//((VTXMATRIX_UV_FADE_DIR*)SubResourceData.pData)[i].vDirection.z = m_pDirection[i].z;

		}
		// When it end
		if (m_pLifeTime[i] > m_tDesc.fLifeTime)
		{
			if (m_tDesc.bRepeat)
			{
				m_pLifeTime[i] = m_pSavedLifeTime[i];
				((VTXMATRIX_UV_FADE_DIR*)SubResourceData.pData)[i].vPosition.x = m_pPosition[i].x;
				((VTXMATRIX_UV_FADE_DIR*)SubResourceData.pData)[i].vPosition.y = m_pPosition[i].y;
				((VTXMATRIX_UV_FADE_DIR*)SubResourceData.pData)[i].vPosition.z = m_pPosition[i].z;

				((VTXMATRIX_UV_FADE_DIR*)SubResourceData.pData)[i].vFade.y = 0.f;
				((VTXMATRIX_UV_FADE_DIR*)SubResourceData.pData)[i].vFade.w = 1.f;

			}
			else
			{
				//((VTXMATRIX_UV_FADE_DIR*)SubResourceData.pData)[i].vPosition.x = m_pPosition[i].x;
				//((VTXMATRIX_UV_FADE_DIR*)SubResourceData.pData)[i].vPosition.y = m_pPosition[i].y;
				//((VTXMATRIX_UV_FADE_DIR*)SubResourceData.pData)[i].vPosition.z = m_pPosition[i].z;

				//((VTXMATRIX_UV_FADE_DIR*)SubResourceData.pData)[i].vFade.x = 1.f;
				//((VTXMATRIX_UV_FADE_DIR*)SubResourceData.pData)[i].vFade.y = 1.f;
				//((VTXMATRIX_UV_FADE_DIR*)SubResourceData.pData)[i].vFade.z = 1.f;
				//((VTXMATRIX_UV_FADE_DIR*)SubResourceData.pData)[i].vFade.w = 1.f;
			}
		}


	}

	m_pContextDevice->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_PointInstance::Init()
{
	D3D11_MAPPED_SUBRESOURCE		SubResourceData;

	m_pContextDevice->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResourceData);
	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		m_pLifeTime[i] = m_pSavedLifeTime[i];
		((VTXMATRIX_UV_FADE_DIR*)SubResourceData.pData)[i].vPosition.x = m_pPosition[i].x;
		((VTXMATRIX_UV_FADE_DIR*)SubResourceData.pData)[i].vPosition.y = m_pPosition[i].y;
		((VTXMATRIX_UV_FADE_DIR*)SubResourceData.pData)[i].vPosition.z = m_pPosition[i].z;

		((VTXMATRIX_UV_FADE_DIR*)SubResourceData.pData)[i].vFade.y = 0.f;
		((VTXMATRIX_UV_FADE_DIR*)SubResourceData.pData)[i].vFade.w = 1.f;
	}
	m_pContextDevice->Unmap(m_pVBInstance, 0);
	//Update_ByDesc();
}

void CVIBuffer_PointInstance::Set_Desc(const POINTINST_DESC & _tDesc)
{
	memcpy(&m_tDesc, &_tDesc, sizeof(POINTINST_DESC));
}

void CVIBuffer_PointInstance::Update_ByDesc()
{
	CRandomManager* pRM = CRandomManager::Get_Instance();
	pRM->Range(0, 999999);

	_float fTemp = 0.f;
	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		if (m_tDesc.fDensity != 0.f)
		{
			//fTemp = pRM->Random() % static_cast<_int>(m_tDesc.fDensity * 10000) * 0.0001f;
			fTemp = rand() % static_cast<_int>(m_tDesc.fDensity * 10000) * 0.0001f;
			fTemp -= m_tDesc.fDensity * 0.5f;
			m_pPosition[i].x = fTemp;

			//fTemp = pRM->Random() % static_cast<_int>(m_tDesc.fDensity * 10000) * 0.0001f;
			fTemp = rand() % static_cast<_int>(m_tDesc.fDensity * 10000) * 0.0001f;
			fTemp -= m_tDesc.fDensity * 0.5f;
			m_pPosition[i].y = fTemp;

			//fTemp = pRM->Random() % static_cast<_int>(m_tDesc.fDensity * 10000) * 0.0001f;
			fTemp = rand() % static_cast<_int>(m_tDesc.fDensity * 10000) * 0.0001f;
			fTemp -= m_tDesc.fDensity * 0.5f;
			m_pPosition[i].z = fTemp;
		}
		else
		{
			// do nothing
		}
		///////////////////////////////////////////

		m_pDirection[i] = m_tDesc.vDirection;
		if (m_tDesc.fDirectionDensity != 0.f)
		{
			//fTemp = pRM->Random() % static_cast<_int>(m_tDesc.fDirectionDensity * 10000) * 0.0001f;
			fTemp = rand() % static_cast<_int>(m_tDesc.fDirectionDensity * 10000) * 0.0001f;
			fTemp -= m_tDesc.fDirectionDensity * 0.5f;
			m_pDirection[i].x += fTemp;
			//fTemp = pRM->Random() % static_cast<_int>(m_tDesc.fDirectionDensity * 10000) * 0.0001f;
			fTemp = rand() % static_cast<_int>(m_tDesc.fDirectionDensity * 10000) * 0.0001f;
			fTemp -= m_tDesc.fDirectionDensity * 0.5f;
			m_pDirection[i].y += fTemp;
			//fTemp = pRM->Random() % static_cast<_int>(m_tDesc.fDirectionDensity * 10000) * 0.0001f;
			fTemp = rand() % static_cast<_int>(m_tDesc.fDirectionDensity * 10000) * 0.0001f;
			fTemp -= m_tDesc.fDirectionDensity * 0.5f;
			m_pDirection[i].z += fTemp;
		}
		else
		{
			// do nothing
		}

		////////////////////////////////////////////
		m_pSpeed[i] = m_tDesc.fSpeed;

		if (m_tDesc.fSpeedDensity != 0.f)
		{
			//fTemp = pRM->Random() % static_cast<_int>(m_tDesc.fSpeedDensity * 10000) * 0.0001f;
			fTemp = rand() % static_cast<_int>(m_tDesc.fSpeedDensity * 10000) * 0.0001f;
			fTemp -= m_tDesc.fSpeedDensity * 0.5f;
			m_pSpeed[i] += fTemp;
		}
		else
		{
			// do nothing
		}

		/////////////////////////////////////////////////

		if (m_tDesc.iLifeTimeDensity == 1
			&& m_tDesc.fLifeTime > 0.f)
		{
			fTemp = pRM->Random() % static_cast<_int>(m_tDesc.fLifeTimeDensityEnd * 10000) * 0.0001f;
			//fTemp = rand() % static_cast<_int>(m_tDesc.fLifeTime * 10000) * 0.0001f;
			m_pLifeTime[i] = fTemp;

			//if (!m_tDesc.bRepeat)
			//{
			//	m_pLifeTime[i] -= m_tDesc.fLifeTime;
			//}
			m_pLifeTime[i] -= m_tDesc.fLifeTimeDensityEnd;
		}
		else
		{
			//m_pLifeTime[i] = m_tDesc.fLifeTime;
			m_pLifeTime[i] = 0.f;
		}
		m_pSavedLifeTime[i] = m_pLifeTime[i];

		/////////////////////////////////////////////

		//m_pCurrentIndex[i] = pRM->Random() % (m_tDesc.iWidth * m_tDesc.iHeight);

		if (m_tDesc.iWidth == 0 || m_tDesc.iHeight == 0)
		{
			m_pCurrentIndex[i] = 0;
		}
		else
		{
			m_pCurrentIndex[i] = rand() % (m_tDesc.iWidth * m_tDesc.iHeight);
		}

		//m_pSpriteElapsedTime[i] = pRM->Random() % static_cast<_int>(m_tDesc.fSpriteInterval * 10000) * 0.0001f;
		m_pSpriteElapsedTime[i] = rand() % static_cast<_int>(m_tDesc.fSpriteInterval * 10000) * 0.0001f;
	}
}

CVIBuffer_PointInstance * CVIBuffer_PointInstance::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _tchar* pShaderFilePath, const char* pTechniqueName)
{
	CVIBuffer_PointInstance* pGameInstance = new CVIBuffer_PointInstance(pDevice, pDeviceContext);

	if (FAILED(pGameInstance->NativeConstruct_Prototype(pShaderFilePath, pTechniqueName)))
	{
		MSGBOX("Failed to Create CVIBuffer_PointInstance");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

CComponent * CVIBuffer_PointInstance::Clone(void * pArg)
{
	CVIBuffer_PointInstance* pGameInstance = new CVIBuffer_PointInstance(*this);

	if (FAILED(pGameInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Clone CVIBuffer_PointInstance");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CVIBuffer_PointInstance::Free()
{
	__super::Free();

	Safe_Delete_Array(m_pDirection);
	Safe_Delete_Array(m_pPosition);
	Safe_Delete_Array(m_pSpeed);
	Safe_Delete_Array(m_pLifeTime);
	Safe_Delete_Array(m_pCurrentIndex);
	Safe_Delete_Array(m_pSpriteElapsedTime);
	Safe_Delete_Array(m_pSavedLifeTime);
}
