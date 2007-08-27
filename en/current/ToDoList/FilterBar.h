#if !defined(AFX_FILTERBAR_H__FAABD9A1_72C4_4731_B7A4_48251860672C__INCLUDED_)
#define AFX_FILTERBAR_H__FAABD9A1_72C4_4731_B7A4_48251860672C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FilterBar.h : header file
//

#include "..\shared\tabbedcombobox.h"
#include "..\shared\dialoghelper.h"
#include "..\shared\encheckcombobox.h"

#include "tdcstruct.h"
#include "tdlprioritycombobox.h"
#include "tdlriskcombobox.h"

/////////////////////////////////////////////////////////////////////////////
// CFilterBar dialog

class CFilteredToDoCtrl;
class CDeferWndMove;

class CFilterBar : public CDialog, public CDialogHelper
{
// Construction
public:
	CFilterBar(CWnd* pParent = NULL);   // standard constructor

	BOOL Create(CWnd* pParentWnd, UINT nID);
	void GetFilter(FTDCFILTER& filter) const { filter = m_filter; }
	void SetFilter(const FTDCFILTER& filter);

	void RefreshFilterControls(const CFilteredToDoCtrl& tdc);
	void SetFilterLabelAlignment(BOOL bLeft);
	void SetPriorityColors(const CDWordArray& aColors);
	int CalcHeight(int nWidth);
	void SetVisibleFilters(const CTDCColumnArray& aFilters);
	BOOL FilterMatches(const FTDCFILTER& filter) { return (filter == m_filter); }

	void EnableMultiSelection(DWORD dwFlags = FB_MULTISELALLOCTO | FB_MULTISELCAT);

protected:
// Dialog Data
	//{{AFX_DATA(CFilterBar)
	enum { IDD = IDD_FILTER_BAR };
	BOOL	m_bMatchAllAllocTo;
	BOOL	m_bMatchAllCategory;
	//}}AFX_DATA
	CTabbedComboBox	m_cbTaskFilter;
	int		m_nFilterWhat;
	CEnCheckComboBox	m_cbAllocToFilter;
	CEnCheckComboBox	m_cbAllocByFilter;
	CEnCheckComboBox	m_cbCategoryFilter;
	CEnCheckComboBox	m_cbStatusFilter;
	CTDLPriorityComboBox	m_cbPriorityFilter;
	CTDLRiskComboBox	m_cbRiskFilter;
	FTDCFILTER m_filter;
	CDWordArray m_aPriorityColors;
	CDWordArray m_aVisibility;

protected:
	int DoModal() { return -1; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFilterBar)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	virtual void OnOK() { /* do nothing */ }

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFilterBar)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	afx_msg void OnSelchangeFilter();
	DECLARE_MESSAGE_MAP()

protected:
	int ReposControls(int nWidth = -1, BOOL bCalcOnly = FALSE);
	BOOL WantShowFilter(TDC_COLUMN nType);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILTERBAR_H__FAABD9A1_72C4_4731_B7A4_48251860672C__INCLUDED_)
