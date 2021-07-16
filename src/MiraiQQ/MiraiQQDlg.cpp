// MiraiQQDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MiraiQQ.h"
#include "MiraiQQDlg.h"
#include "MiraiQ/MIraiQ.h"
#include "MiraiContrlDlg.h"
#include <boost/thread/thread.hpp>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CMiraiQQDlg 对话框


CMiraiQQDlg::CMiraiQQDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMiraiQQDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMiraiQQDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMiraiQQDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CMiraiQQDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CMiraiQQDlg 消息处理程序

BOOL CMiraiQQDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	ShowWindow(SW_NORMAL);

	// TODO: 在此添加额外的初始化代码
	GetDlgItem(IDC_EDIT1)->SetWindowText("ws://localhost:6700");

	//AllocConsole();
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMiraiQQDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMiraiQQDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMiraiQQDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
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


static int runBot()
{

	MiraiQ * mq = MiraiQ::getInstance();
	while(true)
	{
		__int32 ret = mq->deal_a_message();
		if(ret == -1)
		{
			BOOST_LOG_TRIVIAL(info) << "已经断线，五秒后重新连接";
			boost::this_thread::sleep_for(boost::chrono::seconds(5));
			BOOST_LOG_TRIVIAL(info) << "正在重新连接......";
			if(mq->bot_connect() == MIRAI_OK)
			{
				BOOST_LOG_TRIVIAL(info) << "重新连接成功" ;
			}else
			{
				BOOST_LOG_TRIVIAL(info) << "重新连接失败" ;
			}

		}else if(ret == 0)  //no message be dealed
		{
			Sleep(1);
		}else if(ret == 1)  //dealed a message
		{
			continue;
		}

	}

	return 0;
}


void CMiraiQQDlg::OnBnClickedButton1()
{

	if(!LoadLibrary(_T("CQP.dll")))
	{
		AfxMessageBox(_T("CQP.dll not found!"));
		return ;
	}

	CString ws_url;
	CString access_token;
	GetDlgItem(IDC_EDIT1)->GetWindowText(ws_url);
	GetDlgItem(IDC_EDIT2)->GetWindowText(access_token);
	MiraiQ * mq = MiraiQ::getInstance();
	assert(mq);
	if(mq->bot_connect(ws_url.GetBuffer(0),access_token.GetBuffer(0)) != MIRAI_OK)
	{
		AfxMessageBox(_T("登陆失败"));
		return ;
	}
	std::string path_name, exe_name;
	get_program_dir(path_name, exe_name);
	std::vector<boost::filesystem::path>  dll_path_vec = MiraiQ::get_plus_ptr()->find_plus_file(boost::filesystem::path(path_name)/"app");
	for(size_t i = 0;i < dll_path_vec.size();++i)
	{
		BOOST_LOG_TRIVIAL(debug) << dll_path_vec[i];
		mq->load_plus(dll_path_vec[i]);
	}
	mq->call_cq_start_fun();

	//BOOST_LOG_TRIVIAL(debug) << "get_plus_id_list";
	std::vector<__int32> ac_list = mq->get_plus_id_list();

	//调用：启用
	for (size_t i = 0; i < ac_list.size();++i)
	{
		mq->enable_plus(ac_list[i]);
	}
	boost::function0< void> f =  boost::bind(&runBot);
	new boost::thread(f);
	
	
	//CMiraiContrlDlg ddlg;
    //INT_PTR nResponse = ddlg.DoModal();

	CMiraiQQDlg::OnOK();
	// TODO: 在此添加控件通知处理程序代码
}
