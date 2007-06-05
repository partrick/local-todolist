// MenuIconMgr.h: interface for the CMenuIconMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MENUICONMGR_H__0FF0228C_515C_4E93_A957_1952AFD0F3A1__INCLUDED_)
#define AFX_MENUICONMGR_H__0FF0228C_515C_4E93_A957_1952AFD0F3A1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\shared\Subclass.h"
#include <afxtempl.h>

class CMenuIconMgr : public CSubclassWnd  
{
public:
	CMenuIconMgr();
	virtual ~CMenuIconMgr();
	
	BOOL Initialize(CWnd* pWnd);
	int AddImages(const CToolBar& toolbar);
	BOOL AddImage(UINT nCmdID, HICON hIcon);
	int AddImages(const CUIntArray& aCmdIDs, const CImageList& il);
	int AddImages(const CUIntArray& aCmdIDs, UINT nIDBitmap, int nCx, COLORREF crMask);
	
	BOOL ChangeImageID(UINT nCmdID, UINT nNewCmdID);
	
	void ClearImages() { m_mapID2Icon.RemoveAll(); }
	
protected:
	CMap<UINT, UINT, HICON, HICON> m_mapID2Icon, m_mapID2IconSelected;
	
	virtual LRESULT WindowProc(HWND hRealWnd, UINT msg, WPARAM wp, LPARAM lp);
	
	BOOL OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpdis);
	void OnInitMenuPopup(CMenu* pMenu, UINT nIndex, BOOL bSysMenu);
	BOOL OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpmis);
	
	HICON LoadItemImage(UINT nCmdID, BOOL bSelected = FALSE);
	int AddImages(const CUIntArray& aCmdIDs, const CImageList& il, BOOL bSelected);
};

#endif // !defined(AFX_MENUICONMGR_H__0FF0228C_515C_4E93_A957_1952AFD0F3A1__INCLUDED_)
