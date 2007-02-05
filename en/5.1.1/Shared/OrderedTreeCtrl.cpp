// ToDoCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "orderedtreeCtrl.h"

#include "..\shared\holdredraw.h"
#include "..\shared\themed.h"
#include "..\shared\treectrlhelper.h"
#include "..\shared\misc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COrderedTreeCtrl

const UINT MINGUTTER = 16;

static CMap<int, int&, UINT, UINT&> g_mapWidths;

COrderedTreeCtrl::COrderedTreeCtrl() : 

    // because CTreeCtrlHelper wants a reference passed
    // to its constructor we have to pass '*this'. however
    // the compiler complains because 'this' is not yet
    // fully constructed.
#pragma warning (disable: 4355)
	CTreeCtrlHelper(*this),
#pragma warning (default: 4355)

	m_bShowingPosColumn(TRUE), 
	m_crGridlines(OTC_GRIDCOLOR),
	m_crAltLines((COLORREF)-1),
	m_bWantInit(FALSE)
{
	AddGutterColumn(OTC_POSCOLUMNID, "Pos"); // for the pos string
	
	EnableGutterColumnHeaderClicking(OTC_POSCOLUMNID, FALSE);
}

COrderedTreeCtrl::~COrderedTreeCtrl()
{
}

BEGIN_MESSAGE_MAP(COrderedTreeCtrl, CTreeCtrl)
//{{AFX_MSG_MAP(COrderedTreeCtrl)
ON_WM_SETTINGCHANGE()
//}}AFX_MSG_MAP
ON_WM_STYLECHANGED()
ON_NOTIFY_REFLECT_EX(TVN_ITEMEXPANDED, OnItemexpanded)
ON_NOTIFY_REFLECT_EX(NM_CUSTOMDRAW, OnCustomDraw)
ON_NOTIFY_REFLECT_EX(NM_CLICK, OnClick)
ON_NOTIFY_REFLECT(TVN_BEGINDRAG, OnBeginDrag)
ON_REGISTERED_MESSAGE(WM_NCG_GETFIRSTVISIBLETOPLEVELITEM, OnGutterGetFirstVisibleTopLevelItem)
ON_REGISTERED_MESSAGE(WM_NCG_GETNEXTITEM, OnGutterGetNextItem)
ON_REGISTERED_MESSAGE(WM_NCG_DRAWITEM, OnGutterDrawItem)
ON_REGISTERED_MESSAGE(WM_NCG_RECALCCOLWIDTH, OnGutterRecalcColWidth)
ON_REGISTERED_MESSAGE(WM_NCG_POSTNCDRAW, OnGutterPostNcDraw)
ON_REGISTERED_MESSAGE(WM_NCG_GETITEMRECT, OnGutterGetItemRect)
ON_REGISTERED_MESSAGE(WM_NCG_GETFIRSTCHILDITEM, OnGutterGetFirstChildItem)
ON_REGISTERED_MESSAGE(WM_NCG_POSTDRAWITEM, OnGutterPostDrawItem)
ON_REGISTERED_MESSAGE(WM_NCG_HITTEST, OnGutterHitTest)
ON_REGISTERED_MESSAGE(WM_NCG_NOTIFYITEMCLICK, OnGutterNotifyItemClick)
ON_REGISTERED_MESSAGE(WM_NCG_ISITEMSELECTED, OnGutterIsItemSelected)
ON_REGISTERED_MESSAGE(WM_NCG_GETSELECTEDCOUNT, OnGutterGetSelectedCount)
ON_REGISTERED_MESSAGE(WM_NCG_GETPARENTITEM, OnGutterGetParentID)
ON_REGISTERED_MESSAGE(WM_NCG_WANTRECALC, OnGutterWantRecalc)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COrderedTreeCtrl message handlers

void COrderedTreeCtrl::PreSubclassWindow() 
{
	m_gutter.AddRecalcMessage(TVM_INSERTITEM);
	m_gutter.AddRecalcMessage(TVM_INSERTITEM);
	m_gutter.AddRecalcMessage(TVM_DELETEITEM);
	m_gutter.AddRecalcMessage(TVM_EXPAND);
	
	m_gutter.AddRedrawMessage(WM_KEYUP); // only way to catch keyboard navigation (at present)
	m_gutter.AddRedrawMessage(TVM_SELECTITEM);
	m_gutter.AddRedrawMessage(TVM_SORTCHILDREN);
	m_gutter.AddRedrawMessage(TVM_SORTCHILDRENCB);
	m_gutter.AddRedrawMessage(WM_COMMAND, EN_KILLFOCUS);
	
	CTreeCtrl::PreSubclassWindow();
	
	// note: its too early to initialize the gutter here because 
	// MFC hasn't done its bit yet, so initilization is done
	// in WindowProc (for now)
	m_bWantInit = TRUE;
}

