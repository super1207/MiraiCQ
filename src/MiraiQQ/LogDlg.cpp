// LogDlg.cpp : 实现文件
//

//#include "stdafx.h"
//#include "MiraiQQ.h"
#include "LogDlg.h"


// LogDlg 对话框

IMPLEMENT_DYNAMIC(LogDlg, CDialog)

LogDlg::LogDlg(CWnd* pParent /*=NULL*/)
	: CDialog(LogDlg::IDD, pParent)
{

}

LogDlg::~LogDlg()
{
}

void LogDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(LogDlg, CDialog)
END_MESSAGE_MAP()


// LogDlg 消息处理程序
