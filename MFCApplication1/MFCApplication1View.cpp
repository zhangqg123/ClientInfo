// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface 
// (the "Fluent UI") and is provided only as referential material to supplement the 
// Microsoft Foundation Classes Reference and related electronic documentation 
// included with the MFC C++ library software.  
// License terms to copy, use or distribute the Fluent UI are available separately.  
// To learn more about our Fluent UI licensing program, please visit 
// http://go.microsoft.com/fwlink/?LinkId=238214.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// MFCApplication1View.cpp : implementation of the CMFCApplication1View class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "MFCApplication1.h"
#endif
#include "IniFile.h"

#include "MFCApplication1Doc.h"
#include "MFCApplication1View.h"

#include "InfoAdd.h"
#include "client_info_db.h"
//#include "TestDlg.h"
#include "BasicExcel.h"
#include "string_oper.h"
#include "Shpa3api.h"
#include "CSplitString.h"
#include "SplitStr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CSplitStr Split;
CDatabase opr_db;
CList<AlarmTagInfo,AlarmTagInfo> m_AlarmTagInfoList;		//短信报警点链表
CList<AlarmTagInfo,AlarmTagInfo> m_AlarmPhoneInfoList;		//电话报警点链表
CList<UserInfo,UserInfo> m_UserInfoList;		
HANDLE hMutex;
PHONE_INFO g_PhoneInf[20];
CRITICAL_SECTION CriticalSection; 

int g_iCurPhone = 0;
int g_totalPhone=0;
//BOOL bStatic = FALSE;
BOOL m_phoneTimer = FALSE;
CString m_alarmPhoneMessage;
IMPLEMENT_DYNCREATE(CMFCApplication1View, CListView)

BEGIN_MESSAGE_MAP(CMFCApplication1View, CListView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	//添加的代码-----------------------
	ON_WM_TIMER()
	//---------------------------------
    ON_COMMAND(ID_BUTTON5, &CMFCApplication1View::OnButtonAdd)
    ON_COMMAND(ID_BUTTON7, &CMFCApplication1View::OnButtonMod)
    ON_COMMAND(ID_BUTTON8, &CMFCApplication1View::OnButtonDel)
    ON_COMMAND(ID_BUTTON9, &CMFCApplication1View::OnButtonDelDB)
    ON_NOTIFY_REFLECT(NM_DBLCLK, &CMFCApplication1View::OnNMDblclk)
    ON_COMMAND(ID_BUTTON2, &CMFCApplication1View::OnButtonSave)
END_MESSAGE_MAP()

// CMFCApplication1View construction/destruction

CMFCApplication1View::CMFCApplication1View()
    :m_list(GetListCtrl())//得到内置的listctrl引
{
	// TODO: add construction code here
	hMutex = CreateMutex(NULL, FALSE, NULL);
	InitializeCriticalSection(&CriticalSection);
	m_pThread = NULL;
	m_pThread2 = NULL;
	m_bUseActualPickup = FALSE;
	m_receive = 0;
}

CMFCApplication1View::~CMFCApplication1View()
{

	KillTimer(1);
	KillTimer(1000);
	CloseHandle(hMutex);
	opr_db.Close();
	DeleteCriticalSection(&CriticalSection);
	SsmCloseCti();
	WaitForSingleObject(m_pThread,100);
	WaitForSingleObject(m_pThread2,100);
}

BOOL CMFCApplication1View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

    cs.style |= LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SINGLESEL;

	return CListView::PreCreateWindow(cs);
}

