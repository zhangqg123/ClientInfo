// client_info_db.h : Declaration of the Cclient_info_db

#pragma once

// code generated on 2012年9月22日, 12:45

class Cclient_info_db : public CRecordset
{
public:
	Cclient_info_db(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(Cclient_info_db)

// Field/Param Data

// The string types below (if present) reflect the actual data type of the
// database field - CStringA for ANSI datatypes and CStringW for Unicode
// datatypes. This is to prevent the ODBC driver from performing potentially
// unnecessary conversions.  If you wish, you may change these members to
// CString types and the ODBC driver will perform all necessary conversions.
// (Note: You must use an ODBC driver version that is version 3.5 or greater
// to support both Unicode and these conversions).

	CString	m_id;
	CString	m_dev_no;
	CString	m_dev_name;

	CString	m_alarm_name;
//	CString	m_client_pingyin;
	CString	m_alarm_no;
	CString	m_send_status;
	CTime	m_send_date;
/*	CString	m_client_contact_addr;
	CString	m_client_phone_num;
	CString	m_client_birth_month;
	CString	m_client_work_corp;
	CString	m_client_supervisor;
	CString	m_client_sales_num;
	CString	m_client_aga_or;
	CString	m_client_aga_channel;
	CString	m_client_type;
	CTime	m_client_last_contact_time;
	CString	m_client_contact_list;
    long    m_client_index;   //数据库补充一个绝对自增数值，否则无法修改给予中文索引的条目的数据
*/

// Overrides
	// Wizard generated virtual function overrides
	public:
	virtual CString GetDefaultConnect();	// Default connection string

	virtual CString GetDefaultSQL(); 	// default SQL for Recordset
	virtual void DoFieldExchange(CFieldExchange* pFX);	// RFX support

// Implementation
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

    int AddDbOpr(CListCtrl &in_list, int index);
    int ModDbOpr(CListCtrl &in_list, int index);
    int DelDbOpr(CListCtrl &in_list, int index);
private:
    BOOL LoadDbOpr(CListCtrl &in_list, int index);
};


