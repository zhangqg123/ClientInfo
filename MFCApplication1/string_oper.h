#ifndef STRING_OPER_H_
#define STRING_OPER_H_

#include "stdafx.h"
#include <string>
using namespace std;

CString DateTimeToString(CDateTimeCtrl &in_data_time);
void ConvertUTF8ToANSI(CString strUTF8,CString &strANSI);
BOOL    StringToDateTime(CString in_str, CDateTimeCtrl &in_date);
BOOL    StringToDateTime(CString in_str, CTime &in_date);

bool    is_string_integer(CString &str);
bool	is_string_decimal(CString &str);
UINT32  get_string_integer(CString &str);

BOOL    SetDefaultDateTime(CDateTimeCtrl &in_date);
bool    isDateTimeDefault(CDateTimeCtrl &in_date);
BOOL    SetDefaultDateTime(CTime &in_date);
bool    isDateTimeDefault(CTime &temp_time);

enum    {
        DLG_BIRTH_DAY_INDEX = 6,
//        DLG_CONTACT_DATE_INDEX = 14,
        DLG_TOTAL_COUNT = 7,
    };

#endif