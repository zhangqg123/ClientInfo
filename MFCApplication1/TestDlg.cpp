// TestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;

#endif

/////////////////////////////////////////////////////////////////////////////
// CTestDlg dialog

CTestDlg::CTestDlg()

{
	m_intPort = 0;
	m_strSMSCon = _T("hello");
	m_strSMSTel = _T("15699582810");
	m_strIndex = _T("");
	hinstDLL=NULL; 
	hinstDLL=LoadLibrary("sms.dll");
	if (hinstDLL==NULL)
	{
		AfxMessageBox("û�ҵ�sms.dll�ļ�!");
	}
}


void CTestDlg::OnConnection() 
{
	CString strText;
	CString strtmp;
	CString tmp[256];
	strtmp="//�Ϻ�Ѹ����Ϣ�������޹�˾,��ַwww.xunsai.com//";
	typedef int(_stdcall *Sms_Connection)(CString CopyRight,int Com_Port,int Com_BaudRate,CString *Mobile_Type);
	Sms_Connection Proc;
	Proc = (Sms_Connection)GetProcAddress(hinstDLL,"Sms_Connection");
	int iValue = Proc(strtmp,3,115200,tmp);
    if (iValue == 1)
	{
	    strText.Format("����è���ӳɹ�!(����è�ͺ�Ϊ:%s)",*tmp);
        AfxMessageBox(strText);
//		OnSendButton();
	} else if (iValue == 0) {
        AfxMessageBox("����è����ʧ��!(���������Ӷ���è)");
	}
}

void CTestDlg::OnDisconnectionButton() 
{
	typedef int(_stdcall *Sms_Disconnection)();
    Sms_Disconnection Proc;
	Proc = (Sms_Disconnection)GetProcAddress(hinstDLL,"Sms_Disconnection");
	int iValue = Proc();
}

void CTestDlg::OnSendButton() 
{
	CString strText;
	typedef int(_stdcall *Sms_Send)(CString Sms_TelNum,CString Sms_Text);
	Sms_Send Proc;
	Proc = (Sms_Send)GetProcAddress(hinstDLL,"Sms_Send");
	int iValue = Proc(m_strSMSTel,m_strSMSCon);
    if (iValue == 1)
	{
        AfxMessageBox(_T("���ͳɹ�!"));
	} else{
        AfxMessageBox(_T("����ʧ��!"));
	}
//	OnDisconnectionButton();
}

void CTestDlg::OnReceiveButton() 
{
	CString strText;
	CString tmp[1024];
	typedef int(_stdcall *Sms_Receive)(CString Sms_Type,CString *Sms_Text);
	Sms_Receive Proc;
	Proc = (Sms_Receive)GetProcAddress(hinstDLL,"Sms_Receive");
	int iValue = Proc("4",tmp);	
	strText.Format("%s",*tmp);
    AfxMessageBox(strText);
}

void CTestDlg::OnDeleteButton() 
{
	CString strText;
	CString tmp[256];
	typedef int(_stdcall *Sms_Delete)(CString Sms_Index);
	Sms_Delete Proc;
	Proc = (Sms_Delete)GetProcAddress(hinstDLL,"Sms_Delete");
	int iValue = Proc(m_strIndex);	
	if (iValue == 1)
	{
        AfxMessageBox("ɾ���ɹ�!");
	} else{
        AfxMessageBox("ɾ��ʧ��!");
	}
	
}

void CTestDlg::OnOK() 
{
	// TODO: Add extra validation here
	FreeLibrary(hinstDLL);
}
