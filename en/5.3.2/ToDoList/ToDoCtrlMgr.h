// CToDoCtrlMgr.h : header file
//

#if !defined(AFX_TODOCTRLMGR_H__13051D32_D372_4205_BA71_05FAC2159F1C__INCLUDED_)
#define AFX_TODOCTRLMGR_H__13051D32_D372_4205_BA71_05FAC2159F1C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "filteredtodoctrl.h"
#include "preferencesdlg.h"

#include "..\shared\filemisc.h"
#include "..\shared\driveinfo.h"

/////////////////////////////////////////////////////////////////////////////
// CToDoCtrlMgr dialog
 
enum TDCM_PATHTYPE { TDCM_UNDEF = -1, TDCM_REMOVABLE, TDCM_FIXED, TDCM_REMOTE }; // drive types
enum TDCM_DUESTATUS { TDCM_NONE = -1, TDCM_PAST, TDCM_TODAY, TDCM_FUTURE }; 

class CToDoCtrlMgr
{
	// Construction
public:
	CToDoCtrlMgr(CTabCtrl& tabCtrl); 
	~CToDoCtrlMgr();

	inline int GetCount() const { return m_aToDoCtrls.GetSize(); }
	inline int GetSelToDoCtrl() const { return m_tabCtrl.GetCurSel(); }

	void SetPrefs(const CPreferencesDlg* pPrefs);

	void RefreshColumns(int nIndex, CTDCColumnArray& aColumns /*out*/);
	BOOL HasOwnColumns(int nIndex) const;
	void SetHasOwnColumns(int nIndex, BOOL bHas = TRUE);

	CFilteredToDoCtrl& GetToDoCtrl(int nIndex);
	const CFilteredToDoCtrl& GetToDoCtrl(int nIndex) const;

	int RemoveToDoCtrl(int nIndex, BOOL bDelete = FALSE); // returns new selection
	int AddToDoCtrl(CFilteredToDoCtrl* pCtrl, BOOL bLoaded = TRUE);
	BOOL IsPristine(int nIndex) const;
	BOOL IsLoaded(int nIndex) const;
	void SetLoaded(int nIndex, BOOL bLoaded = TRUE);

	BOOL HasTasks(int nIndex) const;
	BOOL AnyHasTasks() const;

	int FindToDoCtrl(const CFilteredToDoCtrl* pTDC) const;
	int FindToDoCtrl(LPCTSTR szFilePath) const;
	CString GetFilePath(int nIndex, BOOL bStrict = TRUE) const;
	void ClearFilePath(int nIndex);
	BOOL IsFilePathEmpty(int nIndex) const;
	TDCM_PATHTYPE GetFilePathType(int nIndex) const;
	TDCM_PATHTYPE RefreshPathType(int nIndex); 
	CString GetFriendlyProjectName(int nIndex) const;

	BOOL RefreshLastModified(int nIndex); // true if changed
	time_t GetLastModified(int nIndex) const;
	void SetModifiedStatus(int nIndex, BOOL bMod);
	BOOL GetModifiedStatus(int nIndex) const;
	void RefreshModifiedStatus(int nIndex);

	int GetReadOnlyStatus(int nIndex) const;
	BOOL RefreshReadOnlyStatus(int nIndex); // true if changed
	void UpdateToDoCtrlReadOnlyUIState(int nIndex);
	void UpdateToDoCtrlReadOnlyUIState(CFilteredToDoCtrl& tdc);

	void SetDueItemStatus(int nIndex, TDCM_DUESTATUS nStatus);
	TDCM_DUESTATUS GetDueItemStatus(int nIndex) const;

	int GetLastCheckoutStatus(int nIndex) const;
	void SetLastCheckoutStatus(int nIndex, BOOL bStatus);
	int CanCheckOut(int nIndex) const;
	int CanCheckIn(int nIndex) const;
	int CanCheckInOut(int nIndex) const;

	void MoveToDoCtrl(int nIndex, int nNumPlaces);
	BOOL CanMoveToDoCtrl(int nIndex, int nNumPlaces) const;

	int SortToDoCtrlsByName();
	BOOL PathSupportsSourceControl(int nIndex) const;
	BOOL PathSupportsSourceControl(LPCTSTR szPath) const;
	BOOL IsSourceControlled(int nIndex) const;

