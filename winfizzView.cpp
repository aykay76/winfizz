// winfizzView.cpp : implementation of the CwinfizzView class
//

#include "stdafx.h"
#include "winfizz.h"

#include "winfizzDoc.h"
#include "winfizzView.h"

#include "script/parser.h"

#include <stdio.h>
#include <gdiplus.h>
using namespace Gdiplus;

#include "glwindows.h"
#include <algorithm>
#include ".\winfizzview.h"
#include "engine/constants.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CwinfizzView

IMPLEMENT_DYNCREATE(CwinfizzView, CView)

BEGIN_MESSAGE_MAP(CwinfizzView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_SIZE()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

// CwinfizzView construction/destruction

CwinfizzView::CwinfizzView()
	: m_gl(NULL)
{
}

CwinfizzView::~CwinfizzView()
{
}

BOOL CwinfizzView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CwinfizzView drawing

void CwinfizzView::OnDraw(CDC* pDC)
{
	CwinfizzDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

}


// CwinfizzView printing

BOOL CwinfizzView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CwinfizzView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CwinfizzView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CwinfizzView diagnostics

#ifdef _DEBUG
void CwinfizzView::AssertValid() const
{
	CView::AssertValid();
}

void CwinfizzView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CwinfizzDoc* CwinfizzView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CwinfizzDoc)));
	return (CwinfizzDoc*)m_pDocument;
}
#endif //_DEBUG


// CwinfizzView message handlers

void CwinfizzView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// now we ask the engine to initialise itself with the script system as it needs
	// to add particular type definitions and namespaces
	Engine::instance()->initialiseScriptSystem();

	CRect rc;
	GetClientRect(rc);
	m_width = rc.Width();
	m_height = rc.Height();

	m_gl = new GLWindows();
	m_gl->create(GetSafeHwnd(), m_width, m_height, 16, true);
	Engine::instance()->setGL(m_gl);

	Engine::instance()->init();
}

void CwinfizzView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
}

void CwinfizzView::OnTimer(UINT nIDEvent)
{
	if (Engine::instance()->step() == false)
	{
		KillTimer(1);
	}

	wchar_t sz[120];
	swprintf(sz, 120, L"%.1f", Engine::instance()->fps());
	AfxGetMainWnd()->SetWindowTextW(sz);

	CView::OnTimer(nIDEvent);
}

BOOL CwinfizzView::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CwinfizzView::OnDestroy()
{
	if (m_gl)
	{
		m_gl->destroy();
		delete m_gl;
	}

	CView::OnDestroy();
}

bool started = false;
Event event;

void CwinfizzView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// kick off the engine
	if (started == false)
	{
		Engine::instance()->start(true);
		SetTimer(1, 1, NULL);
		started = true;
	}
	else
	{
		Engine::instance()->leftButtonDown(point.x, point.y);
	}

	CView::OnLButtonDown(nFlags, point);
}

void CwinfizzView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if (cx && cy && m_gl)
	{
		// we are using a fixed size so we don't need to resize right now
		m_gl->resize(cx, cy);
	}
}

void CwinfizzView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
//	m_gl->destroy();
//	m_gl->create(NULL, 1280, 1024, 32, true);

	CView::OnLButtonDblClk(nFlags, point);
}