void CMFCApplication1View::OnInitialUpdate()
{
	CListView::OnInitialUpdate();        

    DWORD dwStyle = m_list.GetExtendedStyle();

    //选中某行使整行高亮（只适用于报表风格的listctrl）
    dwStyle |= LVS_EX_FULLROWSELECT;
    dwStyle |= LVS_EX_GRIDLINES;//网格线（只适用与报表风格的listctrl）
    m_list.SetExtendedStyle(dwStyle); //设置扩展风格
    m_list.SetBkColor(RGB(200, 200, 200)); //设置背景颜色
    m_list.SetTextBkColor(RGB(200, 200, 200)); //设置文本背景颜色
    m_list.SetTextColor(RGB(10, 10, 80)); //设置文本颜色

    //插入列的标题，为了简单起见，我只插入三列
    m_list.InsertColumn( 0,     _T("客户标识"),     LVCFMT_CENTER);
	m_list.InsertColumn( 1,     _T("设备编号"),     LVCFMT_CENTER);
	m_list.InsertColumn( 2,     _T("设备名称"),     LVCFMT_CENTER);
	m_list.InsertColumn( 3,     _T("报警内容"),     LVCFMT_CENTER);
    m_list.InsertColumn( 4,     _T("报警编号"),     LVCFMT_CENTER);
    m_list.InsertColumn( 5,     _T("状态"),        LVCFMT_CENTER);
    m_list.InsertColumn( 6,     _T("发送时间"),     LVCFMT_CENTER);
    // Set reasonable widths for our columns
    for(int i = 0; i < m_list.GetHeaderCtrl()->GetItemCount(); i++)
    {
        m_list.SetColumnWidth(i, 170);
    }
	GetSetting();

	try
	{

		opr_db.Open(NULL, false, false, _T("ODBC;DSN=24;UID=root;PWD=zqg_zht"));

		if(opr_db.IsOpen())
		{
			CString str;
			str.Format(_T("数据库%s打开成功"), opr_db.GetConnect().GetString());
			//MessageBox(str.GetString());
			hinstDLL=NULL; 
			hinstDLL=LoadLibrary("sms.dll");
			if (hinstDLL==NULL)
			{
				AfxMessageBox("没找到sms.dll文件!");
			}else{
				opr_db.ExecuteSQL(_T("use jstdata;"));
//				CString strCmdi="insert into jst_zc_sms(id,dev_no,dev_name,alarm_no,alarm_name,create_time) select jsa.id,jsa.dev_no,jsd.dev_name,jsa.target_no,jsa.alarm_value,jsa.create_time from jst_zc_alarm jsa left join jst_zc_dev jsd on jsa.dev_no=jsd.dev_no where jsa.send_time> DATE_SUB(CURDATE(),INTERVAL 1 DAY) and jsa.id NOT IN (SELECT id FROM jst_zc_sms) and jsa.send_type='2'";
//				opr_db.ExecuteSQL(_T(strCmdi));
				GetSmsUser();
				g_totalPhone=m_UserInfoList.GetCount();
				OnDisplay();
				CString strCmdu="update jst_zc_sms set send_status='已发送' ,send_date=now()  where send_status is null or send_status!='已发送' limit 3";
				opr_db.ExecuteSQL(_T(strCmdu));
//				::strcpy(g_PhoneInf[0].szPhoneNo,"9,13898480908");
//				::strcpy(g_PhoneInf[1].szPhoneNo,"9,015699582810");
				m_timer=true;
				SetTimer(1,60000,NULL);//定时器ID为1，一个60秒触发一次的定时器
//					SetTimer(1000, 10, NULL);
			}
		}
		else
		{
			MessageBox(_T("数据库打开失败，数据将无法保存到数据库"));
		}
	}
	catch(CDBException* pe)
	{
		// The error code is in pe->m_nRetCode
//		pe->ReportError();
		pe->Delete();
//		OnDisplay();
	}
    
	// TODO: You may populate your ListView with items by directly accessing
	//  its list control through a call to GetListCtrl().
}

void CMFCApplication1View::OnPhone(){

	m_phoneTimer=TRUE;
//	SsmCloseCti();
//	if(!InitCtiBoard()){
//		AfxMessageBox("语音卡配置错误");
//	}else{
		SetTimer(1000, 500, NULL);
//	}
}

BOOL CMFCApplication1View::InitCtiBoard()
{
	//Initialization of CTI driver
	char CurPath[260],ShIndex[260],ShConfig[260];
	GetCurrentDirectory(200,CurPath);
	strcpy(ShConfig,CurPath);
	strcpy(ShIndex,CurPath);
	strcat(ShConfig,"\\ShConfig.ini");
	strcat(ShIndex,"\\ShIndex.ini");
	if( SsmStartCti(ShConfig,ShIndex) != 0) 
	{
		CString str1;
		SsmGetLastErrMsg(str1.GetBuffer(200));
		AfxMessageBox(str1, MB_OK) ;
		PostQuitMessage(0);
		return false;
    }

	//Initialization of channels on trunk-board

	nTotalCh = SsmGetMaxCh(); 
	nTotalCh = 2;
	for(int i=0; i<nTotalCh; i++)
	{
		ChInfo[i].InUse = 0;
		strcpy(ChInfo[i].DtmfBuf,"");
		ChInfo[i].nStep = TRUNK_IDLE;
		SsmSetMinVocDtrEnergy( i, 200000);
	}

	return true;
}

void CMFCApplication1View::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CMFCApplication1View::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CMFCApplication1View diagnostics

#ifdef _DEBUG
void CMFCApplication1View::AssertValid() const
{
	CListView::AssertValid();
}

void CMFCApplication1View::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CMFCApplication1Doc* CMFCApplication1View::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMFCApplication1Doc)));
	return (CMFCApplication1Doc*)m_pDocument;
}
#endif //_DEBUG

