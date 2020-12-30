// InfoAdd.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"

#include "InfoAdd.h"
#include "afxdialogex.h"
#include "string_oper.h"

#include "client_info_db.h"
#include <string>
using namespace std;


extern CDatabase opr_db;

// CInfoAdd dialog

IMPLEMENT_DYNAMIC(CInfoAdd, CDialog)


CInfoAdd::CInfoAdd(CListCtrl &in_list, CWnd* pParent)
    : CDialog(CInfoAdd::IDD, pParent),
    m_list(in_list)
{
    ModOpr = FALSE;
}

CInfoAdd::CInfoAdd(CListCtrl &in_list, int count, CWnd* pParent)  /*mod*/
    : CDialog(CInfoAdd::IDD, pParent),
    m_list(in_list)
{
    ModOpr   = TRUE;
    ModIndex = count;
}

CInfoAdd::~CInfoAdd()
{
}

void CInfoAdd::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO1, send_status);
//    DDX_Control(pDX, IDC_COMBO3, birth_month);
//    DDX_Control(pDX, IDC_COMBO2, aga_or_no);
    DDX_Control(pDX, IDC_DATETIMEPICKER1, birth_day);
    DDX_Control(pDX, IDC_EDIT1, id);
    DDX_Control(pDX, IDC_EDIT2, alarm_name);
//    DDX_Control(pDX, IDC_DATETIMEPICKER2, ContactTime);
//    DDX_Control(pDX, IDC_COMBO4, m_client_type);
}


BEGIN_MESSAGE_MAP(CInfoAdd, CDialog)
    ON_BN_CLICKED(IDOK, &CInfoAdd::OnBnClickedOk)
END_MESSAGE_MAP()


// CInfoAdd message handlers


void CInfoAdd::combo_init(void)
{
    	//出单渠道
//	aga_or_no.ResetContent();
//	aga_or_no.AddString(_T("是"));
//	aga_or_no.AddString(_T("否"));

	//发送状态
	send_status.ResetContent();
	send_status.AddString(_T("已发送"));
	send_status.AddString(_T("未发送"));

//    m_client_type.ResetContent();
//    m_client_type.AddString(_T("公司客户"));
//	m_client_type.AddString(_T("个人客户"));

	//保险计划
/*    birth_month.ResetContent();
    for(int i = 1; i <= 12; i++)
    {
        CString month;
        month.Format(_T("%d"), i);
	    birth_month.AddString(month);
    }	*/
}


BOOL CInfoAdd::OnInitDialog()
{
    CDialog::OnInitDialog();

    combo_init();

//    SetDefaultDateTime(ContactTime);

    if(ModOpr)
    {
        LoadFromCtrlList();
    }
    else
    {
        ModIndex = m_list.GetItemCount();
    }

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}


void CInfoAdd::OnBnClickedOk()
{
    // TODO: Add your control notification handler code here
    int     count = 0;
    CWnd   *p_wnd = NULL;
    CString instr;
    int     nItem = ModIndex;

    if(id.GetWindowTextLength() <= 0)
    {
        MessageBox(_T("客户标识不能空"));
        return;
    }

    if(alarm_name.GetWindowTextLength() <= 0)
    {
        MessageBox(_T("报警内容不能空"));
        return;
    }

    if(ModOpr) /*修改的时候如果改了索引，就是插入操作了*/
    {
        id.GetWindowText(instr);

        if(instr != IndexStr)
        {
            ModOpr = FALSE;
            nItem  = m_list.GetItemCount();
        }
    }    

    while(count < DLG_TOTAL_COUNT)
    {
        p_wnd =  CWnd::GetNextDlgTabItem(p_wnd, FALSE);

        if(count == DLG_BIRTH_DAY_INDEX)
        {
            if(isDateTimeDefault(birth_day))
            {
                instr = "";
            }
            else
            {
                instr = DateTimeToString(birth_day);
            }
        }
/*        else if(count == DLG_CONTACT_DATE_INDEX)
        {
            if(isDateTimeDefault(ContactTime))
            {
                instr = "";
            }
            else
            {
                instr = DateTimeToString(ContactTime);
            }
        }	*/
        else
        {
            p_wnd->GetWindowText(instr);
        }   

        if(count == 0)
        {
            if(!ModOpr)
            {
                m_list.InsertItem(nItem, instr.GetString());
            }
        }
        else
        {
			m_list.SetItemText(nItem, count, instr.GetString());
        }

        count ++;
    }

    Cclient_info_db record(&opr_db);

    if(!ModOpr)
    {
        record.AddDbOpr(m_list, nItem);
    }
    else
    {
        record.ModDbOpr(m_list, nItem);
    }

    CDialog::OnOK();
}

void CInfoAdd::LoadFromCtrlList(void)
{
    CString ModStr;
    CWnd   *p_wnd = NULL;
    int     nItem = ModIndex;

    for(int index = 0; index < DLG_TOTAL_COUNT; index++)
    {
        p_wnd  = CWnd::GetNextDlgTabItem(p_wnd, FALSE);
        ModStr = m_list.GetItemText(nItem, index);

        if(ModStr.GetLength())
        {
            if(index == DLG_BIRTH_DAY_INDEX)
            {
                if(ModStr.GetLength())
                {
                    StringToDateTime(ModStr, birth_day);
                }
                else
                {
                    SetDefaultDateTime(birth_day);
                }
            }
/*            else if(index == DLG_CONTACT_DATE_INDEX)
            {
                if(ModStr.GetLength())
                {
                    StringToDateTime(ModStr, ContactTime);
                }
                else
                {
                    SetDefaultDateTime(ContactTime);
                }
            }	*/
            else
            {
                p_wnd->SetWindowText(ModStr);
            }
        }
    }

    id.GetWindowText(IndexStr);
}
