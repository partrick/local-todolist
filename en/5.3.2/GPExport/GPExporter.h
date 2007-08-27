// GPExporter.h: interface for the CGPExporter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GPEXPORTER_H__F588E6B1_3646_4994_99A2_4223FDDA1A31__INCLUDED_)
#define AFX_GPEXPORTER_H__F588E6B1_3646_4994_99A2_4223FDDA1A31__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\SHARED\IImportExport.h"
#include "..\SHARED\ITasklist.h"

class CXmlItem;

class CGPExporter : public IExportTasklist  
{
public:
	CGPExporter();
	virtual ~CGPExporter();

	// interface implementation
	void Release() { delete this; }

	// caller must copy only
	const char* GetMenuText() { return "GanttProject"; }
	const char* GetFileFilter() { return "GanttProject Files (*.gan)|*.gan||"; }
	const char* GetFileExtension() { return "gan"; }

	bool Export(const ITaskList* pSrcTaskFile, const char* szDestFilePath);

protected:
	void ExportTask(const ITaskList* pSrcTaskFile, HTASKITEM hTask, CXmlItem* pXIDestParent);
};

#endif // !defined(AFX_GPEXPORTER_H__F588E6B1_3646_4994_99A2_4223FDDA1A31__INCLUDED_)
