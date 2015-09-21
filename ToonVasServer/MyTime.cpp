#include "StdAfx.h"
#include "MyTime.h"


CMyTime::CMyTime(void)
{
}


CMyTime::~CMyTime(void)
{
}

CString CMyTime::GetCurTime(){
	CString strYear, strMonth, strDay, strHour, strMin, strSec, strSocketTime;

	//½Ã°£
	CTime curTime = CTime::GetCurrentTime();
	strYear.Format(_T("%d"), curTime.GetYear());
	strMonth.Format(_T("%d"), curTime.GetMonth());
	strDay.Format(_T("%d"), curTime.GetDay());
	strHour.Format(_T("%d"), curTime.GetHour());
	strMin.Format(_T("%d"), curTime.GetMinute());
	strSec.Format(_T("%d"), curTime.GetSecond());
	strSocketTime = strYear+"/"+strMonth+"/"+strDay+", "+strHour+"h"+strMin+"m"+strSec+"s";

	return strSocketTime;
}