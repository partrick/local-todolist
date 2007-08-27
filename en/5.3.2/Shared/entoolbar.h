#if !defined(AFX_ENTOOLBAR_H__9AA29CEC_1405_4BBC_BBD0_94C1BD6D3120__INCLUDED_)
#define AFX_ENTOOLBAR_H__9AA29CEC_1405_4BBC_BBD0_94C1BD6D3120__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// entoolbar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEnToolBar window

class CEnBitmapEx;

class CEnToolBar : public CToolBar
{
	// Construction
public:
	CEnToolBar();
	virtual ~CEnToolBar();
	
	BOOL LoadToolBar(LPCTSTR lpszResourceName, LPCTSTR szImagePath = NULL);
	BOOL LoadToolBar(UINT nIDResource, LPCTSTR szImagePath = NULL);
	BOOL LoadToolBar(UINT nIDResource, UINT nIDImage);
	BOOL SetImage(const CString& sImagePath, COLORREF crMask = (COLORREF)-1);
	BOOL SetImage(UINT nIDImage, COLORREF crMask = (COLORREF)-1);
	int GetButtonCount(BOOL bIgnoreSeparators = FALSE);
	
	void RefreshButtonStates();
	
	// Attributes
protected:
	CImageList m_ilDisabled, m_ilNormal;
	
	// Operations
public:
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEnToolBar)
	//}}AFX_VIRTUAL
	
	// Generated message map functions
protected:
	//{{AFX_MSG(CEnToolBar)
	// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	DECLARE_MESSAGE_MAP()
		
	virtual LRESULT OnItemPrePaint(LPNMTBCUSTOMDRAW /*lpNMCustomDraw*/) { return CDRF_DODEFAULT; }
	virtual LRESULT OnItemPostPaint(LPNMTBCUSTOMDRAW /*lpNMCustomDraw*/) { return CDRF_DODEFAULT; }
	
	BOOL SetImage(CEnBitmapEx* pBitmap, COLORREF crMask);
	void RefreshDisabledImageList(CEnBitmapEx* pBitmap, COLORREF crMask);

	static BOOL GrayScale(CEnBitmapEx* pBitmap, COLORREF crMask = GetSysColor(COLOR_3DFACE));
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ENTOOLBAR_H__9AA29CEC_1405_4BBC_BBD0_94C1BD6D3120__INCLUDED_)
