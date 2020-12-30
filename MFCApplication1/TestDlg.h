// TestDlg.h : header file

#pragma once


/////////////////////////////////////////////////////////////////////////////
// CTestDlg dialog

class CTestDlg 
{
// Construction
public:
	CTestDlg();	// standard constructor

	int		m_intPort;
	CString	m_strSMSCon;
	CString	m_strSMSTel;
	CString	m_strIndex;
    HINSTANCE hinstDLL; 
	


// Implementation
public:

	// Generated message map functions
	//{{AFX_MSG(CTestDlg)
	void OnBtest();
	void OnButton1();
	void OnButton2();
	void OnConnection();
	void OnDisconnectionButton();
	void OnSendButton();
	void OnReceiveButton();
	void OnDeleteButton();
	virtual void OnOK();
	//}}AFX_MSG
};

