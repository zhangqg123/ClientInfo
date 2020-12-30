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
	NO_SEND = 0,	//ʲôҲû�з���
	SEND_SET,		//����״̬���ȴ�Ӧ��
	SEND_LEN,		//���ó��ȣ��ȴ�Ӧ��
	SEND_DATA		//�������ݣ��ȴ�Ӧ��
};

struct LITTLEMSG
{
public:
	bool		m_bISTelNumber;		//�Ƿ��ǵ�һ�ĵ绰���Ǿ�True����False
	CString		m_csTelNumber;
	CString		m_csMsg;
	bool		m_bISSendSucced;	//�Ƿ��ͳɹ�	
	int			m_nSendNum;			//���ʹ�����3�ζ�ʧ�ܾͷ���
	int			m_nAlarmClass;
	int			m_nAlarmLevel;
	int			m_nAlarmGroup;
	
};
static DWORD WINAPI  ThreadOpenTw(void* pthis);
static bool   g_bOneSendIsEndc = true;//���η��ͽ�����־

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
	int			m_nSendLen;		//���һ�����跢�͵ĳ���
	DWORD		m_dwTick;		//����TickCount�����жϳ�ʱ
	int			m_nTimeOutCnt;	//��ʱ�Ĵ���
	int			m_nConnect;		//�Ƿ��Ѿ����ӳɹ�
	int         m_nRepeatNumber;//�����ط�����        CLL    20150520
	int         m_nSleep;        //����Ϣ������ɷ���
	bool        m_bIsEnd;        //֪ͨ�����߳����߳��ѽ���
	CRITICAL_SECTION    m_clock;
	CWinThread* m_sendthread;
	int         m_nSleepTime;   //����ÿ����һ�κ�Ļ���ʱ��    ��λ����
	CPtrList	m_MsgList;		//����Ϣ������
	CPort		m_Port;			//������
	CPlugInObj  *m_pObj;
	BOOL		m_bConnect;		//�Ƿ��Ѿ����ӳɹ�
	MSG_STEP	m_nStep;		//�������Ϣ���͵Ĳ���
	CStringA     m_chPortData;     //�Ӵ��ڶ�ȡ������

};

#endif // !defined(AFX_LITTLEMSGSEND_H__7AB25A26_9784_462E_BD52_4D8DE73167CC__INCLUDED_)
