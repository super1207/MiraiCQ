// PlusMenuDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MiraiQQ.h"
#include "PlusMenuDlg.h"
#include "MiraiQ/MIraiQ.h"
#include <boost/lexical_cast.hpp> 


// CPlusMenuDlg 对话框

IMPLEMENT_DYNAMIC(CPlusMenuDlg, CDialog)

CPlusMenuDlg::CPlusMenuDlg(__int32 ac,CWnd* pParent /*=NULL*/)
	: CDialog(CPlusMenuDlg::IDD, pParent)
{
		this->ac = ac;
}

CPlusMenuDlg::~CPlusMenuDlg()
{
}

void CPlusMenuDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPlusMenuDlg, CDialog)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, &CPlusMenuDlg::OnNMClickList1)
END_MESSAGE_MAP()


BOOL CPlusMenuDlg::OnInitDialog()
{
	
	CListCtrl * list_ctrl = (CListCtrl*)GetDlgItem(IDC_LIST1);
	list_ctrl->InsertColumn(0, _T("标题"), LVCFMT_LEFT, 150);

	//根据AC得到插件
	MiraiQ * mq = MiraiQ::getInstance();
	std::map<__int32,Plus::PlusDef> plus_map = mq->get_plus_ptr()->get_plus_map();
	Plus::PlusDef & plusdef = plus_map[ac];

	this->SetWindowText(("[ " + plusdef.name + " ]").c_str());

	for(size_t i = 0;i < plusdef.menu.size();++i)
	{
		list_ctrl->InsertItem(i,  plusdef.menu[i].function_name.c_str());
	}
	
	return TRUE;
}


// CPlusMenuDlg 消息处理程序

void CPlusMenuDlg::OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<NMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	CListCtrl * list_ctrl = (CListCtrl*)GetDlgItem(IDC_LIST1);
	POSITION pos=list_ctrl->GetFirstSelectedItemPosition();
	int nId=(int)list_ctrl->GetNextSelectedItem(pos);
	if(nId == -1)
	{
		return ;
	}

	void *  ptr = (
		MiraiQ::get_plus_ptr()->get_plusdef(ac).second.menu.at(nId).function_ptr
		);
	((void(__stdcall *)())ptr)();
	


	*pResult = 0;
}
