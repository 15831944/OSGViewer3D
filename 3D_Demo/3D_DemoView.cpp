
// 3D_DemoView.cpp : implementation of the CMy3D_DemoView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "3D_Demo.h"
#endif

#include "3D_DemoDoc.h"
#include "3D_DemoView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMy3D_DemoView

IMPLEMENT_DYNCREATE(CMy3D_DemoView, CView)

BEGIN_MESSAGE_MAP(CMy3D_DemoView, CView)
	// Standard printing commands
	ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_KEYDOWN()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// CMy3D_DemoView construction/destruction

CMy3D_DemoView::CMy3D_DemoView():mOSG(0L)
{
	// TODO: add construction code here

}

CMy3D_DemoView::~CMy3D_DemoView()
{
}

BOOL CMy3D_DemoView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CMy3D_DemoView drawing

void CMy3D_DemoView::OnDraw(CDC* /*pDC*/)
{
	CMy3D_DemoDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}


// CMy3D_DemoView printing




// CMy3D_DemoView diagnostics

#ifdef _DEBUG
void CMy3D_DemoView::AssertValid() const
{
	CView::AssertValid();
}

void CMy3D_DemoView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMy3D_DemoDoc* CMy3D_DemoView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMy3D_DemoDoc)));
	return (CMy3D_DemoDoc*)m_pDocument;
}
#endif //_DEBUG


int CMy3D_DemoView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    // Let MFC create the window before OSG
    if (CView::OnCreate(lpCreateStruct) == -1)
        return -1;

    // Now that the window is created setup OSG
    mOSG = new cOSG(m_hWnd);

    return 1;
}

void CMy3D_DemoView::OnDestroy()
{
    if(mOSG != 0) delete mOSG;

    WaitForSingleObject(mThreadHandle, 1000);

    CView::OnDestroy();
}

void CMy3D_DemoView::OnInitialUpdate()
{
    CView::OnInitialUpdate();

    // Get Filename from DocumentOpen Dialog
    CString csFileName = GetDocument()->GetFileName();

    // Init the osg class

    mOSG->InitOSG(csFileName.GetString());

    // Start the thread to do OSG Rendering
    mThreadHandle = (HANDLE)_beginthread(&cOSG::Render, 0, mOSG); 
}

void CMy3D_DemoView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    // Pass Key Presses into OSG
    mOSG->getViewer()->getEventQueue()->keyPress(nChar);

    // Close Window on Escape Key
    if(nChar == VK_ESCAPE)
    {
        GetParent()->SendMessage(WM_CLOSE);
    }
	else if (nChar == 'R')
	{
		mOSG->spotExpo *= 1.1;
		mOSG->myLight1->setSpotCutoff(mOSG->spotExpo);
	}
	else if (nChar == 'T')
	{
		mOSG->spotExpo *= 0.9;
		mOSG->myLight1->setSpotCutoff(mOSG->spotExpo);
	}
}


BOOL CMy3D_DemoView::OnEraseBkgnd(CDC* pDC)
{
    /* Do nothing, to avoid flashing on MSW */
    return true;
}
