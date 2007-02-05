// XmlFile.cpp: implementation of the CXmlFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XmlFile.h"
#include "misc.h"

#include "..\3rdparty\xmlnodewrapper.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////

#include "xmlcharmap.h"
#include "htmlcharmap.h"

CString& XML2TXT(CString& xml) { return CHtmlCharMap::ConvertFromRep(xml); } // we use the html map for backwards compatibility
CString& TXT2XML(CString& txt) { return CXmlCharMap::ConvertToRep(txt); }
CString& TXT2HTML(CString& txt) { return CHtmlCharMap::ConvertToRep(txt); }

BOOL CXmlFile::s_bFormatOutput = FALSE;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_FIXED_ALLOC(CXmlItem, 1024);

CXmlItem::CXmlItem(CXmlItem* pParent, LPCTSTR szName, LPCTSTR szValue, BOOL bCDATA) :
m_pParent(pParent), m_pSibling(NULL), m_sName(szName), m_sValue(szValue), m_bCData(bCDATA)
{
	ValidateString(m_sValue);
}

CXmlItem::CXmlItem(const CXmlItem& xi, CXmlItem* pParent) : 
m_pParent(pParent), m_pSibling(NULL), m_sName(xi.m_sName), m_sValue(xi.m_sValue), m_bCData(xi.m_bCData)
{
	Copy(xi, TRUE);
}

CXmlItem::~CXmlItem()
{
	Reset();
}

void CXmlItem::Copy(const CXmlItem& xi, BOOL bCopySiblings)
{
	Reset();

	// copy own name and value
	m_sName = xi.GetName();
	m_sValue = xi.GetValue();

	// copy siblings
	if (bCopySiblings)
	{
		const CXmlItem* pXISibling = xi.GetSibling();
		
		if (pXISibling)
			m_pSibling = new CXmlItem(*pXISibling, m_pParent);
	}
	
	// copy children
	POSITION pos = xi.GetFirstItemPos();
	
	while (pos)
	{
		const CXmlItem* pXIChild = xi.GetNextItem(pos);
		ASSERT (pXIChild);
		
		AddItem(*pXIChild, TRUE);
	}
}

void CXmlItem::Reset()
{
	// delete children
	POSITION pos = m_mapItems.GetStartPosition();
	
	while (pos)
	{
		CXmlItem* pXI = NULL;
		CString sItem;
		
		m_mapItems.GetNextAssoc(pos, sItem, pXI);
		delete pXI;
	}
	
	m_mapItems.RemoveAll();
	
	// and siblings
	// note: because sibling ~tor calls its own Reset() the chain 
	// of siblings will be correctly cleaned up
	delete m_pSibling;
	m_pSibling = NULL;
   m_bCData = FALSE;
}

const CXmlItem* CXmlItem::GetItem(LPCTSTR szItemName, LPCTSTR szSubItem) const
{
	return GetItemEx(szItemName, szSubItem);
}

CXmlItem* CXmlItem::GetItem(LPCTSTR szItemName, LPCTSTR szSubItem)
{
	// cast away constness
	return (CXmlItem*)GetItemEx(szItemName, szSubItem);
}

// special internal version
const CXmlItem* CXmlItem::GetItemEx(LPCTSTR szItemName, LPCTSTR szSubItem) const
{
	CXmlItem* pXI = NULL;
	
	m_mapItems.Lookup(szItemName, pXI);
	
	if (pXI && szSubItem && *szSubItem)
		return pXI->GetItem(szSubItem);
	
	return pXI;
}

const CXmlItem* CXmlItem::FindItem(LPCTSTR szItemName, LPCTSTR szItemValue, BOOL bSearchChildren) const
{
	return FindItemEx(szItemName, szItemValue, bSearchChildren);
}

CXmlItem* CXmlItem::FindItem(LPCTSTR szItemName, LPCTSTR szItemValue, BOOL bSearchChildren)
{
	// cast away constness
	return (CXmlItem*)FindItemEx(szItemName, szItemValue, bSearchChildren);
}

const CXmlItem* CXmlItem::FindItem(LPCTSTR szItemName, int nItemValue, BOOL bSearchChildren) const
{
	return FindItemEx(szItemName, ToString(nItemValue), bSearchChildren);
}

const CXmlItem* CXmlItem::FindItem(LPCTSTR szItemName, double dItemValue, BOOL bSearchChildren) const
{
	return FindItemEx(szItemName, ToString(dItemValue), bSearchChildren);
}

CXmlItem* CXmlItem::FindItem(LPCTSTR szItemName, int nItemValue, BOOL bSearchChildren)
{
	// cast away constness
	return (CXmlItem*)FindItemEx(szItemName, ToString(nItemValue), bSearchChildren);
}

CXmlItem* CXmlItem::FindItem(LPCTSTR szItemName, double dItemValue, BOOL bSearchChildren)
{
	// cast away constness
	return (CXmlItem*)FindItemEx(szItemName, ToString(dItemValue), bSearchChildren);
}

