// winfizzView.h : interface of the CwinfizzView class
//


#pragma once

#include "engine/engine.h"

#include <gdiplus.h>
using namespace Gdiplus;

#include "glwindows.h"

class CwinfizzView : public CView
{
protected: // create from serialization only
	CwinfizzView();
	DECLARE_DYNCREATE(CwinfizzView)

// Attributes
public:
	CwinfizzDoc* GetDocument() const;

// Operations
public:

// Overrides
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CwinfizzView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	HDC m_hdc;
	int m_width;
	int m_height;
	GLWindows* m_gl;

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
protected:
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
public:
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};

#ifndef _DEBUG  // debug version in winfizzView.cpp
inline CwinfizzDoc* CwinfizzView::GetDocument() const
   { return reinterpret_cast<CwinfizzDoc*>(m_pDocument); }
#endif

