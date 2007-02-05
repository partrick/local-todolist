// DateHelper.h: interface for the CDateHelper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DATEHELPER_H__2A4E63F6_A106_4295_BCBA_06D03CD67AE7__INCLUDED_)
#define AFX_DATEHELPER_H__2A4E63F6_A106_4295_BCBA_06D03CD67AE7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum DH_DATE
{
	DHD_TODAY,
	DHD_TOMORROW,
	DHD_ENDTHISWEEK, // start of week + 7
	DHD_ENDNEXTWEEK, // DHD_ENDTHISWEEK + 7
	DHD_ENDTHISMONTH, // beginning of next month - 1
	DHD_ENDNEXTMONTH, // get's trickier :)
	DHD_ENDTHISYEAR,
	DHD_ENDNEXTYEAR,
};

enum DH_UNITS
{
	DHU_DAYS = 'D',
	DHU_WEEKS = 'W',
	DHU_MONTHS = 'M',
	DHU_YEARS = 'Y',
};

class CDateHelper  
{
public:
	static int CalcDaysFromTo(const COleDateTime& dateFrom, const COleDateTime& dateTo, BOOL bInclusive);
	static int CalcDaysFromTo(const COleDateTime& dateFrom, DH_DATE nTo, BOOL bInclusive);
	static int CalcDaysFromTo(DH_DATE nFrom, DH_DATE nTo, BOOL bInclusive);
	static double GetDate(DH_DATE nDate); // 12am

	static int CalcWeekdaysFromTo(const COleDateTime& dateFrom, const COleDateTime& dateTo, BOOL bInclusive);
	static void OffsetDate(COleDateTime& date, int nAmount, DH_UNITS nUnits);

	// DOW = 'day of week'
	static CString FormatDate(const COleDateTime& date, BOOL bISOFormat = FALSE, BOOL bWantDOW = FALSE);
	static CString FormatCurrentDate(BOOL bISOFormat = FALSE, BOOL bWantDOW = FALSE);

	static int FirstDayOfWeek();
	static CString GetWeekday(int nWeekday, BOOL bShort); // 1-7, sun-sat
	static CString GetMonth(int nMonth, BOOL bShort); // 1-12, jan-nov
	static void GetWeekdays(BOOL bShort, CStringArray& aWeekDays); // sun-sat
	static void GetMonths(BOOL bShort, CStringArray& aMonths); // jan-nov
};

#endif // !defined(AFX_DATEHELPER_H__2A4E63F6_A106_4295_BCBA_06D03CD67AE7__INCLUDED_)
