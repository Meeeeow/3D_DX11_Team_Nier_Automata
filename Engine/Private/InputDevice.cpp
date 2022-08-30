#include "..\Public\InputDevice.h"
#include "GraphicDebugger.h"

IMPLEMENT_SINGLETON(CInputDevice)

CInputDevice::CInputDevice()
{
	ZeroMemory(m_bKeyDown, sizeof(m_bKeyDown));
	ZeroMemory(m_bKeyUp, sizeof(m_bKeyUp));
	ZeroMemory(m_byteKeyState, sizeof(m_byteKeyState));
	ZeroMemory(m_bMouseUp, sizeof(m_bMouseUp));
	ZeroMemory(m_bMouseDown, sizeof(m_bMouseDown));
}


HRESULT CInputDevice::NativeConsturct(HINSTANCE hInstance, HWND hWnd)
{
	if (FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pInputDevice, nullptr)))
		FAILMSG("CInputDevice - DirectInput8Create");

	if (FAILED(m_pInputDevice->CreateDevice(GUID_SysKeyboard, &m_pKeyboard, nullptr)))
		FAILMSG("CInputDevice - CreateDevice Keyboard");

	if (FAILED(m_pKeyboard->SetDataFormat(&c_dfDIKeyboard)))
		FAILMSG("Keyboard - SetDataFormat");

	if (FAILED(m_pKeyboard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE)))
		FAILMSG("Keyboard - SetCooperativeLevel");

	if (FAILED(m_pKeyboard->Acquire()))
		FAILMSG("Keyboard - Acquire");

	if (FAILED(m_pInputDevice->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
		FAILMSG("CInputDevice - CreateDevice Mouse");

	if (FAILED(m_pMouse->SetDataFormat(&c_dfDIMouse)))
		FAILMSG("Mouse - SetDataFormat");

	if (FAILED(m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE)))
		FAILMSG("Mouse - SetCooperativeLevel");

	if (FAILED(m_pMouse->Acquire()))
		FAILMSG("Mouse - Acquire");

#ifdef _DEBUG
	NOTICEDBG("InputDevice Create");
#endif // _DEBUG

	return S_OK;
}

HRESULT CInputDevice::Update_InputDeviceState()
{
	if (m_pKeyboard == nullptr ||
		m_pMouse == nullptr)
		FAILMSG("Input Device nullptr");

	if (FAILED(m_pKeyboard->GetDeviceState(256, m_byteKeyState)))
		return E_FAIL;

	if (FAILED(m_pMouse->GetDeviceState(sizeof(DIMOUSESTATE), &m_tMouseState)))
		return E_FAIL;

	return S_OK;
}

_bool CInputDevice::Key_Pressing(_ubyte btKeyID)
{
	if (m_byteKeyState[btKeyID] & 0x80)
		return true;
	else
		return false;
}

_bool CInputDevice::Key_Up(_ubyte btKeyID)
{
	if (Key_Pressing(btKeyID))
		return !(m_bKeyUp[btKeyID] = true);

	else if (m_bKeyUp[btKeyID])
		return !(m_bKeyUp[btKeyID] = false);

	return false;
}

_bool CInputDevice::Key_Down(_ubyte btKeyID)
{
	if (Key_Pressing(btKeyID) && !m_bKeyDown[btKeyID])
		return m_bKeyDown[btKeyID] = true;

	if (!Key_Pressing(btKeyID) && m_bKeyDown[btKeyID])
		return m_bKeyDown[btKeyID] = false;

	return false;
}

_bool CInputDevice::Mouse_Pressing(DIMB eMouseButton)
{
	if (Get_DIMouseButtonState(eMouseButton) & 0x80)
		return true;

	return false;
}

_bool CInputDevice::Mouse_Up(DIMB eMouseButton)
{
	if (Mouse_Pressing(eMouseButton))
		return !(m_bMouseUp[(_uint)eMouseButton] = true);
	else if (m_bMouseUp[(_uint)eMouseButton])
		return !(m_bMouseUp[(_uint)eMouseButton] = false);

	return false;
}

_bool CInputDevice::Mouse_Down(DIMB eMouseButton)
{
	if (Mouse_Pressing(eMouseButton) && !m_bMouseDown[(_uint)eMouseButton])
		return m_bMouseDown[(_uint)eMouseButton] = true;
	else if (!Mouse_Pressing(eMouseButton) && m_bMouseDown[(_uint)eMouseButton])
		return m_bMouseDown[(_uint)eMouseButton] = false;

	return false;
}

void CInputDevice::Free()
{
	Safe_Release(m_pMouse);
	Safe_Release(m_pKeyboard);
	Safe_Release(m_pInputDevice);
}