void CMFCApplication1View::GetSetting(void)
{
	CIniFile ini;
	TCHAR tcPath[100];
	::GetModuleFileName(NULL,tcPath,100);
	CString csPath = tcPath;
	csPath = csPath.Left( csPath.ReverseFind('\\') + 1) + _T("config.ini");
//	ini.SetFileName(csPath);
//	m_csCom = ini.GetInt(_T("SET"),_T("COM"));
//	m_csBps = ini.GetInt(_T("SET"),_T("BPS"));
	CString m_csCom,m_csBps;
//	CString strPath=_T("D:\\test\\demo.ini");
 
	::GetPrivateProfileString(_T("SET"), _T("COM"), _T(""), m_csCom.GetBuffer(MAX_PATH),MAX_PATH, csPath);
	::GetPrivateProfileString(_T("SET"), _T("BPS"), _T(""),m_csBps.GetBuffer(MAX_PATH),MAX_PATH,csPath);
	m_com=atoi(m_csCom);
	m_bps=atol(m_csBps);
}
// CMFCApplication1View message handlers


void CMFCApplication1View::OnButtonAdd()
{

	//	OnConnection();
	//    CInfoAdd add(m_list);
	//    add.DoModal();

}



void CMFCApplication1View::OnDisplay()
{
    // TODO: Add your command handler code here
    CListCtrl &m_list = this->GetListCtrl();

    int nItem           = 0;
    int     nTotalShow      = 0xFFFF;
	short   nSubItem        = 0;
	short   nSubItemTotal   = m_list.GetHeaderCtrl()->GetItemCount();
    CString FilterStr;

	m_list.DeleteAllItems();

    Cclient_info_db cRecordset(&opr_db);

//    OnSearchGetShowCount(nTotalShow);
//    OnSearchGetFilterString(FilterStr);

	try
	{
        cRecordset.m_strFilter = FilterStr;
		cRecordset.Open(CRecordset::snapshot, _T("select * from jst_zc_sms order by create_time desc"));

		CString cValue,str;
		CList<AlarmTagInfo,AlarmTagInfo> m_AlarmInfoList;
		while(!cRecordset.IsEOF() && nTotalShow--)
		{
			AlarmTagInfo m_AlarmTagInfo;
			for(nSubItem = 0; nSubItem < nSubItemTotal; nSubItem++)
			{
				cRecordset.GetFieldValue(nSubItem, cValue);

				if(nSubItem == 0)
				{
					m_list.InsertItem(nItem, cValue.GetString());
				}
				else
				{
					if(cValue.GetLength())
					{
//                        if( nSubItem == DLG_BIRTH_DAY_INDEX    || nSubItem == DLG_CONTACT_DATE_INDEX )
                        if( nSubItem == DLG_BIRTH_DAY_INDEX )
                        {
                            CTime time;
                            if(StringToDateTime(cValue, time))
                            {
                                if(!isDateTimeDefault(time))
                                {
									m_list.SetItemText(nItem, nSubItem, cValue.GetString());
                                }
                            }
                        }
                        else
                        {
					//		ConvertUTF8ToANSI(cValue,str);
							m_list.SetItemText(nItem, nSubItem, cValue.GetString());
							if(nSubItem==1){
								m_AlarmTagInfo.dev_no	=_T(cValue.GetString());
							}
							if(nSubItem==2){
								m_AlarmTagInfo.dev_name = cValue;
							}
							if(nSubItem==3){
								m_AlarmTagInfo.alarm_name	=_T(cValue.GetString());
							}
							if(nSubItem==4){
								m_AlarmTagInfo.alarm_no = cValue;
							}
							if(nSubItem==5){
								m_AlarmTagInfo.alarm_status = cValue;
							}
						}
					}
				}
			}
			if(m_AlarmTagInfo.alarm_status!="已发送"){
				WaitForSingleObject(hMutex, INFINITE);
				m_AlarmTagInfoList.AddTail(m_AlarmTagInfo);
				ReleaseMutex(hMutex);
				EnterCriticalSection(&CriticalSection);
				m_AlarmPhoneInfoList.AddTail(m_AlarmTagInfo);
				LeaveCriticalSection(&CriticalSection);
			}
			cRecordset.MoveNext();
			nItem++;
		}

		cRecordset.Close();
//		hMutex = CreateMutex(NULL, FALSE, NULL);
		if(m_AlarmTagInfoList.GetCount()>0){
			m_pThread = AfxBeginThread(ThreadPro,(LPVOID)this);
		}
		if(m_AlarmPhoneInfoList.GetCount()>0){
			if(m_phoneTimer==FALSE){
				m_pThread2 = AfxBeginThread(ThreadPhone,(LPVOID)this);
			}
		}
	} 
	catch(CDBException   *pE) 
	{ 
		pE-> ReportError(); 
		pE-> Delete(); 
	}

}

