#pragma once


// CMiraiLogDlg 对话框

class CMiraiLogDlg : public CDialog
{
	DECLARE_DYNAMIC(CMiraiLogDlg)

public:
	CMiraiLogDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMiraiLogDlg();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnClose();


// 对话框数据
	enum { IDD = IDD_MIRAILOGDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	bool isInEdit;
};
