
// ToolView.h : CToolView 클래스의 인터페이스
//

#pragma once
#include "GameInstance.h"
#include "ToolDoc.h"



class CToolView : public CView
{
protected: // serialization에서만 만들어집니다.
	CToolView();
	DECLARE_DYNCREATE(CToolView)

// 특성입니다.
public:
	class CToolDoc* GetDocument() const;

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 구현입니다.
public:
	virtual ~CToolView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();

public:
	HRESULT						Ready_Prototype_GameObject_ForStatic();
	HRESULT						Ready_Component_ForStatic();

	CGameInstance*				m_pGameInstance = nullptr;
	class CMainTab*				m_pMainTab = nullptr;
	_pGraphicDevice				m_pGraphicDevice = nullptr;
	_pContextDevice				m_pContextDevice = nullptr;

	class CRenderer*			m_pRenderer = nullptr;
	class CToolCamera*			m_pCamera = nullptr;

public:
	_pGraphicDevice				Get_GraphicDevice() { return m_pGraphicDevice; }
	_pContextDevice				Get_ContextDevice() { return m_pContextDevice; }
	CRenderer*					Get_Renderer() { return m_pRenderer; }
	HRESULT						Ready_Component();
	HRESULT						Ready_Light();


	_int						Tick(_double dTimeDelta);
	_int						LateTick(_double dTimeDelta);
	HRESULT						Render();
	
	_int						Picking(_double dTimeDelta);

	afx_msg void				OnMButtonDown(UINT nFlags, CPoint point);
};

#ifndef _DEBUG  // ToolView.cpp의 디버그 버전
inline CToolDoc* CToolView::GetDocument() const
   { return reinterpret_cast<CToolDoc*>(m_pDocument); }
#endif

