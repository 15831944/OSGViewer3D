
// 3D_DemoDoc.h : interface of the CMy3D_DemoDoc class
//


#pragma once


class CMy3D_DemoDoc : public CDocument
{
protected: // create from serialization only
	CMy3D_DemoDoc();
	DECLARE_DYNCREATE(CMy3D_DemoDoc)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual void Serialize(CArchive& ar);
    virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
    CString GetFileName() const { return m_csFileName; }

// Implementation
public:
	virtual ~CMy3D_DemoDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    CString m_csFileName;

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

	// Helper function that sets search content for a Search Handler
};