void CMFCApplication1View::GetSmsUser()
{
    int     nTotalShow      = 0xFFFF;
	short   nSubItem        = 0;
	int phone_count=0;
    CRecordset cRecordset(&opr_db);

	try
	{
		cRecordset.Open(CRecordset::snapshot, _T("Select user_name,phone_no from jst_zc_user where send_status='1'"));
		CString cValue,str;
//		CList<UserInfo,UserInfo> m_UserInfoList;

		while(!cRecordset.IsEOF() && nTotalShow--)
		{
			UserInfo m_UserInfo;
			for(nSubItem = 0; nSubItem < 2; nSubItem++)
			{
				cRecordset.GetFieldValue(nSubItem, cValue);

				if(nSubItem==0){
					m_UserInfo.user_name =_T(cValue.GetString());
				}
				if(nSubItem==1){
					m_UserInfo.phone_no =_T(cValue);
				}
//				if(nSubItem==2){
//					m_UserInfo.send_status = _T(cValue);
//				}
			}
			m_UserInfoList.AddTail(m_UserInfo);
			cRecordset.MoveNext();
			char phoneNo[260];
			::strcpy(phoneNo,"9,");
			::strcat(phoneNo,m_UserInfo.phone_no);
			::strcpy(g_PhoneInf[phone_count].szPhoneNo,phoneNo);
			phone_count++;
		}
		cRecordset.Close();
	} 
	catch(CDBException   *pE) 
	{ 
		pE-> ReportError(); 
		pE-> Delete(); 
	}
}

UINT __cdecl ThreadPro(LPVOID args)
{
	CMFCApplication1View *cav=(CMFCApplication1View*)args;
	bool bResult = false;
	cav->m_timer=false;
	cav->OnSendSms();
	cav->m_timer=true;
	return 0;
}
UINT __cdecl ThreadPhone(LPVOID args)
{
	CMFCApplication1View *cav=(CMFCApplication1View*)args;
	m_phoneTimer=TRUE;
	SsmCloseCti();
	if(!cav->InitCtiBoard()){
		AfxMessageBox("语音卡配置错误");
	}else{

		cav->AlarmPhoneList();
		g_iCurPhone=0;
		while(m_phoneTimer==TRUE)
		{
			cav->DoUserWork();
			Sleep(100);
		}
//		SsmCloseCti();
	}
	return 0;
}

int CMFCApplication1View::GetSelectIndex()
{
    int nItem = 0;
	BOOL selected = false;
	POSITION pos = m_list.GetFirstSelectedItemPosition();

	if (pos == NULL)
	{
		TRACE(_T("No items were selected!\n"));
	}
	else
	{
		while (pos)
		{
			selected = true;
			nItem = m_list.GetNextSelectedItem(pos);
			TRACE(_T("Item %d was selected!\n"), nItem);
		}
	}

	if(!selected)
	{
		return -1;
	}

    return nItem;
}

void CMFCApplication1View::OnConnection() 
{
	CString strText;
	CString strtmp;
	CString tmp[256];
	strtmp="//上海迅赛信息技术有限公司,网址www.xunsai.com//";
	typedef int(_stdcall *Sms_Connection)(CString CopyRight,int Com_Port,int Com_BaudRate,CString *Mobile_Type);
	Sms_Connection Proc;
	Proc = (Sms_Connection)GetProcAddress(hinstDLL,"Sms_Connection");
	int a1=m_com;
	int a2=m_bps;
	int iValue = Proc(strtmp,m_com,m_bps,tmp);
    if (iValue == 1)
	{
//	    strText.Format("短信猫连接成功!(短信猫型号为:%s)",*tmp);
//        AfxMessageBox(strText);
//		OnSendButton(alarm_name);
	} else if (iValue == 0) {
 //       AfxMessageBox("短信猫连接失败!(请重新连接短信猫)");
//		OnDisconnectionButton();
	}
}

