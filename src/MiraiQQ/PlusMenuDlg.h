#pragma once


// CPlusMenuDlg 对话框

class CPlusMenuDlg : public CDialog
{
	DECLARE_DYNAMIC(CPlusMenuDlg)

public:
	CPlusMenuDlg(__int32 ac,CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPlusMenuDlg();

// 对话框数据
	enum { IDD = IDD_PLUS_MENU };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
private:
	__int32 ac;
public:
	afx_msg void OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnPaint();
};
