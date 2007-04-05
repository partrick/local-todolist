// FileRegister.cpp: implementation of the CFileRegister class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FileRegister.h"
#include "regkey.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileRegister::CFileRegister(LPCTSTR szExt, LPCTSTR szFileType)
{
	m_sExt = szExt;
	m_sFileType = szFileType;

	m_sExt.TrimRight();
	m_sExt.TrimLeft();
	m_sFileType.TrimRight();
	m_sFileType.TrimLeft();

	if (!m_sExt.IsEmpty() && m_sExt[0] != '.')
		m_sExt = "." + m_sExt;

	// get the app path
	GetModuleFileName(NULL, m_sAppPath.GetBuffer(_MAX_PATH), _MAX_PATH);
	m_sAppPath.ReleaseBuffer();

	ASSERT (!m_sAppPath.IsEmpty());
}

CFileRegister::~CFileRegister()
{
}

BOOL CFileRegister::RegisterFileType(LPCTSTR szFileDesc, int nIcon, BOOL bAllowShellOpen, LPCTSTR szParams, BOOL bAskUser)
{
	CRegKey reg;
	CString sKey;
	CString sEntry;
	int nRet = IDYES;
	CString sMessage;
	BOOL bSuccess = TRUE, bChange = FALSE;

	ASSERT (!m_sExt.IsEmpty());
	ASSERT (!m_sFileType.IsEmpty());

	if (m_sExt.IsEmpty() || m_sFileType.IsEmpty())
		return FALSE;

	if (reg.Open(HKEY_CLASSES_ROOT, m_sExt) == ERROR_SUCCESS)
	{
		reg.Read("", sEntry);

		// if the current filetype is not correct query the use to reset it
		if (!sEntry.IsEmpty() && sEntry.CompareNoCase(m_sFileType) != 0)
		{
			if (bAskUser)
			{
				sMessage.Format("The file extension %s is used by %s for its %s.\n\nWould you like %s to be the default application for this file type.",
								m_sExt, AfxGetAppName(), szFileDesc, AfxGetAppName());
				
				nRet = AfxMessageBox(sMessage, MB_YESNO | MB_ICONQUESTION);
			}

			bChange = TRUE;
		}
		else
			bChange = sEntry.IsEmpty();

		// if not no then set
		if (nRet != IDNO)
		{
			bSuccess = (reg.Write("", m_sFileType) == ERROR_SUCCESS);

			reg.Close();

			if (reg.Open(HKEY_CLASSES_ROOT, m_sFileType) == ERROR_SUCCESS)
			{
				bSuccess &= (reg.Write("", szFileDesc) == ERROR_SUCCESS);
				reg.Close();

				// app to open file
				if (reg.Open(HKEY_CLASSES_ROOT, m_sFileType + "\\shell\\open\\command") == ERROR_SUCCESS)
				{
					CString sShellOpen;

					if (bAllowShellOpen)
					{
						if (szParams)
							sShellOpen = "\"" + m_sAppPath + "\" \"%1\" " + CString(szParams);
						else
							sShellOpen = "\"" + m_sAppPath + "\" \"%1\"";
					}

					bSuccess &= (reg.Write("", sShellOpen) == ERROR_SUCCESS);
				}
				else
					bSuccess = FALSE;

				// icons
				reg.Close();

				if (reg.Open(HKEY_CLASSES_ROOT, m_sFileType + "\\DefaultIcon") == ERROR_SUCCESS)
				{
					CString sIconPath;
					sIconPath.Format("%s,%d", m_sAppPath, nIcon);
					bSuccess &= (reg.Write("", sIconPath) == ERROR_SUCCESS);
				}
				else
					bSuccess = FALSE;
			}
			else
				bSuccess = FALSE; 
		}
		else
			bSuccess = FALSE;
	}
	else
		bSuccess = FALSE;

	if (bSuccess && bChange)
		SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, 0, 0);

	return bSuccess;
}

BOOL CFileRegister::UnRegisterFileType()
{
	CRegKey reg;
	CString sKey;
	CString sEntry;
	BOOL bSuccess = FALSE;

	ASSERT (!m_sExt.IsEmpty());

	if (m_sExt.IsEmpty())
		return FALSE;

	if (reg.Open(HKEY_CLASSES_ROOT, m_sExt) == ERROR_SUCCESS)
	{
		reg.Read("", sEntry);

		if (sEntry.IsEmpty())
			return TRUE; // we werent the register app so all's well

		ASSERT (!m_sFileType.IsEmpty());

		if (m_sFileType.IsEmpty())
			return FALSE;

		if (sEntry.CompareNoCase(m_sFileType) != 0)
			return TRUE; // we werent the register app so all's well

		// else delete the keys
		reg.Close();
		CRegKey::Delete(HKEY_CLASSES_ROOT, m_sExt);
		CRegKey::Delete(HKEY_CLASSES_ROOT, m_sFileType);
		SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, 0, 0);

		bSuccess = TRUE;
	}

	return bSuccess;
}

