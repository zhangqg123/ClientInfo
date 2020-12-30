
#include "stdafx.h"
#include "string_oper.h"

CString DateTimeToString(CDateTimeCtrl &in_data_time)
{
	CString str;
	CTime   temp_time;
	if( GDT_VALID != in_data_time.GetTime(temp_time))
	{
        MessageBox(NULL, _T("时间转换失败"), NULL, MB_OK );

		return str;
	}
	str = temp_time.Format(_T("%Y-%m-%d"));

	return str;
}
void ConvertUTF8ToANSI(CString strUTF8,CString &strANSI)
{
	int nLen = ::MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,(LPCTSTR)strUTF8,-1,NULL,0); 
	//返回需要的unicode长度   
	WCHAR * wszANSI = new WCHAR[nLen+1];   
	memset(wszANSI, 0, nLen * 2 + 2);   
	nLen = MultiByteToWideChar(CP_UTF8, 0, (LPCTSTR)strUTF8, -1, wszANSI, nLen);    //把utf8转成unicode  
	nLen = WideCharToMultiByte(CP_ACP, 0, wszANSI, -1, NULL, 0, NULL, NULL);        //得到要的ansi长度   
	char *szANSI=new char[nLen + 1];   
	memset(szANSI, 0, nLen + 1);   
	WideCharToMultiByte (CP_ACP, 0, wszANSI, -1, szANSI, nLen, NULL,NULL);          //把unicode转成ansi   
	strANSI = szANSI;   
	delete wszANSI;   
	delete szANSI;   
}
BOOL StringToDateTime(CString in_str, CDateTimeCtrl &in_date)
{
	int nYear;
	int nMonth;
	int nDay;	

	if(3 != sscanf_s(in_str.GetString(), _T("%d-%d-%d"), &nYear, &nMonth, &nDay))
	{
		MessageBox(NULL, _T("时间转换失败"), NULL, MB_OK );
	}

	CTime t_date_time(nYear, nMonth, nDay, 0, 0, 0);

	if(!in_date.SetTime(&t_date_time))
	{
		MessageBox(NULL, _T("时间转换失败"), NULL, MB_OK );

		return false;
	}
	return true;
}

BOOL StringToDateTime(CString in_str, CTime &in_date)
{
	int nYear;
	int nMonth;
	int nDay;	

	if(3 != sscanf_s(in_str.GetString(), _T("%d-%d-%d"), &nYear, &nMonth, &nDay))
	{
        AfxMessageBox(_T("时间转换失败"));
        return FALSE;
	}

	CTime t_date_time(nYear, nMonth, nDay, 0, 0, 0);

    in_date = t_date_time;

    return TRUE;
}

bool is_string_integer(CString &str)
{
	const char *c_str = str.GetString();

	if(!str.GetLength())
	{
		return false;
	}

	for(int i = 0; i < str.GetLength(); i++)
	{
		if(c_str[i] <= '9' && c_str[i] >= '0')
		{
			continue;
		}
		else
		{
			return false;
		}
	}

	return true;
}


bool is_string_decimal(CString &str)
{
	const char *c_str = str.GetString();

	if(!str.GetLength())
	{
		return false;
	}

	if(c_str[0] == '.')
	{
		return false;
	}

	for(int i = 0; i < str.GetLength(); i++)
	{
		if( (c_str[i] <= '9' && c_str[i] >= '0') || c_str[i] == '.')
		{
			continue;
		}
		else
		{
			return false;
		}
	}

	return true;
}

UINT32 get_string_integer(CString &str)
{
	UINT32 number = 0;
	const char *c_str = str.GetString();
	
	sscanf_s(c_str, _T("%lu"), &number);

	return number;
}

BOOL SetDefaultDateTime(CDateTimeCtrl &in_date)
{
	CTime t_date_time(2000, 1, 1, 0, 0, 0);

	if(!in_date.SetTime(&t_date_time))
	{
		MessageBox(NULL, _T("时间转换失败"), NULL, MB_OK );
		return false;
	}
	return true;
}

BOOL SetDefaultDateTime(CTime &in_date)
{
	CTime t_date_time(2000, 1, 1, 0, 0, 0);

	in_date = t_date_time;
	return true;
}

bool    isDateTimeDefault(CTime &temp_time)
{
	if( temp_time.GetYear()  == 2000 && 
		temp_time.GetMonth() == 1    &&
		temp_time.GetDay()   == 1)
	{
		return true;
	}

	return false;
}

bool    isDateTimeDefault(CDateTimeCtrl &in_date)
{
	CTime temp_time;
	in_date.GetTime(temp_time);

	if( temp_time.GetYear()  == 2000 && 
		temp_time.GetMonth() == 1    &&
		temp_time.GetDay()   == 1)
	{
		return true;
	}

	return false;
}