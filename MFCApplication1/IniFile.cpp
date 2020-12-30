#include "stdafx.h" 
#include "IniFile.h" 
#include "locale.h"
#ifdef _DEBUG 
#undef THIS_FILE 
static char THIS_FILE[]=__FILE__; 
#define new DEBUG_NEW 
#endif 

////////////////////////////////////////////////////////////////////// 
// Construction/Destruction 
////////////////////////////////////////////////////////////////////// 

CIniFile::CIniFile() 
{ 
    m_sFileName = "ini.ini"; 
	//FileContainer.SetSize(100,1);
} 

CIniFile::~CIniFile() 
{ 
    if(FileContainer.GetSize() > 0) 
    { 
        FileContainer.RemoveAll(); 
    } 
} 

//设置文件全路径
void CIniFile::SetFileName(CString FileName) 
{ 
    m_sFileName = FileName; 
} 

void CIniFile::SetInt(CString Section, CString Item, int Value) 
{ 
    CString strtemp; 
	strtemp.Format("%d",Value); 
    //保存到FileContainer 
    WriteFileString(Section, Item, strtemp); 
    //保存到ini文件 
    WriteIniFile(); 
} 

void CIniFile::GetAllSectionName(CStringList &list)
{
	CString csLine;
	bFileExist = IniFile.Open(m_sFileName,CFile::modeRead);
	if(!bFileExist)
	{
		return;
	}
	while(IniFile.ReadString(csLine))
	{
		if(csLine[0] != '[' || csLine[csLine.GetLength()-1] != ']')
		{
			continue;
		}
#ifdef UNICODE
		AcsiToUnicode(csLine);
#endif
		csLine = csLine.Left(csLine.Find(_T("]")));//去掉]右边 
		csLine = csLine.Right(csLine.GetLength() - csLine.Find("[")-1);//去掉[左边 
		
		if(list.IsEmpty())
		{
			list.AddHead(csLine);
		}
		else
		{
			list.AddTail(csLine);
		}
	}
	IniFile.Close();
}

void CIniFile::SetString(CString Section, CString Item, CString Value) 
{ 
    //保存到FileContainer 
    WriteFileString(Section, Item, Value); 
    //保存到ini文件 
    WriteIniFile(); 
} 

CString CIniFile::GetString(CString Section, CString Item) 
{ 
    return GetFileString(Section, Item); 
} 

int CIniFile::GetInt(CString Section, CString Item) 
{ 
   //CString strtemp; 
   // strtemp.Format("%d",Value); 
	//  return atoi(GetFileString(Section, Item, strtemp)); 
    return _ttoi(GetFileString(Section, Item)); 
} 

BOOL CIniFile::ReadIniFile() 
{ 
	bFileExist = IniFile.Open(m_sFileName, CFile::modeRead); 

	if(!bFileExist) 
	{ 
		return FALSE; 
	} 

	CString strline; 
	FileContainer.RemoveAll(); 
	
    //将IniFile文件数据读到FileContainer 
	while(IniFile.ReadString(strline)) 
	{ 
#ifdef _UNICODE
		AcsiToUnicode(strline);
#endif
		FileContainer.Add(strline); 
	} 

    IniFile.Close(); 
    return TRUE; 
} 

BOOL CIniFile::WriteIniFile() 
{ 
    bFileExist = IniFile.Open(m_sFileName, CFile::modeCreate | CFile::modeWrite); //|CFile::modeNoTruncate
	if(!bFileExist) 
		return FALSE; 
	//IniFile.SeekToEnd();
    CString strline; 

    //将FileContainer写到IniFile文件 
    for(int i = 0; i< FileContainer.GetSize(); i++) 
    { 
        strline = FileContainer[i]; 
#ifdef UNICODE
		char* old_locale = _strdup( setlocale(LC_CTYPE,NULL) );
		setlocale( LC_CTYPE, "chs");//设定
		IniFile.WriteString(strline + L"\n");//正常写入
		setlocale( LC_CTYPE, old_locale );
		free( old_locale );//还原区域设定 
#else
		IniFile.WriteString(strline + L"\n");
#endif
		 
    } 

    IniFile.Close(); 

    return bFileExist; 
} 


