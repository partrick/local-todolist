// PreferencesToolPage.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "PreferencesToolPage.h"

#include "..\shared\enstring.h"
#include "..\shared\misc.h"
#include "..\shared\filedialogex.h"

#include "..\3rdparty\ini.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPreferencesToolPage property page

const char* REALQUOTE = "\"";
const char* SAFEQUOTE = "{QUOTES}";

IMPLEMENT_DYNCREATE(CPreferencesToolPage, CPropertyPage)

CPreferencesToolPage::CPreferencesToolPage() : CPropertyPage(CPreferencesToolPage::IDD),
												m_eToolPath(FES_COMBOSTYLEBTN | FES_ALLOWURL),
												m_eIconPath(FES_COMBOSTYLEBTN)
{
	//{{AFX_DATA_INIT(CPreferencesToolPage)
	m_sToolPath = _T("");
	m_sCommandLine = _T("");
	m_bRunMinimized = FALSE;
	m_sIconPath = _T("");
	//}}AFX_DATA_INIT

	// load tools
	int nToolCount = AfxGetApp()->GetProfileInt("Tools", "ToolCount", 0);

	for (int nTool = 1; nTool <= nToolCount; nTool++)
	{
		CString sKey;
		sKey.Format("Tools\\Tool%d", nTool);

		USERTOOL ut;
		ut.sToolName = AfxGetApp()->GetProfileString(sKey, "Name", "");
		ut.sToolPath = AfxGetApp()->GetProfileString(sKey, "Path", "");
		ut.sCmdline = AfxGetApp()->GetProfileString(sKey, "CmdLine", "0xffffffff"); // deliberately odd default to test for existence
		ut.bRunMinimized = AfxGetApp()->GetProfileInt(sKey, "RunMinimized", FALSE);

		ut.sIconPath = AfxGetApp()->GetProfileString(sKey, "IconPath", "");
		
		if (ut.sCmdline == "0xffffffff")
		{
			// backward compatibility
			BOOL nIncludeCmdlinePath = AfxGetApp()->GetProfileInt(sKey, "IncludeCmdlinePath", -1);

			if (nIncludeCmdlinePath) // 1 or -1
				ut.sCmdline = CmdIDToPlaceholder(ID_TOOLS_PATHNAME);
			else
				ut.sCmdline.Empty();
		}

		// replace safe quotes with real quotes
		ut.sCmdline.Replace(SAFEQUOTE, REALQUOTE);

		m_aTools.Add(ut);
	}

	m_eCmdLine.AddButton(1, "", CEnString(IDS_PTP_PLACEHOLDERS));
	m_eCmdLine.SetDropMenuButton(1);
}

CPreferencesToolPage::~CPreferencesToolPage()
{
}

void CPreferencesToolPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPreferencesToolPage)
	DDX_Control(pDX, IDC_CMDLINE, m_eCmdLine);
	DDX_Control(pDX, IDC_TOOLPATH, m_eToolPath);
	DDX_Control(pDX, IDC_TOOLLIST, m_lcTools);
	DDX_Text(pDX, IDC_TOOLPATH, m_sToolPath);
	DDX_Text(pDX, IDC_CMDLINE, m_sCommandLine);
	DDX_Check(pDX, IDC_RUNMINIMIZED, m_bRunMinimized);
	DDX_Text(pDX, IDC_ICONPATH, m_sIconPath);
	DDX_Control(pDX, IDC_ICONPATH, m_eIconPath);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPreferencesToolPage, CPropertyPage)
	//{{AFX_MSG_MAP(CPreferencesToolPage)
	ON_BN_CLICKED(IDC_NEWTOOL, OnNewtool)
	ON_BN_CLICKED(IDC_DELETETOOL, OnDeletetool)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_TOOLLIST, OnEndlabeleditToollist)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_TOOLLIST, OnItemchangedToollist)
	ON_EN_CHANGE(IDC_TOOLPATH, OnChangeToolpath)
	ON_NOTIFY(LVN_KEYDOWN, IDC_TOOLLIST, OnKeydownToollist)
	ON_EN_CHANGE(IDC_CMDLINE, OnChangeCmdline)
	ON_COMMAND_RANGE(ID_TOOLS_PATHNAME, ID_TOOLS_SELTASKCOMMENTS, OnInsertPlaceholder)
	ON_BN_CLICKED(IDC_RUNMINIMIZED, OnRunminimized)
	ON_BN_CLICKED(IDC_TESTTOOL, OnTesttool)
	ON_EN_CHANGE(IDC_ICONPATH, OnChangeIconPath)
	ON_BN_CLICKED(IDC_IMPORT, OnImport)
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(WM_EE_BTNCLICK, OnEEBtnClick)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreferencesToolPage message handlers

