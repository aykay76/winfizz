// winfizzDoc.h : interface of the CwinfizzDoc class
//


#pragma once

class CwinfizzDoc : public CDocument
{
protected: // create from serialization only
	CwinfizzDoc();
	DECLARE_DYNCREATE(CwinfizzDoc)

// Attributes
public:

// Operations
public:

// Overrides
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CwinfizzDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};