void CMFCApplication1View::OnSendSms()
{
	OnConnection();

	CString	m_id;
	CString	m_dev_name;
	CString	m_dev_no;

	CString	m_alarm_name;
	CString	m_alarm_no;
	CString	m_send_status;
	CTime	m_send_date;
	CString alarm_message;
    CTime      Time;
	POSITION pos1,pos2,posu;
	AlarmTagInfo AlarmTagInfoStruct;
	int i=0;
	for (pos1 = m_AlarmTagInfoList.GetHeadPosition(); ( pos2 = pos1 ) != NULL;  )
	{
		AlarmTagInfoStruct = m_AlarmTagInfoList.GetNext(pos1); 
		m_send_status       = AlarmTagInfoStruct.alarm_status;

		if (AlarmTagInfoStruct.alarm_status != "已发送")
		{
			m_dev_name = AlarmTagInfoStruct.dev_name;
			m_dev_no = AlarmTagInfoStruct.dev_no;
			m_alarm_name = AlarmTagInfoStruct.alarm_name;
			m_alarm_no = AlarmTagInfoStruct.alarm_no;
			WaitForSingleObject(hMutex, INFINITE);
			if(m_AlarmTagInfoList.GetCount() > 0)
			{
				m_AlarmTagInfoList.RemoveAt(pos2);
			}
			ReleaseMutex(hMutex);
			alarm_message=m_dev_name+"报警::"+m_alarm_name;
			OnSendButton(alarm_message);
			i=i+1;
			if(i>=3)
				break;
//			Sleep(5000);
		}
	}
	WaitForSingleObject(hMutex, INFINITE);
	if(m_AlarmTagInfoList.GetCount() > 0)
	{
		m_AlarmTagInfoList.RemoveAll();
	}
	ReleaseMutex(hMutex);

	OnDisconnectionButton();
}

void CMFCApplication1View::OnButtonMod()
{
//	OnSendButton();
    // TODO: Add your command handler code here

	int nItem = GetSelectIndex();

    if(nItem == -1)
    {
        return ;
    }

    CInfoAdd Mod(m_list, nItem);
    Mod.DoModal();	
}

