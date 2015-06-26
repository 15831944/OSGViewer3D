
// 3D_DemoDoc.cpp : implementation of the CMy3D_DemoDoc class
//

#include "stdafx.h"

#include "3D_DemoDoc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMy3D_DemoDoc

IMPLEMENT_DYNCREATE(CMy3D_DemoDoc, CDocument)

BEGIN_MESSAGE_MAP(CMy3D_DemoDoc, CDocument)
END_MESSAGE_MAP()


// CMy3D_DemoDoc construction/destruction

CMy3D_DemoDoc::CMy3D_DemoDoc()
{
	// TODO: add one-time construction code here
	m_csFileName = "biped.3ds";
}

CMy3D_DemoDoc::~CMy3D_DemoDoc()
{
}

BOOL CMy3D_DemoDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
    m_csFileName = lpszPathName;
    if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;


	return TRUE;
}




// CMy3D_DemoDoc serialization

void CMy3D_DemoDoc::Serialize(CArchive& ar)
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


// CMy3D_DemoDoc diagnostics

#ifdef _DEBUG
void CMy3D_DemoDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMy3D_DemoDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CMy3D_DemoDoc commands
