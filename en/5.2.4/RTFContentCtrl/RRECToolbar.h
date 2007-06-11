#if !defined(AFX_RRECEnToolBar_H__E0596D7D_418C_49B9_AC57_2F0BF93053C9__INCLUDED_)
#define AFX_RRECEnToolBar_H__E0596D7D_418C_49B9_AC57_2F0BF93053C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RRECEnToolBar.h : header file
//

#include "..\3rdparty\FontComboBox.h"
#include "..\3rdparty\SizeComboBox.h"

#include "..\shared\entoolbar.h"

/////////////////////////////////////////////////////////////////////////////
// CRRECEnToolBar window

class CRRECToolBar : public CEnToolBar
{
// Construction
public:
	CRRECToolBar();
	BOOL Create(CWnd* parent);

	BOOL SetButtonState(int nID, UINT nState);
	BOOL IsButtonChecked(int nID) const;
	BOOL CheckButton(int nID, BOOL bChecked);
	operator CToolBarCtrl&() { return GetToolBarCtrl(); }

	BOOL GetDroppedState() const;

// Attributes
public:

// Operations
public:

	BOOL SetFontName( const CString& font );
	BOOL SetFontSize( int size );
	void SetFontColor( COLORREF color, BOOL bForeground );
	COLORREF GetFontColor(BOOL bForeground) { return bForeground ? m_crText : m_crBack; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRRECEnToolBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRRECToolBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CRRECEnToolBar)
	afx_msg void OnSelchangeFont();
	afx_msg void OnSelchangeSize();
	afx_msg LRESULT OnColorButton( WPARAM color, LPARAM nCtrlID);
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	afx_msg LRESULT OnSetFont(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnIdleUpdateCmdUI(WPARAM wParam, LPARAM lParam);
	afx_msg void OnColorDropDown(NMHDR* pNMHDR, LRESULT* pResult);
DECLARE_MESSAGE_MAP()

	LRESULT OnItemPostPaint(LPNMTBCUSTOMDRAW lpNMCustomDraw);

private:

	CFontComboBox	m_font;
	CSizeComboBox	m_size;
//	CColourPicker	m_color;
//	CColourPicker	m_colorBk;
	COLORREF		m_crText, m_crBack;

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RRECEnToolBar_H__E0596D7D_418C_49B9_AC57_2F0BF93053C9__INCLUDED_)
