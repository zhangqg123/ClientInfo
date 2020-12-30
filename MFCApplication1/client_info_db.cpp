// client_info_db.h : Implementation of the Cclient_info_db class



// Cclient_info_db implementation

// code generated on 2012年9月22日, 12:45

#include "stdafx.h"
#include "client_info_db.h"
#include "string_oper.h"

IMPLEMENT_DYNAMIC(Cclient_info_db, CRecordset)

    Cclient_info_db::Cclient_info_db(CDatabase* pdb)
    : CRecordset(pdb)
{
    m_id = L"";
    m_dev_no = L"";
    m_dev_name = L"";
    m_alarm_name = L"";
//    m_client_pingyin = L"";
    m_alarm_no = L"";
    m_send_status = L"";
    m_send_date;
/*    m_client_contact_addr = L"";
    m_client_phone_num = L"";
    m_client_birth_month = L"";
    m_client_work_corp = L"";
    m_client_supervisor = L"";
    m_client_sales_num = L"";
    m_client_aga_or = L"";
    m_client_aga_channel = L"";
    m_client_type = L"";
    m_client_last_contact_time;
    m_client_contact_list = L"";
    m_client_index = 0;
	*/
    m_nFields = 18;
    m_nDefaultType = dynaset;
}

CString Cclient_info_db::GetDefaultConnect()
{
    return _T("DSN=test32;SERVER=localhost;UID=root;PWD=zqg_zht;DATABASE=jeecg-boot-214;PORT=3306");
}

CString Cclient_info_db::GetDefaultSQL()
{
    return _T("[jst_zc_sms]");
}

void Cclient_info_db::DoFieldExchange(CFieldExchange* pFX)
{
    pFX->SetFieldType(CFieldExchange::outputColumn);
    // Macros such as RFX_Text() and RFX_Int() are dependent on the
    // type of the member variable, not the type of the field in the database.
    // ODBC will try to automatically convert the column value to the requested type
    RFX_Text(pFX, _T("[id]"), m_id);
    RFX_Text(pFX, _T("[dev_no]"), m_dev_no);
    RFX_Text(pFX, _T("[dev_name]"), m_dev_name);
    RFX_Text(pFX, _T("[alarm_name]"), m_alarm_name);
//    RFX_Text(pFX, _T("[client_pingyin]"), m_client_pingyin);
    RFX_Text(pFX, _T("[alarm_no]"), m_alarm_no);
    RFX_Text(pFX, _T("[send_status]"), m_send_status);
    RFX_Date(pFX, _T("[send_date]"), m_send_date);
/*    RFX_Text(pFX, _T("[client_contact_addr]"), m_client_contact_addr);
    RFX_Text(pFX, _T("[client_phone_num]"), m_client_phone_num);
    RFX_Text(pFX, _T("[client_birth_month]"), m_client_birth_month);
    RFX_Text(pFX, _T("[client_work_corp]"), m_client_work_corp);
    RFX_Text(pFX, _T("[client_supervisor]"), m_client_supervisor);
    RFX_Text(pFX, _T("[client_sales_num]"), m_client_sales_num);
    RFX_Text(pFX, _T("[client_aga_or]"), m_client_aga_or);
    RFX_Text(pFX, _T("[client_aga_channel]"), m_client_aga_channel);
    RFX_Text(pFX, _T("[client_type]"), m_client_type);
    RFX_Date(pFX, _T("[client_last_contact_time]"), m_client_last_contact_time);
    RFX_Text(pFX, _T("[client_contact_list]"), m_client_contact_list);
    RFX_Long(pFX, _T("[client_index]"), m_client_index);
*/
}
/////////////////////////////////////////////////////////////////////////////
// Cclient_info_db diagnostics

#ifdef _DEBUG
void Cclient_info_db::AssertValid() const
{
    CRecordset::AssertValid();
}

void Cclient_info_db::Dump(CDumpContext& dc) const
{
    CRecordset::Dump(dc);
}
#endif //_DEBUG