BOOL CFileRegister::IsRegisteredAppInstalled()
{
	ASSERT (!m_sExt.IsEmpty());

	if (m_sExt.IsEmpty())
		return FALSE;

	CString sRegAppPath = GetRegisteredAppPath();

	if (!sRegAppPath.IsEmpty())
	{
		CFileStatus fs;

		return CFile::GetStatus(sRegAppPath, fs);
	}

	return FALSE;
}

BOOL CFileRegister::IsRegisteredApp()
{
	CRegKey reg;
	CString sEntry;

	ASSERT (!m_sExt.IsEmpty());

	if (m_sExt.IsEmpty())
		return FALSE;

	// if the file type is not empty we check the file type first
	if (!m_sFileType.IsEmpty())
	{
		if (reg.Open(HKEY_CLASSES_ROOT, m_sExt) == ERROR_SUCCESS)
		{
			reg.Read("", sEntry);

			if (sEntry.IsEmpty() || sEntry.CompareNoCase(m_sFileType) != 0)
				return FALSE;
		}
	}

	// then we check the app itself
	CString sRegAppFileName = GetRegisteredAppPath(TRUE);

	if (!sRegAppFileName.IsEmpty())
	{
		char szFName[_MAX_FNAME], szExt[_MAX_EXT];
		_splitpath(m_sAppPath, NULL, NULL, szFName, szExt);
		strcat(szFName, szExt);

		if (sRegAppFileName.CompareNoCase(szFName) == 0)
			return TRUE;
	}

	return FALSE;
}

CString CFileRegister::GetRegisteredAppPath(BOOL bFilenameOnly)
{
	CRegKey reg;
	CString sEntry, sAppPath;

	ASSERT (!m_sExt.IsEmpty());

	if (reg.Open(HKEY_CLASSES_ROOT, m_sExt) == ERROR_SUCCESS)
	{
		reg.Read("", sEntry);

		if (!sEntry.IsEmpty())
		{
			reg.Close();

			if (reg.Open(HKEY_CLASSES_ROOT, sEntry) == ERROR_SUCCESS)
			{
				reg.Close();

				// app to open file
				if (reg.Open(HKEY_CLASSES_ROOT, sEntry + CString("\\shell\\open\\command")) == ERROR_SUCCESS)
					reg.Read("", sAppPath);
			}
		}
	}

	// note: apps often have parameters after so we do this
	char szDrive[_MAX_DRIVE], szDir[_MAX_DIR], szFName[_MAX_FNAME];
	_splitpath(sAppPath, szDrive, szDir, szFName, NULL);
	lstrcat(szFName, ".exe");
	
	if (bFilenameOnly)
		sAppPath = szFName;
	else
	{
		// else rebuild path
		_makepath(sAppPath.GetBuffer(MAX_PATH), szDrive, szDir, szFName, NULL);
		sAppPath.ReleaseBuffer();
	}
	
	return sAppPath;
}

// static versions
BOOL CFileRegister::IsRegisteredAppInstalled(LPCTSTR szExt)
{
	return CFileRegister(szExt).IsRegisteredAppInstalled();
}

CString CFileRegister::GetRegisteredAppPath(LPCTSTR szExt, BOOL bFilenameOnly)
{
	return CFileRegister(szExt).GetRegisteredAppPath(bFilenameOnly);
}

BOOL CFileRegister::RegisterFileType(LPCTSTR szExt, LPCTSTR szFileType, LPCTSTR szFileDesc, int nIcon, BOOL bAllowShellOpen, LPCTSTR szParams, BOOL bAskUser)
{
	return CFileRegister(szExt, szFileType).RegisterFileType(szFileDesc, nIcon, bAllowShellOpen, szParams, bAskUser);
}

BOOL CFileRegister::UnRegisterFileType(LPCTSTR szExt, LPCTSTR szFileType)
{
	return CFileRegister(szExt, szFileType).UnRegisterFileType();
}

BOOL CFileRegister::IsRegisteredApp(LPCTSTR szExt, LPCTSTR szFileType)
{
	return CFileRegister(szExt, szFileType).IsRegisteredApp();
}