BOOL CPreferencesToolPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	m_ilSys.Initialize();
	m_lcTools.SetImageList(m_ilSys.GetImageList(), LVSIL_SMALL);

	CRect rList;
	m_lcTools.GetClientRect(rList);

	m_lcTools.InsertColumn(0, CEnString(IDS_PTP_TOOLNAME), LVCFMT_LEFT, 150);
	m_lcTools.InsertColumn(1, CEnString(IDS_PTP_TOOLPATH), LVCFMT_LEFT, 250);
	m_lcTools.InsertColumn(2, CEnString(IDS_PTP_ARGUMENTS), LVCFMT_LEFT, rList.Width() - 400);
	m_lcTools.InsertColumn(3, CEnString(IDS_PTP_ICONPATH), LVCFMT_LEFT, 0);

	m_lcTools.SetExtendedStyle(m_lcTools.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
	
	// add tools we loaded from the registry
	for (int nTool = 0; nTool < m_aTools.GetSize(); nTool++)
	{
		int nImage = m_ilSys.GetFileImageIndex(m_aTools[nTool].sToolPath);
		
		if (!m_aTools[nTool].sIconPath.IsEmpty()) 
			nImage = m_ilSys.GetFileImageIndex(m_aTools[nTool].sIconPath);
			
		int nIndex = m_lcTools.InsertItem(nTool, m_aTools[nTool].sToolName, nImage);

		m_lcTools.SetItemText(nIndex, 1, m_aTools[nTool].sToolPath);
		m_lcTools.SetItemText(nIndex, 2, m_aTools[nTool].sCmdline);
		m_lcTools.SetItemText(nIndex, 3, m_aTools[nTool].sIconPath);
		m_lcTools.SetItemData(nIndex, m_aTools[nTool].bRunMinimized);
	}

	m_lcTools.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
	OnItemchangedToollist(NULL, NULL);

	EnableControls();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPreferencesToolPage::OnNewtool() 
{
	int nIndex = m_lcTools.InsertItem(m_lcTools.GetItemCount(), CEnString(IDS_PTP_NEWTOOL), -1);
	m_lcTools.SetItemText(nIndex, 2, CmdIDToPlaceholder(ID_TOOLS_PATHNAME));

	m_lcTools.SetItemState(nIndex, LVIS_SELECTED, LVIS_SELECTED);
	m_lcTools.SetFocus();
	m_lcTools.EditLabel(nIndex);
}

void CPreferencesToolPage::OnDeletetool() 
{
	int nSel = GetCurSel();

	if (nSel >= 0)
	{
		m_lcTools.DeleteItem(nSel);
		m_sToolPath.Empty();
		m_sIconPath.Empty();

		EnableControls();
		UpdateData(FALSE);
	}
}

void CPreferencesToolPage::OnEndlabeleditToollist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	
	if (pDispInfo->item.pszText)
	{
		int nSel = GetCurSel();

		if (nSel >= 0)
		{
			m_lcTools.SetItemText(nSel, 0, pDispInfo->item.pszText);

			GetDlgItem(IDC_TOOLPATH)->SetFocus();
		}
	}
	
	*pResult = 0;
}

void CPreferencesToolPage::OnItemchangedToollist(NMHDR* /*pNMHDR*/, LRESULT* /*pResult*/) 
{
//	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	EnableControls();
	int nSel = GetCurSel();

	if (nSel >= 0)
	{
		m_sToolPath = m_lcTools.GetItemText(nSel, 1);
		m_sCommandLine = m_lcTools.GetItemText(nSel, 2);
		m_sIconPath = m_lcTools.GetItemText(nSel, 3);
		m_bRunMinimized = m_lcTools.GetItemData(nSel);
	}
	else
	{
		m_sToolPath.Empty();
		m_sCommandLine.Empty();
		m_bRunMinimized = FALSE;
		m_sIconPath.Empty();
	}

	UpdateData(FALSE);

//	*pResult = 0;
}

void CPreferencesToolPage::EnableControls()
{
	int nSel = GetCurSel();

	GetDlgItem(IDC_NEWTOOL)->EnableWindow(m_lcTools.GetItemCount() < 16);
	GetDlgItem(IDC_DELETETOOL)->EnableWindow(nSel >= 0);
	GetDlgItem(IDC_TOOLPATH)->EnableWindow(nSel >= 0);
	GetDlgItem(IDC_ICONPATH)->EnableWindow(nSel >= 0);
	GetDlgItem(IDC_CMDLINE)->EnableWindow(nSel >= 0);
	GetDlgItem(IDC_RUNMINIMIZED)->EnableWindow(nSel >= 0);

	m_eCmdLine.EnableButton(1, nSel >= 0);
}