void CMFCApplication1View::OnTimer(UINT_PTR nIDEvent)
{
	if(nIDEvent==1)
	{
		if(m_timer==false){
			return;
		}

//		CString strCmdi="insert into jst_zc_sms(id,dev_no,dev_name,alarm_no,alarm_name,create_time) select jsa.id,jsa.dev_no,jsd.dev_name,jsa.target_no,jsa.alarm_value,jsa.create_time from jst_zc_alarm jsa left join jst_zc_dev jsd on jsa.dev_no=jsd.dev_no where jsa.send_time> DATE_SUB(CURDATE(),INTERVAL 1 DAY) and jsa.id NOT IN (SELECT id FROM jst_zc_sms) and jsa.send_type='2'";
//		opr_db.ExecuteSQL(_T(strCmdi));
		OnDisplay();
		CString strCmdu="update jst_zc_sms set send_status='已发送' ,send_date=now() where send_status is null or send_status!='已发送' limit 3";
		opr_db.ExecuteSQL(_T(strCmdu));	
//		int ss=1;
	}else{
//		if (bStatic == TRUE) 
//		{
//			KillTimer(1000); 
//			SsmCloseCti();
//			m_phoneTimer=FALSE;
//			bStatic == FALSE;
//		} 
//		if(nIDEvent==1000){
//			AlarmPhoneList();
//			DoUserWork();
//		}else{
			CListView::OnTimer(nIDEvent);
//		}
	}
}
void CMFCApplication1View::AlarmPhoneList()
{
	CString	m_id;
	CString	m_dev_name;
	CString	m_dev_no;

	CString	m_alarm_name;
	CString	m_alarm_no;
	CString	m_send_status;
	CTime	m_send_date;
	CString alarm_message;
    CTime      Time;
	POSITION pos1,pos2,posu;
	AlarmTagInfo AlarmTagInfoStruct;
	int i=0;
	for (pos1 = m_AlarmPhoneInfoList.GetHeadPosition(); ( pos2 = pos1 ) != NULL;  )
	{
		AlarmTagInfoStruct = m_AlarmPhoneInfoList.GetNext(pos1); 
		m_send_status       = AlarmTagInfoStruct.alarm_status;

		if (AlarmTagInfoStruct.alarm_status != "已发送")
		{
			m_dev_name = AlarmTagInfoStruct.dev_name;
			m_dev_no = AlarmTagInfoStruct.dev_no;
			m_alarm_name = AlarmTagInfoStruct.alarm_name;
			m_alarm_no = AlarmTagInfoStruct.alarm_no;
			EnterCriticalSection(&CriticalSection);
			if(m_AlarmPhoneInfoList.GetCount() > 0)
			{
				m_AlarmPhoneInfoList.RemoveAt(pos2);
			}
			LeaveCriticalSection(&CriticalSection);
			alarm_message=m_dev_name+","+m_alarm_name+",";
			m_alarmPhoneMessage+=alarm_message;
			i=i+1;
			if(i>=3)
				break;
		}
	}
	EnterCriticalSection(&CriticalSection);
	if(m_AlarmPhoneInfoList.GetCount() > 0)
	{
		m_AlarmPhoneInfoList.RemoveAll();
	}
	LeaveCriticalSection(&CriticalSection);
}
void CMFCApplication1View::DoUserWork()
{
//	bStatic = FALSE;
	int ch=0;
	int i=0;
	int s1=ChInfo[i].nStep;
	char phoneno[260];
	switch( ChInfo[i].nStep )
	{
		case TRUNK_IDLE:
			Sleep(5000);
			ChInfo[i].nStep = USER_IDLE;
			break;
		case USER_IDLE:
			strcpy(ChInfo[i].pPhoNumBuf,"");
			SsmClearRxDtmfBuf(ch);
			sgh=SsmGetHookState(ch);
			strcpy(ChInfo[i].pPhoNumBuf,g_PhoneInf[g_iCurPhone].szPhoneNo);
			g_iCurPhone++;

			ChInfo[i].nStep = USER_GET_PHONE_NUM;
			break;
		case USER_GET_PHONE_NUM:
			SsmPickup(ch);
			ChInfo[i].InUse = 1;
					
			ChInfo[i].nToTrkCh = ch; 
			ChInfo[i].nStep = USER_WAIT_DIAL_TONE;
			break;
		case USER_WAIT_DIAL_TONE:
			ch = ChInfo[i].nToTrkCh;
			if ( SsmAutoDial(i, ChInfo[i].pPhoNumBuf) ==0  )
			{
				ChInfo[i].InUse = 2;
				ChInfo[i].nStep = USER_WAIT_REMOTE_PICKUP;
				SsmClearAMDResult(ch);
			}
			else 
			{
				SsmHangup(ch);
				ChInfo[i].InUse = 0;
				SsmClearRxDtmfBuf(ch);

		//		SsmSendTone(ch,1);				//send busy tone
				ChInfo[i].nStep = USER_WAIT_HANGUP;
			}
			break;

		case USER_WAIT_REMOTE_PICKUP: 
			ch = ChInfo[i].nToTrkCh;
//			ch=0;
			ChInfo[i].lineState = SsmGetChState(i);
			if (!m_bUseActualPickup)
			{
				if(SsmChkAutoDial(i) == 7)		//remote user pickup
				{
					int pf=0;
					if( SsmGetRxDtmfLen(i) >= 1 ) {
						pf=1;
						SsmGetDtmfStr(i,ChInfo[i].DtmfBuf);
						if (ChInfo[i].DtmfBuf[0] == '1') {
			//				ChInfo[i].InUse = 4;
							Split.SetSplitFlag(",");
							Split.SetSequenceAsOne(TRUE);
							Split.SetData(m_alarmPhoneMessage);
							CStringArray array;
							Split.GetSplitStrArray(array);
							SsmClearFileList(i); 

							for (int j=0; j< array.GetSize(); j++)
							{
								CString tmp=array.GetAt(j);
								char CurPath[260],ShConfig[260];
								GetCurrentDirectory(200,CurPath);
								::strcpy(ShConfig,CurPath);
								strcat(ShConfig,"\\voice\\"+tmp+".wav");
								SsmAddToFileList(i,ShConfig,-1,0,0xFFFFFF);
							}

/*
							char CurPath[260],ShConfig[260],ShConfig1[260],ShConfig2[260];
							GetCurrentDirectory(200,CurPath);
							::strcpy(ShConfig,CurPath);
							::strcpy(ShConfig1,CurPath);
							::strcpy(ShConfig2,CurPath);
							strcat(ShConfig,"\\bgmusic.wav");
							strcat(ShConfig1,"\\温湿度1.wav");
							strcat(ShConfig2,"\\温湿度2.wav");
							SsmClearFileList(i); 
							SsmAddToFileList(i,ShConfig1,-1,0,0xFFFFFF);
							SsmAddToFileList(i,ShConfig2,-1,0,0xFFFFFF);
							SsmAddToFileList(i,ShConfig,-1,0,0xFFFFFF);
						*/
							ChInfo[i].DtmfBuf[0] = 0;
							ChInfo[i].nStep = USER_PLAY;
							SsmClearRxDtmfBuf(i);
						}
					}
					if(pf==0 && SsmGetChStateKeepTime(i)>30000){
						ChInfo[i].nStep = USER_WAIT_HANGUP;
					}
				}
				else if(SsmGetHookState(i) == 0)		//user hangup
				{
					SsmHangup(i);
					ChInfo[i].InUse = 0;
					SsmClearRxDtmfBuf(i);
					
					SsmClearRxDtmfBuf(i);
					ChInfo[i].nStep = TRUNK_IDLE;
				}
				else if((ChInfo[i].lineState == S_CALL_PENDING) ||
					(SsmChkAutoDial(i) ==10)  ) //busy tone or 30s time out
				{
					SsmHangup(i);
					ChInfo[i].InUse = 0;
					SsmClearRxDtmfBuf(i);
					
				//	SsmSendTone(i,1);					//send busy tone
					ChInfo[i].nStep = USER_WAIT_HANGUP;
				}
			}
			else
			{
				if(SsmChkAutoDial(ch) == 7)		//remote user pickup
				{
					SsmTalkWith(i, ch);
				}
				if(SsmGetAMDResult(ch) == 0)		//real user pickup
				{
					ChInfo[ch].InUse = 4;
					
					ChInfo[i].nStep = USER_TALKING;
				}
				else if(SsmGetAMDResult(ch) == 2)		//real user pickup
				{
					ChInfo[ch].InUse = 7;	//color ring or answer machine
				}
				else if(SsmGetHookState(i) == 0)		//user hangup
				{
					SsmHangup(ch);
					ChInfo[ch].InUse = 0;
					SsmClearRxDtmfBuf(ch);
					
					SsmClearRxDtmfBuf(i);
					ChInfo[i].nStep = TRUNK_IDLE;
				}
				else if((ChInfo[ch].lineState == S_CALL_PENDING) || (SsmChkAutoDial(ch) ==10)) //busy tone or 30s time out
				{
					SsmHangup(ch);
					ChInfo[ch].InUse = 0;
					SsmClearRxDtmfBuf(ch);
					
					SsmSendTone(i,1);					//send busy tone
					ChInfo[i].nStep = USER_WAIT_HANGUP;
				}
			}
			break ;
		case USER_PLAY:
			SsmPlayFileList(i);
//			g_iCurPhone++;
			ChInfo[i].nStep = USER_TALKING;
			break;
		case USER_TALKING:
			ch = ChInfo[i].nToTrkCh;
			ChInfo[i].lineState = SsmGetChState(i);

			if( !SsmGetHookState(i) ||		//user hang up
				 (ChInfo[i].lineState == S_CALL_PENDING ))
			{
				SsmStopPlayFileList(i);
				SsmClearFileList(i);
				ChInfo[i].InUse = 0;
				ChInfo[i].nStep = USER_WAIT_HANGUP;
			}
			break;
			
		case USER_WAIT_HANGUP:
			SsmHangup(i); 
			SsmClearRxDtmfBuf(i);
			if(g_iCurPhone>g_totalPhone){
				int n = g_iCurPhone;
				char t[256];
				string s;
				sprintf(t, "%d", n);
				s = t;
				cout << s << endl;				
			//	bStatic=TRUE;
				m_phoneTimer=FALSE;
				SsmCloseCti();
			}

			ChInfo[i].nStep = TRUNK_IDLE;
			break;

		default:
			ChInfo[i].nStep = TRUNK_IDLE;
			break;
	}//end switch
	ChInfo[i].iDialResult = SsmChkAutoDial(i);
	if((ChInfo[i].iDialResult ==10)||(ChInfo[i].iDialResult == 3)\
		||(ChInfo[i].iDialResult == 4)||(ChInfo[i].iDialResult == 5)\
		||(ChInfo[i].iDialResult == 6)||(ChInfo[i].iDialResult == 11)\
		||(ChInfo[i].iDialResult == 12)) //busy tone or 30s time out
		{
			if(ChInfo[i].iDialResult == 10)
				sprintf(ChInfo[i].szAlarmLog,"通道[%d] 没有接听电话",i);
			else if(ChInfo[i].iDialResult == 3)
				sprintf(ChInfo[i].szAlarmLog,"通道[%d] 没有拨号音，线路可能松动",i);
			else if(ChInfo[i].iDialResult == 4)
				sprintf(ChInfo[i].szAlarmLog,"通道[%d] 被叫忙",i);
			else if(ChInfo[i].iDialResult == 5)
				sprintf(ChInfo[i].szAlarmLog,"通道[%d] 线路出现异常",i);
			else if(ChInfo[i].iDialResult == 6)
				sprintf(ChInfo[i].szAlarmLog,"通道[%d] 没有回铃音，可能频率配置有问题",i);
			else if(ChInfo[i].iDialResult == 11)
				sprintf(ChInfo[i].szAlarmLog,"通道[%d] 外呼失败",i);
			else if(ChInfo[i].iDialResult == 12)
				sprintf(ChInfo[i].szAlarmLog,"通道[%d] 被叫为空号",i);

	//		AlarmLog(ChInfo[i].szAlarmLog);
		
			SsmClearFileList(i);
			SsmHangup(i);
			ChInfo[i].InUse = 0;
			SsmClearRxDtmfBuf(i);
			strcpy(ChInfo[i].pPhoNumBuf,"");
			ChInfo[i].nStep = TRUNK_IDLE;
	}
}


