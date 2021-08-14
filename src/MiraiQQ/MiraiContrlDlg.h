#pragma once

// CMiraiContrlDlg 对话框

class CMiraiContrlDlg : public CDialog
{
	DECLARE_DYNAMIC(CMiraiContrlDlg)

public:
	CMiraiContrlDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMiraiContrlDlg();
	virtual BOOL OnInitDialog();

// 对话框数据
	enum { IDD = IDD_MIRAICONTRLDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnClose();
	afx_msg LRESULT onShowTask(WPARAM wParam,LPARAM lParam);//托盘
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton8();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
