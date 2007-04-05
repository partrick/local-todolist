// EnCommandLineInfo.h: interface for the CEnCommandLineInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENCOMMANDLINEINFO_H__71F4DB4E_3B2B_4F6A_B80F_0BB288E68335__INCLUDED_)
#define AFX_ENCOMMANDLINEINFO_H__71F4DB4E_3B2B_4F6A_B80F_0BB288E68335__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CEnCommandLineInfo : public CCommandLineInfo  
{
public:
	CEnCommandLineInfo();
	virtual ~CEnCommandLineInfo();

	BOOL GetOption(LPCTSTR szFlag, CStringArray* pParams = NULL) const; 
	BOOL GetOption(LPCTSTR szFlag, CString& sParam) const; 

	static CString ResolveShortcut(LPCTSTR szShortcut);

protected:
	CMapStringToString m_mapCommandLine;
	CString m_sLastOption;
	int m_nLastParameter;

protected:
	virtual void ParseParam(LPCTSTR lpszParam, BOOL bFlag, BOOL bLast);


};

#endif // !defined(AFX_ENCOMMANDLINEINFO_H__71F4DB4E_3B2B_4F6A_B80F_0BB288E68335__INCLUDED_)
