#pragma once


// ArgChangeDlg 对话框

class ArgChangeDlg : public CDialogEx
{
	DECLARE_DYNAMIC(ArgChangeDlg)

public:
	ArgChangeDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~ArgChangeDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_CONSTANT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString v_v_trap;
	CString v_v_lucky;
	CString v_v_dest;
	CString v_prob_s;
	CString v_prob_l;
	CString v_discount;
	CString v_epsilon;
	CString v_learning_rate;
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};
