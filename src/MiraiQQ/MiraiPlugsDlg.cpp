// MiraiPlugsDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MiraiQQ.h"
#include "MiraiPlugsDlg.h"
#include "PlusMenuDlg.h"
#include "MiraiQ/MIraiQ.h"
#include <boost/lexical_cast.hpp> 

// CMiraiPlugsDlg 对话框

IMPLEMENT_DYNAMIC(CMiraiPlugsDlg, CDialog)

CMiraiPlugsDlg::CMiraiPlugsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMiraiPlugsDlg::IDD, pParent)
{

}


CMiraiPlugsDlg::~CMiraiPlugsDlg()
{
}

void CMiraiPlugsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, plus_list_ctrl);
}


BEGIN_MESSAGE_MAP(CMiraiPlugsDlg, CDialog)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, &CMiraiPlugsDlg::OnNMClickList1)
	ON_BN_CLICKED(IDC_BUTTON2, &CMiraiPlugsDlg::OnBnClickedButton2)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


BOOL CMiraiPlugsDlg::OnInitDialog()
{
	CListCtrl * list_ctrl = (CListCtrl*)GetDlgItem(IDC_LIST1);
	list_ctrl->SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	list_ctrl->InsertColumn(0, _T("名称"), LVCFMT_LEFT, 100);
	list_ctrl->InsertColumn(1, _T("版本"), LVCFMT_LEFT, 100); 
	list_ctrl->InsertColumn(2, _T("作者"), LVCFMT_LEFT, 100); 
	list_ctrl->InsertColumn(3, _T(""), LVCFMT_LEFT, 3); 
	MiraiQ * mq = MiraiQ::getInstance();
	std::map<__int32,Plus::PlusDef> plus_map = mq->get_plus_ptr()->get_plus_map();
	int i = 0;
	for(std::map<__int32,Plus::PlusDef>::iterator iter = plus_map.begin();iter!=plus_map.end();iter++)
	{

		list_ctrl->InsertItem(i, iter->second.name.c_str());
		list_ctrl->SetItemText(i, 1,iter->second.version.c_str());
		list_ctrl->SetItemText(i, 2,iter->second.author.c_str());
		list_ctrl->SetItemText(i,3,boost::lexical_cast<std::string>(iter->second.ac).c_str());
		++i;
		//iter->second.
	}
	return TRUE;
}


// CMiraiPlugsDlg 消息处理程序

void CMiraiPlugsDlg::OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<NMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	//得到选中的插件的AC
	CListCtrl * list_ctrl = (CListCtrl*)GetDlgItem(IDC_LIST1);
	POSITION pos=list_ctrl->GetFirstSelectedItemPosition();
	int nId=(int)list_ctrl->GetNextSelectedItem(pos);
	if(nId == -1)
	{
		return ;
	}
	CString ac_str=list_ctrl->GetItemText(nId,3);
	
	

	//根据AC得到描述
	MiraiQ * mq = MiraiQ::getInstance();
	int int_ac =  boost::lexical_cast<__int32>(ac_str);
	std::map<__int32,Plus::PlusDef> plus_map = mq->get_plus_ptr()->get_plus_map();
	Plus::PlusDef & plusdef = plus_map[int_ac];
	
	//设置描述
	CEdit * des_edit = (CEdit*)GetDlgItem(IDC_EDIT1);
	des_edit->SetWindowText(plusdef.description.c_str());
	static char sz[] = "                                                                ";
	GetDlgItem(IDC_STATIC33)->SetWindowText(sz);
	GetDlgItem(IDC_STATIC35)->SetWindowText(sz);
	GetDlgItem(IDC_STATIC33)->SetWindowText(plusdef.author.c_str());
	GetDlgItem(IDC_STATIC35)->SetWindowText(plusdef.version.c_str());
	*pResult = 0;
}

void CMiraiPlugsDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	CListCtrl * list_ctrl = (CListCtrl*)GetDlgItem(IDC_LIST1);
	int current_pos =  list_ctrl->GetSelectionMark();
	if(current_pos == -1)
	{
		AfxMessageBox(_T("请选中一个插件"));
		return ;
	}

	CString ac_str=list_ctrl->GetItemText(current_pos,3);
	int int_ac =  boost::lexical_cast<__int32>(ac_str);
	CPlusMenuDlg dlg(int_ac);
	dlg.DoModal();
}

void CMiraiPlugsDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	//设置蓝色背景色
	CRect rect;
	GetClientRect(rect);
	dc.FillSolidRect(rect,RGB(0,245,255)); 
	
	// 不为绘图消息调用 CDialog::OnPaint()
}

HBRUSH CMiraiPlugsDlg::OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor )
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何属性
	if (pWnd->GetDlgCtrlID() == IDC_STATIC31 ||
		pWnd->GetDlgCtrlID() == IDC_STATIC32 ||
		pWnd->GetDlgCtrlID() == IDC_STATIC33 ||
		pWnd->GetDlgCtrlID() == IDC_STATIC34 ||
		pWnd->GetDlgCtrlID() == IDC_STATIC35 ||
		pWnd->GetDlgCtrlID() == IDC_STATIC36 )
	{
		//pDC->SetBkMode(TRANSPARENT);   
		pDC->SetBkColor(RGB(0,245,255));   
		return (HBRUSH)GetStockObject(HOLLOW_BRUSH);
	}

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}
