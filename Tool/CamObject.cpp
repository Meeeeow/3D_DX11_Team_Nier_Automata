#include "stdafx.h"
#include "CamObject.h"
#include "MainFrm.h"
#include "ToolView.h"
#include "MainTab.h"
#include "CameraTab.h"

CCamObject::CCamObject(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CToolObjectBase(pGraphicDevice, pContextDevice)
{
}

HRESULT CCamObject::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CCamObject::NativeConstruct(void * pArg)
{
	return S_OK;
}

_int CCamObject::Tick(_double dTimeDelta)
{
	return _int();
}

_int CCamObject::LateTick(_double dTimeDelta)
{
	return _int();
}

HRESULT CCamObject::Render()
{
	return S_OK;
}

void CCamObject::Free()
{
}
