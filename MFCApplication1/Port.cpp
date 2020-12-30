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
	m_hPortHandle		= (HANDLE)-1;	//�豸����ÿ�
	m_bConnected		= FALSE;		//δ����
	m_dwThreadID		= 0;			//�����߳�ID
	m_hThread			= NULL;			//�߳̾����
	m_nPort				= 1;
	m_nDataLen			= 0;
	m_bReadData			= FALSE;

	m_osWrite.Offset	= 0;			//�첽����ṹ��Ա-�ļ��ֽ�ƫ����
	m_osWrite.OffsetHigh= 0;			//�첽����ṹ��Ա-�ļ���λ�ֽ�ƫ����
	m_osRead.hEvent		= NULL;
	m_osRead.Offset		= 0;			//�첽����ṹ��Ա-�ļ��ֽ�ƫ����
	m_osRead.OffsetHigh	= 0;			//�첽����ṹ��Ա-�ļ��ֽڸ�λƫ����
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

   	SetCommMask(m_hPortHandle, 0 ) ;			//ȡ���¼�֪ͨ,�ȴ������߳�
	EscapeCommFunction(m_hPortHandle,CLRDTR ) ;	//���DTR(�����ն˾���)�ź�
	PurgeComm(m_hPortHandle,	PURGE_TXABORT	//��ֹ����д,��ʹδ���
		| PURGE_RXABORT							//��ֹ���ж�,��ʹδ���
		|PURGE_TXCLEAR							//�����������
		| PURGE_RXCLEAR							//�����뻺����
		) ;
	CloseHandle(m_hPortHandle) ;				//�ر��豸���(���ھ��)
	m_hPortHandle = HANDLE(-1);
	m_bConnected = FALSE;						//�������ӱ�־Ϊ"δ����"
	
	//�̱߳�����ǰ��������
	for(int i=0; i<100; i++)
	{
		if(m_dwThreadID == 0)	//�ȴ������߳��˳�
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
	m_bConnected = TRUE;								//���ӱ�־
	m_nPort = nPort;
	COMMTIMEOUTS  CommTimeOuts ;						//��ʱ�ṹ

	if(m_hPortHandle != HANDLE(-1))  // �Ѿ���������״̬��ֱ���˳�
	{
		return FALSE;
	}
	//ΪOVERLAPPED�ṹ���첽���佨��һ���¼�
	m_osRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL); //�����¼�
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

	//����һ���������
	TCHAR str[16];						//��Ż���������ֵĴ�
	_stprintf_s(str,16,_T("HLS_MUTEX%d"),m_nPort);//�����������Ϊmutex+�˿ں�
	m_pMutex = new CMutex(FALSE,str);	//����һ���������

	TCHAR szPort[16];
	_stprintf_s(szPort,16, _T("\\\\.\\COM%d"),m_nPort);
	// open COMM device
	m_hPortHandle =	CreateFile( szPort, GENERIC_READ | GENERIC_WRITE,
		0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
		NULL);
	if(m_hPortHandle == INVALID_HANDLE_VALUE ) //����COM�ļ�ʧ��
	{
	//	DWORD dword = GetLastError();
		Close();
		return FALSE;
	}
	else
	{
		//����windows������յ�һ���ַ����������뻺�������¼�.
		SetCommMask(m_hPortHandle, EV_RXCHAR) ;
		// ����ϵͳ������
		SetupComm(m_hPortHandle, 4096, 1024) ;
		// ��ջ������е��κ���Ϣ
		PurgeComm(m_hPortHandle, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR ) ;
	
		memset(&CommTimeOuts, 0, sizeof(COMMTIMEOUTS));
		CommTimeOuts.ReadIntervalTimeout = MAXDWORD;
		CommTimeOuts.ReadTotalTimeoutConstant = MAXDWORD;
		CommTimeOuts.ReadTotalTimeoutMultiplier = MAXDWORD;
		//���ó�ʱ.
		SetCommTimeouts(m_hPortHandle, &CommTimeOuts );
	}
	if(!SetupDCB(nBaud, nDataBit, juge, StopBit))
	{
		Close();
		return FALSE;
	}
	// ����һ�������߳��Եȴ�ָ���¼��ķ���
	DWORD dwThreadID;
	HANDLE hThread =
		CreateThread( (LPSECURITY_ATTRIBUTES) NULL,	//ȱʡ��ȫ����
		0,											//��ʼջ�ߴ�,Ϊ0�Զ�����ȱʡֵ
		(LPTHREAD_START_ROUTINE) CommWatchProc,		//ִ���̵߳ĺ����׵�ַ
		(LPVOID)this,								//ָ�����ݸ��̵߳�32λ����ֵ
		0,											//��־,0��ʾ�߳̽���������ִ�� 
		&dwThreadID);								//�����߳�ID

	if(hThread == NULL)
	{
		Close();
		return FALSE;
	}
	
	m_dwThreadID = dwThreadID;		//�����߳�IDW
	m_hThread  = hThread;			//�����߳̾��
	
	EscapeCommFunction(m_hPortHandle,SETDTR );	//����DTR(�豸�ն˾���)�ź�

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
	//ע��:���µĴ�����ٱ�ִ��,��Ϊ��������cacheд����,�Ƚ�
	//С��I/O����(С�ڼ�ǧ�ֽ�)�����������,��������true,��ʹ
	//overlapped������ָ��Ҳ������.
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
					// δ���,����д�ֽ���
					dwBytesSent += dwBytesWritten;
					continue;
				}
				else// ������
				{
					ClearCommError(m_hPortHandle, &dwErrorFlags, &ComStat ) ;
					break;
				}
			}
			dwBytesSent += dwBytesWritten;
		}
		else
		{
			// ������������
			ClearCommError(m_hPortHandle, &dwErrorFlags, &ComStat ) ;
			return FALSE;
		}
	}
	return TRUE;
}

