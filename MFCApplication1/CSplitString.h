// CSplitString.h : main header file for the CSPLITSTRING application
//

#if !defined(AFX_CSPLITSTRING_H__0E2F056F_5AC4_4F19_A60C_201C9332A0AE__INCLUDED_)
#define AFX_CSPLITSTRING_H__0E2F056F_5AC4_4F19_A60C_201C9332A0AE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CCSplitStringApp:
// See CSplitString.cpp for the implementation of this class
//

class CCSplitStringApp : public CWinApp
{
public:
	CCSplitStringApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCSplitStringApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CCSplitStringApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CSPLITSTRING_H__0E2F056F_5AC4_4F19_A60C_201C9332A0AE__INCLUDED_)
