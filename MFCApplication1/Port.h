// Port.h: interface for the CPort class.
//	������ ������ ����ɭ ������Ц���Ĵ���
//////////////////////////////////////////////////////////////////////
/************************ʹ��˵��**********************************
1�������򴮿�д����ֻ�����Send��������
2�������ݿ��Բ�ȡ���ַ���
	A����ѯʽ,�ڴ򿪴���Open�����д��ݴ��ھ��NULL��Ȼ�������ڵĵ���
		IfGetData��ѯ�Ƿ��յ������ݣ����յ�������ReadAndClear����
	B����Ϣʽ,�ڴ򿪴���Open�����д��������Ĵ��ھ����һ�������յ���
		���ݣ��ͻ���ô��ڷ�����ϢWM_HLS_GET_DATA,�ڴ����д������Ϣ
		����ReadAndClear�������ݼ���
*******************************************************************/
#if !defined(AFX_PORT_H__44FA13E1_6015_4D5D_83DD_F4673D4D1B27__INCLUDED_)
#define AFX_PORT_H__44FA13E1_6015_4D5D_83DD_F4673D4D1B27__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

 #include"afxwin.h"
enum PORT_JUGE
{
	NONE	= NOPARITY,		//��У��
	ODD		= ODDPARITY,	//��У��
	EVEN	= EVENPARITY	//żУ��
};

enum PORT_STOPBIT
{
	ONEBIT	= ONESTOPBIT,	//1λֹͣλ
	TWOBIT	= TWOSTOPBITS	//2λֹͣλ
};

const int PORT_BUF_LEN		= 2048;	//CPortʹ�õĻ���������󳤶�
const DWORD WM_HLS_GET_DATA	= WM_USER + 1000;	//���͸����ڵ���Ϣ

class CMutex;
class CPort : public CObject  
{
public:
	//�򿪴��ڣ�����������:���ڵľ��,���ں�,������,����λ,У��,ֹͣλ
//	BOOL Open(HWND Hwnd, int nPort, int nBaud, int nDataBit, PORT_JUGE juge, PORT_STOPBIT StopBit);		//��������
	BOOL Open(int nPort, int nBaud, int nDataBit, PORT_JUGE juge, PORT_STOPBIT StopBit);		//��������
	//�رմ���
	void Close();
	//�򴮿ڷ�������chData���ݴ�ŵ�ָ��dwDataLen���ݳ�
	BOOL Send(const char * chData, DWORD dwDataLen);
	//�ӻ�������ȡ���ݣ�����ջ�����pBuf��������ָ��nBufLen�������ĳ�
	int ReadAndClear(char * pBuf, int nBufLen);
	//�ӻ�������ȡ���ݣ�����ջ�����
	int Read(char * pBuf, int nBufLen);
	//�쿴���������ж���������
	int GetBufDataLen();
	//�Ƿ����յ����µ�����
	BOOL IfGetData();
	//��ջ�����
	void ClearBuf();
	CPort();
	virtual ~CPort();
private:
	DWORD ReadDataBlock(LPSTR lpszBlock, DWORD dwMaxLength,DWORD dwMinLength);
	BOOL SetupDCB(int nBaud, int nByteSize, PORT_JUGE juge, PORT_STOPBIT StopBit);			//����DCB
	void PutDataInBuf(const BYTE * chData, DWORD dwDataLen);
public:
	CMutex *m_pMutex;			//ͬ���¼�
private:
	HANDLE	m_hPortHandle;		//���ھ��
	BYTE	m_nPort;			//���ں�
	
	HANDLE  m_hThread;			//�����߳�
	DWORD   m_dwThreadID;		//�߳�ID
	BOOL	m_bConnected;		//�Ƿ�����״̬
	OVERLAPPED m_osWrite,m_osRead;//���������ص���дʹ�õ��¼�.��д�첽I/O����

	BYTE	m_pBuf[PORT_BUF_LEN];	//CPortʹ�õĻ�����
	int		m_nDataLen;				//�������Ѿ��е�����
	BOOL	m_bReadData;			//�ж��Ƿ��ֶ���������

// 	HWND	m_Hwnd;					//���ڵľ����һ���յ��������ͻ��򴰿ڷ���Ϣ

friend DWORD FAR PASCAL CommWatchProc( LPSTR lpData );
};

#endif // !defined(AFX_PORT_H__44FA13E1_6015_4D5D_83DD_F4673D4D1B27__INCLUDED_)
