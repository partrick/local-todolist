// TreeCtrlHelper.cpp: implementation of the CTreeCtrlHelper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TreeCtrlHelper.h"
#include "holdredraw.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const int HVISIBLE = 20;

CTreeCtrlHelper::CTreeCtrlHelper(CTreeCtrl& tree) : m_tree(tree)
{

}

CTreeCtrlHelper::~CTreeCtrlHelper()
{

}

int CTreeCtrlHelper::GetItemPos(HTREEITEM hti, HTREEITEM htiParent)
{
	ASSERT (m_tree.GetParentItem(hti) == htiParent);
	
	int nPos = 1;
	HTREEITEM htiChild = m_tree.GetChildItem(htiParent);
	
	while (htiChild && htiChild != hti)
	{
		nPos++;
		htiChild = m_tree.GetNextItem(htiChild, TVGN_NEXT);
	}
	
	return nPos;
}

int CTreeCtrlHelper::GetItemLevel(HTREEITEM hti)
{
	int nLevel = 0;
	hti = m_tree.GetParentItem(hti);
	
	while (hti)
	{
		nLevel++;
		hti = m_tree.GetParentItem(hti);
	}
	
	return nLevel;
}

BOOL CTreeCtrlHelper::HasFocus(BOOL bIncEditing)
{
	CWnd* pFocus = m_tree.GetFocus();
	
	return (pFocus && (pFocus == &m_tree || (bIncEditing && pFocus == m_tree.GetEditControl())));
}

void CTreeCtrlHelper::ExpandAll(BOOL bExpand, HTREEITEM hti)
{
   CHoldRedraw hr(m_tree);

   ExpandItem(hti, bExpand);
}

void CTreeCtrlHelper::ExpandItem(HTREEITEM hti, BOOL bExpand)
{
   if (hti)
      m_tree.Expand(hti, bExpand ? TVE_EXPAND : TVE_COLLAPSE);

   HTREEITEM htiChild = m_tree.GetChildItem(hti);

   while (htiChild)
   {
      ExpandItem(htiChild, bExpand);
      htiChild = m_tree.GetNextItem(htiChild, TVGN_NEXT);
   }
}

void CTreeCtrlHelper::GetClientRect(LPRECT lpRect, HTREEITEM htiFrom)
{
	m_tree.GetClientRect(lpRect);

	if (htiFrom)
	{
		CRect rItem;
		m_tree.GetItemRect(htiFrom, rItem, FALSE);
		lpRect->top = max(0, rItem.top); // rItem.top could be invisible
	}
}

HTREEITEM CTreeCtrlHelper::GetFirstVisibleTopLevelItem(int& nPos)
{
	HTREEITEM htiTopVis = GetTopLevelParentItem(m_tree.GetFirstVisibleItem());

	// iterate the top level items to find out what pos this is
	nPos = GetItemPos(htiTopVis, NULL);
	
	return htiTopVis;
}

HTREEITEM CTreeCtrlHelper::GetTopLevelParentItem(HTREEITEM hti)
{
	HTREEITEM htiPrevParent = hti;
	hti = m_tree.GetParentItem(hti);

	while (hti)
	{
		htiPrevParent = hti;
		hti = m_tree.GetParentItem(hti);
	}
	
	return htiPrevParent;
}

void CTreeCtrlHelper::InvalidateItem(HTREEITEM hti, BOOL bChildren)
{
	CRect rItem;
	
	if (m_tree.GetItemRect(hti, rItem, FALSE))
		m_tree.InvalidateRect(rItem);
	
	if (bChildren && IsItemExpanded(hti))
	{
		HTREEITEM htiChild = m_tree.GetChildItem(hti);
		
		while (htiChild)
		{
			InvalidateItem(htiChild, TRUE);
			htiChild = m_tree.GetNextItem(htiChild, TVGN_NEXT);
		}
	}
}

int CTreeCtrlHelper::BuildVisibleIndexMap(CMapIndices& index)
{
	index.RemoveAll();
	
	HTREEITEM hti = m_tree.GetChildItem(NULL);
	
	while (hti)
	{
		AddVisibleItemToIndex(index, hti);
		hti = m_tree.GetNextItem(hti, TVGN_NEXT);
	}
	
	return index.GetCount();
}

