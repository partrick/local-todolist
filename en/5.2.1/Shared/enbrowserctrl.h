#if !defined(AFX_ENBROWSERCTRL_H__73203E8C_CCFC_40B0_BE3E_3BC06DD234EB__INCLUDED_)
#define AFX_ENBROWSERCTRL_H__73203E8C_CCFC_40B0_BE3E_3BC06DD234EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// enbrowserctrl.h : header file
//

#include "webbrowserctrl.h"

/////////////////////////////////////////////////////////////////////////////
// CEnBrowserCtrl window

class CEnBrowserCtrl : public CWebBrowserCtrl
{
// Construction
public:
	CEnBrowserCtrl();
	virtual ~CEnBrowserCtrl();

// Operations
public:
	void Print(LPCTSTR szFile = NULL);
	void PrintPreview(LPCTSTR szFile = NULL);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEnBrowserCtrl)
	//}}AFX_VIRTUAL

protected:
	int m_nAction;

	// Generated message map functions
protected:
	//{{AFX_MSG(CEnBrowserCtrl)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// event sink notification handlers
	afx_msg void OnNavigateComplete2(LPDISPATCH pDisp, VARIANT FAR* URL);
	DECLARE_EVENTSINK_MAP()

protected:
	BOOL SafeExecWB(long cmdID, long cmdexecopt, VARIANT* pvaIn, VARIANT* pvaOut);

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ENBROWSERCTRL_H__73203E8C_CCFC_40B0_BE3E_3BC06DD234EB__INCLUDED_)