	CString UpdateTabItemText(int nIndex);
	CString GetTabItemText(int nIndex) const;
	CString GetTabItemTooltip(int nIndex) const;

	int ArchiveDoneTasks(int nIndex);
	CString GetArchivePath(LPCTSTR szFilePath) const;
	CString GetArchivePath(int nIndex) const;

	void SetNeedsPreferenceUpdate(int nIndex, BOOL bNeed);
	BOOL GetNeedsPreferenceUpdate(int nIndex) const;
	void SetAllNeedPreferenceUpdate(BOOL bNeed, int nExcept = -1);

	void PreparePopupMenu(CMenu& menu, UINT nID1, int nMax = 20) const;

	// Implementation
protected:
	struct TDCITEM
	{
		TDCITEM() 
		{ 
			pTDC = NULL; 
			bModified = FALSE; 
			bLastStatusReadOnly = -1; 
			tLastMod = 0; 
			bLastCheckoutSuccess = -1;
            nPathType = TDCM_UNDEF;
			nDueStatus = TDCM_NONE;
			bNeedPrefUpdate = TRUE;
			nCreated = -1;
			bLoaded = TRUE;
			bHasOwnColumns = FALSE;
		}
		
		TDCITEM(CFilteredToDoCtrl* pCtrl, int nIndex, BOOL _bLoaded) 
		{ 
			pTDC = pCtrl; 
			
			bModified = FALSE; 
			bLastStatusReadOnly = -1;
			tLastMod = 0;
			bLastCheckoutSuccess = -1;
			nDueStatus = TDCM_NONE;
			bNeedPrefUpdate = TRUE;
			nCreated = nIndex;
			bLoaded = _bLoaded;
			bHasOwnColumns = FALSE;
			
			CString sFilePath = pCtrl->GetFilePath();
			
			if (!sFilePath.IsEmpty())
			{
				bLastStatusReadOnly = (CDriveInfo::IsReadonlyPath(sFilePath) > 0);
				tLastMod = FileMisc::GetLastModified(sFilePath);
			}
			
            RefreshPathType();
		}
		
		CFilteredToDoCtrl* pTDC;
		BOOL bModified;
		BOOL bLastStatusReadOnly;
		time_t tLastMod;
		BOOL bLastCheckoutSuccess;
        TDCM_PATHTYPE nPathType;
		TDCM_DUESTATUS nDueStatus;
		BOOL bNeedPrefUpdate;
		int nCreated; // creation index regardless of actual position
		BOOL bLoaded;
		BOOL bHasOwnColumns;
		
		inline TDCM_PATHTYPE GetPathType() const { return nPathType; }
        
        void RefreshPathType() 
        { 
			LPCTSTR szFilePath = pTDC->GetFilePath();

			nPathType = TranslatePathType(CDriveInfo::GetPathType(szFilePath));
        }

		CString GetFriendlyProjectName() const
		{
			return pTDC->GetFriendlyProjectName(nCreated);
		}

		static TDCM_PATHTYPE TranslatePathType(int nDriveInfoType)
		{
            switch (nDriveInfoType)
            {
            case DRIVE_REMOTE:
                return TDCM_REMOTE;
				
            case DRIVE_REMOVABLE:
            case DRIVE_CDROM:
                return TDCM_REMOVABLE;
				
            case DRIVE_FIXED:
                return TDCM_FIXED;
			}

			// all else
			return TDCM_UNDEF;
		}
	};
	
	CArray<TDCITEM, TDCITEM&> m_aToDoCtrls;
	CTabCtrl& m_tabCtrl;
	const CPreferencesDlg* m_pPrefs;

protected:
	TDCITEM& GetTDCItem(int nIndex);
	const TDCITEM& GetTDCItem(int nIndex) const;

	// sort function
	static int SortProc(const void* v1, const void* v2);
	BOOL AreToDoCtrlsSorted() const;

	BOOL PathTypeSupportsSourceControl(TDCM_PATHTYPE nType) const;
	const CPreferencesDlg& Prefs() const;

	void RestoreFilter(CFilteredToDoCtrl* pCtrl);
	void SaveFilter(const CFilteredToDoCtrl* pCtrl) const;

	void RestoreColumns(TDCITEM* pTDCI);
	void SaveColumns(const TDCITEM* pTDCI) const;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TODOCTRLMGR_H__13051D32_D372_4205_BA71_05FAC2159F1C__INCLUDED_)