CString CIniFile::GetFileString(CString Section, CString Item) 
{ 
	CString csValue = _T("0");

    bFileExist = ReadIniFile();//打开文件 
    if(bFileExist == FALSE) 
        return csValue;//文件打开出错或文件为空，返回默认值 
	int i = 0; 
	int iFileLines = FileContainer.GetSize(); 
	CString strline,str; 

    while(i < iFileLines) 
    { 
        strline = FileContainer.GetAt(i++); 

        strline.TrimLeft(); 
        if(strline.GetAt(0)=='[')//查找Section，第一个必须为[ 
        { 
            str = strline.Left(strline.Find("]"));//去掉]右边 
            str = str.Right(str.GetLength() - str.Find("[")-1);//去掉[左边 

            str.TrimLeft(); 
            str.TrimRight(); 

            if(Section == str)//找到Section 
            { 
                while(i < iFileLines) 
                { 
                    strline = FileContainer.GetAt(i++); 
                    strline.TrimLeft(); 

                    if(strline.GetAt(0) == '[') 
                        return csValue;//如果到达下一个[]，即找不到,返回默认值 
                     
                    str = strline.Left(strline.Find("="));//去掉=右边 

                    str.TrimLeft(); 
                    str.TrimRight(); 

                    if(Item == str)//找到Item 
                    { 
                        str = strline.Right(strline.GetLength() - strline.Find("=") - 1);//去掉=左边 

                        str.TrimLeft(); 
                        str.TrimRight(); 

                        return str;
                    } 
                } 
                return csValue;//找不到,返回默认值 
            } 

        } 

    } 
    return csValue;//找不到,返回默认值 
} 

void CIniFile::WriteFileString(CString Section, CString Item, CString Value) 
{ 
    int i = 0; 

	ReadIniFile();

    int iFileLines = FileContainer.GetSize(); 

    CString strline,str; 

    while(i < iFileLines) 
    { 
        strline = FileContainer.GetAt(i++); 

        strline.TrimLeft(); 
        if(strline.GetAt(0) == '[')//查找Section，第一个必须为[ 
        { 
            str = strline.Left(strline.Find("]"));//去掉]右边 
            str = str.Right(str.GetLength()-str.Find("[") - 1);//去掉[左边 

            str.TrimLeft(); 
            str.TrimRight(); 

            if(Section == str)//找到Section 
            { 
                while(i < iFileLines) 
                { 
                    strline = FileContainer.GetAt(i++); 
                    strline.TrimLeft(); 

                    if(strline.GetAt(0) == '[')//如果到达下一个[]，即找不到Item 
                    { 
                        break; 
                    } 
                     
                    str = strline.Left(strline.Find("="));//去掉=右边 

                    str.TrimLeft(); 
                    str.TrimRight(); 

                    if(Item == str)//找到Item 
                    { 
                        strline = Item + "=" + Value; 
                        FileContainer[i-1] = strline; 
                        return; 
                    } 
                } 
                //找不到Item 
                strline = Item + "=" + Value; 
                FileContainer.InsertAt(i-1, strline); 
                return; 
            } 
        } 
    } 
    //找不到Section 
    //直接在最后加入Section，Item，Value 
    FileContainer.Add("[" + Section + L"]"); 
    FileContainer.Add(Item + "=" + Value); 
    return; 
} 

BOOL CIniFile::AcsiToUnicode(CString &str)
{
	char *szBuf = new char[ str.GetLength()+1]; //数量要加1


	for (int i = 0 ; i < str.GetLength(); i++)
	{
		szBuf[i] = (CHAR)str.GetAt(i);
	}  
	szBuf[str.GetLength()]='\0';   //这里，必须要加上，否则会在结尾片显示一个"铪"字。
	// USES_CONVERSION;
	//char * sz=W2A(str.GetBuffer());  //这些方法我都试过，不行的。
	BOOL bok= CharToUnicode(szBuf , str);
	delete []szBuf;
	return bok;
}

//此函数是原版引用的。不需要任何更改
/////////////////////////////////////////////////////////////////////////////////////////
// 将Char型字符转换为Unicode字符
int CIniFile::CharToUnicode(char *pchIn, CString &pstrOut)
{
	int nLen;
	WCHAR *ptch;

	if(pchIn == NULL)
	{
		return 0;
	}
	nLen = MultiByteToWideChar(CP_ACP, 0, pchIn, -1, NULL, 0);//取得所需缓存的多少
	ptch = new WCHAR[nLen];//申请缓存空间
	MultiByteToWideChar(CP_ACP, 0, pchIn, -1, ptch, nLen);//转码
	//pstrOut->Format(_T("%s"), ptch);
	pstrOut = ptch;
	delete [] ptch;
	return nLen;
}