// special internal version
const CXmlItem* CXmlItem::FindItemEx(LPCTSTR szItemName, LPCTSTR szItemValue, BOOL bSearchChildren) const
{
	// check our name and value
	if (m_sName.CompareNoCase(szItemName) == 0 && m_sValue.Compare(szItemValue) == 0)
		return this;
	
	const CXmlItem* pFound = NULL;
	
	// search all our children
	if (bSearchChildren)
	{
		POSITION pos = GetFirstItemPos();
		
		while (pos && !pFound)
		{
			const CXmlItem* pXIChild = GetNextItem(pos);
			ASSERT (pXIChild);
			
			pFound = pXIChild->FindItemEx(szItemName, szItemValue, TRUE); // child will search its own siblings
		}
	}
	
	// then our siblings
	if (!pFound)
	{
		// only get first sibling as each sibling does its own
		const CXmlItem* pXISibling = GetSibling();
		
		if (pXISibling)
			pFound = pXISibling->FindItemEx(szItemName, szItemValue, TRUE);
	}
	
	return pFound;
}

LPCTSTR CXmlItem::GetItemValue(LPCTSTR szItemName, LPCTSTR szSubItem) const
{
	const CXmlItem* pXI = GetItem(szItemName, szSubItem);
	
	if (pXI)
		return pXI->GetValue();
	else
	{
		static CString NULL_VALUE;
		return NULL_VALUE;
	}
}

int CXmlItem::GetItemCount(LPCTSTR szItemName) const
{
	int nCount = 0;
	const CXmlItem* pXI = GetItem(szItemName);

	while (pXI)
	{
		nCount++;
		pXI = pXI->GetSibling();
	}

	return nCount;
}

CXmlItem* CXmlItem::AddItem(LPCTSTR szName, LPCTSTR szValue, BOOL bCDATA)
{
	ASSERT (szName && *szName);
	
	if (!(szName && *szName))
		return NULL;

	CXmlItem* pXI = new CXmlItem(this, szName, szValue, bCDATA);
	
	return AddItem(pXI);
}

CXmlItem* CXmlItem::AddItem(const CXmlItem& xi, BOOL bCopySiblings)
{
	CXmlItem* pXI = new CXmlItem(this);
	pXI->Copy(xi, bCopySiblings);

	return AddItem(pXI);
}

CXmlItem* CXmlItem::AddItem(CXmlItem* pXI)
{
	CXmlItem* pXIParent = pXI->GetParent();
	
	if (pXIParent && pXIParent != this)
		pXIParent->RemoveItem(pXI);
	
	pXI->m_pParent = this;
	
	// if an item of the same name already exists then add this
	// item as a sibling to the existing item else its a new item
	// so add and map name to this object
	CXmlItem* pXPExist = GetItem(pXI->GetName());
	
	if (pXPExist)
		pXPExist->AddSibling(pXI);
	else
		m_mapItems[pXI->GetName()] = pXI;
	
	return pXI;
}

CXmlItem* CXmlItem::AddItem(LPCTSTR szName, int nValue)
{
	return AddItem(szName, ToString(nValue));
}

CXmlItem* CXmlItem::AddItem(LPCTSTR szName, const double& dValue)
{
	return AddItem(szName, ToString(dValue));
}

CXmlItem* CXmlItem::SetItemValue(LPCTSTR szName, LPCTSTR sValue, BOOL bCData)
{
   CXmlItem* pXI = GetItem(szName);

   if (!pXI)
      return AddItem(szName, sValue, bCData);

   // else already exists
   pXI->SetValue(sValue);
   pXI->SetCDATA(bCData);

   return pXI;
}

CXmlItem* CXmlItem::SetItemValue(LPCTSTR szName, int nValue)
{
	return SetItemValue(szName, ToString(nValue));
}

CXmlItem* CXmlItem::SetItemValue(LPCTSTR szName, const double& dValue)
{
	return SetItemValue(szName, ToString(dValue));
}

BOOL CXmlItem::SetName(LPCTSTR szName)
{
	// can't have any siblings
	if (!szName || !(*szName) || GetSibling())
		return FALSE;
	
	m_sName = szName;
	return TRUE;
}

void CXmlItem::SetValue(LPCTSTR szValue)
{
	m_sValue = szValue;
	ValidateString(m_sValue);
}

void CXmlItem::SetValue(int nValue)
{
	m_sValue = ToString(nValue);
}

void CXmlItem::SetValue(const double& dValue)
{
	m_sValue = ToString(dValue);
}