LRESULT COrderedTreeCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if (m_bWantInit && !m_gutter.IsInitialized())
	{
		m_bWantInit = FALSE;
		m_gutter.Initialize(GetSafeHwnd());
	}
	
	return CTreeCtrl::WindowProc(message, wParam, lParam);
}

void COrderedTreeCtrl::SetAlternateLineColor(COLORREF color)
{
	if (m_crAltLines != color)
	{
		m_crAltLines = color;
		Invalidate();
		RedrawGutter();
	}
}

BOOL COrderedTreeCtrl::PtInHeader(CPoint ptScreen) const
{
	return m_gutter.PtInHeader(ptScreen);
}

COLORREF COrderedTreeCtrl::GetItemLineColor(HTREEITEM hti)
{
	if (m_crAltLines != -1 && ItemLineIsOdd(hti))
		return m_crAltLines;
	
	// else
	return GetSysColor(COLOR_WINDOW);
}

void COrderedTreeCtrl::ShowGutterPosColumn(BOOL bShow)
{
	if (m_bShowingPosColumn != bShow)
	{
		m_bShowingPosColumn = bShow;
		m_gutter.RecalcGutter();
	}
}

void COrderedTreeCtrl::SetGridlineColor(COLORREF color)
{
	if (m_crGridlines != color)
	{
		m_crGridlines = color;
		
		if (GetSafeHwnd())
		{
			m_gutter.Redraw();
			Invalidate();
		}
	}
}

int COrderedTreeCtrl::AddGutterColumn(UINT nColID, LPCTSTR szTitle, UINT nWidth, UINT nTextAlign)
{
	return m_gutter.AddColumn(nColID, szTitle, nWidth, nTextAlign);
}

void COrderedTreeCtrl::PressGutterColumnHeader(UINT nColID, BOOL bPress)
{
	m_gutter.PressHeader(nColID, bPress);
}

void COrderedTreeCtrl::SetGutterColumnHeaderTitle(UINT nColID, LPCTSTR szTitle, LPCTSTR szFont)
{
	m_gutter.SetHeaderTitle(nColID, szTitle, szFont, FALSE);
}

void COrderedTreeCtrl::SetGutterColumnHeaderTitle(UINT nColID, UINT nSymbol, LPCTSTR szFont)
{
	// Valik - cast to TCHAR is necessary or the compiler complains under VC 7.1
	m_gutter.SetHeaderTitle(nColID, CString(static_cast<TCHAR>(nSymbol)), szFont, TRUE);
}

void COrderedTreeCtrl::SetGutterColumnSort(UINT nColID, NCGSORT nSortDir)
{
	m_gutter.SetColumnSort(nColID, nSortDir);
}

void COrderedTreeCtrl::EnableGutterColumnHeaderClicking(UINT nColID, BOOL bEnable)
{
	m_gutter.EnableHeaderClicking(nColID, bEnable);
}

LRESULT COrderedTreeCtrl::OnGutterGetFirstVisibleTopLevelItem(WPARAM /*wParam*/, LPARAM lParam)
{
	ASSERT (lParam);
	return (LRESULT)GetFirstVisibleTopLevelItem(*((LPINT)lParam));
}

LRESULT COrderedTreeCtrl::OnGutterGetNextItem(WPARAM /*wParam*/, LPARAM lParam)
{
	return (LRESULT)CTreeCtrl::GetNextItem((HTREEITEM)lParam, TVGN_NEXT);
}

LRESULT COrderedTreeCtrl::OnGutterWantRecalc(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	BuildVisibleIndexMap();

	return 0L; // always
}

LRESULT COrderedTreeCtrl::OnGutterGetFirstChildItem(WPARAM /*wParam*/, LPARAM lParam)
{
	HTREEITEM hti = (HTREEITEM)lParam;
	
	if (ItemHasChildren(hti) && (GetItemState(hti, TVIS_EXPANDED) & TVIS_EXPANDED))
		return (LRESULT)GetChildItem(hti);
	
	return 0;
}

