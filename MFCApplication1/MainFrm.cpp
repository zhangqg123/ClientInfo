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

// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "MFCApplication1.h"

#include "MFCApplication1Doc.h"
#include "MFCApplication1View.h"
#include "MainFrm.h"

#include "client_info_db.h"
#include "string_oper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
    ON_COMMAND(ID_BUTTON_SCH, &CMainFrame::OnButtonSch)
    ON_COMMAND(ID_EDIT2, &CMainFrame::OnEdit2)
//    ON_COMMAND(ID_EDIT3, &CMainFrame::OnEdit3)
    ON_COMMAND(ID_EDIT4, &CMainFrame::OnEdit4)
//    ON_COMMAND(ID_EDIT5, &CMainFrame::OnEdit5)
//    ON_COMMAND(ID_COMBO2, &CMainFrame::OnCombo2)
//    ON_COMMAND(ID_COMBO3, &CMainFrame::OnCombo3)
//    ON_COMMAND(ID_EDIT7, &CMainFrame::OnEdit7)
//    ON_COMMAND(ID_COMBO4, &CMainFrame::OnCombo4)
END_MESSAGE_MAP()

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_OFF_2007_BLUE);
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;

    m_wndRibbonBar.Create(this);
    m_wndRibbonBar.LoadFromResource(IDR_RIBBON);
/*
    CMFCRibbonBaseElement *p_ele   = m_wndRibbonBar.FindByID(ID_COMBO2);
    CMFCRibbonComboBox    *p_combo = dynamic_cast<CMFCRibbonComboBox *>(p_ele);

    p_combo->AddItem(_T("是"));
    p_combo->AddItem(_T("否"));

    p_ele   = m_wndRibbonBar.FindByID(ID_COMBO4);
    p_combo = dynamic_cast<CMFCRibbonComboBox *>(p_ele);

    p_combo->AddItem(_T("公司客户"));
    p_combo->AddItem(_T("个人客户"));

    p_ele   = m_wndRibbonBar.FindByID(ID_COMBO3);
    p_combo = dynamic_cast<CMFCRibbonComboBox *>(p_ele);

    p_combo->AddItem(_T("是"));
    p_combo->AddItem(_T("否"));
*/
	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	CString strTitlePane1;
	CString strTitlePane2;
	bNameValid = strTitlePane1.LoadString(IDS_STATUS_PANE1);
	ASSERT(bNameValid);
	bNameValid = strTitlePane2.LoadString(IDS_STATUS_PANE2);
	ASSERT(bNameValid);
	m_wndStatusBar.AddElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE1, strTitlePane1, TRUE), strTitlePane1);
	m_wndStatusBar.AddExtendedElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE2, strTitlePane2, TRUE), strTitlePane2);

	// enable Visual Studio 2005 style docking window behavior
	CDockingManager::SetDockingMode(DT_SMART);
	// enable Visual Studio 2005 style docking window auto-hide behavior
	EnableAutoHidePanes(CBRS_ALIGN_ANY);
	// set the visual manager and style based on persisted value
	OnApplicationLook(theApp.m_nAppLook);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame message handlers

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_VS_2008:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_WINDOWS_7:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(TRUE);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
	}

	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}
/*
void CMainFrame::OnSearchGetShowCount(int &count)
{
    int nTotalShow = 0;
    CMFCRibbonBaseElement *p_ele  = m_wndRibbonBar.FindByID(ID_EDIT7);
    CMFCRibbonEdit        *p_edit = dynamic_cast<CMFCRibbonEdit *>(p_ele);

    CString TempStr   = p_edit->GetEditText();
    CString CtrlTitle = p_edit->GetText();

    if(TempStr.GetLength() && CtrlTitle == _T("条目"))
    {
        if(is_string_integer(TempStr))
        {
            nTotalShow = get_string_integer(TempStr);
            count = nTotalShow;
        }
        else
        {
            AfxMessageBox(_T("条目不是整数，将不考虑"));
        }
    }
}
*/
void CMainFrame::OnButtonSch()
{
    // TODO: Add your command handler code here
    CListCtrl &m_list = ((CMFCApplication1View *)GetActiveView())->GetListCtrl();

    int     nItem           = 0;
    int     nTotalShow      = 0xFFFF;
	short   nSubItem        = 0;
	short   nSubItemTotal   = m_list.GetHeaderCtrl()->GetItemCount();
    CString FilterStr;

	m_list.DeleteAllItems();

    Cclient_info_db cRecordset(&opr_db);

//    OnSearchGetShowCount(nTotalShow);
    OnSearchGetFilterString(FilterStr);

	try
	{
        cRecordset.m_strFilter = FilterStr;
		cRecordset.Open(CRecordset::snapshot, _T("jst_zc_sms"));

		CString cValue,str;

		while(!cRecordset.IsEOF() && nTotalShow--)
		{
			for(nSubItem = 0; nSubItem < nSubItemTotal; nSubItem++)
			{
				cRecordset.GetFieldValue(nSubItem, cValue);

				if(nSubItem == 0)
				{
					m_list.InsertItem(nItem, cValue.GetString());
				}
				else
				{
					if(cValue.GetLength())
					{
//                        if( nSubItem == DLG_BIRTH_DAY_INDEX    || nSubItem == DLG_CONTACT_DATE_INDEX )
                        if( nSubItem == DLG_BIRTH_DAY_INDEX )
                        {
                            CTime time;
                            if(StringToDateTime(cValue, time))
                            {
                                if(!isDateTimeDefault(time))
                                {
									m_list.SetItemText(nItem, nSubItem, cValue.GetString());
                                }
                            }
                        }
                        else
                        {
					//		ConvertUTF8ToANSI(cValue,str);
							m_list.SetItemText(nItem, nSubItem, cValue.GetString());
                        }
					}
				}
			}

			cRecordset.MoveNext();
			nItem++;
		}

		cRecordset.Close();
	} 
	catch(CDBException   *pE) 
	{ 
		pE-> ReportError(); 
		pE-> Delete(); 
	}
}