BOOL CXmlItem::RemoveItem(CXmlItem* pXI)
{
	if (!pXI)
		return FALSE;
	
	// lookup by name first
	LPCTSTR szName = pXI->GetName();
	CXmlItem* pXIMatch = GetItem(szName);
	
	if (!pXIMatch)
		return FALSE;
	
	// now search the sibling chain looking for exact match
	CXmlItem* pXIPrevSibling = NULL;
	
	while (pXIMatch != pXI)
	{
		pXIPrevSibling = pXIMatch;
		pXIMatch = pXIMatch->GetSibling();
	}
	
	if (!pXIMatch) // no match
		return FALSE;
	
	// else
	ASSERT (pXIMatch == pXI);
	
	CXmlItem* pNextSibling = pXI->GetSibling();
	
	if (!pXIPrevSibling) // head of the chain
	{
		if (!pNextSibling)
			m_mapItems.RemoveKey(szName);
		else
			m_mapItems[szName] = pNextSibling;
	}
	else // somewhere else in the chain
	{
		pXIPrevSibling->m_pSibling = pNextSibling; // can be NULL
	}
	
	// clear item's sibling
	pXI->m_pSibling = NULL;
	
	// and parent
	pXI->m_pParent = NULL;
	
	return TRUE;
}

BOOL CXmlItem::DeleteItem(CXmlItem* pXI)
{
	if (RemoveItem(pXI))
	{
		delete pXI;
		return TRUE;
	}
	
	return FALSE;
}

BOOL CXmlItem::DeleteItem(LPCTSTR szItemName)
{
	CXmlItem* pXI = GetItem(szItemName);

	if (pXI)
	{
		pXI->Reset(); // delete children and siblings
		DeleteItem(pXI); // delete pXI
	}

	return TRUE;
}

BOOL CXmlItem::AddSibling(CXmlItem* pXI)
{
	ASSERT (pXI);
	
	if (!pXI)
		return FALSE;
	
	// must share the same name and parent
	ASSERT (m_sName.CompareNoCase(pXI->GetName()) == 0 && m_pParent == pXI->GetParent());
	
	if (!(m_sName.CompareNoCase(pXI->GetName()) == 0 && m_pParent == pXI->GetParent()))
		return FALSE;
	
	if (!m_pSibling)
		m_pSibling = pXI;
	else
		m_pSibling->AddSibling(pXI); // recursive
	
	return TRUE;
}

POSITION CXmlItem::GetFirstItemPos() const
{
	return m_mapItems.GetStartPosition();
}

const CXmlItem* CXmlItem::GetNextItem(POSITION& pos) const
{
	if (!pos)
		return NULL;
	
	CString sTemp;
	CXmlItem* pItem = NULL;
	
	m_mapItems.GetNextAssoc(pos, sTemp, pItem);
	return pItem;
}

CXmlItem* CXmlItem::GetNextItem(POSITION& pos)
{
	if (!pos)
		return NULL;
	
	CString sTemp;
	CXmlItem* pItem = NULL;
	
	m_mapItems.GetNextAssoc(pos, sTemp, pItem);
	return pItem;
}

BOOL CXmlItem::NameMatches(const CXmlItem* pXITest) const
{
	if (!pXITest)
		return FALSE;
	
	return NameMatches(pXITest->GetName());
}

BOOL CXmlItem::NameMatches(LPCTSTR szName) const
{
	return (m_sName.CompareNoCase(szName) == 0);
}

BOOL CXmlItem::ValueMatches(const CXmlItem* pXITest, BOOL bIgnoreCase) const
{
	if (!pXITest)
		return FALSE;
	
	// else
	return ValueMatches(pXITest->GetValue(), bIgnoreCase);
}

BOOL CXmlItem::ValueMatches(LPCTSTR szValue, BOOL bIgnoreCase) const
{
	if (bIgnoreCase)
		return (m_sValue.CompareNoCase(szValue) == 0);
	
	// else
	return (m_sValue == szValue);
}

BOOL CXmlItem::ItemValueMatches(const CXmlItem* pXITest, LPCTSTR szItemName, BOOL bIgnoreCase) const
{
	if (!szItemName || !*szItemName)
		return FALSE;
	
	const CXmlItem* pXIItem = GetItem(szItemName);
	const CXmlItem* pXITestItem = pXITest->GetItem(szItemName);
	
	if (pXIItem && pXITestItem)
		return pXIItem->ValueMatches(pXITestItem, bIgnoreCase);
	
	// else
	return FALSE;
}

void CXmlItem::SortItems(LPCTSTR szItemName, LPCTSTR szKeyName, BOOL bAscending)
{
	SortItems(szItemName, szKeyName, XISK_STRING, bAscending);
}

void CXmlItem::SortItemsI(LPCTSTR szItemName, LPCTSTR szKeyName, BOOL bAscending)
{
	SortItems(szItemName, szKeyName, XISK_INT, bAscending);
}

void CXmlItem::SortItemsF(LPCTSTR szItemName, LPCTSTR szKeyName, BOOL bAscending)
{
	SortItems(szItemName, szKeyName, XISK_FLOAT, bAscending);
}

