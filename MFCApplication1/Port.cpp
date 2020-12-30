// Port.cpp: implementation of the CPort class.
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
// #include"afxwin.h"
#include <afxmt.h>

#include "Port.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

const int  MAXBLOCK			= 80;
DWORD FAR PASCAL CommWatchProc( LPSTR lpData );
///////////////////////////////////////////////////////////
CPort::CPort()
{		
	m_hPortHandle		= (HANDLE)-1;	//设备句柄置空
	m_bConnected		= FALSE;		//未连接
	m_dwThreadID		= 0;			//监听线程ID
	m_hThread			= NULL;			//线程句柄空
	m_nPort				= 1;
	m_nDataLen			= 0;
	m_bReadData			= FALSE;

	m_osWrite.Offset	= 0;			//异步传输结构成员-文件字节偏移量
	m_osWrite.OffsetHigh= 0;			//异步传输结构成员-文件高位字节偏移量
	m_osRead.hEvent		= NULL;
	m_osRead.Offset		= 0;			//异步传输结构成员-文件字节偏移量
	m_osRead.OffsetHigh	= 0;			//异步传输结构成员-文件字节高位偏移量
	m_osWrite.hEvent	= NULL;
//	m_Hwnd				= NULL;
}

CPort::~CPort()
{
	Close();
}

void CPort::Close()
{
	if(!m_bConnected)
	{
		return;
	}

   	SetCommMask(m_hPortHandle, 0 ) ;			//取消事件通知,等待结束线程
	EscapeCommFunction(m_hPortHandle,CLRDTR ) ;	//清除DTR(数据终端就绪)信号
	PurgeComm(m_hPortHandle,	PURGE_TXABORT	//终止所有写,即使未完成
		| PURGE_RXABORT							//终止所有读,即使未完成
		|PURGE_TXCLEAR							//清输出缓冲区
		| PURGE_RXCLEAR							//清输入缓冲区
		) ;
	CloseHandle(m_hPortHandle) ;				//关闭设备句柄(串口句柄)
	m_hPortHandle = HANDLE(-1);
	m_bConnected = FALSE;						//设置连接标志为"未连接"
	
	//线程被结束前阻塞程序
	for(int i=0; i<100; i++)
	{
		if(m_dwThreadID == 0)	//等待接受线程退出
			break;
		Sleep(10);
	}
	if(m_dwThreadID == 0)
	{
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}
	else
	{
		TerminateThread(m_hThread,0xFFFFFFFF);
		m_dwThreadID = 0;
		m_hThread = NULL;
	}

	if(m_osRead.hEvent != NULL)
	{
		CloseHandle(m_osRead.hEvent);
		m_osRead.hEvent = NULL;
	}
	if(m_osWrite.hEvent != NULL)
	{
		CloseHandle(m_osWrite.hEvent);
		m_osWrite.hEvent = NULL;
	}
	if(m_pMutex != NULL)
	{
		delete m_pMutex;
		m_pMutex = NULL;
	}
}

BOOL CPort::Open(/*HWND Hwnd, */int nPort, int nBaud, int nDataBit, PORT_JUGE juge, PORT_STOPBIT StopBit)
{
	if(nPort < 0 || nPort > 64)
		return FALSE;
/*	m_Hwnd = Hwnd;*/
	m_bConnected = TRUE;								//连接标志
	m_nPort = nPort;
	COMMTIMEOUTS  CommTimeOuts ;						//超时结构

	if(m_hPortHandle != HANDLE(-1))  // 已经处于连接状态，直接退出
	{
		return FALSE;
	}
	//为OVERLAPPED结构的异步传输建立一个事件
	m_osRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL); //无名事件
	if(m_osRead.hEvent == NULL)
	{
		return FALSE;
	}
	else
	{
		m_osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		if(m_osWrite.hEvent == NULL)
		{
			Close();
			return FALSE;
		}
	}

	//建立一个互斥对象
	TCHAR str[16];						//存放互斥对象名字的串
	_stprintf_s(str,16,_T("HLS_MUTEX%d"),m_nPort);//互斥对象名字为mutex+端口号
	m_pMutex = new CMutex(FALSE,str);	//建立一个互斥对象

	TCHAR szPort[16];
	_stprintf_s(szPort,16, _T("\\\\.\\COM%d"),m_nPort);
	// open COMM device
	m_hPortHandle =	CreateFile( szPort, GENERIC_READ | GENERIC_WRITE,
		0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
		NULL);
	if(m_hPortHandle == INVALID_HANDLE_VALUE ) //创建COM文件失败
	{
	//	DWORD dword = GetLastError();
		Close();
		return FALSE;
	}
	else
	{
		//设置windows报告接收的一个字符并放入输入缓冲区的事件.
		SetCommMask(m_hPortHandle, EV_RXCHAR) ;
		// 设置系统缓冲区
		SetupComm(m_hPortHandle, 4096, 1024) ;
		// 清空缓冲区中的任何信息
		PurgeComm(m_hPortHandle, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR ) ;
	
		memset(&CommTimeOuts, 0, sizeof(COMMTIMEOUTS));
		CommTimeOuts.ReadIntervalTimeout = MAXDWORD;
		CommTimeOuts.ReadTotalTimeoutConstant = MAXDWORD;
		CommTimeOuts.ReadTotalTimeoutMultiplier = MAXDWORD;
		//设置超时.
		SetCommTimeouts(m_hPortHandle, &CommTimeOuts );
	}
	if(!SetupDCB(nBaud, nDataBit, juge, StopBit))
	{
		Close();
		return FALSE;
	}
	// 建立一个辅助线程以等待指定事件的发生
	DWORD dwThreadID;
	HANDLE hThread =
		CreateThread( (LPSECURITY_ATTRIBUTES) NULL,	//缺省安全属性
		0,											//初始栈尺寸,为0自动分配缺省值
		(LPTHREAD_START_ROUTINE) CommWatchProc,		//执行线程的函数首地址
		(LPVOID)this,								//指定传递给线程的32位参数值
		0,											//标志,0表示线程建立后立即执行 
		&dwThreadID);								//返回线程ID

	if(hThread == NULL)
	{
		Close();
		return FALSE;
	}
	
	m_dwThreadID = dwThreadID;		//辅助线程IDW
	m_hThread  = hThread;			//辅助线程句柄
	
	EscapeCommFunction(m_hPortHandle,SETDTR );	//发送DTR(设备终端就绪)信号

	return TRUE;
}