void CTreeCtrlHelper::AddVisibleItemToIndex(CMapIndices& index, HTREEITEM hti)
{
	ASSERT (hti);
	
	int nIndex = index.GetCount();
	index[hti] = nIndex;
	
	if (IsItemExpanded(hti))
	{
		HTREEITEM htiChild = m_tree.GetChildItem(hti);
		
		while (htiChild)
		{
			AddVisibleItemToIndex(index, htiChild);
			htiChild = m_tree.GetNextItem(htiChild, TVGN_NEXT);
		}
	}
}

void CTreeCtrlHelper::EnsureVisibleEx(HTREEITEM hti, BOOL bVPartialOK, BOOL bHPartialOK)
{
	CRect rItem;

	if (m_tree.GetItemRect(hti, rItem, FALSE))
	{
		CRect rClient, rText, rIntersect;

		m_tree.GetClientRect(rClient);
	
		BOOL bNeedHScroll = TRUE, bNeedVScroll = TRUE;

		// vertically
		rClient.InflateRect(0, m_tree.GetItemHeight());

		if (rIntersect.IntersectRect(rClient, rItem))
		{
			if (bVPartialOK || (rItem.top >= rClient.top && rItem.bottom <= rClient.bottom))
				bNeedVScroll = FALSE;
		}

		// horizontally
		rClient.DeflateRect(HVISIBLE, 0);
		m_tree.GetItemRect(hti, rText, TRUE);

		if (rIntersect.IntersectRect(rClient, rText))
		{
			if (bHPartialOK || (rText.left >= rClient.left && rText.right <= rClient.right))
				bNeedHScroll = FALSE;
		}

		// see if we're close enough already
		if (!bNeedVScroll && !bNeedHScroll)
			return;

		m_tree.SetRedraw(FALSE);

		// vertical scroll
		if (bNeedVScroll)
		{
			// now get us as close as possible first
			// Only use CTreeCtrl::EnsureVisible if we're not in the right vertical pos
			// because it will also modify the horizontal pos which we don't want
			m_tree.EnsureVisible(hti);
			m_tree.GetItemRect(hti, rItem, FALSE);

			CRect rOrg(rItem);
			
			if (rItem.top < rClient.top || (bVPartialOK && rItem.bottom < rClient.top))
			{
				while (rClient.top > (bVPartialOK ? rItem.bottom : rItem.top))
				{
					m_tree.SendMessage(WM_VSCROLL, SB_LINEUP);
					m_tree.GetItemRect(hti, rItem, TRUE); // check again
					
					// check for no change
					if (rItem == rOrg)
						break;
					
					rOrg = rItem;
				}
			}
			else if (rItem.bottom > rClient.bottom || (bVPartialOK && rItem.top > rClient.bottom))
			{
				while (rClient.bottom < (bVPartialOK ? rItem.top : rItem.bottom))
				{
					m_tree.SendMessage(WM_VSCROLL, SB_LINEDOWN);
					m_tree.GetItemRect(hti, rItem, TRUE); // check again
					
					// check for no change
					if (rItem == rOrg)
						break;
					
					rOrg = rItem;
				}
			}
			
			bNeedVScroll = TRUE;
		}

		// horizontal scroll
		// reset scroll pos if we previously called CTreeCtrl::EnsureVisible
		if (bNeedVScroll)
		{
			m_tree.SendMessage(WM_HSCROLL, SB_LEFT);
			m_tree.GetItemRect(hti, rText, TRUE);
		}

		if (bNeedVScroll || bNeedHScroll)
		{
			rItem = rText;
			CRect rOrg(rItem);

			if (rItem.left < rClient.left || (bHPartialOK && rItem.right < rClient.left))
			{
				while (rClient.left > (bHPartialOK ? rItem.right : rItem.left))
				{
					m_tree.SendMessage(WM_HSCROLL, SB_LINELEFT);
					m_tree.GetItemRect(hti, rItem, TRUE); // check again

					// check for no change
					if (rItem == rOrg)
						break;

					rOrg = rItem;
				}
			}
			else if (rItem.right > rClient.right || (bHPartialOK && rItem.left > rClient.right))
			{
				while (rClient.right < (bHPartialOK ? rItem.left : rItem.right))
				{
					m_tree.SendMessage(WM_HSCROLL, SB_LINERIGHT);
					m_tree.GetItemRect(hti, rItem, TRUE); // check again

					// check for no change
					if (rItem == rOrg)
						break;

					rOrg = rItem;
				}
			}
		}
		
		m_tree.SetRedraw(TRUE);
	}
	else
		m_tree.EnsureVisible(hti);
}

