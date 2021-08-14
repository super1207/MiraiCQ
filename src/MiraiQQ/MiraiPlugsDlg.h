#pragma once
#include "afxcmn.h"


// CMiraiPlugsDlg 对话框

class CMiraiPlugsDlg : public CDialog
{
	DECLARE_DYNAMIC(CMiraiPlugsDlg)

public:
	CMiraiPlugsDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMiraiPlugsDlg();

// 对话框数据
	enum { IDD = IDD_MIRAIPLUGSDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl plus_list_ctrl;
	afx_msg void OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButton2();
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