BOOL CPort::SetupDCB(int nBaud, int nByteSize, PORT_JUGE juge, PORT_STOPBIT StopBit)
{
	DCB        dcb ;
	dcb.DCBlength		= sizeof( DCB ) ;
	GetCommState( m_hPortHandle, &dcb ) ;

	dcb.BaudRate		= nBaud;
	dcb.ByteSize		= nByteSize;
	dcb.Parity			= juge;
	dcb.StopBits		= StopBit;
	
	dcb.fOutxDsrFlow	= FALSE;
	dcb.fDtrControl		= DTR_CONTROL_ENABLE ;
	dcb.fOutxCtsFlow	= FALSE;
	dcb.fRtsControl		= RTS_CONTROL_ENABLE ;
	dcb.fBinary			= TRUE ;
	dcb.fParity			= TRUE ;

	return SetCommState(m_hPortHandle,&dcb);
}

BOOL CPort::Send(const char * chData, DWORD dwDataLen)
{
	if(m_hPortHandle==(HANDLE)-1)
		return FALSE;
	BOOL        fWriteStat ;
	DWORD       dwBytesWritten ;
	DWORD       dwErrorFlags;
	DWORD		dwError;
	DWORD       dwBytesSent=0;
	COMSTAT     ComStat;
	fWriteStat = WriteFile( m_hPortHandle, chData, dwDataLen,
		&dwBytesWritten, &m_osWrite) ;
	//注意:以下的代码很少被执行,因为驱动器的cache写操作,比较
	//小的I/O操作(小于几千字节)可以立刻完成,函数返回true,即使
	//overlapped操作被指定也是这样.
	if (!fWriteStat)
	{
		if(GetLastError() == ERROR_IO_PENDING)
		{	
			while(!GetOverlappedResult(m_hPortHandle,
				&m_osWrite, &dwBytesWritten, TRUE ))
			{
				dwError = GetLastError();
				if(dwError == ERROR_IO_INCOMPLETE)
				{
					// 未完成,修正写字节数
					dwBytesSent += dwBytesWritten;
					continue;
				}
				else// 错误发生
				{
					ClearCommError(m_hPortHandle, &dwErrorFlags, &ComStat ) ;
					break;
				}
			}
			dwBytesSent += dwBytesWritten;
		}
		else
		{
			// 发生其他错误
			ClearCommError(m_hPortHandle, &dwErrorFlags, &ComStat ) ;
			return FALSE;
		}
	}
	return TRUE;
}

DWORD CPort::ReadDataBlock(LPSTR lpszBlock, DWORD dwMaxLength, DWORD dwMinLength)
{
	BOOL       fReadStat ;
	COMSTAT    ComStat ;			//用于保存设备状态的结构
	DWORD      dwErrorFlags;
	DWORD      dwLength;
	DWORD      dwError;
	ClearCommError(m_hPortHandle, &dwErrorFlags, &ComStat ) ;
	dwLength = min(dwMaxLength, ComStat.cbInQue) ;
	if(dwLength > dwMinLength)
	{
		fReadStat = ReadFile(m_hPortHandle,lpszBlock,							//输入缓冲区指针
			dwLength,							//设定长度 
			&dwLength,							//实际长度 
			&m_osRead ) ;						//OVERLAPPED结构指针
		if (!fReadStat)							//成功
		{
			if(GetLastError() == ERROR_IO_PENDING)
			{
				//异步传输仍然在进行,应适当延时以等待
				while(!GetOverlappedResult(m_hPortHandle,&m_osRead,&dwLength,TRUE))
				{
					dwError = GetLastError();
					if(dwError == ERROR_IO_INCOMPLETE)//传输未完成
						continue;
					else// 真正地发生了错误,处理之
					{
						ClearCommError(m_hPortHandle,&dwErrorFlags, &ComStat ) ;
						break;
					}
				}
			}
			else//失败
			{
				dwLength = 0 ;
				ClearCommError(m_hPortHandle, &dwErrorFlags, &ComStat ) ;
			}
		}
	}
	else
		dwLength = 0;
	return dwLength;
}

