// LittleMsgSend.cpp: implementation of the CLittleMsgSend class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LittleMsgSend.h"
//#include "Tw125DevDLL.h"
//#include "PlugInObj.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLittleMsgSend::CLittleMsgSend(CPlugInObj* pObj)
{
	m_nConnect	= 0;
	m_nSendLen	= 0;
	m_nTimeOutCnt = 0;
	m_nRepeatNumber = 1;    //    CLL   20150520
	m_nSleep = false;
	InitializeCriticalSection(&m_clock);
	m_bIsEnd = false;
	m_nSleepTime = 20000;
	m_pObj = pObj;
	m_bConnect	= FALSE;
	m_nStep		= NO_SEND;
	m_chPortData = "";
}

CLittleMsgSend::~CLittleMsgSend()
{
	DeleteCriticalSection(&m_clock);
	DeleteAllMsg();
	if (m_sendthread != NULL)
	{		
		UINT  exitcode = 0;
		AfxEndThread(exitcode);
	}
}

BOOL CLittleMsgSend::Open()
{	
	m_sendthread = AfxBeginThread((AFX_THREADPROC)ThreadOpenTw, this, THREAD_PRIORITY_NORMAL);
	return m_nConnect;
}
DWORD WINAPI ThreadOpenTw(void* pthis)
{
	Sleep(5000);
	CString csTemp;
	while(true)
	{
		if (((CLittleMsgSend*)pthis)->m_nConnect == 1)
		{
   			if(g_bOneSendIsEndc)
			{
				g_bOneSendIsEndc = false;
				//避免定时短信线程并发操作短信猫造成短信猫死机
//				if (!((CLittleMsgSend*)pthis)->m_pObj->m_bOnTimerSend)
				if (false)
				{
					((CLittleMsgSend*)pthis)->OnTime();
				}
				else
				{
					g_bOneSendIsEndc = true;
				}
				
				Sleep(5000);
			}
		}
		else
		{
			//CloseTw125();
			//((CLittleMsgSend*)pthis)->m_nConnect = OpenTw125();       //  程序运行慢的主要因素    CLL    20150519
			((CLittleMsgSend*)pthis)->m_Port.Close();
			((CLittleMsgSend*)pthis)->m_nConnect = ((CLittleMsgSend*)pthis)->OpenCom();
			if(((CLittleMsgSend*)pthis)->m_nConnect == 1)
			{
//				csTemp.Format(IDS_LINKDEVICESUCC);
			}
			else
			{
//				csTemp.Format(IDS_LINKDEVICEEOR);
			}
			((CLittleMsgSend*)pthis)->WriteSMSPLogFile(csTemp);
		}
		Sleep(5000);
	}
	return 0;
}

BOOL CLittleMsgSend::OpenCom()
{
//	m_bConnect = m_pObj->Open(m_Port);
	return m_bConnect;
}

void CLittleMsgSend::DeleteAllMsg()
{
	while(m_MsgList.GetCount() > 0)
	{
		LITTLEMSG * lpMsg = (LITTLEMSG *)m_MsgList.RemoveHead();
		delete lpMsg;
	}
}

void CLittleMsgSend::CopyBSTRtoString(CString & csMsg, BSTR & bstr)
{
	csMsg = "";
	BYTE * byData = (BYTE *)(bstr);
	for(int i=0; i<140; i+=2)
	{
		if(byData[i] == 0 && byData[i+1] == 0)
			break;
		csMsg += byData[i+1];
		csMsg += byData[i];
	}
}

BOOL CLittleMsgSend::AddMessageToList(const CString & csTelNum, const CString & csMessage)
{
	CString csTemp = csTelNum;
	int Len = csTemp.GetLength();
	if(Len != 11)
		return FALSE;
/*	for(int i=0; i<Len; i++)
	{
		if(csTemp<'0' || csTemp>'9')
			return FALSE;
	}*/
	LITTLEMSG * lpLtMsg = new LITTLEMSG;
	lpLtMsg->m_bISTelNumber = TRUE;
	lpLtMsg->m_csTelNumber = csTelNum;
	lpLtMsg->m_csMsg = csMessage;
	lpLtMsg->m_nSendNum = 0;
	
	EnterCriticalSection(&m_clock);
	m_MsgList.AddTail(lpLtMsg);
	LeaveCriticalSection(&m_clock);
	return TRUE;
}

