
// 3D_DemoView.h : interface of the CMy3D_DemoView class
//

#pragma once

#include "MFC_OSG.h"

class CMy3D_DemoView : public CView
{
protected: // create from serialization only
	CMy3D_DemoView();
	DECLARE_DYNCREATE(CMy3D_DemoView)

// Attributes
public:
	CMy3D_DemoDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
    virtual void OnInitialUpdate();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// Implementation
public:
	virtual ~CMy3D_DemoView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    cOSG* mOSG;
    HANDLE mThreadHandle;

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
    afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnDestroy();
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

#ifndef _DEBUG  // debug version in 3D_DemoView.cpp
inline CMy3D_DemoDoc* CMy3D_DemoView::GetDocument() const
   { return reinterpret_cast<CMy3D_DemoDoc*>(m_pDocument); }
#endif