int CPreferencesToolPage::GetCurSel()
{
	int nSel = -1;
	POSITION pos = m_lcTools.GetFirstSelectedItemPosition();

	if (pos)
		nSel = m_lcTools.GetNextSelectedItem(pos);

	return nSel;
}

void CPreferencesToolPage::OnChangeToolpath() 
{
	int nSel = GetCurSel();

	if (nSel >= 0)
	{
		UpdateData();

		m_lcTools.SetItemText(nSel, 1, m_sToolPath);

		// update the image
		LVITEM lvi;
		lvi.mask = LVIF_IMAGE;
		lvi.iItem = nSel;
		if (m_sIconPath.IsEmpty())
			lvi.iImage = m_ilSys.GetFileImageIndex(m_sToolPath);	
		else 
			lvi.iImage = m_ilSys.GetFileImageIndex(m_sIconPath);

		m_lcTools.SetItem(&lvi);
	}
	else
		ASSERT (0);
}

void CPreferencesToolPage::OnChangeIconPath() 
{
	int nSel = GetCurSel();

	if (nSel >= 0)
	{
		UpdateData();

		m_lcTools.SetItemText(nSel, 3, m_sIconPath);

		// update the image
		LVITEM lvi;
		lvi.mask = LVIF_IMAGE;
		lvi.iItem = nSel;

		if (m_sIconPath.IsEmpty())
			lvi.iImage = m_ilSys.GetFileImageIndex(m_sToolPath);	
		else 
			lvi.iImage = m_ilSys.GetFileImageIndex(m_sIconPath);

		m_lcTools.SetItem(&lvi);
	}
	else
		ASSERT (0);
}

int CPreferencesToolPage::GetUserTools(CUserToolArray& aTools) const
{
	aTools.Copy(m_aTools);

	return aTools.GetSize();
}

BOOL CPreferencesToolPage::GetUserTool(int nTool, USERTOOL& tool) const
{
	if (nTool >= 0 && nTool < m_aTools.GetSize())
	{
		tool = m_aTools[nTool];
		return TRUE;
	}

	return FALSE;
}

void CPreferencesToolPage::OnOK() 
{
	CPropertyPage::OnOK();
	
	// save tools to registry and m_aTools
	int nToolCount = m_lcTools.GetItemCount();

	for (int nTool = 0; nTool < nToolCount; nTool++)
	{
		USERTOOL ut;

		ut.sToolName = m_lcTools.GetItemText(nTool, 0);
		ut.sToolPath = m_lcTools.GetItemText(nTool, 1);
		ut.sCmdline = m_lcTools.GetItemText(nTool, 2);
		ut.sIconPath = m_lcTools.GetItemText(nTool, 3);
		ut.bRunMinimized = m_lcTools.GetItemData(nTool);

		CString sKey;
		sKey.Format("Tools\\Tool%d", nTool + 1);
		
		AfxGetApp()->WriteProfileString(sKey, "Name", ut.sToolName);
		AfxGetApp()->WriteProfileString(sKey, "Path", ut.sToolPath);
		AfxGetApp()->WriteProfileString(sKey, "IconPath", ut.sIconPath);
		AfxGetApp()->WriteProfileInt(sKey, "RunMinimized", ut.bRunMinimized);
		
		// GetPrivateProfileString strips a leading/trailing quote pairs if 
		// it finds them so we replace quotes with safe quotes
		ut.sCmdline.Replace(REALQUOTE, SAFEQUOTE);
		AfxGetApp()->WriteProfileString(sKey, "Cmdline", ut.sCmdline);
		
		m_aTools.Add(ut);
	}

	AfxGetApp()->WriteProfileInt("Tools", "ToolCount", nToolCount);
}

void CPreferencesToolPage::OnKeydownToollist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDown = (LV_KEYDOWN*)pNMHDR;
	
	switch (pLVKeyDown->wVKey)
	{
	case VK_DELETE:
		OnDeletetool();
		break;

	case VK_F2:
		{
			int nSel = GetCurSel();

			if (nSel >= 0)
				m_lcTools.EditLabel(nSel);
		}
		break;
	}
	
	*pResult = 0;
}

void CPreferencesToolPage::OnChangeCmdline() 
{
	int nSel = GetCurSel();

	if (nSel >= 0)
	{
		UpdateData();

		m_lcTools.SetItemText(nSel, 2, m_sCommandLine);
	}
}

void CPreferencesToolPage::OnInsertPlaceholder(UINT nCmdID) 
{
	m_eCmdLine.ReplaceSel(CmdIDToPlaceholder(nCmdID), TRUE);
}

void CPreferencesToolPage::OnRunminimized() 
{
	int nSel = GetCurSel();

	if (nSel >= 0)
	{
		UpdateData();

		m_lcTools.SetItemData(nSel, m_bRunMinimized);
	}
}