void CXmlItem::SortItems(LPCTSTR szItemName, LPCTSTR szKeyName, XI_SORTKEY nKey, BOOL bAscending)
{
	if (!szItemName || !szKeyName)
		return;
	
	// 1. sort immediate children first
	CXmlItem* pXIItem = GetItem(szItemName);
	
	if (!pXIItem)
		return;
	
	// make sure item has key value
	if (!pXIItem->GetItem(szKeyName))
		return;
	
	// make sure at least one sibling exists
	BOOL bContinue = (pXIItem->GetSibling() != NULL);
	
	while (bContinue)
	{
		CXmlItem* pXIPrev = NULL;
		CXmlItem* pXISibling = NULL;
		
		// get this again because we have to anyway 
		// for subsequent loops
		pXIItem = GetItem(szItemName);
		
		// reset continue flag so that if there are no
		// switches then the sorting is done
		bContinue = FALSE;
		pXISibling = pXIItem->GetSibling();
		
		while (pXISibling)
		{
			int nCompare = CompareItems(pXIItem, pXISibling, szKeyName, nKey);
			
			if (!bAscending)
				nCompare = -nCompare;
			
			if (nCompare > 0)
			{
				// switch items
				if (pXIPrev)
					pXIPrev->m_pSibling = pXISibling;
				
				else // we're at the head of the chain
					m_mapItems[szItemName] = pXISibling;
				
				pXIItem->m_pSibling = pXISibling->m_pSibling;
				pXISibling->m_pSibling = pXIItem;
				pXIPrev = pXISibling;
				
				bContinue = TRUE; // loop once more
			}
			else
			{
				pXIPrev = pXIItem;
				pXIItem = pXISibling;
			}

			pXISibling = pXIItem->GetSibling(); // next
		}
	}
	
	// 2. sort children's children
	pXIItem = GetItem(szItemName);
	
	while (pXIItem)
	{
		pXIItem->SortItems(szItemName, szKeyName, nKey, bAscending);
		pXIItem = pXIItem->GetSibling();
	}
}

int CXmlItem::CompareItems(const CXmlItem* pXIItem1, const CXmlItem* pXIItem2, 
						   LPCTSTR szKeyName, XI_SORTKEY nKey)
{
	LPCTSTR szValue1 = pXIItem1->GetItemValue(szKeyName);
	LPCTSTR szValue2 = pXIItem2->GetItemValue(szKeyName);
	
	double dDiff = 0;
	
	switch (nKey)
	{
	case XISK_STRING:
		dDiff = (double)CString(szValue1).CompareNoCase(szValue2);
		break;
		
	case XISK_INT:
		dDiff = (double)(atoi(szValue1) - atoi(szValue2));
		break;
		
	case XISK_FLOAT:
		dDiff = atof(szValue1) - atof(szValue2);
		break;
	}
	
	return (dDiff < 0) ? -1 : ((dDiff > 0) ? 1 : 0);
}

BOOL CXmlItem::IsCDATA() const 
{ 
	return m_bCData;// || (m_sName == CDATA); 
}

void CXmlItem::ValidateString(CString& sText, char cReplace)
{
	// remove nasties that XML does not like
	int nLen = sText.GetLength();

	for(int nChar = 0; nChar < nLen; nChar++)
	{
		UINT c = sText[nChar];

		// some specific chars we don't like
		switch (c)
		{
		case '�':
			sText.SetAt(nChar, cReplace);
			break;
		}
		
		if ((c < 0x20 && (c == 0x09 || c == 0x0A || c == 0x0D)) || 
			(c >= 0x20 && c < 0x82) || c > 0x9F) // 0x82-0x9F are special windows chars 
			continue; // valid

		// else
		sText.SetAt(nChar, cReplace);
	}
}

BOOL CXmlItem::NameIs(LPCTSTR szName) const 
{ 
	return (m_sName == szName); 
}

LPCTSTR CXmlItem::GetName() const 
{ 
	return m_sName; 
}

LPCTSTR CXmlItem::GetValue() const 
{ 
	return m_sValue; 
}

int CXmlItem::GetNameLen() const 
{ 
	return m_sName.GetLength(); 
}

int CXmlItem::GetValueLen() const 
{ 
	return m_sValue.GetLength(); 
}

int CXmlItem::GetItemCount() const 
{ 
	return m_mapItems.GetCount(); 
}

int CXmlItem::GetValueI() const 
{ 
	return atoi(m_sValue); 
}

int CXmlItem::GetItemValueI(LPCTSTR szItemName, LPCTSTR szSubItemName) const 
{ 
	return atoi(GetItemValue(szItemName, szSubItemName)); 
}

double CXmlItem::GetValueF() const 
{ 
	return Misc::Atof(m_sValue); 
}

double CXmlItem::GetItemValueF(LPCTSTR szItemName, LPCTSTR szSubItemName) const 
{ 
	return Misc::Atof(GetItemValue(szItemName, szSubItemName)); 
}

void CXmlItem::DeleteAllItems() 
{ 
	Reset(); 
}

const CXmlItem* CXmlItem::GetParent() const 
{ 
	return m_pParent; 
}

const CXmlItem* CXmlItem::GetSibling() const 
{ 
	return m_pSibling; 
}

CXmlItem* CXmlItem::GetParent() 
{ 
	return m_pParent; 
}

