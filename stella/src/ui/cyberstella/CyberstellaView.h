// CyberstellaView.h : interface of the CCyberstellaView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CYBERSTELLAVIEW_H__7FB621FE_3CB8_11D6_ACFC_0048546D2F04__INCLUDED_)
#define AFX_CYBERSTELLAVIEW_H__7FB621FE_3CB8_11D6_ACFC_0048546D2F04__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GlobalData.hxx"
#include "ListData.hxx"
#include "PropsSet.hxx"

class CCyberstellaView : public CFormView
{
protected: // create from serialization only
	CCyberstellaView();
	DECLARE_DYNCREATE(CCyberstellaView)

public:
	//{{AFX_DATA(CCyberstellaView)
	enum { IDD = IDD_CYBERSTELLA_FORM };
	CListCtrl	m_List;
	//}}AFX_DATA

// Attributes
public:
	CCyberstellaDoc* GetDocument();
    CGlobalData* m_pGlobalData;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCyberstellaView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCyberstellaView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CCyberstellaView)
	afx_msg void OnConfig();
	afx_msg void OnPlay();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
    // methods
    PropertiesSet& GetPropertiesSet() const {return *m_pPropertiesSet;}
    static int CALLBACK ListViewCompareFunc( LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort );
    DWORD PopulateRomList();
    void ClearList();
    void togglePause();
    DWORD ReadRomData(CListData* pListData) const;
    DWORD Initialize();
    HRESULT PlayROM( HWND hwnd, LPCTSTR ctszPathName, 
                     LPCTSTR pszFriendlyName, 
                     CGlobalData* rGlobalData );
    // members
    PropertiesSet* m_pPropertiesSet;
    bool m_bIsPause;
};

#ifndef _DEBUG  // debug version in CyberstellaView.cpp
inline CCyberstellaDoc* CCyberstellaView::GetDocument()
   { return (CCyberstellaDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CYBERSTELLAVIEW_H__7FB621FE_3CB8_11D6_ACFC_0048546D2F04__INCLUDED_)