void CMFCApplication1View::OnSendButton(CString alarm_name) 
{
	CString strText;
	POSITION pos3,pos4;
	UserInfo UserInfoStruct;
	typedef int(_stdcall *Sms_Send)(CString Sms_TelNum,CString Sms_Text);
	Sms_Send Proc;
	int iValue;
	Proc = (Sms_Send)GetProcAddress(hinstDLL,"Sms_Send");
	for (pos3 = m_UserInfoList.GetHeadPosition(); ( pos4 = pos3 ) != NULL;  )
	{
		UserInfoStruct = m_UserInfoList.GetNext(pos3); 
		iValue = Proc(_T(UserInfoStruct.phone_no),_T(alarm_name));
//		int iValue = Proc(_T("15699582810"),_T(alarm_name));
//		if(iValue==1)
//		{
//	        AfxMessageBox(_T("发送成功!"));
//		} else{
//	        AfxMessageBox(_T("发送失败!"));
//		}
		Sleep(5000);
	}
//	OnDisconnectionButton();
}
void CMFCApplication1View::OnDisconnectionButton() 
{
	typedef int(_stdcall *Sms_Disconnection)();
    Sms_Disconnection Proc;
	Proc = (Sms_Disconnection)GetProcAddress(hinstDLL,"Sms_Disconnection");
	int iValue = Proc();
}

