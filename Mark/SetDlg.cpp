// SetDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Mark.h"
#include "SetDlg.h"


// CSetDlg 对话框

IMPLEMENT_DYNAMIC(CSetDlg, CDialog)

CSetDlg::CSetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetDlg::IDD, pParent)
	, m_radio(2)
	, m_width(35)
	, m_height(40)
	, ifFixed(false)
	, ifAutoSave(false)
	, ifSaveOrigin(true)
{

}

CSetDlg::~CSetDlg()
{
}

void CSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO1, m_radio);
	DDX_Control(pDX, IDC_CHECK1, m_check);
	DDX_Text(pDX, IDC_FIXWIDTH, m_width);
	DDV_MinMaxInt(pDX, m_width, 0, 720);
	DDX_Text(pDX, IDC_FIXHEIGHT, m_height);
	DDV_MinMaxInt(pDX, m_height, 0, 576);
	DDX_Control(pDX, IDC_CHECK2, m_check2);
	DDX_Control(pDX, IDC_CHECK3, m_check3);
}


BEGIN_MESSAGE_MAP(CSetDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CSetDlg::OnBnClickedOk)
END_MESSAGE_MAP()



// 对话框初始化

BOOL CSetDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_check.SetCheck(ifFixed);
	m_check2.SetCheck(ifAutoSave);
	m_check3.SetCheck(ifSaveOrigin);
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

// CSetDlg 消息处理程序

void CSetDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	ifFixed = (m_check.GetCheck()!=0);
	ifAutoSave = (m_check2.GetCheck()!=0);
	ifSaveOrigin = (m_check3.GetCheck()!=0);
	OnOK();
}