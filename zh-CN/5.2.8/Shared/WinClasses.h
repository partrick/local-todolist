// WinClasses.h: interface for the CWinClasses class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WINCLASSES_H__4A958ACC_A399_4B69_A988_FA70F16C3EAC__INCLUDED_)
#define AFX_WINCLASSES_H__4A958ACC_A399_4B69_A988_FA70F16C3EAC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// global helper function for everyone to use
void TRACEWND(LPCTSTR szFunctionName, HWND hWnd);

class CWinClasses  
{
public:
	static CString GetClass(HWND hWnd);
	static CString GetClassEx(HWND hWnd); // will also determine the base type of mfc (Afx) classes
	static BOOL IsControlClass(HWND hWnd);
	static BOOL IsControlClass(LPCTSTR szClass);
	static BOOL IsClass(HWND hWnd, LPCTSTR szClass);
	static BOOL IsClassEx(HWND hWnd, LPCTSTR szClass);
	static BOOL IsClass(LPCTSTR szClass, LPCTSTR szWndClass) { return (lstrcmpi(szClass, szWndClass) == 0); }
	static BOOL IsClassEx(LPCTSTR szClass, LPCTSTR szWndClass);

	static BOOL IsDialog(HWND hWnd);
	static BOOL IsEditControl(HWND hWnd);
	static BOOL IsRichEditControl(LPCTSTR szClass);
	static BOOL IsRichEditControl(HWND hWnd);

protected:
	static CMapStringToPtr s_mapCtrlClasses;
};

#endif // !defined(AFX_WINCLASSES_H__4A958ACC_A399_4B69_A988_FA70F16C3EAC__INCLUDED_)