void CMFCApplication1View::OnButtonDel()
{
    // TODO: Add your command handler code here

	int nItem = GetSelectIndex();

    if(nItem == -1)
    {
        return ;
    }

    m_list.DeleteItem(nItem);
}


void CMFCApplication1View::OnButtonDelDB()
{
    // TODO: Add your command handler code here

    int nItem = GetSelectIndex();

    if(nItem == -1)
    {
        return ;
    }

    //to delete the item from the database

    Cclient_info_db record(&opr_db);

    record.DelDbOpr(m_list, nItem);

    m_list.DeleteItem(nItem);
}


void CMFCApplication1View::DelListViewAll(void)
{
    for(int index = m_list.GetItemCount() - 1; index >= 0; index--)
	{
		m_list.DeleteItem(index);
	}
}

void CMFCApplication1View::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    
    OnButtonMod();
    
    // TODO: Add your control notification handler code here
    *pResult = 0;
}

using namespace YExcel;

void CMFCApplication1View::OnButtonSave()
{
    int ItemCount = m_list.GetItemCount();
	int subnItemCount = m_list.GetHeaderCtrl()->GetItemCount();

	if(!ItemCount || !subnItemCount)
	{
		return;
	}

	if(!this->file_path.GetLength()) //如果文件名为空，弹出对话框
	{
		static TCHAR BASED_CODE szFilter[] = _T("Datasheet Files (*.xls)|*.xls||");

		CFileDialog file(false, _T("csv"), _T("保单保存"), 6, szFilter);
		file.DoModal();

		this->file_path = file.GetPathName();
	}

	if(!this->file_path.GetLength()) //如果文件名为空，弹出对话框
	{
		return;
	}

	BasicExcel e;
	e.New(1);
	e.RenameWorksheet("Sheet1", "Test1");
	BasicExcelWorksheet* sheet = e.GetWorksheet("Test1");
	BasicExcelCell* cell;

	if(!sheet)
	{
		return;
	}

    //写完标题再写内容
    CString TitleContent;
    HDITEM hdi;

    enum   
    { 
        BufSize = 256 ,
    };

    TCHAR  lpBuffer[BufSize];
 
	for(int subnItem = 0; subnItem < subnItemCount; subnItem++)
	{
        hdi.mask       = HDI_TEXT;
        hdi.pszText    = lpBuffer;
        hdi.cchTextMax = BufSize;

        m_list.GetHeaderCtrl()->GetItem(subnItem, &hdi);

        TitleContent = hdi.pszText;

		if(TitleContent.GetLength())
		{
			cell = sheet->Cell(0, subnItem);
			cell->SetString(TitleContent.GetString());
		}
	}

	//遍历所有行
	CString subContent;
	for(int nItem = 0; nItem < ItemCount; nItem++)
	{
		for(int subnItem = 0; subnItem < subnItemCount; subnItem++)
		{
			subContent = m_list.GetItemText(nItem, subnItem);

			if(subContent.GetLength())
			{
				cell = sheet->Cell(nItem + 1, subnItem);
				cell->SetString(subContent.GetString());
			}
		}
	}

	e.SaveAs("aa.xls");

	CopyFile(_T("aa.xls"), file_path.GetString(), false);
}
void CMFCApplication1View::OnDestroy() 
{
	CMFCApplication1View::OnDestroy();
	SsmCloseCti();	
}