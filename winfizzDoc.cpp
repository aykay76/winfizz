// winfizzDoc.cpp : implementation of the CwinfizzDoc class
//

#include "stdafx.h"
#include "winfizz.h"

#include "winfizzDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CwinfizzDoc

IMPLEMENT_DYNCREATE(CwinfizzDoc, CDocument)

BEGIN_MESSAGE_MAP(CwinfizzDoc, CDocument)
END_MESSAGE_MAP()


// CwinfizzDoc construction/destruction

CwinfizzDoc::CwinfizzDoc()
{
	// TODO: add one-time construction code here

}

CwinfizzDoc::~CwinfizzDoc()
{
}

BOOL CwinfizzDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	return TRUE;
}




// CwinfizzDoc serialization

void CwinfizzDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


// CwinfizzDoc diagnostics

#ifdef _DEBUG
void CwinfizzDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CwinfizzDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CwinfizzDoc commands
