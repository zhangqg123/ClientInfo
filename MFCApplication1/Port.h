// Port.h: interface for the CPort class.
//	串口类 编制者 何林森 参照徐笑军的代码
//////////////////////////////////////////////////////////////////////
/************************使用说明**********************************
1、此类向串口写数据只需调用Send函数即可
2、读数据可以采取两种方法
	A、查询式,在打开串口Open函数中传递窗口句柄NULL，然后再周期的调用
		IfGetData查询是否收到了数据，如收到，调用ReadAndClear读出
	B、消息式,在打开串口Open函数中传递真正的窗口句柄，一旦串口收到了
		数据，就会向该窗口发送消息WM_HLS_GET_DATA,在窗口中处理此消息
		调用ReadAndClear读出数据即可
*******************************************************************/
#if !defined(AFX_PORT_H__44FA13E1_6015_4D5D_83DD_F4673D4D1B27__INCLUDED_)
#define AFX_PORT_H__44FA13E1_6015_4D5D_83DD_F4673D4D1B27__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

 #include"afxwin.h"
enum PORT_JUGE
{
	NONE	= NOPARITY,		//无校验
	ODD		= ODDPARITY,	//奇校验
	EVEN	= EVENPARITY	//偶校验
};

enum PORT_STOPBIT
{
	ONEBIT	= ONESTOPBIT,	//1位停止位
	TWOBIT	= TWOSTOPBITS	//2位停止位
};

const int PORT_BUF_LEN		= 2048;	//CPort使用的缓冲区的最大长度
const DWORD WM_HLS_GET_DATA	= WM_USER + 1000;	//发送给窗口的消息

class CMutex;
class CPort : public CObject  
{
public:
	//打开串口，参数依次是:窗口的句柄,串口号,波特率,数据位,校验,停止位
//	BOOL Open(HWND Hwnd, int nPort, int nBaud, int nDataBit, PORT_JUGE juge, PORT_STOPBIT StopBit);		//建立连接
	BOOL Open(int nPort, int nBaud, int nDataBit, PORT_JUGE juge, PORT_STOPBIT StopBit);		//建立连接
	//关闭串口
	void Close();
	//向串口发送数据chData数据存放的指针dwDataLen数据长
	BOOL Send(const char * chData, DWORD dwDataLen);
	//从缓冲区读取数据，并清空缓冲区pBuf缓冲区的指针nBufLen缓冲区的长
	int ReadAndClear(char * pBuf, int nBufLen);
	//从缓冲区读取数据，不清空缓冲区
	int Read(char * pBuf, int nBufLen);
	//察看缓冲区中有多少数据了
	int GetBufDataLen();
	//是否又收到了新的数据
	BOOL IfGetData();
	//清空缓冲区
	void ClearBuf();
	CPort();
	virtual ~CPort();
private:
	DWORD ReadDataBlock(LPSTR lpszBlock, DWORD dwMaxLength,DWORD dwMinLength);
	BOOL SetupDCB(int nBaud, int nByteSize, PORT_JUGE juge, PORT_STOPBIT StopBit);			//设置DCB
	void PutDataInBuf(const BYTE * chData, DWORD dwDataLen);
public:
	CMutex *m_pMutex;			//同步事件
private:
	HANDLE	m_hPortHandle;		//串口句柄
	BYTE	m_nPort;			//串口号
	
	HANDLE  m_hThread;			//监听线程
	DWORD   m_dwThreadID;		//线程ID
	BOOL	m_bConnected;		//是否连接状态
	OVERLAPPED m_osWrite,m_osRead;//监听进程重叠读写使用的事件.读写异步I/O操作

	BYTE	m_pBuf[PORT_BUF_LEN];	//CPort使用的缓冲区
	int		m_nDataLen;				//缓冲中已经有的数据
	BOOL	m_bReadData;			//判断是否又读入了数据

// 	HWND	m_Hwnd;					//窗口的句柄，一旦收到数据他就会向窗口发消息

friend DWORD FAR PASCAL CommWatchProc( LPSTR lpData );
};

#endif // !defined(AFX_PORT_H__44FA13E1_6015_4D5D_83DD_F4673D4D1B27__INCLUDED_)