BOOL CTreeCtrlHelper::ItemLineIsOdd(CMapIndices& index, HTREEITEM hti)
{
	// simple check on whether Visible item map has been created
	// for the first time
	if (m_tree.GetCount() && !index.GetCount())
		BuildVisibleIndexMap(index);
	
	int nIndex = -1;
	
	if (index.Lookup(hti, nIndex))
		return (nIndex % 2);
	
	return FALSE;
}

void CTreeCtrlHelper::SetItemIntegral(HTREEITEM hti, int iIntegral)
{
	TVITEMEX tvi;
	tvi.mask = TVIF_HANDLE | TVIF_INTEGRAL;
	tvi.hItem = hti;
	tvi.iIntegral = iIntegral;
	
	m_tree.SetItem((LPTVITEM)&tvi);
}

BOOL CTreeCtrlHelper::IsItemExpanded(HTREEITEM hItem) const
{
	return m_tree.GetItemState(hItem, TVIS_EXPANDED) & TVIS_EXPANDED;
}

void CTreeCtrlHelper::SetItemChecked(HTREEITEM hti, TCH_CHECK nChecked)
{
	int nCheckState = 0;
   
   switch (nChecked)
   {
   case TCHC_UNCHECKED:
      nCheckState = INDEXTOSTATEIMAGEMASK(1);
      break;
      
   case TCHC_CHECKED:
      nCheckState = INDEXTOSTATEIMAGEMASK(2);
      break;
      
   case TCHC_MIXED:
      nCheckState = INDEXTOSTATEIMAGEMASK(3);
      break;
   }

   if (nCheckState)
	m_tree.SetItemState(hti, nCheckState, TVIS_STATEIMAGEMASK);
}

void CTreeCtrlHelper::SetItemChecked(HTREEITEM hti, BOOL bChecked)
{
	SetItemChecked(hti, bChecked ? TCHC_CHECKED : TCHC_UNCHECKED);
}

TCH_CHECK CTreeCtrlHelper::GetItemCheckState(HTREEITEM hti)
{
	int nCheckState = m_tree.GetItemState(hti, TVIS_STATEIMAGEMASK);
	return (TCH_CHECK)((nCheckState  >> 12) - 1);
}

CString CTreeCtrlHelper::GetItemPath(HTREEITEM hti, int nMaxElementLen) const
{ 
	CString sPath;
	
	HTREEITEM htiParent = m_tree.GetParentItem(hti);
	
	while (htiParent)
	{
		CString sParent = m_tree.GetItemText(htiParent);
		
		if (nMaxElementLen != -1 && sParent.GetLength() > nMaxElementLen)
			sParent = sParent.Left(nMaxElementLen) + "...";
		
		sParent.TrimLeft();
		sParent.TrimRight();
		
		sPath = sParent + " / "+ sPath;
		htiParent = m_tree.GetParentItem(htiParent);
	}
	
	return sPath;
}

HTREEITEM CTreeCtrlHelper::GetTopLevelParentItem(HTREEITEM hti) const
{
	if (!hti)
		return NULL;
	
	HTREEITEM htiParent = m_tree.GetParentItem(hti);
	
	while (htiParent)
	{
		hti = htiParent; // cache this because the next parent might be the root
		htiParent = m_tree.GetParentItem(hti);
	}
	
	return hti; // return the one before the root
}