BOOL CLittleMsgSend::AddMessageToList(const CString & csTelNum, const CString & csMessage ,int nClass,int nGroup,int nLevel)
{
	CString csTemp = csTelNum;
	int Len = csTemp.GetLength();
	if(Len != 11)
		return FALSE;
/*	for(int i=0; i<Len; i++)
	{
		if(csTemp<'0' || csTemp>'9')
			return FALSE;
	}*/
	LITTLEMSG * lpLtMsg = new LITTLEMSG;
	lpLtMsg->m_bISTelNumber = TRUE;
	lpLtMsg->m_csTelNumber = csTelNum;
	lpLtMsg->m_csMsg = csMessage;
	lpLtMsg->m_nSendNum = 0;
	lpLtMsg->m_nAlarmClass = nClass;
	lpLtMsg->m_nAlarmGroup = nGroup;
	lpLtMsg->m_nAlarmLevel = nLevel;
	
	EnterCriticalSection(&m_clock);
	m_MsgList.AddTail(lpLtMsg);
	LeaveCriticalSection(&m_clock);
	return TRUE;
}


int CLittleMsgSend::SendLogSMS(char* PhoneNum,char* Content,int nClass,int nGroup,int nLevel)
{
	//开始发送报警短信
	int l_iReSendNum = 1;
	CString l_csSmsLog;
	CString csPhoneNum = PhoneNum;
	CString csContent = Content;
	while(l_iReSendNum > 0)
	{
		BOOL bSendOk = FALSE;
		int nSendCount = 0;
		while(!bSendOk)
		{
			bSendOk = SendSMS(PhoneNum,Content);
			nSendCount++;
			//确保运行时只发送一次且短信猫断开时可以退出
			if (nSendCount == 15 || bSendOk)
			{
				break;
			}
			Sleep(2000);
		}
		CString csNum;
		csNum.Format(_T("QHJ: 报警发送轮询次数 %d OK[%d]"),nSendCount, bSendOk);
		OutputDebugString(csNum);
		if(bSendOk)
		{
	//		l_csSmsLog.Format(IDS_SENDCESUCC,csPhoneNum,csContent);
			WriteSMSPLogFile(l_csSmsLog);
			return 1;
		}
		else
		{
			CString csErr = _T("AT+CMGS 报文返回ERROR");
//			l_csSmsLog.Format(IDS_SENDEOR,csPhoneNum,csContent, nClass, nGroup, nLevel, csErr);
			WriteSMSPLogFile(l_csSmsLog);
			l_iReSendNum--;
		}
	}

	return -1;
}

BOOL CLittleMsgSend::SendToCom(const char * chData, DWORD dwDataLen)
{
	m_Port.ClearBuf();
	BOOL bResult = m_Port.Send(chData, dwDataLen);
	m_dwTick = ::GetTickCount();
	return bResult;
}

