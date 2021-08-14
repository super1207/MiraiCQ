// MiraiQQ.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "MiraiQQ.h"
#include "MiraiQQDlg.h"
#include "MiraiContrlDlg.h"
#include "gobal_value.h"
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>

#include <boost/filesystem.hpp>

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;

namespace logging=boost::log;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMiraiQQApp

BEGIN_MESSAGE_MAP(CMiraiQQApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CMiraiQQApp 构造

CMiraiQQApp::CMiraiQQApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CMiraiQQApp 对象

CMiraiQQApp theApp;

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
// CMiraiQQApp 初始化

BOOL CMiraiQQApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));
	m_pMainWnd = NULL;

	std::string path_name,exe_name;
	get_program_dir(path_name,exe_name);
	boost::log::core::get()->add_global_attribute("TimeStamp", boost::log::attributes::local_clock());
	boost::log::register_simple_formatter_factory< boost::log::trivial::severity_level, char >("Severity");
	logging::add_common_attributes();
	boost::log::add_file_log(  
		keywords::auto_flush = true,  
		keywords::file_name =  path_name + "/log/sign_%Y-%m-%d_%H-%M-%S.%N.log",  
		keywords::rotation_size = 10 * 1024 * 1024,  
		keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),  
		boost::log::keywords::format = "[%TimeStamp%](%Severity%): %Message%", 
		keywords::min_free_space=3 * 1024 * 1024  
		);  
	if( boost::filesystem::exists(path_name + "/debug.txt"))
	{
		logging::core::get()->set_filter(logging::trivial::severity>=logging::trivial::trace);
	}else
	{
		logging::core::get()->set_filter(logging::trivial::severity>=logging::trivial::info);
	}
	

	{
		static CMiraiQQDlg dlg;
		INT_PTR nResponse = dlg.DoModal();
		if(nResponse != 2)
		{
			static CMiraiContrlDlg ddlg;
			//m_pMainWnd = &ddlg;
			INT_PTR nResponse = ddlg.DoModal();
		}
		HANDLE hself = GetCurrentProcess();
		TerminateProcess(hself, 0);
	}
	{
		
	}
	//{
	//	CMiraiQQDlg dlg;
	//	m_pMainWnd = &dlg;
	//	INT_PTR nResponse = dlg.DoModal();
	//}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}
