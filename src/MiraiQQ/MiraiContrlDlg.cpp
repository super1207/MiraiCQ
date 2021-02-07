// MiraiContrlDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MiraiQQ.h"
#include "MiraiContrlDlg.h"
#include "MiraiPlugsDlg.h"
#include "MiraiQ/MIraiQ.h"
#include <boost/thread/thread.hpp>


// CMiraiContrlDlg 对话框

IMPLEMENT_DYNAMIC(CMiraiContrlDlg, CDialog)

CMiraiContrlDlg::CMiraiContrlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMiraiContrlDlg::IDD, pParent)
{

}

CMiraiContrlDlg::~CMiraiContrlDlg()
{
}

void CMiraiContrlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMiraiContrlDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON3, &CMiraiContrlDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CMiraiContrlDlg::OnBnClickedButton4)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CMiraiContrlDlg 消息处理程序

void CMiraiContrlDlg::OnBnClickedButton3()
{
	CMiraiPlugsDlg dlg;
	dlg.DoModal();
	// TODO: 在此添加控件通知处理程序代码
}

void CMiraiContrlDlg::OnBnClickedButton4()
{
	if(MiraiQ::getInstance()->is_bot_connect() == MIRAIQ_BOT_IS_CONNECT)
	{
		AfxMessageBox("websocket已经成功连接");
	}else
	{
		AfxMessageBox("websocket没有成功链接");
	}
	// TODO: 在此添加控件通知处理程序代码
}
static void stop_tip()
{
	MessageBoxA(NULL,"正在关闭（5s）","",MB_OK);
}
void CMiraiContrlDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	boost::function0< void> f =  boost::bind(&stop_tip);
	new boost::thread(f);
	MiraiQ * mq = MiraiQ::getInstance();
	mq->call_cq_stop_fun();
	HANDLE hself = GetCurrentProcess();
	TerminateProcess(hself, 0);
	CDialog::OnClose();
}