BOOL Cclient_info_db::LoadDbOpr(CListCtrl &in_list, int index)
{
    CTime      Time;
    CListCtrl &m_list  = in_list;    
    CString    TempStr = m_list.GetItemText(index, 0);

    if(TempStr.GetLength() <= 0)
    {
        MessageBox(NULL, _T("添加数据库的索引为空"), NULL, MB_OK);
        return FALSE;
    }

    TempStr = m_list.GetItemText(index, DLG_BIRTH_DAY_INDEX);

    if(TempStr.GetLength())
    {
        if(!StringToDateTime(TempStr, Time))
        {
            MessageBox(NULL, _T("转换发送时间失败"), NULL, MB_OK);
            return FALSE;
        }
    }
/*
    TempStr = m_list.GetItemText(index, DLG_CONTACT_DATE_INDEX);

    if(TempStr.GetLength())
    {
        if(!StringToDateTime(TempStr, Time))
        {
            MessageBox(NULL, _T("转换客户沟通时间失败"), NULL, MB_OK);
            return FALSE;
        }
    }
*/
    m_id       = m_list.GetItemText(index, 0);
    m_dev_no       = m_list.GetItemText(index, 1);
    m_dev_name       = m_list.GetItemText(index, 2);
    m_alarm_name       = m_list.GetItemText(index, 3);
//    m_client_pingyin    = m_list.GetItemText(index, 2);
    m_alarm_no   = m_list.GetItemText(index, 4);
    m_send_status       = m_list.GetItemText(index, 5);

    TempStr = m_list.GetItemText(index, 6);
    if(TempStr.GetLength())
    {
        StringToDateTime(TempStr, m_send_date);
    }
    else
    {
        SetDefaultDateTime(m_send_date);
    }

/*    m_client_contact_addr   = m_list.GetItemText(index, 5);
    m_client_phone_num      = m_list.GetItemText(index, 6);
    m_client_birth_month    = m_list.GetItemText(index, 7);
    m_client_work_corp      = m_list.GetItemText(index, 8);
    m_client_supervisor     = m_list.GetItemText(index, 9);
    m_client_sales_num      = m_list.GetItemText(index, 10);
    m_client_aga_or         = m_list.GetItemText(index, 11);
    m_client_aga_channel    = m_list.GetItemText(index, 12);
    m_client_type           = m_list.GetItemText(index, 13);

    TempStr = m_list.GetItemText(index, 14);

    if(TempStr.GetLength())
    {
        StringToDateTime(TempStr, m_client_last_contact_time);
    }
    else
    {
        SetDefaultDateTime(m_client_last_contact_time);
    }

    m_client_contact_list   = m_list.GetItemText(index, 15);
*/
    return TRUE;
}

int Cclient_info_db::AddDbOpr(CListCtrl &in_list, int index)
{
    CListCtrl &m_list = in_list;
    CString    id = m_list.GetItemText(index, 0);

    if(id.GetLength() <= 0)
    {
        AfxMessageBox(_T("添加数据库的索引为空"));
    }    

    try  
    {  
        Open(CRecordset::snapshot, _T("jst_zc_sms"));  

        AddNew();  

        if(!LoadDbOpr(in_list, index))
        {
            AfxMessageBox(_T("从list导入失败"));
            return -1;
        }

        if(!Update())  
        {  
            AfxMessageBox(_T("Add New failed!"));  
        }  

        this->Close();
    }  
    catch(CDBException* pe)  
    {  
        // The error code is in pe->m_nRetCode  
        pe->ReportError();  
        pe->Delete();  
    }  

    return 0;
}

int Cclient_info_db::ModDbOpr(CListCtrl &in_list, int index)
{
    CListCtrl &m_list       = in_list;
    CString    id  = m_list.GetItemText(index, 0);

    if(id.GetLength() <= 0)
    {
        AfxMessageBox(_T("修改数据库的索引为空"));
    }

    try  
    {  
        this->m_strFilter = _T("id = '");
        this->m_strFilter += id;
        this->m_strFilter += "'";

        BOOL rc = Open(CRecordset::snapshot, _T("jst_zc_sms"));  

        if(IsEOF())
        {
            AfxMessageBox(_T("修改数据库时的记录为空"));
            return -1;
        }

        Edit(); 

        if(!LoadDbOpr(in_list, index))
        {
            AfxMessageBox(_T("从list导入失败"));
            return -1;
        } 

        if(!Update())  
        {  
            AfxMessageBox(_T("Modify failed!"));  
        }  

        this->Close();
    }  
    catch(CDBException* pe)  
    {  
        // The error code is in pe->m_nRetCode  
        pe->ReportError();  
        pe->Delete();  
    }  
    return 0;
}

int Cclient_info_db::DelDbOpr(CListCtrl &in_list, int index)
{
    CListCtrl &m_list = in_list;

    CString    id = m_list.GetItemText(index, 0);

    if(id.GetLength() <= 0)
    {
        MessageBox(NULL, _T("添加数据库的list行不对"), NULL, MB_OK);
    }

    try  
    {  
        this->m_strFilter = _T("id = '");
        this->m_strFilter += id;
        this->m_strFilter += "'";

        Open(CRecordset::snapshot, _T("jst_zc_sms")); 

        if(IsEOF())
        {
            AfxMessageBox(_T("删除数据库时的记录为空"));
            return -1;
        }

        Delete();  

        if(!this->IsDeleted())
        {
            AfxMessageBox(_T("删除失败!"));
        }

        this->Close();
    }  
    catch(CDBException* pe)  
    {  
        // The error code is in pe->m_nRetCode  
        pe->ReportError();  
        pe->Delete();  
    }  
    return 0;
}
