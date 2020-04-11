// ArgChangeDlg.cpp: 实现文件
//

#include "pch.h"
#include "AI-project1.h"
#include "ArgChangeDlg.h"
#include "afxdialogex.h"


// ArgChangeDlg 对话框

IMPLEMENT_DYNAMIC(ArgChangeDlg, CDialogEx)

ArgChangeDlg::ArgChangeDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_CONSTANT, pParent)
    , v_v_trap(_T(""))
    , v_v_lucky(_T(""))
    , v_v_dest(_T(""))
    , v_prob_s(_T(""))
    , v_prob_l(_T(""))
    , v_discount(_T(""))
    , v_epsilon(_T(""))
    , v_learning_rate(_T(""))
{

}

ArgChangeDlg::~ArgChangeDlg()
{
}

void ArgChangeDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT1, v_v_trap);
    DDX_Text(pDX, IDC_EDIT2, v_v_lucky);
    DDX_Text(pDX, IDC_EDIT3, v_v_dest);
    DDX_Text(pDX, IDC_EDIT4, v_prob_s);
    DDX_Text(pDX, IDC_EDIT5, v_prob_l);
    DDX_Text(pDX, IDC_EDIT6, v_discount);
    DDX_Text(pDX, IDC_EDIT7, v_epsilon);
    DDX_Text(pDX, IDC_EDIT8, v_learning_rate);
}


BEGIN_MESSAGE_MAP(ArgChangeDlg, CDialogEx)
    ON_BN_CLICKED(IDOK, &ArgChangeDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// ArgChangeDlg 消息处理程序


void ArgChangeDlg::OnBnClickedOk()
{
    UpdateData(true);
    double tmp1,tmp2;
    tmp1 = _ttoi(v_prob_s);
    tmp2 = _ttoi(v_prob_l);
    if (tmp1 + tmp2 > 1 || tmp1 < 0 || tmp2 < 0)
    {
        AfxMessageBox(L"直行和左偏概率有误, 请检查");
        return;
    }
    //也许还需要其它的参数检查


    CDialogEx::OnOK();
}


BOOL ArgChangeDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    UpdateData(false);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}