BOOL CLittleMsgSend::SendSMS(char* PhoneNum,char* Content)
{
	BOOL bSendComplete = FALSE;
	CString csLogInfo;
	char chData[1024] = {0};

	// 电话号码转换
	CString csTemp = PhoneNum;
	csTemp = csTemp.Mid(2, csTemp.GetLength());
	int Len = csTemp.GetLength();
	CString csTelNumOK;
	if(Len == 11)	//电话号码是奇数
		csTemp += _T('F');
	Len = csTemp.GetLength();
	for(int i=0; i<Len-1; i+=2)//把电话号码两两倒转
	{
		csTelNumOK += csTemp[i+1];
		csTelNumOK += csTemp[i];
	}

	// 发送内容转换
	CString csMessage = Content;
	BSTR bstr = csMessage.AllocSysString();
	CString csMsg;
	CopyBSTRtoString(csMsg, bstr);
	::SysFreeString(bstr);
	int nLen = csMsg.GetLength();
	if (nLen < 2)
		return FALSE;
	CString csMsgOK = _T("");
	USES_CONVERSION;
	for (int i = 0; i<nLen; i++)
	{
		char* pChar = (char*)(csMsg.GetBuffer(0) + i);
		csTemp.Format(_T("%02X"), BYTE(*pChar));
		csMsgOK += csTemp;
		csMsg.ReleaseBuffer(0);
	}

	if (m_nStep == NO_SEND)
	{
		if (m_MsgList.GetCount()>0)
		{
			BOOL bcom = SendToCom("AT+CMGF=0\xD", 10);
			m_nStep = SEND_SET;
			csLogInfo.Format(_T("QHJ: SendToCom (NO_SEND 1) : AT+CMGF=0  成功与否 %d"), bcom);
			//OutputDebugString(csLogInfo);
		}
	}
	if (m_nStep == SEND_SET)
	{
		if(m_Port.IfGetData())
		{
			int nLen = m_Port.Read(chData, 1000);
			for (int i = 0;i < nLen;i++)
			{
				if (chData[i]=='\0')
				{
					chData[i] = ' ';
				}
			}
			chData[nLen] = '\0';
			m_chPortData += chData;
			m_chPortData.MakeUpper();
			if (m_chPortData.IsEmpty())
			{
				m_nStep = NO_SEND;
				m_chPortData = "";
				return bSendComplete;
			}
			CString csPortData(m_chPortData);

			csPortData.Replace(_T("\r"), _T(" "));
			csPortData.Replace(_T("\n"), _T(" "));
			csLogInfo.Format(_T("QHJ: Read (SEND_SET 2) : %s"), csPortData);
			//OutputDebugString(csLogInfo);

			csPortData.MakeUpper();
			if (csPortData.Find(_T("OK"))!= -1 || csPortData.Find(_T(">")) != -1)
			{
#ifdef CHINATELECOM
				sprintf(chData, "AT+CMGS=%03d\xD", 56 + csMsgOK.GetLength() / 2);
#else
				sprintf(chData, "AT+CMGS=%03d\xD", 15 + csMsgOK.GetLength() / 2);
#endif

				BOOL bcom = SendToCom(chData, 12);

				CString csTemp(chData);
				csTemp.Replace(_T("\r"), _T(" "));
				csTemp.Replace(_T("\n"), _T(" "));
				csLogInfo.Format(_T("QHJ: SendToCom (SEND_SET 2): %s  成功与否 %d "), csTemp, bcom);
				//OutputDebugString(csLogInfo);

				m_nStep = SEND_LEN;
				m_chPortData = "";
			}
		}
	}
	if(m_nStep == SEND_LEN)
	{
		if(m_Port.IfGetData())
		{
			int nLen = m_Port.Read(chData, 1000);
			for (int i = 0;i < nLen;i++)
			{
				if (chData[i]=='\0')
				{
					chData[i] = ' ';
				}
			}
			chData[nLen] = '\0';
			CString tmp(chData);
			m_chPortData += chData;
			CString csPortData(m_chPortData);

			csPortData.Replace(_T("\r"), _T(" "));
			csPortData.Replace(_T("\n"), _T(" "));
			csLogInfo.Format(_T("QHJ: Read (SEND_LEN 3): %s"), csPortData);
			//OutputDebugString(csLogInfo);

			csPortData.MakeUpper();
#ifdef CHINATELECOM
			if (1)
#else
			if (csPortData.Find('>')!= -1)
#endif
			{
				USES_CONVERSION;
#ifdef CHINATELECOM
				sprintf(chData,"0b%s1001061720210106000000000100%02X%s\x1A\xD", T2A((TCHAR*)csTelNumOK.GetBuffer(0)), (csMsgOK).GetLength()/2, T2A((TCHAR*)csMsgOK.GetBuffer(0)));
				m_nSendLen = 56 + (lpMsg->m_csMsg).GetLength();
#else
				sprintf(chData,"0011000D9168%s000801%02X%s\x1A\xD", T2A((TCHAR*)csTelNumOK.GetBuffer(0)), (csMsgOK).GetLength()/2, T2A((TCHAR*)csMsgOK.GetBuffer(0)));
				m_nSendLen = 34 + (csMsgOK).GetLength();
#endif
				csTelNumOK.ReleaseBuffer();
				csMsgOK.ReleaseBuffer();
				BOOL bOk = SendToCom(chData, m_nSendLen);//发送的内容

				CString csTemp(chData);
				csTemp.Replace(_T("\r"), _T(" "));
				csTemp.Replace(_T("\n"), _T(" "));
				csLogInfo.Format(_T("QHJ: SendToCom (SEND_LEN 3): %s  成功与否 %d "), csTemp, bOk);
				//OutputDebugString(csLogInfo);

				m_nStep = SEND_DATA;
				m_chPortData = "";
			}
		}
	}
	if (m_nStep == SEND_DATA)
	{
		if(m_Port.IfGetData())
		{
			int nLen = m_Port.Read(chData, 1000);
			for (int i = 0;i < nLen;i++)
			{
				if (chData[i] == '\0')
				{
					chData[i] = ' ';
				}
			}
			m_chPortData += chData;
			CString cs1;
			cs1 = m_chPortData;

			cs1.Replace(_T("\r"), _T(" "));
			cs1.Replace(_T("\n"), _T(" "));
			csLogInfo.Format(_T("QHJ: Read (SEND_DATA 4): %s"), cs1);
			//OutputDebugString(csLogInfo);
			if (cs1.Find(_T("OK")) != -1)
			{
				bSendComplete = TRUE;
			}

			m_nStep = NO_SEND;
			m_chPortData = "";
		}
	}

	return bSendComplete;
}

