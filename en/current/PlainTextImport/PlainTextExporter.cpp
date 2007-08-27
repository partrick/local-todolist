// PlainTextExporter.cpp: implementation of the CPlainTextExporter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PlainTextImport.h"
#include "PlainTextExporter.h"
#include "optionsdlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const LPCTSTR ENDL = "\n";

CPlainTextExporter::CPlainTextExporter()
{

}

CPlainTextExporter::~CPlainTextExporter()
{

}

bool CPlainTextExporter::Export(const ITaskList* pSrcTaskFile, const char* szDestFilePath)
{
	COptionsDlg dialog(FALSE);

	if (dialog.DoModal() != IDOK)
		return false;
	
	INDENT = dialog.GetIndent();

	CStdioFile fileOut;

	if (!fileOut.Open(szDestFilePath, CFile::modeCreate | CFile::modeWrite))
		return false;

	// else
	int nDepth = 0;
	const ITaskList4* pITL4 = static_cast<const ITaskList4*>(pSrcTaskFile);
	
	// export report title as dummy task
	if (dialog.GetWantProject())
	{
		CString sTitle = pITL4->GetReportTitle();
		
		if (sTitle.IsEmpty())
			sTitle = pITL4->GetProjectName();
		
//		if (!sTitle.IsEmpty())
//			nDepth++;
	
		// note: we export the title even if it's empty
		// to maintain consistency with the importer that the first line
		// is always the outline name
		sTitle += ENDL;
		fileOut.WriteString(sTitle);
	}
	
	// export first task
	ExportTask(pSrcTaskFile, pSrcTaskFile->GetFirstTask(), fileOut, nDepth);
	
	return true;
}

void CPlainTextExporter::ExportTask(const ITaskList* pSrcTaskFile, HTASKITEM hTask, 
									CStdioFile& fileOut, int nDepth)
{
	if (!hTask)
		return;

	// export each task as '[indent]title|comments' on a single line
	CString sTask;

	// indent
	for (int nTab = 0; nTab < nDepth; nTab++)
		sTask += INDENT;

	// title
	sTask += pSrcTaskFile->GetTaskTitle(hTask);

	// comments
	CString sComments = pSrcTaskFile->GetTaskComments(hTask);

	if (!sComments.IsEmpty())
	{
		// remove all carriage returns
		sComments.Replace("\r\n", "");
		sComments.Replace("\n", "");

		sTask += '|';
		sTask += sComments;
	}

	// add carriage return
	sTask += ENDL;

	// save write to file
	fileOut.WriteString(sTask);

	// copy across first child
	ExportTask(pSrcTaskFile, pSrcTaskFile->GetFirstTask(hTask), fileOut, nDepth + 1);

	// copy across first sibling
	ExportTask(pSrcTaskFile, pSrcTaskFile->GetNextTask(hTask), fileOut, nDepth);
}