LRESULT COrderedTreeCtrl::OnGutterDrawItem(WPARAM /*wParam*/, LPARAM lParam)
{
	NCGDRAWITEM* pNCGDI = (NCGDRAWITEM*)lParam;
	
	if (pNCGDI->nColID == OTC_POSCOLUMNID)
	{
		CRect rItem(pNCGDI->rItem);

		NcDrawItem(pNCGDI->pDC, pNCGDI->dwItem, pNCGDI->dwParentItem, 
					pNCGDI->nColID, rItem, pNCGDI->nLevel, 
					pNCGDI->nItemPos, pNCGDI->bSelected, pNCGDI->rWindow);
		
		return TRUE; // we handled it
	}
	
	// else
	return FALSE;
}

LRESULT COrderedTreeCtrl::OnGutterPostDrawItem(WPARAM /*wParam*/, LPARAM lParam)
{
	NCGDRAWITEM* pNCGDI = (NCGDRAWITEM*)lParam;

	if (pNCGDI->dwParentItem)
		PostNcDrawItem(pNCGDI->pDC, pNCGDI->dwParentItem, pNCGDI->rItem, pNCGDI->nLevel);
	
	return FALSE; // to let our parent handle it too
}

LRESULT COrderedTreeCtrl::OnGutterPostNcDraw(WPARAM /*wParam*/, LPARAM lParam)
{
	NCGDRAWITEM* pNCGDI = (NCGDRAWITEM*)lParam;
	
	PostNcDraw(pNCGDI->pDC, pNCGDI->rWindow);
	
	return FALSE; // to let our parent handle it too
}

LRESULT COrderedTreeCtrl::OnGutterRecalcColWidth(WPARAM /*wParam*/, LPARAM lParam)
{
	NCGRECALCCOLUMN* pNCRC = (NCGRECALCCOLUMN*)lParam;
	
	return RecalcColumnWidth(pNCRC->pDC, pNCRC->nColID, pNCRC->nWidth);
}

LRESULT COrderedTreeCtrl::OnGutterGetItemRect(WPARAM /*wParam*/, LPARAM lParam)
{
	NCGITEMRECT* pNCGGI = (NCGITEMRECT*)lParam;
	
	return GetItemRect((HTREEITEM)pNCGGI->dwItem, &pNCGGI->rItem, TRUE);
}

LRESULT COrderedTreeCtrl::OnGutterGetParentID(WPARAM /*wParam*/, LPARAM lParam)
{
	return (DWORD)GetParentItem((HTREEITEM)lParam);
}

LRESULT COrderedTreeCtrl::OnGutterIsItemSelected(WPARAM /*wParam*/, LPARAM lParam)
{
	NCGITEMSELECTION* pNCGIS = (NCGITEMSELECTION*)lParam;
	
	HTREEITEM hti = (HTREEITEM)pNCGIS->dwItem;
	pNCGIS->bSelected = (GetItemState(hti, TVIF_STATE) & TVIS_SELECTED);
	
	return TRUE;
}

LRESULT COrderedTreeCtrl::OnGutterGetSelectedCount(WPARAM /*wParam*/, LPARAM lParam)
{
	int* pCount = (int*)lParam;
	*pCount = GetSelectedItem() ? 1 : 0;
	
	return TRUE;
}