void CLittleMsgSend::FilterPhoneNum(CStringA& SourPhone)
{
	// 去掉号码前的"+"
	if(SourPhone[0] == '+')
	{
		SourPhone = SourPhone.Mid(1);
	}
	//如果是特殊服务号码，不要加86
	if((SourPhone.GetLength()<=10)||(SourPhone.Left(3)==_T("106")))
	{
		//不要加86，如果是特服号码
	}
	else
	{
		// 在号码前加"86"
		if(SourPhone.Left(2) != "86")  
		{
			SourPhone.Insert(0,'8');
			SourPhone.Insert(1,'6');
		}
	}
}
void CLittleMsgSend::WriteSMSPLogFile(CString csLog)
{
	CString str = _T("C:\\短信报警记录");
	if (!PathIsDirectory(str))
	{
		::CreateDirectory(str, NULL);//创建目录,已有的话不影响
	}

	FILE*	pLogFile=NULL;
	CTime t=CTime::GetCurrentTime();
	char logfilename[512];
	memset(logfilename,0,sizeof(logfilename));
//	CString cstime = t.Format(IDS_LOGFILENAME);
//	CStringA cstimeA= cstime;
//	strcpy(logfilename,cstimeA);
	
//	pLogFile = fopen(logfilename,"a+t");

//	if(pLogFile==NULL)
//	{
//		return;
//	}
	
	CString smaplog;
	smaplog=_T("\n")+t.Format(_T("%Y-%m-%d %H:%M:%S"))+_T("\t")+csLog;
	CStringA smaplogA = smaplog;
	//写字符串
	fwrite(smaplogA.GetBuffer(0),sizeof(char),smaplogA.GetLength(),pLogFile);
	smaplogA.ReleaseBuffer();
	//关闭文件
	if(pLogFile)
	{
		fclose(pLogFile);
	}
}

int CLittleMsgSend::SendSMSMessage(char* f_PhoneNum,char* f_Content,int nClass,int nGroup,int nLevel)
{
	int nRet = 0;
	int l_iSmsNum=0;
	CStringA l_SmsBackContent=f_Content;
	CStringA l_PhoneNum=f_PhoneNum;
	FilterPhoneNum(l_PhoneNum);
	int iSmsBackLen=l_SmsBackContent.GetLength();
	int iPos=0;
	int ilen=0;
	int cindex=0;
	char l_BackSms[512];
	memset(l_BackSms,0,sizeof(l_BackSms));	
	for(int j=0;j<iSmsBackLen;j++)
	{
		TCHAR c;
		c=l_SmsBackContent.GetAt(j);
		l_BackSms[cindex]=c;
		cindex++;
		int ic=(int)c;
		if(ic>=0&&ic<=127)
		{
			ilen++;
		}
		iPos=(cindex-ilen)/2+ilen;
		if(iPos/70>=1)
		{
			l_iSmsNum++;
			//分段发送短信
			sprintf(l_BackSms,"%s",l_BackSms);
			nRet = SendLogSMS(l_PhoneNum.GetBuffer(0),l_BackSms,nClass, nGroup, nLevel);
			l_PhoneNum.ReleaseBuffer();
			memset(l_BackSms,0,sizeof(l_BackSms));
			ilen=0;
			cindex=0;
		}
	}
	//少于64，一条发送
	if(iPos<70)
	{
		sprintf(l_BackSms,"%s",l_BackSms);
		nRet = SendLogSMS(l_PhoneNum.GetBuffer(0),l_BackSms,nClass, nGroup, nLevel);
		l_PhoneNum.ReleaseBuffer();
	}
	
	m_dwTick = ::GetTickCount();

	return nRet;
}

