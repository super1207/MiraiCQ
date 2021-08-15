#pragma once


// LogDlg 对话框

class LogDlg : public CDialog
{
	DECLARE_DYNAMIC(LogDlg)

public:
	LogDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~LogDlg();

// 对话框数据
	enum { IDD = IDD_LOGDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
