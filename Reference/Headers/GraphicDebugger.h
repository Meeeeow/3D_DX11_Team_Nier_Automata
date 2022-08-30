#pragma once
#ifndef __ENGINE_GRAPHIC_DEBUGGER_H__
#define __ENGINE_GRAPHIC_DEBUGGER_H__

#include "Base.h"
BEGIN(Engine)
class ENGINE_DLL CGraphicDebugger final : public CBase
{
	DECLARE_SINGLETON(CGraphicDebugger)
private:
	CGraphicDebugger();
	virtual ~CGraphicDebugger() DEFAULT;

public:
	enum class COLOR : unsigned short {
		BLACK				= 0,
		DARKBLUE			= 1,
		DARKGREEN			= 2,
		DARKSKY				= 3,
		DARKRED				= 4,
		DARKPURPLE			= 5,
		DARKYELLOW			= 6,
		GRAY				= 7,
		DARKGRAY			= 8,
		BLUE				= 9,
		GREEN				= 10,
		SKY					= 11,
		RED					= 12,
		PURPLE				= 13,
		YELLOW				= 14,
		WHITE				= 15
	};

public:
	HRESULT					Initialize(HWND hMainWnd);
	HRESULT					ConsoleTextColor(COLOR eColor);

public: // Default In, Out Stream
	void					Notice(const char* pNotify, _bool CRLF = true);
	void					Notice(const char* pNotify1, COLOR iColor1
		, const char* pNotify2, COLOR iColor2
		, const char* pNotify3, COLOR iColor3);
	void					Request();

private: // For Debugging.
	HRESULT					DebuggingRequest(const char* pNotify);

private: // For Object.
	HRESULT					Object();
	HRESULT					ObjectSelect(list<class CGameObject*>* pList, const wchar_t* pNotify);
	HRESULT					ObjectRender(list<class CGameObject*>* pList);
	HRESULT					EffectObjectRenderDetail(class CGameObject* pEffectObject);
	HRESULT					ObjectState(list<class CGameObject*>* pList);

private: // For Level.
	HRESULT					Level();

	
private: // For Light.
	HRESULT					Light();
	HRESULT					LightSelect(list<class CLight*>* pList, const wchar_t* pNotify);
	HRESULT					LightActive(list<class CLight*>* pList);
	HRESULT					LightDeActive(list<class CLight*>* pList);
	HRESULT					LightDelete(list<class CLight*>* pList);

private: // For RenderTarget
	HRESULT					RenderTarget();

private: // For Bullet Time
	HRESULT					BulletTime();

private: // For Debug Groups
	HRESULT					DebugGroup();

private:
	BOOL					m_bConsole = FALSE;
	HWND					m_hWnd;
	HWND					m_hMainWnd;


public:


public:
	virtual void			Free() override;

};
END
#endif // !__ENGINE_GRAPHIC_DEBUGGER_H__