// MiraiLogDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MiraiQQ.h"
#include "MiraiLogDlg.h"
#include "gobal_value.h"


// CMiraiLogDlg 对话框

IMPLEMENT_DYNAMIC(CMiraiLogDlg, CDialog)

CMiraiLogDlg::CMiraiLogDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMiraiLogDlg::IDD, pParent)
{
	isInEdit = false;
}

CMiraiLogDlg::~CMiraiLogDlg()
{
	
}

void CMiraiLogDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMiraiLogDlg, CDialog)
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_WM_CLOSE()
END_MESSAGE_MAP()

BOOL CMiraiLogDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	((CButton*)GetDlgItem(IDC_CHECK1))->SetCheck(FALSE);
	((CEdit*)GetDlgItem(IDC_EDIT1))->SetLimitText(UINT_MAX);
	SetTimer(1,200,NULL);
	return 0;
}

void CMiraiLogDlg::OnTimer( UINT_PTR nIDEvent )
{
	switch (nIDEvent){
		case 1:
			{
				if(!((CButton*)GetDlgItem(IDC_CHECK1))->GetCheck())
				{
					std::string str = GetLog();
					CEdit * edit = ((CEdit*)GetDlgItem(IDC_EDIT1));
					CString dat;
					edit->GetWindowText(dat);
					if(dat != str.c_str())
					{
						edit->SetWindowText(str.c_str());
						edit->SetSel(edit->GetWindowTextLength(),edit->GetWindowTextLength());
					}
				}
			}
			break;
		default:
			break;
	}
}

void CMiraiLogDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	//设置蓝色背景色
	CRect rect;
	GetClientRect(rect);
	dc.FillSolidRect(rect,RGB(0,245,255)); 

	// 不为绘图消息调用 CDialog::OnPaint()
}

HBRUSH CMiraiLogDlg::OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor )
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何属性
	if (pWnd->GetDlgCtrlID() == IDC_CHECK1)
	{
		pDC->SetBkColor(RGB(0,245,255));    
		return (HBRUSH)GetStockObject(HOLLOW_BRUSH);
	}

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}

void CMiraiLogDlg::OnClose()
{
	//KillTimer(1);
	CDialog::OnClose();
}

// CMiraiLogDlg 消息处理程序
