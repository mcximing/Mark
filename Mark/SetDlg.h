#pragma once
#include "afxwin.h"


// CSetDlg 对话框

class CSetDlg : public CDialog
{
	DECLARE_DYNAMIC(CSetDlg)

public:
	CSetDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSetDlg();

// 对话框数据
	enum { IDD = IDD_SET_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	int m_radio;
	CButton m_check;
	CButton m_check2;
	CButton m_check3;
	bool ifFixed;
	bool ifAutoSave;
	int m_width;
	int m_height;
	bool ifSaveOrigin;
};