LRESULT COrderedTreeCtrl::OnGutterHitTest(WPARAM /*wParam*/, LPARAM lParam)
{
	CPoint point(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	
	UINT nFlags = 0;
	HTREEITEM hti = HitTest(point, &nFlags);
	
	return (LRESULT)hti;
}

void COrderedTreeCtrl::PostNcDraw(CDC* /*pDC*/, const CRect& /*rWindow*/)
{
}

void COrderedTreeCtrl::NcDrawItem(CDC* pDC, DWORD dwItem, DWORD dwParentItem, UINT nColID, CRect& rItem, 
								  int nLevel, int nPos, BOOL bSelected, const CRect& rWindow)
{
	HTREEITEM hti = (HTREEITEM)dwItem;
	
	if (nColID == OTC_POSCOLUMNID) // this is all we deal with
	{
		// extract the parent pos
		CString sPos, sParentPos; 
		static CMap<DWORD, DWORD, CString, LPCTSTR> mapParentPos;
		
		if (dwParentItem)
			VERIFY(mapParentPos.Lookup(dwParentItem, sParentPos));
		
		BOOL bHasChildren = ItemHasChildren(hti);
		
		if (nPos == -1) // it means we have to figure it out for ourselves
			nPos = GetItemPos(hti, (HTREEITEM)dwParentItem);
		
		if (nLevel == -1) // likewise
			nLevel = GetItemLevel(hti);
		
		// default
		
		if (sParentPos.IsEmpty())
			sPos.Format("%d", nPos);
		else
			sPos.Format("%s.%d", sParentPos, nPos);
//			sPos.Format("%s%s%d", sParentPos, Misc::GetDecimalSeparator(), nPos);
		
		// add to map for our children
		if (bHasChildren)
			mapParentPos[dwItem] = sPos;
		
		// modify for actual output
		if (bHasChildren && !IsItemExpanded(hti))
			sPos += "...";
		
		else if (nLevel == 0)
			sPos += ".";
		
		if (CRect().IntersectRect(rItem, rWindow)) // something to see
		{
			int nSaveDC = pDC->SaveDC();
			
			// fill background of selected item
			BOOL bFocused = HasFocus(/*!(GetStyle() & TVS_FULLROWSELECT)*/);
			BOOL bDropHilited = (GetItemState(hti, TVIS_DROPHILITED) & TVIS_DROPHILITED);
			
			if (bDropHilited)
				pDC->FillSolidRect(rItem, GetSysColor(COLOR_3DFACE));
			
			else if (bSelected)
			{
				if (bFocused)
					pDC->FillSolidRect(rItem, GetSysColor(COLOR_HIGHLIGHT));
				else
					pDC->FillSolidRect(rItem, GetSysColor(COLOR_3DFACE));
			}
			else if (m_crAltLines != -1 && ItemLineIsOdd(hti))
				pDC->FillSolidRect(rItem, m_crAltLines);
			
			// draw pos
			COLORREF crTextColor = GetSysColor((bSelected && bFocused) ? COLOR_HIGHLIGHTTEXT : COLOR_WINDOWTEXT);
			
			pDC->SetTextColor(crTextColor);
			pDC->SetBkMode(TRANSPARENT);
			
			rItem.left += NCG_COLPADDING;
			pDC->DrawText(sPos, rItem, DT_SINGLELINE | DT_VCENTER | DT_LEFT);
			
			pDC->RestoreDC(nSaveDC);
		}
		
		// vertical divider
		if (!bSelected && m_crGridlines != -1)
			pDC->FillSolidRect(rItem.right - 1, rItem.top, 1, rItem.Height(), m_crGridlines);
	}
}

void COrderedTreeCtrl::PostNcDrawItem(CDC* pDC, DWORD /*dwItem*/, const CRect& rItem, int nLevel)
{
	if (nLevel == 0 && m_crGridlines != -1)
		pDC->FillSolidRect(rItem.left, rItem.bottom - 1, rItem.Width(), 1, m_crGridlines);
}

BOOL COrderedTreeCtrl::OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	// invalidate client area 
	if (m_crAltLines != -1)
	{
		CRect rClient;
		CTreeCtrlHelper::GetClientRect(rClient, pNMTreeView->itemNew.hItem);
		InvalidateRect(rClient);
	}
	else if (pNMTreeView->itemNew.state & TVIS_EXPANDED)
	{
		InvalidateItem(pNMTreeView->itemNew.hItem);
	}
	
	m_gutter.RecalcGutter();
	
	*pResult = 0;
	return FALSE;
}

