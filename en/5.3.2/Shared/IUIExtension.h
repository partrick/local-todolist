// IUIExtension.h: interface and implementation of the IUIExtension class.
//
/////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_IUIEXTENSION_H__7741547B_BA15_4851_A41B_2B4EC1DC12D5__INCLUDED_)
#define AFX_IUIEXTENSION_H__7741547B_BA15_4851_A41B_2B4EC1DC12D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Windows.h>

// function to be exported from dll to create instance of interface
#ifdef _EXPORTING // declare this in project settings for dll _only_
#	define DLL_DECLSPEC __declspec(dllexport)
#else
#	define DLL_DECLSPEC __declspec(dllimport)
#endif 

#define IUIEXTENSION_VERSION 0x0000

class IUIExtensionWindow;
class IUIExtension;
class ITaskList;

typedef IUIExtension* (*PFNCREATEUIEXT)(); // function prototype
extern "C" DLL_DECLSPEC IUIExtension* CreateUIExtensionInterface();

typedef int (*PFNGETVERSION)(); // function prototype
extern "C" DLL_DECLSPEC int GetInterfaceVersion();

#pragma warning(disable:4505)
#pragma warning(disable:4189)

// helper method
static IUIExtension* CreateUIExtensionInterface(const char* szDllPath)
{
    IUIExtension* pInterface = NULL;
    HMODULE hDll = LoadLibrary(szDllPath);
	
    if (hDll)
    {
        PFNCREATEUIEXT pCreate = (PFNCREATEUIEXT)GetProcAddress(hDll, "CreateUIExtensionInterface");
		
        if (pCreate)
		{
			// check version
			PFNGETVERSION pVersion = (PFNGETVERSION)GetProcAddress(hDll, "GetInterfaceVersion");

			if (!IUIEXTENSION_VERSION || (pVersion && pVersion() >= IUIEXTENSION_VERSION))
				pInterface = pCreate();
		}

		if (hDll && !pInterface)
			FreeLibrary(hDll);
    }
	
    return pInterface;
}

static BOOL IsUIExtemsionDll(const char* szDllPath)
{
    HMODULE hDll = LoadLibrary(szDllPath);
	
    if (hDll)
    {
		PFNCREATEUIEXT pCreateImp = (PFNCREATEUIEXT)GetProcAddress(hDll, "CreateUIExtensionInterface");
		FreeLibrary(hDll);

		return (pCreateImp != NULL);
	}

	return FALSE;
}

class IUIExtension
{
public:
    virtual void Release() = 0; // releases the interface

	virtual const char* GetMenuText() = 0; // caller must copy result only
	virtual HICON GetIcon() = 0;

	virtual IUIExtensionWindow* CreateExtensionWindow(HWND hParent, BOOL bShow = TRUE, LPSIZE pSize = NULL) = 0;
};

enum 
{ 
	UIU_EDIT	= 0x01, 
	UIU_ADD		= 0x02, 
	UIU_DELETE  = 0x04,
	UIU_MOVE	= 0x08,
//  UIU_

	UIU_ALL		= 0xff
};

class IUIExtensionWindow
{
public:
	virtual BOOL Create(HWND hParent, BOOL bShow = TRUE, LPSIZE pSize = NULL) = 0;
	virtual BOOL Show(BOOL bShow = TRUE) = 0;
	virtual BOOL IsShowing() const = 0;

	virtual void Update(const ITaskList* pTasks, DWORD dwFlags = UIU_ALL) = 0;
	virtual void Release() = 0;
};

static void ReleaseUIExtensionInterface(IUIExtension*& pInterface)
{
    if (pInterface)
    {
        pInterface->Release();
        pInterface = NULL;
    }
}

#endif // !defined(AFX_IUIEXTENSION_H__7741547B_BA15_4851_A41B_2B4EC1DC12D5__INCLUDED_)
