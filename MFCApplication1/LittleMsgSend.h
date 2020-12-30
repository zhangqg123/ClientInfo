// LittleMsgSend.h: interface for the CLittleMsgSend class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LITTLEMSGSEND_H__7AB25A26_9784_462E_BD52_4D8DE73167CC__INCLUDED_)
#define AFX_LITTLEMSGSEND_H__7AB25A26_9784_462E_BD52_4D8DE73167CC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include   <afx.h>
#include "resource.h"
#include "Port.h"

enum MSG_STEP
{
	NO_SEND = 0,	//什么也没有发送
	SEND_SET,		//设置状态，等待应答
	SEND_LEN,		//设置长度，等待应答
	SEND_DATA		//发送数据，等待应答
};

struct LITTLEMSG
{
public:
	bool		m_bISTelNumber;		//是否是单一的电话，是就True，否False
	CString		m_csTelNumber;
	CString		m_csMsg;
	bool		m_bISSendSucced;	//是否发送成功	
	int			m_nSendNum;			//发送次数，3次都失败就放弃
	int			m_nAlarmClass;
	int			m_nAlarmLevel;
	int			m_nAlarmGroup;
	
};
static DWORD WINAPI  ThreadOpenTw(void* pthis);
static bool   g_bOneSendIsEndc = true;//单次发送结束标志

class CPlugInObj;
class CLittleMsgSend  
{
public:
	CLittleMsgSend(CPlugInObj* pObj);
	virtual ~CLittleMsgSend();

	void OnTime();
	BOOL AddMessageToList(const CString & csTelNum, const CString & csMessage );
	BOOL AddMessageToList(const CString & csTelNum, const CString & csMessage ,int nClass,int nGroup,int nLevel);
	BOOL Open();
	BOOL OpenCom();
	BOOL SendSMS(char* PhoneNum,char* Content);
	BOOL SendToCom(const char * chData, DWORD dwDataLen);

	BOOL IfTimeInterval(int nInterval);
	void WriteSMSPLogFile(CString csLog);
	int SendLogSMS(char* PhoneNum,char* Content,int nClass,int nGroup,int nLevel);
	void FilterPhoneNum(CStringA& SourPhone);
	int SendSMSMessage(char* f_PhoneNum,char* f_Content ,int nClass = 0,int nGroup = 0,int nLevel = 0);
	void DeleteAllMsg();
	void CopyBSTRtoString(CString & csMsg, BSTR & bstr);
	
public:
	int			m_nSendLen;		//最后一个步骤发送的长度
	DWORD		m_dwTick;		//保存TickCount用于判断超时
	int			m_nTimeOutCnt;	//超时的次数
	int			m_nConnect;		//是否已经连接成功
	int         m_nRepeatNumber;//短信重发次数        CLL    20150520
	int         m_nSleep;        //短消息的链表可发锁
	bool        m_bIsEnd;        //通知发送线程主线程已结束
	CRITICAL_SECTION    m_clock;
	CWinThread* m_sendthread;
	int         m_nSleepTime;   //短信每发送一次后的缓冲时间    单位毫秒
	CPtrList	m_MsgList;		//短消息的链表
	CPort		m_Port;			//串口类
	CPlugInObj  *m_pObj;
	BOOL		m_bConnect;		//是否已经连接成功
	MSG_STEP	m_nStep;		//保存短消息发送的步骤
	CStringA     m_chPortData;     //从串口读取的数据

};

#endif // !defined(AFX_LITTLEMSGSEND_H__7AB25A26_9784_462E_BD52_4D8DE73167CC__INCLUDED_)
