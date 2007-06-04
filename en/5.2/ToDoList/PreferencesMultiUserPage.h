#if !defined(AFX_PREFERENCESMULTIUSERPAGE_H__D7484AFF_704B_4FA9_94DA_9AB2F5364816__INCLUDED_)
#define AFX_PREFERENCESMULTIUSERPAGE_H__D7484AFF_704B_4FA9_94DA_9AB2F5364816__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PreferencesMultiUserPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPreferencesMultiUserPage dialog

enum // source control
{
	STSS_NONE,
	STSS_LANONLY,
	STSS_ALL,
};

enum // reload
{
	RO_NO,
	RO_ASK,
	RO_AUTO,
};

class CPreferencesMultiUserPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CPreferencesMultiUserPage)

// Construction
public:
	CPreferencesMultiUserPage();
	~CPreferencesMultiUserPage();

	int GetReadonlyReloadOption() const;
	int GetTimestampReloadOption() const;
	BOOL GetEnableSourceControl() const { return m_bEnableSourceControl; }
	BOOL GetSourceControlLanOnly() const { return m_bEnableSourceControl && m_bSourceControlLanOnly; }
	BOOL GetAutoCheckOut() const { return m_bEnableSourceControl && m_bAutoCheckOut; }
	BOOL GetCheckoutOnCheckin() const { return m_bEnableSourceControl && m_bCheckoutOnCheckin; }
	BOOL GetCheckinOnClose() const { return m_bEnableSourceControl && m_bCheckinOnClose; }
	UINT GetRemoteFileCheckFrequency() const { return m_nRemoteFileCheckFreq; }
	UINT GetCheckinOnNoEditTime() const { return (m_bEnableSourceControl && m_bCheckinNoChange) ? m_nCheckinNoEditTime : 0; }
	BOOL GetFormatXmlOutput() const { return m_bFormatXmlOutput; }

protected:
// Dialog Data
	//{{AFX_DATA(CPreferencesMultiUserPage)
	enum { IDD = IDD_PREFMULTIUSER_PAGE };
	CComboBox	m_cbNoEditTime;
	BOOL	m_bCheckinNoChange;
	BOOL	m_bFormatXmlOutput;
	//}}AFX_DATA
	CComboBox	m_cbRemoteFileCheck;
	BOOL	m_bEnableSourceControl;
	BOOL	m_bSourceControlLanOnly;
	BOOL	m_bAutoCheckOut;
	BOOL	m_bCheckoutOnCheckin;
	int		m_nReadonlyReloadOption;
	int		m_nTimestampReloadOption;
	BOOL	m_bCheckinOnClose;
	UINT	m_nRemoteFileCheckFreq;
	BOOL	m_bPromptReloadOnWritable;
	BOOL	m_bPromptReloadOnTimestamp;
	UINT    m_nCheckinNoEditTime;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPreferencesMultiUserPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPreferencesMultiUserPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckinonnoedit();
	//}}AFX_MSG
	afx_msg void OnEnablesourcecontrol();
	afx_msg void OnPromptreloadonwritable();
	afx_msg void OnPromptreloadontimestamp();
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREFERENCESMULTIUSERPAGE_H__D7484AFF_704B_4FA9_94DA_9AB2F5364816__INCLUDED_)