HTREEITEM CTreeCtrlHelper::GetNextTopLevelItem(HTREEITEM hti, BOOL bNext) const
{
	HTREEITEM htiParent = GetTopLevelParentItem(hti);
	HTREEITEM htiGoto = NULL;
	
	if (htiParent)
	{
		if (bNext)
			htiGoto = m_tree.GetNextItem(htiParent, TVGN_NEXT);
		else
		{
			// if the item is not htiParent then jump to it first
			if (hti != htiParent)
				htiGoto = htiParent;
			else
				htiGoto = m_tree.GetNextItem(htiParent, TVGN_PREVIOUS);

			if (htiGoto == hti)
				htiGoto = NULL; // nowhere to go
		}
	}
		
	return htiGoto;
}

int CTreeCtrlHelper::GetDistanceToEdge(HTREEITEM htiFrom, TCH_EDGE nToEdge)
{
	CRect rClient, rItem;

	if (m_tree.GetItemRect(htiFrom, rItem, FALSE))
	{
		m_tree.GetClientRect(rClient);
		int nItemHeight = m_tree.GetItemHeight();

		switch (nToEdge)
		{
		case TCHE_TOP:
			return (rItem.top - rClient.top) / nItemHeight;

		case TCHE_BOTTOM:
			return (rClient.bottom - rItem.bottom) / nItemHeight;
		}
	}

	return 0;
}

void CTreeCtrlHelper::SetMinDistanceToEdge(HTREEITEM htiFrom, TCH_EDGE nToEdge, int nItems)
{
	switch (nToEdge)
	{
	case TCHE_BOTTOM:
		{
			int nBorder = GetDistanceToEdge(htiFrom, TCHE_BOTTOM);

			while (nBorder < nItems)
			{
				m_tree.SendMessage(WM_VSCROLL, SB_LINEDOWN);
				nBorder++;
			}
		}
		break;

	case TCHE_TOP:
		{
			int nBorder = GetDistanceToEdge(htiFrom, TCHE_TOP);

			while (nBorder < nItems)
			{
				m_tree.SendMessage(WM_VSCROLL, SB_LINEUP);
				nBorder++;
			}
		}
		break;
	}
}

HTREEITEM CTreeCtrlHelper::GetNextPageVisibleItem(HTREEITEM hti) const
{
	// if we're the last visible item then its just the page count
	// figure out how many items to step
	HTREEITEM htiNext = m_tree.GetNextVisibleItem(hti);

	if (!htiNext || !IsFullyVisible(htiNext))
	{
		int nCount = m_tree.GetVisibleCount();

		// keep going until we get NULL and then return
		// the previous item
		while (hti && nCount--)
		{
			hti = GetNextVisibleItem(hti);

			if (hti)
				htiNext = hti;
		}
	}
	else // we keep going until we're the last visible item
	{
		// keep going until we get NULL and then return
		// the previous item
		while (hti)
		{
			hti = m_tree.GetNextVisibleItem(hti);
			
			if (hti && IsFullyVisible(hti))
				htiNext = hti;
			else
				hti = NULL;
		}
	}

	return htiNext;
}

HTREEITEM CTreeCtrlHelper::GetPrevPageVisibleItem(HTREEITEM hti) const
{
	// if we're the first visible item then its just the page count
	// figure out how many items to step
	HTREEITEM htiPrev = m_tree.GetPrevVisibleItem(hti);

	if (!htiPrev || !IsFullyVisible(htiPrev))
	{
		int nCount = m_tree.GetVisibleCount();

		// keep going until we get NULL and then return
		// the previous item
		while (hti && nCount--)
		{
			hti = GetPrevVisibleItem(hti);

			if (hti)
				htiPrev = hti;
		}
	}
	else // we keep going until we're the first visible item
	{
		// keep going until we get NULL and then return
		// the previous item
		while (hti)
		{
			hti = m_tree.GetPrevVisibleItem(hti);
			
			if (hti && IsFullyVisible(hti))
				htiPrev = hti;
			else
				hti = NULL;
		}
	}

	return htiPrev;
}