LRESULT CPreferencesToolPage::OnEEBtnClick(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case IDC_CMDLINE:
		if (lParam == 1)
		{
			CMenu menu, *pSubMenu;
			
			if (menu.LoadMenu(IDR_PLACEHOLDERS))
			{
				pSubMenu = menu.GetSubMenu(0); 
				
				if (pSubMenu)
				{
					CRect rButton = m_eCmdLine.GetButtonRect((UINT)1);

					TPMPARAMS tpmp;
					tpmp.cbSize = sizeof(TPMPARAMS);
					tpmp.rcExclude = rButton;
					
					::TrackPopupMenuEx(*pSubMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON, 
										rButton.right, rButton.top, *this, &tpmp);
				}
			}
		}
	}

	return 0L;
}

void CPreferencesToolPage::OnTesttool() 
{
   int nTool = GetCurSel();

   if (nTool != -1)
   {
      USERTOOL ut;
      
      ut.sToolName = m_lcTools.GetItemText(nTool, 0);
	   ut.sToolPath = m_lcTools.GetItemText(nTool, 1);
	   ut.sCmdline = m_lcTools.GetItemText(nTool, 2);
	   ut.bRunMinimized = m_lcTools.GetItemData(nTool);

	   AfxGetMainWnd()->SendMessage(WM_PTP_TESTTOOL, 0, (LPARAM)&ut);
   }
}

CString CPreferencesToolPage::CmdIDToPlaceholder(UINT nCmdID) 
{
	switch (nCmdID)
	{
	case ID_TOOLS_PATHNAME: return "\"$(pathname)\"";       
	case ID_TOOLS_FILETITLE: return "$(filetitle)";      
	case ID_TOOLS_FOLDER: return "$(folder)";         
	case ID_TOOLS_FILENAME: return "$(filename)";       
	case ID_TOOLS_SELTASKID: return "$(selTID)";       
	case ID_TOOLS_SELTASKTITLE: return "$(selTTitle)";       
	case ID_TOOLS_USERDATE: return "$(userdate, var_date1, \"Date Prompt\", default_date)";       
	case ID_TOOLS_USERFILEPATH: return "\"$(userfile, var_file1, \"File Prompt\", default_path)\"";   
	case ID_TOOLS_USERFOLDER: return "$(userfolder, var_folder1, \"Folder Prompt\", default_folder)";     
	case ID_TOOLS_USERTEXT: return "$(usertext, var_text1, \"Text Prompt\", default_text)";       
	case ID_TOOLS_TODAYSDATE: return "$(todaysdate)";     
	case ID_TOOLS_TODOLIST: return "\"$(todolist)\""; 
	case ID_TOOLS_SELTASKEXTID: return "\"$(selTExtID)\""; 
	case ID_TOOLS_SELTASKCOMMENTS: return "\"$(selTComments)\""; 
	}

	return "";
}

void CPreferencesToolPage::OnImport() 
{
	BOOL bContinue = TRUE;

	while (bContinue)
	{
		CFileDialogEx dialog(TRUE, "ini", NULL, 
							OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, 
							CEnString(IDS_INIFILEFILTER));
		
		if (dialog.DoModal() == IDOK)
		{
			CIni ini(dialog.GetPathName());

			int nTools = ini.GetInt("Tools", "ToolCount", 0);

			if (!nTools)
				bContinue = (AfxMessageBox(IDS_INIHASNOTOOLS, MB_YESNO) == IDYES);
			else
			{
				int nCurCount = m_lcTools.GetItemCount();

				for (int nTool = 0; nTool < nTools; nTool++)
				{
					CString sKey;
					sKey.Format("Tools\\Tool%d", nTool + 1);
					
					CString sName = ini.GetString(sKey, "Name");
					CString sPath = ini.GetString(sKey, "Path");
					CString sIconPath = ini.GetString(sKey, "IconPath");
					BOOL bRunMinimized = ini.GetBool(sKey, "RunMinimized", FALSE);
					CString sCmdline = ini.GetString(sKey, "Cmdline");

					// replace safe quotes with real quotes
					sCmdline.Replace(SAFEQUOTE, REALQUOTE);

					// add tool to list
					int nImage = m_ilSys.GetFileImageIndex(sPath);
					
					if (!sIconPath.IsEmpty()) 
						nImage = m_ilSys.GetFileImageIndex(sIconPath);
						
					int nIndex = m_lcTools.InsertItem(nCurCount + nTool, sName, nImage);

					m_lcTools.SetItemText(nIndex, 1, sPath);
					m_lcTools.SetItemText(nIndex, 2, sCmdline);
					m_lcTools.SetItemText(nIndex, 3, sIconPath);
					m_lcTools.SetItemData(nIndex, bRunMinimized);
				}

				bContinue = FALSE;
			}
		}
		else
			bContinue = FALSE; // cancelled
	}
}
