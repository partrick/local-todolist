// EnCommandLineInfo.cpp: implementation of the CEnCommandLineInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EnCommandLineInfo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEnCommandLineInfo::CEnCommandLineInfo()
{
	m_nLastParameter = -1;		
}

CEnCommandLineInfo::~CEnCommandLineInfo()
{

}

void CEnCommandLineInfo::ParseParam(LPCTSTR lpszParam, BOOL bFlag, BOOL bLast)
{
	CString sLookup;

	if (bFlag) 
	{
		m_sLastOption = lpszParam; 	   // save in case other value specified
		m_sLastOption.MakeUpper();

		// this is a "flag" (begins with / or -)
		m_mapCommandLine[m_sLastOption] = "";    // default value is "TRUE"
		m_nLastParameter = -1;		
	} 
	else if (!m_sLastOption.IsEmpty()) // must be a parameter for the last option
	{
		m_nLastParameter++;

		sLookup.Format("%s_PARAMETER_%d", m_sLastOption, m_nLastParameter);
		m_mapCommandLine[sLookup] = lpszParam;
	}
	
	// Call base class so MFC can see this param/token.
	CCommandLineInfo::ParseParam(lpszParam, bFlag, bLast);

	if (bLast)
	{
		// if someone has dropped a file onto an application shortcut then
		// windows appends the filename to the end of the commandline
		// which means we have no way of knowing whether it's a parameter
		// for the last option or not so we test to see if its a valid
		// file and if it is we set it to m_strFileName
		DWORD dwAttrib = GetFileAttributes(m_strFileName);

		if (dwAttrib == 0xffffffff)
		{
			if (GetFileAttributes(lpszParam) != 0xffffffff)
				m_strFileName = lpszParam;
			else
				m_strFileName.Empty();
		}
	}
}

BOOL CEnCommandLineInfo::GetOption(LPCTSTR szFlag, CStringArray& aParams) const
{
	CString sFlag(szFlag), sLookup, sParameter;
	sFlag.MakeUpper();

	if (!m_mapCommandLine.Lookup(sFlag, sParameter))
		return FALSE;

	aParams.RemoveAll();

	int nParam = 0;
	sLookup.Format("%s_PARAMETER_%d", sFlag, nParam);

	while (m_mapCommandLine.Lookup(sLookup, sParameter))
	{
		aParams.Add(sParameter);

		nParam++;
		sLookup.Format("%s_PARAMETER_%d", sFlag, nParam);
	}

	return TRUE;
}

BOOL CEnCommandLineInfo::SetOption(LPCTSTR szFlag, LPCTSTR szParam)
{
	CString sFlag(szFlag), sLookup, sParameter;
	sFlag.MakeUpper();

	// option cannot already exist
	if (m_mapCommandLine.Lookup(sFlag, sParameter))
		return FALSE;

	// create flag
	m_mapCommandLine[sFlag] = "";

	// set szParam as the one and only option parameter
	sLookup.Format("%s_PARAMETER_0", sFlag);
	m_mapCommandLine[sLookup] = szParam;

	return TRUE;
}

BOOL CEnCommandLineInfo::SetOption(LPCTSTR szFlag, DWORD dwParam)
{
	CString sParam;
	sParam.Format("%d", dwParam);

	return SetOption(szFlag, sParam);
}

BOOL CEnCommandLineInfo::GetOption(LPCTSTR szFlag, CString& sParam) const
{
	CStringArray aParams;

	if (GetOption(szFlag, aParams))
	{
		if (aParams.GetSize())
			sParam = aParams[0];

		return TRUE;
	}

	return FALSE;
}

CString CEnCommandLineInfo::GetOption(LPCTSTR szFlag) const
{
	CString sOption;
	GetOption(szFlag, sOption);

	return sOption;
}

BOOL CEnCommandLineInfo::HasOption(LPCTSTR szFlag) const
{
	CString sOption;
	
	return GetOption(szFlag, sOption);
}

void CEnCommandLineInfo::DeleteOption(LPCTSTR szFlag)
{
	m_mapCommandLine.RemoveKey(szFlag);
}

CString CEnCommandLineInfo::ResolveShortcut(LPCTSTR szShortcut)
{
	// start by checking its a valid file
	if (::GetFileAttributes(szShortcut) == 0xffffffff)
		return "";

    CoInitialize(NULL);

	HRESULT hResult;
	IShellLink*	psl;
	CString sTarget(szShortcut);
	
	hResult = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
								IID_IShellLink, (LPVOID*)&psl);
	
	if (SUCCEEDED(hResult))
	{
		LPPERSISTFILE ppf;
		
		hResult = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf);

		if (SUCCEEDED(hResult))
		{
			WCHAR wsz[MAX_PATH];	// buffer for Unicode string
			
			MultiByteToWideChar(CP_ACP, 0, szShortcut, -1, wsz, MAX_PATH);
			hResult = ppf->Load(wsz, STGM_READ);
			
			if (SUCCEEDED(hResult))
			{
				hResult = psl->Resolve(NULL, SLR_ANY_MATCH | SLR_NO_UI);

				if (SUCCEEDED(hResult))
				{
					TCHAR szPath[MAX_PATH];
					WIN32_FIND_DATA wfd;
					
//fabio_2005
#if _MSC_VER >= 1400
					strcpy_s(szPath, szShortcut);
#else
					strcpy(szPath, szShortcut);
#endif
					hResult = psl->GetPath(szPath, MAX_PATH, (WIN32_FIND_DATA*)&wfd, SLGP_SHORTPATH);

					if (SUCCEEDED(hResult))
						sTarget = CString(szPath);
				}
			}
		
			ppf->Release();
		}
		
		psl->Release();
	}

	CoUninitialize();
	
	return sTarget;
}
