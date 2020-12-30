#if !defined(AFX_INIFILE_H__33CAF5C6_7820_43EB_8D84_42CC1856C35C__INCLUDED_) 
#define AFX_INIFILE_H__33CAF5C6_7820_43EB_8D84_42CC1856C35C__INCLUDED_ 

#if _MSC_VER > 1000 
#pragma once 
#endif // _MSC_VER > 1000 

class CIniFile   
{ 
public: 
	CIniFile(); 
	virtual ~CIniFile();

    void SetFileName(CString FileName); 
	void GetAllSectionName(CStringList &list);
	void SetString(CString Section, CString Item, CString Value); 
	void SetInt(CString Section, CString Item, int Value); 
    CString GetString(CString Section, CString Item); 
    int GetInt(CString Section, CString Item); 

private:
	
	void WriteFileString(CString Section, CString Item, CString Value); 
	CString GetFileString(CString Section, CString Item);  
	BOOL AcsiToUnicode(CString &str);
	int CharToUnicode(char *pchIn, CString &pstrOut);
    BOOL WriteIniFile(); 
    BOOL ReadIniFile(); //读取文件内容到数组中
    //ini file 
private:
    CString m_sFileName; 
    CStdioFile IniFile; 
    BOOL  bFileExist; 
    CStringArray FileContainer; 

}; 

#endif // !defined(AFX_INIFILE_H__33CAF5C6_7820_43EB_8D84_42CC1856C35C__INCLUDED_) 