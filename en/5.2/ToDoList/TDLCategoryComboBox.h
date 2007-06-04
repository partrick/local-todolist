#if !defined(AFX_TDLCATEGORYCOMBOBOX_H__23DEAFA7_C661_477D_B583_FDB36C11FDC3__INCLUDED_)
#define AFX_TDLCATEGORYCOMBOBOX_H__23DEAFA7_C661_477D_B583_FDB36C11FDC3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TDLCategoryComboBox.h : header file
//

#include "..\shared\checkcombobox.h"

/////////////////////////////////////////////////////////////////////////////
// CTDLCategoryComboBox window

class CTDLCategoryComboBox : public CCheckComboBox
{
	// Construction
public:
	CTDLCategoryComboBox(BOOL bEnable = TRUE);
	
	BOOL EnableMultiSelection(BOOL bEnable = TRUE);
	BOOL GetCheck(int nIndex) const;
	int GetChecked(CStringArray& aItems) const;
	void SetChecked(const CStringArray& aItems);
	int SetCheck(int nIndex, BOOL bCheck = TRUE);

protected:
	BOOL m_bMultiSel;
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTDLCategoryComboBox)
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL
	// 	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	
	// for listbox
	virtual LRESULT ScWindowProc(HWND hRealWnd, UINT msg, WPARAM wp, LPARAM lp);
	
	// for editbox
	virtual LRESULT WindowProc(HWND hRealWnd, UINT msg, WPARAM wp, LPARAM lp);
	
	// Implementation
public:
	virtual ~CTDLCategoryComboBox();
	
	// Generated message map functions
protected:
	//{{AFX_MSG(CTDLCategoryComboBox)
	//}}AFX_MSG
	afx_msg void OnLBSelChange();
	
	DECLARE_MESSAGE_MAP()

	virtual void DrawItemText(HDC hdc, int nItem, CRect rText, const CString& sText, UINT nState);

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TDLCATEGORYCOMBOBOX_H__23DEAFA7_C661_477D_B583_FDB36C11FDC3__INCLUDED_)