BOOL COrderedTreeCtrl::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMCUSTOMDRAW* pNMCD = (NMCUSTOMDRAW*)pNMHDR;
	NMTVCUSTOMDRAW* pTVCD = (NMTVCUSTOMDRAW*)pNMCD;
	
	if (pNMCD->dwDrawStage == CDDS_ITEMPREPAINT)
	{
		HTREEITEM hti = (HTREEITEM)pTVCD->nmcd.dwItemSpec;

		// do the odd lines
		if (m_crAltLines != -1 && ItemLineIsOdd(hti))
		{
	 		BOOL bFullRow = (GetStyle() & TVS_FULLROWSELECT);
         
			CRect rect(pNMCD->rc);
			pTVCD->clrTextBk = GetAlternateLineColor();
						
			if (bFullRow)
				*pResult |= CDRF_NEWFONT;
			else
			{
				COLORREF crOld = ::SetBkColor(pTVCD->nmcd.hdc, pTVCD->clrTextBk);
				::ExtTextOut(pTVCD->nmcd.hdc, rect.left, rect.top, ETO_OPAQUE, rect, NULL, 0, NULL);
				::SetBkColor(pTVCD->nmcd.hdc, crOld);
			}
		}

      if (m_crGridlines != -1)
		*pResult |= CDRF_NOTIFYPOSTPAINT;
	}
	else if (pNMCD->dwDrawStage == CDDS_ITEMPOSTPAINT && m_crGridlines != -1)
	{
		CDC* pDC = CDC::FromHandle(pNMCD->hdc);
		
		// if the next visible item is a top level item then draw a horz line below us
		HTREEITEM hti = (HTREEITEM)pTVCD->nmcd.dwItemSpec;
		HTREEITEM htiNext = CTreeCtrl::GetNextVisibleItem(hti);
		
		BOOL bFullRow = (GetStyle() & TVS_FULLROWSELECT);
		
		if (!GetParentItem(htiNext))
		{
			// but if the bottom of the text coincides with the bottom of the 
			// item and we have the focus then take care not to draw over the focus rect
			// but only if we do _not have TVS_FULLROWSELECT style
			if ((pNMCD->uItemState & CDIS_FOCUS) && !bFullRow)
			{
				CRect rText;//(pNMCD->rc);
				GetItemRect(hti, rText, TRUE);
				
				if (rText.bottom == pNMCD->rc.bottom)
				{
					pDC->FillSolidRect(pNMCD->rc.left, pNMCD->rc.bottom - 1, rText.left - pNMCD->rc.left, 1, m_crGridlines);
					pDC->FillSolidRect(rText.right, pNMCD->rc.bottom - 1, pNMCD->rc.right - rText.right, 1, m_crGridlines);
				}
				else
					pDC->FillSolidRect(pNMCD->rc.left, pNMCD->rc.bottom - 1, pNMCD->rc.right - pNMCD->rc.left, 1, m_crGridlines);
			}
			else
				pDC->FillSolidRect(pNMCD->rc.left, pNMCD->rc.bottom - 1, pNMCD->rc.right - pNMCD->rc.left, 1, m_crGridlines);
		}
		
		*pResult |= CDRF_DODEFAULT;
	}
	
	return FALSE; // to continue routing
}

int COrderedTreeCtrl::RecalcColumnWidth(CDC* pDC, UINT nColID, UINT& nWidth)
{
	switch (nColID)
	{
	case OTC_POSCOLUMNID:
		if (m_bShowingPosColumn)
		{
			g_mapWidths.RemoveAll(); // to handle a font change
			HTREEITEM hti = GetChildItem(NULL);
			int nPos = 1;
			int nMaxWidth = 0;
			
			while (hti)
			{
				int nItemWidth = GetGutterWidth(hti, 0, nPos, pDC);
				nMaxWidth = max(nMaxWidth, nItemWidth);
				
				hti = CTreeCtrl::GetNextItem(hti, TVGN_NEXT);
				nPos++;
			}
			
			nWidth = max(nMaxWidth, MINGUTTER);
		}
		else
			nWidth = 0;
		
		// rebuild visible item map if there are other columns
		if (m_bShowingPosColumn || m_gutter.GetColumnCount() > 1)
			BuildVisibleIndexMap();
		
		return TRUE;
	}
	
	return 0;
}

UINT COrderedTreeCtrl::GetGutterWidth(HTREEITEM hti, int nLevel, int nPos, CDC* pDC)
{
	UINT nMaxWidth = 0;
	
	if (GetItemState(hti, TVIS_EXPANDED) & TVIS_EXPANDED)
	{
		HTREEITEM htiChild = GetChildItem(hti);
		int nPosChild = 1;
		nLevel++;
		
		while (htiChild)
		{
			UINT nWidth = GetGutterWidth(htiChild, nLevel, nPosChild, pDC);
			nMaxWidth = max(nMaxWidth, nWidth);
			
			htiChild = CTreeCtrl::GetNextItem(htiChild, TVGN_NEXT);
			nPosChild++;
		}
	}
	else if (ItemHasChildren(hti)/* && ShowingEllipsis(nLevel)*/)
	{
		nMaxWidth = GetWidth(-1, pDC); // ellipsis
	}
	
	return nMaxWidth + GetWidth(nPos, pDC);
}