DWORD CPort::ReadDataBlock(LPSTR lpszBlock, DWORD dwMaxLength, DWORD dwMinLength)
{
	BOOL       fReadStat ;
	COMSTAT    ComStat ;			//���ڱ����豸״̬�Ľṹ
	DWORD      dwErrorFlags;
	DWORD      dwLength;
	DWORD      dwError;
	ClearCommError(m_hPortHandle, &dwErrorFlags, &ComStat ) ;
	dwLength = min(dwMaxLength, ComStat.cbInQue) ;
	if(dwLength > dwMinLength)
	{
		fReadStat = ReadFile(m_hPortHandle,lpszBlock,							//���뻺����ָ��
			dwLength,							//�趨���� 
			&dwLength,							//ʵ�ʳ��� 
			&m_osRead ) ;						//OVERLAPPED�ṹָ��
		if (!fReadStat)							//�ɹ�
		{
			if(GetLastError() == ERROR_IO_PENDING)
			{
				//�첽������Ȼ�ڽ���,Ӧ�ʵ���ʱ�Եȴ�
				while(!GetOverlappedResult(m_hPortHandle,&m_osRead,&dwLength,TRUE))
				{
					dwError = GetLastError();
					if(dwError == ERROR_IO_INCOMPLETE)//����δ���
						continue;
					else// �����ط����˴���,����֮
					{
						ClearCommError(m_hPortHandle,&dwErrorFlags, &ComStat ) ;
						break;
					}
				}
			}
			else//ʧ��
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
// 		SendMessage(m_Hwnd, WM_HLS_GET_DATA, 0, 0);//�򴰿ڷ�����Ϣ

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

DWORD FAR PASCAL CommWatchProc( LPSTR lpData )	//��������
{
	DWORD       dwEvtMask ;						//�¼�����
	CPort* pPort=(CPort*)lpData;				//CPortָ��
	OVERLAPPED  os ;							//�ص��ṹ
	BYTE		abIn[MAXBLOCK + 1] ;			//���ջ�����
	DWORD       dwLength;						//���յ������ݳ���
	memset( &os, 0, sizeof( OVERLAPPED ) ) ;
	os.hEvent = CreateEvent( NULL,TRUE,	FALSE,NULL ) ;								// no name
	if(os.hEvent == NULL)
	{
		return FALSE;
	}
	//�յ�һ���ַ����������뻺������
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