BOOL CLittleMsgSend::IfTimeInterval(int nInterval)
{
	DWORD dwTick = ::GetTickCount();
	if(dwTick - m_dwTick < nInterval)
		return FALSE;
	m_dwTick = dwTick;
	return TRUE;
}

void CLittleMsgSend::OnTime()
{
	if (m_nConnect != 1)
	{
		g_bOneSendIsEndc = true;
		return;
	}
	if(m_MsgList.GetCount() == 0)
	{
		g_bOneSendIsEndc = true;		
		return;
	}
	if (m_bIsEnd && m_sendthread != NULL)
	{		
		UINT  exitcode = 0;
		AfxEndThread(exitcode);
	}
	CString csTemp;
//	csTemp.LoadString(IDS_PREPARESEND);
	WriteSMSPLogFile(csTemp);

	if(IfTimeInterval(10000))//10秒
	{
		g_bOneSendIsEndc = true;
		return;
	}
//	csTemp.LoadString(IDS_BEGINSEND);
	WriteSMSPLogFile(csTemp);
	//调用信号强度函数
	//Tw125CSQ();

    LITTLEMSG * lpMsg = (LITTLEMSG *)m_MsgList.GetHead();
	if (lpMsg->m_nSendNum >= 3)
	{
		//超时操作删除不发
		CString csTemp;
//		csTemp.Format(IDS_MORETIMEDEIETE,lpMsg->m_csMsg,lpMsg->m_csTelNumber,lpMsg->m_nAlarmLevel,lpMsg->m_nAlarmGroup,lpMsg->m_nAlarmClass);
		WriteSMSPLogFile(csTemp);
		
        EnterCriticalSection(&m_clock);
		lpMsg = (LITTLEMSG *)m_MsgList.RemoveHead();
		delete lpMsg;
		LeaveCriticalSection(&m_clock);	
		g_bOneSendIsEndc = true;
		// 是否需要原厂设置 ATZ 
		//BOOL bcom = SendToCom("ATZ\xD", 3);
		return;
	}

	CStringA csTelNumber = lpMsg->m_csTelNumber;
	CStringA csMsg = lpMsg->m_csMsg;

	int nRes = 0;
	
	EnterCriticalSection(&m_clock);
	int nCount = m_MsgList.GetCount();
	LeaveCriticalSection(&m_clock);
	if(nCount > 0)
	{
		for(int i = 0; i< 3 ; ++i)
		{
	        nRes = SendSMSMessage(csTelNumber.GetBuffer(csTelNumber.GetLength()),csMsg.GetBuffer(csMsg.GetLength()),lpMsg->m_nAlarmClass,lpMsg->m_nAlarmGroup,lpMsg->m_nAlarmLevel);
			csTelNumber.ReleaseBuffer();
			csMsg.ReleaseBuffer();
			Sleep(5000);
		}
		
		if (nRes == 1)
		{
			EnterCriticalSection(&m_clock);
			if(m_MsgList.GetCount() > 0)
			{						
				LITTLEMSG * lpMsghead = (LITTLEMSG *)m_MsgList.RemoveHead();
				delete lpMsghead;
			}
			LeaveCriticalSection(&m_clock);
		}				
		else
		{
			//不成功
			m_nConnect = 0;
			lpMsg->m_nSendNum++;
		}
	}			
	
	g_bOneSendIsEndc = true;
}
