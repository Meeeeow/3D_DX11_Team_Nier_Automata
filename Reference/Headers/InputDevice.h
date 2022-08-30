#pragma once
#ifndef __ENGINE_INPUT_DEVICE_H__
#define __ENGINE_INPUT_DEVICE_H__

#include "Base.h"
BEGIN(Engine)
class CInputDevice final : public CBase
{
	DECLARE_SINGLETON(CInputDevice)
private:
	explicit CInputDevice();
	virtual ~CInputDevice() DEFAULT;

public:
	enum class DIMM : _uint {
		DIMM_X = 0,
		DIMM_Y = 1,
		DIMM_W = 2,
		DIMM_END = 3
	};
	enum class DIMB : _uint {
		DIMB_LB = 0,
		DIMB_RB = 1,
		DIMB_WB = 2,
		DIMB_XB = 3,
		DIMB_END = 4
	};

public:
	HRESULT						NativeConsturct(HINSTANCE hInstance, HWND hWnd);
	HRESULT						Update_InputDeviceState();


public:
	_byte						Get_DIKeyState(_ubyte btKeyID) {
		return m_byteKeyState[btKeyID];
	}
	_byte						Get_DIMouseButtonState(DIMB eMouseButton) {
		return m_tMouseState.rgbButtons[(_uint)eMouseButton];
	}
	_long						Get_DIMouseMoveState(DIMM eMouseMove) {
		return ((_long*)&m_tMouseState)[(_uint)eMouseMove];
	}


public:
	_bool						Key_Pressing(_ubyte btKeyID);
	_bool						Key_Up(_ubyte btKeyID);
	_bool						Key_Down(_ubyte btKeyID);

	_bool						Mouse_Pressing(DIMB eMouseButton);
	_bool						Mouse_Up(DIMB eMouseButton);
	_bool						Mouse_Down(DIMB eMouseButton);

private:
	LPDIRECTINPUT8				m_pInputDevice = nullptr;
	LPDIRECTINPUTDEVICE8		m_pKeyboard = nullptr;
	LPDIRECTINPUTDEVICE8		m_pMouse = nullptr;

private:
	_byte						m_byteKeyState[256];
	_bool						m_bKeyUp[256];
	_bool						m_bKeyDown[256];

	DIMOUSESTATE				m_tMouseState;
	_bool						m_bMouseUp[4];
	_bool						m_bMouseDown[4];

public:
	virtual void				Free() override;

};
END
#endif // !__ENGINE_INPUT_DEVICE_H__