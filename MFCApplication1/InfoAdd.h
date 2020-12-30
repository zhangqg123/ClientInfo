#pragma once
#include "afxwin.h"
#include "afxdtctl.h"


// CInfoAdd dialog

class CInfoAdd : public CDialog
{
	DECLARE_DYNAMIC(CInfoAdd)

public:
    CInfoAdd(CListCtrl &in_list, CWnd* pParent = NULL);   
    CInfoAdd(CListCtrl &in_list, int count, CWnd* pParent = NULL);   /*mod*/

	virtual ~CInfoAdd();

// Dialog Data
	enum { IDD = IDD_DIALOG_ADD };    

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
    CComboBox   send_status;
//    CComboBox   birth_month;
//    CComboBox   aga_or_no;    
    CEdit       id;
    CEdit       alarm_name;
    CListCtrl   &m_list;
 //   CComboBox   m_client_type;
    CDateTimeCtrl birth_day;

    BOOL        ModOpr;
    int         ModIndex;
    CString     IndexStr;  /*check the client sign checked or not, when do a mod*/

public:
    void combo_init(void);
    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedOk();    
private:
    CDateTimeCtrl ContactTime;
public:
    void LoadFromCtrlList(void);
};