void CPort::PutDataInBuf(const BYTE * chData, DWORD dwDataLen)
{
	CSingleLock lock(m_pMutex);
	lock.Lock();
	if(dwDataLen > 0)
	{
		m_bReadData = TRUE;
		if(m_nDataLen + dwDataLen < PORT_BUF_LEN)
		{
			memcpy(m_pBuf+m_nDataLen, chData, dwDataLen);
			m_nDataLen += dwDataLen;
		}
		else if(m_nDataLen < PORT_BUF_LEN - 1)
		{
			memcpy(m_pBuf+m_nDataLen, chData, PORT_BUF_LEN-m_nDataLen-1);
			m_nDataLen = PORT_BUF_LEN - 1;
		}
	}
	lock.Unlock();
// 	if(m_Hwnd != NULL)
// 		SendMessage(m_Hwnd, WM_HLS_GET_DATA, 0, 0);//向窗口发送消息

}

int CPort::GetBufDataLen()
{
	int nLen;
	CSingleLock lock(m_pMutex);
	lock.Lock();
	nLen = m_nDataLen;
	lock.Unlock();
	return nLen;
}
int CPort::ReadAndClear(char * pBuf, int nBufLen)
{
	m_bReadData = FALSE;
	int nReadLen = 0;
	CSingleLock lock(m_pMutex);
	lock.Lock();

	if(m_nDataLen <= nBufLen)
	{
		memcpy(pBuf, m_pBuf, m_nDataLen);
		nReadLen = m_nDataLen;
		m_nDataLen = 0;
	}
	else
	{
		memcpy(pBuf, m_pBuf, nBufLen);
		nReadLen = nBufLen;
		memcpy(m_pBuf, m_pBuf+nBufLen, m_nDataLen - nBufLen);
		m_nDataLen -= nBufLen;
	}

	lock.Unlock();
	return nReadLen;
}

int CPort::Read(char * pBuf, int nBufLen)
{
	m_bReadData = FALSE;
	int nReadLen = 0;
	CSingleLock lock(m_pMutex);
	lock.Lock();

	if(m_nDataLen <= nBufLen)
	{
		memcpy(pBuf, m_pBuf, m_nDataLen);
		nReadLen = m_nDataLen;
	}
	else
	{
		memcpy(pBuf, m_pBuf, nBufLen);
		nReadLen = nBufLen;
	}

	lock.Unlock();
	return nReadLen;
}

BOOL CPort::IfGetData()
{
	BOOL bGet;
	CSingleLock lock(m_pMutex);
	lock.Lock();
	bGet = m_bReadData;
	m_bReadData = FALSE;
	lock.Unlock();
	return bGet;
}

void CPort::ClearBuf()
{
	CSingleLock lock(m_pMutex);
	lock.Lock();
	m_nDataLen = 0;
	lock.Unlock();
}

DWORD FAR PASCAL CommWatchProc( LPSTR lpData )	//监听进程
{
	DWORD       dwEvtMask ;						//事件掩码
	CPort* pPort=(CPort*)lpData;				//CPort指针
	OVERLAPPED  os ;							//重叠结构
	BYTE		abIn[MAXBLOCK + 1] ;			//接收缓冲区
	DWORD       dwLength;						//接收到的数据长度
	memset( &os, 0, sizeof( OVERLAPPED ) ) ;
	os.hEvent = CreateEvent( NULL,TRUE,	FALSE,NULL ) ;								// no name
	if(os.hEvent == NULL)
	{
		return FALSE;
	}
	//收到一个字符并置于输入缓冲区中
	if (!SetCommMask( pPort->m_hPortHandle, EV_RXCHAR ))
		return FALSE;
	while(pPort->m_bConnected)
	{
		dwEvtMask = 0;
		WaitCommEvent(pPort->m_hPortHandle , &dwEvtMask, NULL );

		if((dwEvtMask & EV_RXCHAR) == EV_RXCHAR)
		{
			do
			{
				dwLength = pPort->ReadDataBlock((LPSTR)abIn, MAXBLOCK,0);
				if(dwLength > 0)
				{
					pPort->PutDataInBuf(abIn, dwLength);
				}
			}while(dwLength > 0);
		}
	}
	CloseHandle(os.hEvent);
	pPort->m_dwThreadID  = 0;
	return TRUE;
} // end of CommWatchProc()
