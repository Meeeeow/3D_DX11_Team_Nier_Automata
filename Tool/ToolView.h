
// ToolView.h : CToolView Ŭ������ �������̽�
//

#pragma once
#include "GameInstance.h"
#include "ToolDoc.h"



class CToolView : public CView
{
protected: // serialization������ ��������ϴ�.
	CToolView();
	DECLARE_DYNCREATE(CToolView)

// Ư���Դϴ�.
public:
	class CToolDoc* GetDocument() const;

// �۾��Դϴ�.
public:

// �������Դϴ�.
public:
	virtual void OnDraw(CDC* pDC);  // �� �並 �׸��� ���� �����ǵǾ����ϴ�.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// �����Դϴ�.
public:
	virtual ~CToolView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ������ �޽��� �� �Լ�
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

#ifndef _DEBUG  // ToolView.cpp�� ����� ����
inline CToolDoc* CToolView::GetDocument() const
   { return reinterpret_cast<CToolDoc*>(m_pDocument); }
#endif