void CMainFrame::OnEdit2()
{
    // TODO: Add your command handler code here
}


//void CMainFrame::OnEdit3()
//{
    // TODO: Add your command handler code here
//}


void CMainFrame::OnEdit4()
{
    // TODO: Add your command handler code here
}


void CMainFrame::OnEdit1()
{
    // TODO: Add your command handler code here
}

/*
void CMainFrame::OnEdit5()
{
    // TODO: Add your command handler code here
}


void CMainFrame::OnCombo2()
{
    // TODO: Add your command handler code here
}


void CMainFrame::OnCombo3()
{
    // TODO: Add your command handler code here
}
*/

void CMainFrame::OnCombo1()
{
    // TODO: Add your command handler code here
}

/*
void CMainFrame::OnEdit7()
{
    // TODO: Add your command handler code here
}


void CMainFrame::OnCombo4()
{
    // TODO: Add your command handler code here
}

*/
int CMainFrame::OnSearchGetFilterString(CString &FilterStr)
{
    CString SubFilter;

    CMFCRibbonBaseElement *p_ele  = m_wndRibbonBar.FindByID(ID_EDIT2);
    CMFCRibbonEdit        *p_edit = dynamic_cast<CMFCRibbonEdit *>(p_ele);

    CString TempStr   = p_edit->GetEditText();
    CString CtrlTitle = p_edit->GetText();

    if(CtrlTitle == _T("编号"))
    {
        if(TempStr.GetLength())
        {
            SubFilter = _T("id = '") + TempStr + _T("'");

            if(FilterStr.GetLength())
            {
                FilterStr += _T(" && ");
            }

            FilterStr += SubFilter;
        }
    }

/*    p_ele  = m_wndRibbonBar.FindByID(ID_EDIT3);
    p_edit = dynamic_cast<CMFCRibbonEdit *>(p_ele);

    TempStr   = p_edit->GetEditText();
    CtrlTitle = p_edit->GetText();

    if(CtrlTitle == _T("出生月份"))
    {
        if(TempStr.GetLength())
        {
            SubFilter = _T("client_birth_month = '") + TempStr + _T("'");

            if(FilterStr.GetLength())
            {
                FilterStr += _T(" && ");
            }

            FilterStr += SubFilter;
        }
    }
*/
    p_ele  = m_wndRibbonBar.FindByID(ID_EDIT4);
    p_edit = dynamic_cast<CMFCRibbonEdit *>(p_ele);

    TempStr   = p_edit->GetEditText();
    CtrlTitle = p_edit->GetText();

    if(CtrlTitle == _T("联系电话"))
    {
        if(TempStr.GetLength())
        {
            SubFilter = _T("client_phone_num = '") + TempStr + _T("'");

            if(FilterStr.GetLength())
            {
                FilterStr += _T(" && ");
            }

            FilterStr += SubFilter;
        }
    }
/*
    p_ele  = m_wndRibbonBar.FindByID(ID_EDIT5);
    p_edit = dynamic_cast<CMFCRibbonEdit *>(p_ele);

    TempStr   = p_edit->GetEditText();
    CtrlTitle = p_edit->GetText();

    if(CtrlTitle == _T("报警内容"))
    {
        if(TempStr.GetLength())
        {
            SubFilter = _T("alarm_name = '") + TempStr + _T("'");

            if(FilterStr.GetLength())
            {
                FilterStr += _T(" && ");
            }

            FilterStr += SubFilter;
        }
    }

    p_ele  = m_wndRibbonBar.FindByID(ID_COMBO2);
    CMFCRibbonComboBox *p_combo = dynamic_cast<CMFCRibbonComboBox *>(p_ele);

    TempStr   = p_combo->GetEditText();
    CtrlTitle = p_combo->GetText();

    if(CtrlTitle == _T("沟通了否"))
    {
        if(TempStr.GetLength())
        {
            SubFilter = _T("client_contact_list = '") + TempStr + _T("'");

            if(FilterStr.GetLength())
            {
                FilterStr += _T(" && ");
            }

            FilterStr += SubFilter;
        }
    }

    p_ele   = m_wndRibbonBar.FindByID(ID_COMBO3);
    p_combo = dynamic_cast<CMFCRibbonComboBox *>(p_ele);

    TempStr   = p_combo->GetEditText();
    CtrlTitle = p_combo->GetText();

    if(CtrlTitle == _T("AGA客户"))
    {
        if(TempStr.GetLength())
        {
            SubFilter = _T("client_aga_or = '") + TempStr + _T("'");

            if(FilterStr.GetLength())
            {
                FilterStr += _T(" && ");
            }

            FilterStr += SubFilter;
        }
    }

    p_ele   = m_wndRibbonBar.FindByID(ID_COMBO4);
    p_combo = dynamic_cast<CMFCRibbonComboBox *>(p_ele);

    TempStr   = p_combo->GetEditText();
    CtrlTitle = p_combo->GetText();

    if(CtrlTitle == _T("客户类型"))
    {
        if(TempStr.GetLength())
        {
            SubFilter = _T("client_type = '") + TempStr + _T("'");

            if(FilterStr.GetLength())
            {
                FilterStr += _T(" && ");
            }

            FilterStr += SubFilter;
        }
    }
*/
    return 0;
}
