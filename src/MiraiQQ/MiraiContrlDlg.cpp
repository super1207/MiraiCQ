// MiraiContrlDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MiraiQQ.h"
#include "MiraiContrlDlg.h"
#include "MiraiPlugsDlg.h"
#include "gobal_value.h"
#include "MiraiQ/MIraiQ.h"
#include "MiraiLogDlg.h"
#include "httphelp.h"

#include <boost/thread/thread.hpp>
#include <boost\cast.hpp>
#include <boost\lexical_cast.hpp> 

#include <atlimage.h>


#define WM_SHOWTASK WM_USER+1

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
	ON_MESSAGE(WM_SHOWTASK,onShowTask)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON1, &CMiraiContrlDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON8, &CMiraiContrlDlg::OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON5, &CMiraiContrlDlg::OnBnClickedButton5)
	ON_WM_TIMER()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

static NOTIFYICONDATA nid;
BOOL CMiraiContrlDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	nid.cbSize=(DWORD)sizeof(NOTIFYICONDATA);
	nid.hWnd=this->m_hWnd;
	nid.uID=IDR_MAINFRAME;
	nid.uFlags=NIF_ICON|NIF_MESSAGE|NIF_TIP ;
	nid.uCallbackMessage=WM_SHOWTASK;
	nid.hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	strcpy_s(nid.szTip,"MiraiCQ");
	Shell_NotifyIcon(NIM_ADD,&nid); 
	SetTimer(1,500,NULL);
	
	return 0;

}


LRESULT CMiraiContrlDlg::onShowTask(WPARAM wParam,LPARAM lParam)
{
	if(wParam!=IDR_MAINFRAME)
		return 1;
	if(lParam == WM_LBUTTONDBLCLK)
	{
		this->ShowWindow(SW_SHOW);
	}
	return 0;
}

// CMiraiContrlDlg 消息处理程序

void CMiraiContrlDlg::OnBnClickedButton3()
{
	CMiraiPlugsDlg dlg;
	dlg.DoModal();
	// TODO: 在此添加控件通知处理程序代码
}

//得到执行的exe的名字和路径(windows)
static void get_program_dir(std::string &path_name, std::string &exe_name)
{
	char exe_path[MAX_PATH];
	if (GetModuleFileNameA(NULL, exe_path, MAX_PATH) == 0)
	{
		return ;
		//throw logic_error("GetModuleFileNameA错误");
	}
	std::string exe_path_string = exe_path;
	size_t pos = exe_path_string.find_last_of('\\', exe_path_string.length());
	path_name = exe_path_string.substr(0, pos);
	exe_name = exe_path_string.substr(pos + 1);
}

static CImage user_image;

void CMiraiContrlDlg::OnBnClickedButton4()
{
	static CFont fontok,fontno;
	static int i = 0;
	static int isnewcon = 0;
	if(i == 0)
	{
		++i;
		fontok.CreatePointFont(150,"微软雅黑");
		fontno.CreatePointFont(150,"微软雅黑");
	}
	if(MiraiQ::getInstance()->is_bot_connect() == MIRAIQ_BOT_IS_CONNECT)
	{
		GetDlgItem(IDC_STATIC_STATUS)->SetFont(&fontok);
		GetDlgItem(IDC_STATIC_STATUS)->SetWindowText("已连接");
		if(isnewcon == 0)
		{
			isnewcon = 1;
			__int64 qq = 0;
			try
			{
				Json::Value ret_json = MiraiQ::get_bot_ptr()->getLoginQQ();
				qq = ret_json["data"]["user_id"].asInt64();
			}catch(const std::exception & )
			{

			}
			if(qq != 0)
			{
				std::string url = "http://q1.qlogo.cn/g?b=qq&nk="+boost::lexical_cast<std::string>(qq)+"&s=640";
				std::string ct;
				if(HttpGet(url,"user.jpg",ct,5000))
				{
					if(!user_image.IsNull())
					{
						user_image.Destroy();
					}
					std::string path_name,exe_name;
					get_program_dir(path_name,exe_name);
					user_image.Load((path_name+"//user.jpg").c_str());
					if (!user_image.IsNull())  
					{  
						{
							int height, width;
							CRect rect1;
							height = user_image.GetHeight();
							width = user_image.GetWidth();
							CRect rect;
							GetDlgItem( IDC_STATIC11 )->GetClientRect(&rect);
							CDC* pDC = GetDlgItem( IDC_STATIC11 ) ->GetDC();
							HDC hDC = pDC ->GetSafeHdc();
							SetStretchBltMode(hDC,STRETCH_HALFTONE); 
							rect1 = CRect(rect.TopLeft(), CSize(rect.Width()+1,rect.Height()+1));
							user_image.StretchBlt(hDC,rect1,SRCCOPY);
							ReleaseDC(pDC);
						}
					}
				}
			}
		}
		isnewcon = 1;
		//AfxMessageBox("websocket已经成功连接");
	}else
	{
		GetDlgItem(IDC_STATIC_STATUS)->SetFont(&fontno);
		GetDlgItem(IDC_STATIC_STATUS)->SetWindowText("已断开");
		isnewcon = 0;
		//AfxMessageBox("websocket没有成功链接");
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
	MiraiQ * mq = MiraiQ::getInstance();
	::Shell_NotifyIcon(NIM_DELETE,&nid);
	boost::function0< void> f =  boost::bind(&stop_tip);
	new boost::thread(f);
	mq->call_cq_stop_fun();
	HANDLE hself = GetCurrentProcess();
	::Shell_NotifyIcon(NIM_DELETE,&nid);
	TerminateProcess(hself, 0);
	CDialog::OnClose();
}

void CMiraiContrlDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(nID==SC_MINIMIZE)
	{
		ShowWindow(SW_HIDE);
		return ;
	}
	CDialog::OnSysCommand(nID, lParam);
}

void CMiraiContrlDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	//设置蓝色背景色
	CRect rect;
	GetClientRect(rect);
	dc.FillSolidRect(rect,RGB(0,245,255)); 

	if (!user_image.IsNull())  
	{  
		{
			int height, width;
			CRect rect1;
			height = user_image.GetHeight();
			width = user_image.GetWidth();
			CRect rect;
			GetDlgItem( IDC_STATIC11 )->GetClientRect(&rect);
			CDC* pDC = GetDlgItem( IDC_STATIC11 ) ->GetDC();
			HDC hDC = pDC ->GetSafeHdc();
			SetStretchBltMode(hDC,STRETCH_HALFTONE); 
			rect1 = CRect(rect.TopLeft(), CSize(rect.Width()+1,rect.Height()+1));
			user_image.StretchBlt(hDC,rect1,SRCCOPY);
			ReleaseDC(pDC);
		}
	}
	// 不为绘图消息调用 CDialog::OnPaint()
}

void CMiraiContrlDlg::OnBnClickedButton1()
{
	std::string path_name,exe_name;
	get_program_dir(path_name,exe_name);
	WinExec((path_name + "/" +exe_name).c_str() , SW_SHOWNORMAL);
	OnClose();
	// TODO: 在此添加控件通知处理程序代码
}

void CMiraiContrlDlg::OnBnClickedButton8()
{
	static CMiraiLogDlg dlg;
	static bool isfirst = true;
	if(isfirst)
	{
		isfirst = false;
		dlg.Create(IDD_MIRAILOGDLG, this);
		dlg.ShowWindow(SW_SHOW);
	}else
	{
		dlg.ShowWindow(SW_SHOW);
	}
	
	//std::string path_name,exe_name;
	//get_program_dir(path_name,exe_name);
	//WinExec(("explorer.exe " + path_name + "\\log\\").c_str() , SW_SHOWNORMAL);
	// TODO: 在此添加控件通知处理程序代码
}

void CMiraiContrlDlg::OnBnClickedButton5()
{
	AfxMessageBox("MiraiCQ\nv0.1.1\nhttps://github.com/super1207/MiraiCQ");
	// TODO: 在此添加控件通知处理程序代码
}

void CMiraiContrlDlg::OnTimer( UINT_PTR nIDEvent )
{
	switch (nIDEvent){
		case 1:
			OnBnClickedButton4();
			break;
		default:
			break;
	}
}

HBRUSH CMiraiContrlDlg::OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor )
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何属性
	if (pWnd->GetDlgCtrlID() == IDC_STATIC_STATUS)
	{
		CString t;
		GetDlgItem(IDC_STATIC_STATUS)->GetWindowText(t);
		if(t == "已连接")
		{
			pDC->SetBkColor(RGB(0,245,255));    
			pDC->SetTextColor(RGB(0,0,0));
		}else
		{
			pDC->SetBkColor(RGB(255,0,0));
			pDC->SetTextColor(RGB(0,0,0));
		}
		
		  
		return (HBRUSH)GetStockObject(HOLLOW_BRUSH);
	}

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}
