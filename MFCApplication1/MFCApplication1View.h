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

// MFCApplication1View.h : interface of the CMFCApplication1View class
//

#pragma once
extern CDatabase opr_db;

struct threadInfo
{
	unsigned int nItem;//范围
//	CListCtrl &mmlist;//主窗口句柄，用于消息的发送
};

struct AlarmTagInfo
{
	CString dev_no;	
	CString dev_name;	
	CString alarm_no;	//点名
	CString alarm_name;	
	CString alarm_status;
};
struct UserInfo
{
	CString user_name;	//点名
	CString phone_no;	
	CString send_status;
};
enum APP_USER_STATE {
	TRUNK_IDLE,
	USER_IDLE,
	USER_GET_PHONE_NUM,
	USER_WAIT_DIAL_TONE,
	USER_WAIT_REMOTE_PICKUP,
	USER_PLAY,
	USER_TALKING,
	USER_WAIT_HANGUP
};


typedef struct{
	// trunck channel vars
	bool  EnCalled;
	int   lineState;
	int InUse;
	char DtmfBuf[251];
	// user channel  vars
	APP_USER_STATE	nStep;
	int				nToTrkCh;
	char			pPhoNumBuf[31];
	int				nTimeOut;
	char szAlarmLog[100];
	int iDialResult;
}CH_INFO;

typedef struct{
	
	CTime time_Dial;
	CTimeSpan timespan_Dial;
	char  szPhoneNo[30];
	char  szExt[6];
	int   iLev;
	BOOL  bListened; 
	BOOL  bDialed;   
	int   nExtDealy; 
	int   iDialTimes;
}PHONE_INFO;

UINT __cdecl ThreadPro(LPVOID args);
UINT __cdecl ThreadPhone(LPVOID args);

class CMFCApplication1View : public CListView
{
private:
    CListCtrl &m_list;
    CString    file_path;

protected: // create from serialization only
	CMFCApplication1View();
	DECLARE_DYNCREATE(CMFCApplication1View)
	CWinThread *m_pThread;
	CWinThread *m_pThread2;
    threadInfo m_Info;

// Attributes
public:
	CMFCApplication1Doc* GetDocument() const;
    HINSTANCE hinstDLL; 
	int nItem;
	int m_com;
	long m_bps;
	BOOL m_timer;
	CH_INFO ChInfo[1000];
	WORD	nTotalCh;
	int sgh;
	BOOL m_bUseActualPickup;
	int m_receive;

//	CList<AlarmTagInfo,AlarmTagInfo> m_AlarmTagInfoList;		//报警点链表
//	AlarmTagInfo m_AlarmTagInfo;

// Operations
public:
    int GetSelectIndex();
	void OnDisplay();
	void GetSmsUser();
	void OnSendSms();
	void GetSetting(void);
	void DoUserWork();
	BOOL InitCtiBoard();
	void OnPhone();
	void AlarmPhoneList();
// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // called first time after construct

// Implementation
public:
	virtual ~CMFCApplication1View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnButtonAdd();
    afx_msg void OnButtonMod();
    afx_msg void OnButtonDel();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnButtonDelDB();
    void DelListViewAll(void);
    afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnButtonSave();
	void OnConnection();
	void OnSendButton(CString alarm_name);
	void OnDisconnectionButton();
	afx_msg void OnDestroy();

};

#ifndef _DEBUG  // debug version in MFCApplication1View.cpp
inline CMFCApplication1Doc* CMFCApplication1View::GetDocument() const
   { return reinterpret_cast<CMFCApplication1Doc*>(m_pDocument); }
#endif

