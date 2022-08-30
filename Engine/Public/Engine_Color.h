#pragma once
#ifndef __ENGINE_COLOR_H__
#define __ENGINE_COLOR_H__

namespace Engine {

#define		_INDIANRED				XMFLOAT4(0.8f, 0.36f, 0.36f, 1.f)
#define		_ORANGERED				XMFLOAT4(1.f, 0.27f, 0.f, 1.f)
#define		_SALMON					XMFLOAT4(0.98f, 0.5f, 0.44f, 1.f)
#define		_DARKSALMON				XMFLOAT4(0.91f, 0.59f, 0.48f, 1.f)
#define		_TOMATO					XMFLOAT4(1.f, 0.39f, 0.28f, 1.f)
#define		_ALICEBLUE				XMFLOAT4(0.94f, 0.97f, 1.f, 1.f)
#define		_GOLDENROD				XMFLOAT4(0.85f, 0.65f, 0.12f, 1.f)
#define		_DARKGOLDENROD			XMFLOAT4(0.72f, 0.52f, 0.04f, 1.f)
#define		_DARKORCHID				XMFLOAT4(0.6f, 0.2f, 0.8f, 1.f)
#define		_DARKMAGENTA			XMFLOAT4(0.55f, 0.f, 0.55f, 1.f)
#define		_FLORALWHITE			XMFLOAT4(1.f, 0.98f, 0.94f, 1.f)
#define		_LIGHTCORAL				XMFLOAT4(0.94f, 0.5f, 0.5f, 1.f)
#define		_PALEVIOLETRED			XMFLOAT4(0.85f, 0.43f, 0.57f, 1.f)
#define		_STEELBLUE				XMFLOAT4(0.27f, 0.5f, 0.7f, 1.f)
#define		_SLATEBLUE				XMFLOAT4(0.41f, 0.35f, 0.8f, 1.f)
#define		_MEDIUMSLATEBLUE		XMFLOAT4(0.48f, 0.4f, 0.93f, 1.f)
#define		_SNOW					XMFLOAT4(1.f, 0.98f, 0.98f, 1.f)
#define		_SANDYBROWN				XMFLOAT4(0.95f, 0.64f, 0.37f, 1.f)
#define		_SLATEGRAY				XMFLOAT4(0.43f, 0.5f, 0.56f, 1.f)
#define		_PEACHPUFF				XMFLOAT4(1.f, 0.85f, 0.72f, 1.f)
#define		_MISTYROSE				XMFLOAT4(1.f, 0.89f, 1.f, 1.f)
#define		_LAVENDERBLUSH			XMFLOAT4(1.f, 0.94f, 0.96f, 1.f)
#define		_DARKGRAY				XMFLOAT4(1.f, 0.97f, 0.86f, 1.f)
#define		_DIMGRAY				XMFLOAT4(0.41f, 0.41f, 0.41f, 1.f)
#define		_TEAL					XMFLOAT4(0.f, 0.5f, 0.5f, 1.f)
#define		_NOCOLOR				XMFLOAT4(0.f, 0.f, 0.f, 1.f)

	class CProportionConverter {
	public:
		explicit CProportionConverter() {}
		virtual ~CProportionConverter() DEFAULT;

	public:
		XMFLOAT4	operator() (XMFLOAT4 Color, float Percent, bool Alpha = false)
		{
			XMVECTOR		vPreColor = XMLoadFloat4(&Color);
			float			fRatio = Percent / 100.f;
			XMVECTOR		vNewColor = vPreColor * fRatio;

			if (!Alpha)
				vNewColor = XMVectorSetW(vNewColor, 1.f);

			XMFLOAT4		vResultColor;
			XMStoreFloat4(&vResultColor, vNewColor);
			
			return vResultColor;
		}
	};

}

#endif // !__ENGINE_COLOR_H__