BOOL COrderedTreeCtrl::OnClick(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
{
	// make sure the selection is correctly set
	CPoint point(::GetMessagePos());
	ScreenToClient(&point);
	
	HTREEITEM htiHit = HitTest(point);
	
	if (htiHit)
		SelectItem(htiHit);
	
	*pResult = 0;
	
	return FALSE; // to continue routing
}

void COrderedTreeCtrl::OnBeginDrag(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMTREEVIEW* pNMTV = (NMTREEVIEW*)pNMHDR;
	SelectItem(pNMTV->itemNew.hItem);
	
	m_gutter.Redraw();
	*pResult = 0;
}

UINT COrderedTreeCtrl::GetWidth(int nNumber, CDC* pDC)
{
	UINT nWidth = 0;
	
	if (g_mapWidths.Lookup(nNumber, nWidth))
		return nWidth;
	
	if (nNumber >= 0)
	{
		CString sNumber;
		sNumber.Format("%d.", nNumber);
		nWidth = pDC->GetTextExtent(sNumber).cx;
	}
	else if (nNumber == -1)
		nWidth = pDC->GetTextExtent("...").cx;
	
	g_mapWidths[nNumber] = nWidth;
	return nWidth;
}

LRESULT COrderedTreeCtrl::OnGutterNotifyItemClick(WPARAM /*wParam*/, LPARAM lParam)
{
	NCGITEMCLICK* pNGIC = (NCGITEMCLICK*)lParam;
	ASSERT (pNGIC);
	
	EndLabelEdit(FALSE); // always
	
	HTREEITEM hti = (HTREEITEM)pNGIC->dwItem;
	SelectItem(hti); // always
	
	switch (pNGIC->nMsgID)
	{
	case WM_NCLBUTTONDBLCLK:
	case WM_LBUTTONDBLCLK:
		if (hti && ItemHasChildren(hti))
		{
			// kill any label edit that might have been started by
			// the first button click
			EndLabelEdit(TRUE);
			
			UINT nHitFlags = 0;
			HitTest(pNGIC->ptClick, &nHitFlags);
			
			if (pNGIC->nMsgID != WM_LBUTTONDBLCLK ||
				!(nHitFlags & (TVHT_ONITEMBUTTON | TVHT_ONITEMLABEL | TVHT_ONITEMICON)))
			{
				BOOL bExpanded = IsItemExpanded(hti);
				Expand(hti, bExpanded ? TVE_COLLAPSE : TVE_EXPAND);
				
				CRect rItem;
				CTreeCtrlHelper::GetClientRect(rItem, hti);
				InvalidateRect(rItem);
				
				return TRUE; // handled
			}
		}
		break;
	}
	
	return FALSE;
}

void COrderedTreeCtrl::OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpSS)
{
	if (nStyleType == GWL_STYLE)
	{
		BOOL bButtonsOld = lpSS->styleOld & TVS_HASBUTTONS;
		BOOL bButtonsNew = lpSS->styleNew & TVS_HASBUTTONS;
		
		if (bButtonsOld != bButtonsNew)
			RecalcGutter();
		
		else if (bButtonsNew)
		{
			UINT uStyleOld = lpSS->styleOld & (TVS_HASLINES | TVS_LINESATROOT);
			UINT uStyleNew = lpSS->styleNew & (TVS_HASLINES | TVS_LINESATROOT);
			
			if (uStyleOld != uStyleNew)
				RecalcGutter();
		}
	}
	
	CTreeCtrl::OnStyleChanged(nStyleType, lpSS);
}


void COrderedTreeCtrl::OnSettingChange(UINT /*uFlags*/, LPCTSTR /*lpszSection*/) 
{
	// there is the strangest 'bug' under XP using the default 96DPI font
	// setting, such that allowing the treectrl to receive this message
	// results in corrupted expansion icons.
	
	// to cause the minimum sideeffects we check for XP, theming and whether
	// buttons are visible
	// nope. disable completed for now
	//	if (!((GetStyle() & TVS_HASBUTTONS) && CThemed(this).AreControlsThemed()))
	//		CTreeCtrl::OnSettingChange(uFlags, lpszSection);
}


