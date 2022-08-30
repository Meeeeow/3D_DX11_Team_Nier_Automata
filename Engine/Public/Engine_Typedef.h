#pragma once
#ifndef __ENGINE_TYPE_DEFINES_H__
#define __ENGINE_TYPE_DEFINES_H__

namespace Engine {

	typedef	unsigned	char			_ubyte;
	typedef signed		char			_byte;

	typedef	unsigned	short			_ushort;
	typedef signed		short			_short;

	typedef	unsigned	int				_uint;
	typedef	signed		int				_int;

	typedef	unsigned	long			_ulong;
	typedef	signed		long			_long;

	typedef	float						_float;
	typedef	double						_double;

	typedef	bool						_bool;

	typedef	wchar_t						_tchar;

	typedef	ID3D11Device*				_pGraphicDevice;
	typedef ID3D11DeviceContext*		_pContextDevice;

	typedef	XMFLOAT2					_float2;
	typedef XMFLOAT3					_float3;
	typedef XMFLOAT4					_float4;

	typedef XMVECTOR					_vector;
	typedef FXMVECTOR					_fvector;
	typedef GXMVECTOR					_gvector;
	typedef HXMVECTOR					_hvector;

	typedef XMFLOAT4X4					_float4x4;

	typedef XMMATRIX					_matrix;
	typedef FXMMATRIX					_fmatrix;

	enum class OBJSTATE : _int {
		ERR			= -1,
		ENABLE		=  0,
		DISABLE		=  1,
		DEAD		=  2
	};

	enum class LIGHTSTATE : _uint {
		ALWAYS		= 0,
		DURATION	= 1,
		DISABLE		= 2,
		WAIT		= 3,
		NONE		= 4
	};

	enum class DIRECTION : _uint {
		FORWARD		= 0,
		BACKWARD	= 1,
		LEFT		= 2,
		RIGHT		= 3,
		DIAGONAL_LEFT = 4,
		DIAGONAL_RIGHT = 5,
		DIAGONAL_BACKLEFT = 6,
		DIAGONAL_BACKRIGHT = 7,
		DIAGONAL_LOOKUP = 8,
		NONE		= 9 
	};

	enum class HIT : _uint {
		SINGLE		= 0,
		MULTI		= 1,
		NONE		= 2
	};


}
#endif // !__ENGINE_TYPE_DEFINES_H__