HTREEITEM CTreeCtrlHelper::GetNextVisibleItem(HTREEITEM hti, BOOL bAllowChildren) const
{
	HTREEITEM htiNext = NULL;
	
	// try our first child if we're expanded
	if (bAllowChildren && m_tree.ItemHasChildren(hti) && IsItemExpanded(hti))
		htiNext = m_tree.GetChildItem(hti);
	else
	{
		// try next sibling
		htiNext = m_tree.GetNextItem(hti, TVGN_NEXT);
		
		// finally look up the parent chain as far as we can
		if (!htiNext)
		{
			HTREEITEM htiParent = hti;

			while (htiParent && !htiNext)
			{
				htiParent = m_tree.GetParentItem(htiParent);

				if (htiParent)
					htiNext = m_tree.GetNextItem(htiParent, TVGN_NEXT);
			}
		}
	}

	return htiNext == TVI_ROOT ? NULL : htiNext;
}

HTREEITEM CTreeCtrlHelper::GetPrevVisibleItem(HTREEITEM hti, BOOL bAllowChildren) const
{
	// try our prior sibling
	HTREEITEM htiPrev = m_tree.GetNextItem(hti, TVGN_PREVIOUS);
	
	// if we have one then first try its last child
	if (htiPrev)
	{
		if (bAllowChildren && m_tree.ItemHasChildren(htiPrev) && IsItemExpanded(htiPrev))
		{
			HTREEITEM htiChild = m_tree.GetChildItem(htiPrev);
			
			while (htiChild)
			{
				htiPrev = htiChild;
				htiChild = m_tree.GetNextItem(htiChild, TVGN_NEXT);
			}
		}
		// else we settle for htiPrev as-is
	}
	else // get parent
		htiPrev = m_tree.GetParentItem(hti);

	return htiPrev == TVI_ROOT ? NULL : htiPrev;
}

BOOL CTreeCtrlHelper::IsFullyVisible(HTREEITEM hti) const
{
	CRect rClient, rItem, rIntersect;

	m_tree.GetClientRect(rClient);
	m_tree.GetItemRect(hti, rItem, FALSE);

	return (rIntersect.IntersectRect(rItem, rClient) && rIntersect == rItem);
}

int CTreeCtrlHelper::FindItem(HTREEITEM htiFind, HTREEITEM htiStart)
{
	// try same first
	if (htiFind == htiStart)
		return 0;

	// then try up
	HTREEITEM htiPrev = GetPrevVisibleItem(htiStart);

	while (htiPrev)
	{
		if (htiPrev == htiFind)
			return -1;

		htiPrev = GetPrevVisibleItem(htiPrev);
	}

	// else try down
	HTREEITEM htiNext = GetNextVisibleItem(htiStart);

	while (htiNext)
	{
		if (htiNext == htiFind)
			return 1;

		htiNext = GetNextVisibleItem(htiNext);
	}

	// else
	return 0;
}

BOOL CTreeCtrlHelper::IsItemBold(HTREEITEM hti)
{
	return (m_tree.GetItemState(hti, TVIS_BOLD) & TVIS_BOLD);
}

void CTreeCtrlHelper::SetItemBold(HTREEITEM hti, BOOL bBold)
{
	m_tree.SetItemState(hti, bBold ? TVIS_BOLD : 0, TVIS_BOLD);
}

void CTreeCtrlHelper::SetItemStateEx(HTREEITEM hti, UINT nState, UINT nMask, BOOL bChildren)
{
	if (hti)
		m_tree.SetItemState(hti, nState, nMask);

	if (bChildren)
	{
		HTREEITEM htiChild = m_tree.GetChildItem(hti);
		
		while (htiChild)
		{
			SetItemStateEx(htiChild, nState, nMask, TRUE);
			htiChild = m_tree.GetNextItem(htiChild, TVGN_NEXT);
		}
	}
}

void CTreeCtrlHelper::SetTopLevelItemsBold(BOOL bBold)
{
	// clear all bold states
	SetItemStateEx(NULL, 0, TVIS_BOLD, TRUE);

	// set top items bold
	if (bBold)
	{
		HTREEITEM hti = m_tree.GetChildItem(NULL);
		
		while (hti)
		{
			SetItemBold(hti, TRUE);
			hti = m_tree.GetNextItem(hti, TVGN_NEXT);
		}
	}
}
