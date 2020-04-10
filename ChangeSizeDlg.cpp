// ChangeSizeDlg.cpp: 实现文件
//

#include "pch.h"
#include "AI-project1.h"
#include "ChangeSizeDlg.h"
#include "afxdialogex.h"


// ChangeSizeDlg 对话框

IMPLEMENT_DYNAMIC(ChangeSizeDlg, CDialogEx)

ChangeSizeDlg::ChangeSizeDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
	, v_row(_T(""))
	, v_col(_T(""))
{

}

ChangeSizeDlg::~ChangeSizeDlg()
{
}

void ChangeSizeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, v_row);
	DDX_Text(pDX, IDC_EDIT2, v_col);
}


BEGIN_MESSAGE_MAP(ChangeSizeDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &ChangeSizeDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// ChangeSizeDlg 消息处理程序


void ChangeSizeDlg::OnBnClickedOk()
{
	UpdateData(true);
	if (_ttoi(v_col) > 100 || _ttoi(v_row) > 100 || _ttoi(v_col) <= 0 || _ttoi(v_row) <= 0)
	{
		AfxMessageBox(L"输入的值不正确. \n行和列的合法范围是 [1,100].");
	}
	else CDialogEx::OnOK();
}


void ChangeSizeDlg::preset(int row, int col)
{
	CString s;
	s.Format(L"%d", row);
	v_row = s;
	s.Format(L"%d", col);
	v_col = s;
}


BOOL ChangeSizeDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	UpdateData(false);


	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
