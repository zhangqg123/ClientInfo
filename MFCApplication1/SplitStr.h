// SplitStr.h: interface for the CSplitStr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPLITSTR_H__6795A4EF_A407_411B_942E_9521EF8A480D__INCLUDED_)
#define AFX_SPLITSTR_H__6795A4EF_A407_411B_942E_9521EF8A480D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSplitStr  
{
private:
	BOOL    m_bSequenceAsOne;
	CString m_sData;
    CString m_sSplitFlag;
public:
	void  GetSplitStrArray(CStringArray &array);
	CString GetData();
	void SetData(CString sData);
	BOOL GetSequenceAsOne()  {return m_bSequenceAsOne;};
	void SetSequenceAsOne(BOOL bSequenceAsOne) {m_bSequenceAsOne = bSequenceAsOne;};

	CString GetSplitFlag()  {return m_sSplitFlag;};
	void SetSplitFlag(CString sSplitFlag) {m_sSplitFlag = sSplitFlag;};

	CSplitStr();
	virtual ~CSplitStr();

};

#endif // !defined(AFX_SPLITSTR_H__6795A4EF_A407_411B_942E_9521EF8A480D__INCLUDED_)