CXmlItem* CXmlItem::GetSibling() 
{ 
	return m_pSibling; 
}

BOOL CXmlItem::IsAttribute(int nMaxAttribLen) const 
{ 
	return (GetValueLen() <= nMaxAttribLen && !m_mapItems.GetCount() && !IsCDATA()); 
}

CString CXmlItem::ToString(int nValue) 
{ 
	CString sValue; sValue.Format("%d", nValue); return sValue; 
}

CString CXmlItem::ToString(double dValue) 
{ 
	CString sValue; sValue.Format("%.8f", dValue); return sValue; 
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXmlFile::CXmlFile(LPCTSTR szRootItemName) : m_xiRoot(NULL, szRootItemName)
{
}

CXmlFile::~CXmlFile()
{
	
}

BOOL CXmlFile::Load(LPCTSTR szFilePath, LPCTSTR szRootItemName, IXmlParse* pCallback)
{
	if (!szFilePath || !*szFilePath)
		return FALSE;
	
	if (GetFileHandle() != (HANDLE)CFile::hFileNull)
		Close();
	
	if (!szRootItemName && *(m_xiRoot.GetName()))
		szRootItemName = m_xiRoot.GetName();
	
	m_pCallback = pCallback;
	
	CXmlDocumentWrapper doc;
	
	BOOL bRes = doc.Load(szFilePath);
	
	if (bRes && !ParseRootItem(szRootItemName, &doc))
		bRes = FALSE;
	
	return bRes;
}

BOOL CXmlFile::Save(LPCTSTR szFilePath)
{
	if (!szFilePath || !*szFilePath)
		return FALSE;
	
	if (GetFileHandle() != (HANDLE)CFile::hFileNull)
		Close();
	
	if (Open(szFilePath, XF_WRITE))
	{
		BOOL bRes = SaveEx();
		Close();
		
		return bRes;
	}

	// error handling
	m_nFileError = GetLastError();
	
	return FALSE;
}

BOOL CXmlFile::Open(LPCTSTR szFilePath, XF_OPEN nOpenFlag)
{
	if (!szFilePath || !*szFilePath)
		return FALSE;
	
	if (GetFileHandle() != (HANDLE)CFile::hFileNull)
		Close();

	BOOL bRes = FALSE;
	
	switch (nOpenFlag)
	{
	case XF_READ:
		bRes = CFile::Open(szFilePath, CFile::shareDenyNone | CFile::modeRead);
		break;
		
	case XF_WRITE:
		bRes = CFile::Open(szFilePath, CFile::shareExclusive | CFile::modeWrite | CFile::modeCreate);
		break;
		
	case XF_READWRITE:
		bRes = CFile::Open(szFilePath, CFile::shareExclusive | CFile::modeReadWrite | 
			CFile::modeCreate | CFile::modeNoTruncate);
		break;
	}

	if (!bRes)
		m_nFileError = GetLastError();
	
	return bRes;
}

BOOL CXmlFile::SaveEx()
{
	if (GetFileHandle() == (HANDLE)CFile::hFileNull)
		return FALSE;
	
	BOOL bRes = FALSE;
	CString sXml;

	if (Export(sXml))
	{	
		try
		{
			Seek(0, CFile::begin); // move to start
			Write((const void*)(LPCTSTR)sXml, sXml.GetLength());
			::SetEndOfFile(GetFileHandle());

			bRes = TRUE;
		}
		catch (...)
		{
			m_nFileError = GetLastError();
		}
	}
	
	return bRes;
}

BOOL CXmlFile::LoadEx(LPCTSTR szRootItemName, IXmlParse* pCallback)
{
	m_nFileError = XFL_NONE; // reset
	
	if (GetFileHandle() == (HANDLE)CFile::hFileNull)
	{
		m_nFileError = ERROR_INVALID_HANDLE;
		return FALSE;
	}
	
	// concatenate entire file into one long string
	CString sLine;
	char* pFileContents = NULL;
	
	try
	{
		Seek(0, CFile::begin); // move to start
		
		// preallocate string to avoid excessive memory allocations
		// Valik - Cast to int to prevent data loss warning (C4244) under VC 7.1
		int nLength = static_cast<int>(GetLength());
		pFileContents = new char[nLength + 1];
		ZeroMemory(pFileContents, nLength + 1);
		
		const UINT BUFLEN = 1024 * 100;
		char BUFFER[BUFLEN];
		BOOL bContinue = TRUE;
		char* pFilePtr = pFileContents;
		
		while (bContinue)
		{
			UINT nRead = Read(BUFFER, BUFLEN);
			
			if (nRead)
			{
				CopyMemory(pFilePtr, BUFFER, nRead);
				pFilePtr += BUFLEN;
			}
			
			bContinue = (nRead == BUFLEN);
		}
		
		pFileContents[nLength] = 0;
	}
	catch (...)
	{
		m_nFileError = GetLastError();

		// cleanup
		delete [] pFileContents;
		m_pCallback = NULL;

		return FALSE;
	}
	
	if (!szRootItemName && m_xiRoot.GetNameLen())
		szRootItemName = m_xiRoot.GetName();
	
	m_pCallback = pCallback;
	
	LPCTSTR szFile = pFileContents;
	BOOL bRes = FALSE;
	
	try
	{
		CXmlDocumentWrapper doc;

		if (doc.IsValid())
		{
			if (!doc.LoadXML(szFile))
			{
				// try removing any bad chars
				CString sFile(szFile);
				FixInputString(sFile, szRootItemName);

				// then try again
				if (!doc.LoadXML(sFile))
					m_nFileError = XFL_BADMSXML;
			}

			// now read it into CXmlItem structures
			if (m_nFileError == XFL_NONE)
			{
				if (!ParseRootItem(szRootItemName, &doc))
					m_nFileError = XFL_MISSINGROOT;
				else
					bRes = TRUE;
			}
		}
	}
	catch (...)
	{
		m_nFileError = XFL_BADMSXML;
	}
	
	// cleanup
	delete [] pFileContents;
	m_pCallback = NULL;
	
	return bRes;
}

void CXmlFile::FixInputString(CString& sXml, LPCTSTR szRootItem)
{
	CXmlItem::ValidateString(sXml);

	// check for any other obvious problems
	if (szRootItem)
	{
		CString sRoot(szRootItem);
		sRoot = '<' + sRoot;

		int nRoot = sXml.Find(sRoot);
		int nHeader = sXml.Find("<?xml");

		if (nHeader > nRoot)
		{
			// what should we do?
			
/*
			// if there is another instance of szRootItem after the 
			// header tag then try deleting everything before the header
			// tag
			if (sXml.Find(sRoot, nHeader) != -1)
			{
				sXml = sXml.Right(nHeader);
			}
			else // try moving the header to the start
			{
				int nHeaderEnd = sXml.Find('>', nHeader) + 1;
				CString sHeader = sXml.Mid(nHeader, nHeaderEnd - nHeader);

				sXml = sHeader + sXml.Left(nHeader) + sXml.Right(nHeaderEnd);
			}
*/
		}
	}
	
}

BOOL CXmlFile::ParseRootItem(LPCTSTR szRootItemName, CXmlDocumentWrapper* pDoc)
{
	ASSERT (pDoc);
	
	m_xiRoot.Reset();
	
	CString sRootItem(szRootItemName), sItem;
	sRootItem.TrimLeft();
	sRootItem.TrimRight();
	
	// save off the header string
	// Valik - Change IXMLDOMNode* to IXMLDOMNodePtr to prevent an ambiguous symbol error (C2872) in VC 7.1
	MSXML2::IXMLDOMNodePtr pNode = pDoc->AsNode();
	CXmlNodeWrapper node(pNode);
	pNode->Release();
	
	if (0 != node.Name().CompareNoCase(sRootItem))
		return FALSE;
	
	m_sHeader = pDoc->GetHeader();
	
	// parse rest of file
	ParseItem(m_xiRoot, &node);
		
	return TRUE;
}

BOOL CXmlFile::ParseItem(CXmlItem& xi, CXmlNodeWrapper* pNode)
{
	int nNumAttrib = pNode->NumAttributes();
	
	for (int nAttrib = 0; nAttrib < nNumAttrib; nAttrib++)
	{
		CString sName(pNode->GetAttribName(nAttrib));
		CString sVal(pNode->GetAttribVal(nAttrib));
		
		xi.AddItem(sName, sVal);

		if (!ContinueParsing(sName, sVal))
			return TRUE;
	}
	
	int nNumNodes = pNode->NumNodes();
	
	for (int nNode = 0; nNode < nNumNodes; nNode++)
	{
		// Valik - Change IXMLDOMNode* to IXMLDOMNodePtr to prevent an ambiguous symbol error (C2872) in VC 7.1
		MSXML2::IXMLDOMNodePtr pChildNode = pNode->GetNode(nNode);
		CXmlNodeWrapper nodeChild(pChildNode);
		pChildNode->Release();
		
		CString sName(nodeChild.Name());
		CString sVal(nodeChild.GetText());
		
		// Valik - Fully qualify NODE_CDATA_SECTION to prevent an ambiguous symbol error (C2872) in VC 7.1
		BOOL bCData = (nodeChild.NodeTypeVal() == MSXML2::NODE_CDATA_SECTION);
		
		// if sName is empty then sVal relates to pNode
		if (!sName.IsEmpty())
		{
			CXmlItem* pXI = xi.AddItem(sName, sVal, bCData);
			
			if (!ContinueParsing(sName, sVal))
				return TRUE;
			
			ParseItem(*pXI, &nodeChild);
		}
		else
      {
			xi.SetValue(sVal);
			xi.SetCDATA(bCData);
      }
	}
	
	return TRUE;
}

void CXmlFile::Copy(const CXmlFile& file)
{
	m_xiRoot.Reset();
	m_xiRoot.Copy(file.m_xiRoot, TRUE);
}

BOOL CXmlFile::Export(CString& sOutput) const
{
	BOOL bRes = FALSE;
	sOutput.Empty();

	try
	{
		CXmlDocumentWrapper doc(m_sHeader, m_xiRoot.GetName());
		
		if (BuildDOM(&doc))
		{
			sOutput = doc.GetXML(TRUE);
			
			if (s_bFormatOutput && sOutput.GetLength())
				sOutput.Replace("><", ">\r\n<");

			bRes = TRUE;
		}
	}
	catch (...)
	{
		m_nFileError = XFL_BADMSXML;
	}
	
	return bRes;
}

BOOL CXmlFile::BuildDOM(CXmlDocumentWrapper* pDoc) const
{
	if (!pDoc || !pDoc->IsValid())
		return FALSE;
	
	// Valik - Change IXMLDOMNode* to IXMLDOMNodePtr to prevent an ambiguous symbol error (C2872) in VC 7.1
	MSXML2::IXMLDOMNodePtr pNode = pDoc->AsNode();
	CXmlNodeWrapper node(pNode);
	pNode->Release();
	
	BOOL bRes = Export(&m_xiRoot, &node);
	
	return bRes;
}

BOOL CXmlFile::Transform(LPCTSTR szTransformPath, CString& sOutput, const CString& sEncoding) const
{
	CXmlDocumentWrapper doc(m_sHeader, m_xiRoot.GetName());
	
	if (BuildDOM(&doc))
	{
		sOutput = doc.Transform(szTransformPath);

		// encoding
		// note: the Transform function always returns UTF-16
		if (!sEncoding.IsEmpty())
			sOutput.Replace("UTF-16", sEncoding);
	}
	else
		sOutput.Empty();
	
	return TRUE;
}

BOOL CXmlFile::TransformToFile(LPCTSTR szTransformPath, LPCTSTR szOutputPath, const CString& sEncoding) const
{
	CString sOutput;
	
	if (Transform(szTransformPath, sOutput, sEncoding))
	{
		CFile output;
		
		if (output.Open(szOutputPath, CFile::modeCreate | CFile::modeWrite))
		{
			output.Write((void*)(LPCTSTR)sOutput, sOutput.GetLength());
			output.Close();
			
			return TRUE;
		}
	}
	
	return FALSE;
}

BOOL CXmlFile::Export(const CXmlItem* pItem, CXmlNodeWrapper* pNode) const
{
	// own value
	if (pItem->GetValueLen())
		pNode->SetText(pItem->GetValue());
	
	// attributes and items
	POSITION pos = pItem->GetFirstItemPos();
	int nNode = 0;
	
	while (pos)
	{
		const CXmlItem* pXIChild = pItem->GetNextItem(pos);
		ASSERT (pXIChild);
		
		CString sItem = pXIChild->GetName();
		
		if (pXIChild->IsAttribute())
		{
			ASSERT (!pXIChild->GetSibling());
			pNode->SetValue(sItem, pXIChild->GetValue());
		}
		else if (pXIChild->IsCDATA())
		{
			// create a named node to wrap the CDATA
			MSXML2::IXMLDOMNodePtr pChildNode = pNode->InsertNode(nNode++, (LPCTSTR)sItem);
			MSXML2::IXMLDOMCDATASectionPtr pCData = pNode->ParentDocument()->createCDATASection((LPCTSTR)pXIChild->GetValue());
			pChildNode->appendChild(pCData);
		}
		else // node
		{
			while (pXIChild)
			{
				// Valik - Change IXMLDOMNode* to IXMLDOMNodePtr to prevent an ambiguous symbol error (C2872) in VC 7.1
				MSXML2::IXMLDOMNodePtr pChildNode = pNode->InsertNode(nNode++, (LPCTSTR)sItem);
				CXmlNodeWrapper nodeChild(pChildNode);
				ASSERT (nodeChild.IsValid());
				pChildNode->Release();
				
				Export(pXIChild, &nodeChild);
				
				// siblings
				pXIChild = pXIChild->GetSibling();
			}
		}
	}
	
	return TRUE;
}

void CXmlFile::Trace() const 
{ 
#ifdef _DEBUG
	CString sXml;
	Export(sXml);
	
	// because the string might be long, output it in chunks of 255 chars
	int nPos = 0;
	
	while (nPos < sXml.GetLength())
	{
		OutputDebugString(sXml.Mid(nPos, 255));
		nPos += 255;
	}
#endif
}

void CXmlFile::Reset() 
{ 
	m_xiRoot.Reset(); 
}

int CXmlFile::GetItemCount() const 
{ 
	return m_xiRoot.GetItemCount(); 
}

void CXmlFile::Close() 
{ 
	CFile::Close(); 
}

int CXmlFile::GetLastFileError() 
{ 
	return m_nFileError; 
}

void CXmlFile::EnableFormattedOutput(BOOL bEnable) 
{ 
	s_bFormatOutput = bEnable; 
}

const CXmlItem* CXmlFile::Root() const 
{ 
	return &m_xiRoot; 
}

CXmlItem* CXmlFile::Root() 
{ 
	return &m_xiRoot; 
}

const CXmlItem* CXmlFile::GetItem(LPCTSTR szItemName) const 
{ 
	return m_xiRoot.GetItem(szItemName); 
} 

CXmlItem* CXmlFile::GetItem(LPCTSTR szItemName) 
{ 
	return m_xiRoot.GetItem(szItemName); 
}

const CXmlItem* CXmlFile::FindItem(LPCTSTR szItemName, LPCTSTR szItemValue, BOOL bSearchChildren) const
{ 
	return m_xiRoot.FindItem(szItemName, szItemValue, bSearchChildren); 
}

CXmlItem* CXmlFile::FindItem(LPCTSTR szItemName, LPCTSTR szItemValue, BOOL bSearchChildren)
{ 
	return m_xiRoot.FindItem(szItemName, szItemValue, bSearchChildren); 
}

const CXmlItem* CXmlFile::FindItem(LPCTSTR szItemName, int nItemValue, BOOL bSearchChildren) const
{ 
	return m_xiRoot.FindItem(szItemName, nItemValue, bSearchChildren); 
}

CXmlItem* CXmlFile::FindItem(LPCTSTR szItemName, int nItemValue, BOOL bSearchChildren)
{ 
	return m_xiRoot.FindItem(szItemName, nItemValue, bSearchChildren); 
}

const CXmlItem* CXmlFile::FindItem(LPCTSTR szItemName, double dItemValue, BOOL bSearchChildren) const
{ 
	return m_xiRoot.FindItem(szItemName, dItemValue, bSearchChildren); 
}

CXmlItem* CXmlFile::FindItem(LPCTSTR szItemName, double dItemValue, BOOL bSearchChildren)
{ 
	return m_xiRoot.FindItem(szItemName, dItemValue, bSearchChildren); 
}

CXmlItem* CXmlFile::AddItem(LPCTSTR szName, LPCTSTR szValue) 
{ 
	return m_xiRoot.AddItem(szName, szValue); 
}

CXmlItem* CXmlFile::AddItem(LPCTSTR szName, int nValue) 
{ 
	return m_xiRoot.AddItem(szName, nValue); 
}

CXmlItem* CXmlFile::AddItem(LPCTSTR szName, const double& dValue) 
{ 
	return m_xiRoot.AddItem(szName, dValue); 
}

BOOL CXmlFile::DeleteItem(CXmlItem* pXI) 
{ 
	return m_xiRoot.DeleteItem(pXI); 
}

LPCTSTR CXmlFile::GetItemValue(LPCTSTR szItemName, LPCTSTR szSubItemName) const 
{ 
	return m_xiRoot.GetItemValue(szItemName, szSubItemName); 
}

int CXmlFile::GetItemValueI(LPCTSTR szItemName, LPCTSTR szSubItemName) const 
{ 
	return m_xiRoot.GetItemValueI(szItemName, szSubItemName); 
}

double CXmlFile::GetItemValueF(LPCTSTR szItemName, LPCTSTR szSubItemName) const 
{ 
	return m_xiRoot.GetItemValueF(szItemName, szSubItemName); 
}

CString CXmlFile::GetFilePath() const 
{ 
	return CFile::GetFilePath(); 
}

CString CXmlFile::GetFileName() const 
{ 
	CString sFilePath = GetFilePath(); 

	char szFileName[MAX_PATH], szExt[_MAX_EXT];
	
	_splitpath(sFilePath, NULL, NULL, szFileName, szExt);
	strcat(szFileName, szExt);

	return szFileName;
}

const HANDLE CXmlFile::GetFileHandle() const 
{ 
	return (HANDLE)CFile::m_hFile; 
}

LPCTSTR CXmlFile::GetHeader() const 
{ 
	return m_sHeader; 
}

void CXmlFile::SetHeader(LPCTSTR szHeader) 
{ 
	m_sHeader = szHeader; m_sHeader.MakeLower(); 
}

void CXmlFile::SortItems(LPCTSTR szItemName, LPCTSTR szKeyName, BOOL bAscending)
{ 
	m_xiRoot.SortItems(szItemName, szKeyName, bAscending); 
}

void CXmlFile::SortItemsI(LPCTSTR szItemName, LPCTSTR szKeyName, BOOL bAscending)
{ 
	m_xiRoot.SortItemsI(szItemName, szKeyName, bAscending); 
}

void CXmlFile::SortItemsF(LPCTSTR szItemName, LPCTSTR szKeyName, BOOL bAscending)
{ 
	m_xiRoot.SortItemsF(szItemName, szKeyName, bAscending); 
}
	
BOOL CXmlFile::ContinueParsing(LPCTSTR szItem, LPCTSTR szValue) 
{ 
	return (!m_pCallback || m_pCallback->Continue(szItem, szValue)); 
}

