
// AI-project1Dlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "AI-project1.h"
#include "AI-project1Dlg.h"
#include "afxdialogex.h"
#include "maze.h"
#include "agent.cpp"
#include "ChangeSizeDlg.h"
#include "ArgChangeDlg.h"
#include<utility>

typedef std::pair<int, int> dint;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 自定义变量

const double PI = 3.14159265358979;


enum Status {
	READY, RUNNING, RUNNED
};
Status status = Status::READY;

static UINT INDICATORS[] = { IDS_BAR_STATUS, IDS_BAR_STEP };


bool showResult = false;

Maze* maze = nullptr;
Agent* agent = nullptr;
int algo=0;
MapElem typeToSet = MapElem::ROAD;
CList<CRect, CRect> m_listRect;

// X-length-col, Y-width-row, start from 0.
int orgX=120, orgY=100, length=800, width=500, col=16, row=16;

/* 
dint point: from 0,0 to length,width; 
dint grid: from 0,0 to col,row, 
POINT POINT: from orgX,orgY to orgX+length,orgY+width
RECT Rect: based on POINT
*/

dint getGrid(dint point)
{
	return std::make_pair(int((1.0 * point.second - orgY) / (width * 1.0 / row)), int((1.0 * point.first - orgX) / (length * 1.0 / col)));
}

RECT getRect(dint grid)
{
	RECT ans;
	ans.top = orgY + width * 1.0 / row * grid.first;
	ans.bottom = orgY + width * 1.0 / row * (grid.first + 1.0);
	ans.left = orgX + length * 1.0 / col * grid.second;
	ans.right = orgX + length * 1.0 / col * (grid.second + 1.0);
	return ans;
}

void initMaze()
{
	if (maze != nullptr)
	{
		delete maze;
	}
	row = col = 16;
		maze = new Maze(row, col);
		vector<pair<int, int> > walls = { make_pair(0,2), make_pair(1,2), make_pair(2,2), make_pair(2,6), make_pair(2,8), make_pair(3,2),
make_pair(3,3), make_pair(3,4), make_pair(3,5), make_pair(3,6), make_pair(3,8), make_pair(3,9), make_pair(3,10),
make_pair(3,11), make_pair(3,12), make_pair(3,13), make_pair(3,14), make_pair(4,14), make_pair(5,2), make_pair(5,3),
make_pair(5,4), make_pair(5,5), make_pair(5,6), make_pair(5,7), make_pair(5,8), make_pair(5,11), make_pair(5,12), make_pair(5,14),
make_pair(6,8), make_pair(6,9), make_pair(6,10), make_pair(6,11), make_pair(6,14), make_pair(7,2), make_pair(7,3), make_pair(7,4),
make_pair(7,5), make_pair(7,6), make_pair(7,2), make_pair(7,8), make_pair(7,9), make_pair(7,14), make_pair(8,1), make_pair(8,2),
make_pair(8,6), make_pair(8,8), make_pair(8,11), make_pair(8,12), make_pair(8,14), make_pair(9,1), make_pair(9,4), make_pair(9,6),
make_pair(9,8), make_pair(9,12), make_pair(9,14), make_pair(10,2), make_pair(10,4), make_pair(10,5), make_pair(10,6), make_pair(10,8),
make_pair(10,10), make_pair(10,12), make_pair(10,13), make_pair(10,14), make_pair(11,6), make_pair(11,8), make_pair(11,10), make_pair(11,12),
make_pair(12,2), make_pair(12,3), make_pair(12,4), make_pair(12,8), make_pair(12,12), make_pair(12,13), make_pair(12,14), make_pair(12,2), make_pair(13,2),
make_pair(13,4), make_pair(13,5), make_pair(13,6), make_pair(13,7), make_pair(13,8), make_pair(13,10), make_pair(14,2), make_pair(14,4),
make_pair(14,8), make_pair(14,9), make_pair(14,10), make_pair(14,11), make_pair(14,12), make_pair(14,13), make_pair(14,14), make_pair(15,6) };
		for (auto i : walls) {
			maze->setWall(i.first, i.second);
		}
		maze->setTrap(5, 10);
		maze->setTrap(7, 1);
		maze->setTrap(8, 9);
		maze->setTrap(11, 11);
		maze->setTrap(12, 15);
		maze->setLucky(row-1, col-1);
		//maze->setLucky(row-1, col-1);
}


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CAIproject1Dlg 对话框



CAIproject1Dlg::CAIproject1Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_AIPROJECT1_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	colorBlue = RGB(0, 122, 204);
	nullPen = CPen::FromHandle((HPEN)GetStockObject(NULL_PEN));
	nullBrush = CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH));
	whiteBrush.CreateSolidBrush(RGB(255, 255, 255));
	blackPen.CreatePen(0, 2, RGB(0, 0, 0));
	blackBrush.CreateSolidBrush(RGB(0, 0, 0));
	whitePen.CreatePen(0, 2, RGB(255, 255, 255));
	bluePen.CreatePen(0, 1, colorBlue);
	font.CreatePointFont(70, TEXT("Arial"));
	bigFont.CreatePointFont(105, TEXT("Arial"));

	initMaze();
	agent = new Agent(maze, algo);
}

void CAIproject1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAIproject1Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_RESET, &CAIproject1Dlg::OnBnClickedButtonReset)
    ON_BN_CLICKED(IDC_BUTTON_DISPLAY, &CAIproject1Dlg::OnBnClickedButtonDisplay)
	ON_BN_CLICKED(IDC_BUTTON_DEFAULT, &CAIproject1Dlg::OnBnClickedButtonDefault)
	ON_BN_CLICKED(IDC_BUTTON_AUTORUN, &CAIproject1Dlg::OnBnClickedButtonAutorun)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO1, IDC_RADIO3, &CAIproject1Dlg::OnBnClickedRadio)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_BUTTON_SEQ1, IDC_BUTTON_SEQ50, &CAIproject1Dlg::OnBnClickedButtonSeq)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_BUTTON_REV1, IDC_BUTTON_REV50, &CAIproject1Dlg::OnBnClickedButtonRev)
	ON_BN_CLICKED(IDC_BUTTON_CHANGE, &CAIproject1Dlg::OnBnClickedButtonChange)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_CHANGE_ARG, &CAIproject1Dlg::OnBnClickedButtonChangeArg)
END_MESSAGE_MAP()


// CAIproject1Dlg 消息处理程序

BOOL CAIproject1Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	SetProcessDPIAware();
	CRect rect;
	GetWindowRect(&rect);
	m_listRect.AddTail(rect);//对话框的区域
	CWnd* pWnd = GetWindow(GW_CHILD);//获取子窗体
	while (pWnd)
	{
		pWnd->GetWindowRect(rect);//子窗体的区域
		m_listRect.AddTail(rect);           //CList<CRect,CRect> m_listRect成员变量
		pWnd = pWnd->GetNextWindow();//取下一个子窗体
	}


	GetDlgItem(IDC_MAZE)->GetWindowRect(&rect);
	ScreenToClient(&rect);
	orgX = rect.left + 60;
	orgY = rect.top + 50;
	length = rect.Width() - 120;
	width = rect.Height() - 80;

	((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(true);
	m_bar.Create(this);
	m_bar.SetIndicators(INDICATORS, 2);
	m_bar.SetPaneInfo(0, IDS_BAR_STATUS, SBPS_NORMAL, 150);
	m_bar.SetPaneInfo(1, IDS_BAR_STEP, SBPS_STRETCH, 0);
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, IDS_BAR_STEP);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CAIproject1Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CAIproject1Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
		
	}
	else
	{
		CDialogEx::OnPaint();
		draw();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CAIproject1Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



// 在指定格点画墙
void CAIproject1Dlg::drawWall(int row, int col)
{
	CClientDC dc(this);
	CRect cRect = getRect(make_pair(row, col));
	dc.SelectObject(blackBrush);
	dc.SelectObject(blackPen);
	dc.Rectangle(cRect);

	//dc.SelectObject(tmp);
	//cBrush.DeleteObject();
	//dc.DeleteDC();
}


void CAIproject1Dlg::drawMaze()
{
	if (maze != nullptr)
	{
		CClientDC dc(this);
		CRect cRect;
		cRect.SetRect(orgX, orgY, orgX + length, orgY + width);
		dc.SelectObject(blackPen);
		dc.SelectObject(whiteBrush);
		dc.Rectangle(cRect);

		auto tmp = maze->getMaze();
		for (int i = 0; i != tmp.size(); i++)
		{
			for (int j = 0; j != tmp[i].size(); j++)
			{
				drawCell(i, j);
			}
		}
		
	}
}


void CAIproject1Dlg::OnBnClickedButtonReset()
{
	agent->clearResult();
	maze->clearEst();
	draw();
	setStatus(READY);
}


void CAIproject1Dlg::drawTrap(int row, int col)
{
	CClientDC dc(this);
	CBrush cBrush;
	CRect cRect = getRect(make_pair(row, col));
	auto center = cRect.CenterPoint();
	int width = min(cRect.Height(), cRect.Width())*0.8;
	cRect.SetRect(CPoint(center - CPoint(width / 2, width / 2)), center + CPoint(width / 2, width / 2));

	cBrush.CreateSolidBrush(RGB(232, 17, 35));
	auto tmp = dc.SelectObject(cBrush);
	dc.SelectObject(nullPen);
	dc.Rectangle(cRect);
	dc.SelectObject(whitePen);
	dc.MoveTo(cRect.TopLeft());
	dc.LineTo(CPoint(cRect.BottomRight()-CPoint(2,2)));
	dc.MoveTo(cRect.TopLeft().x, cRect.BottomRight().y-2);
	dc.LineTo(cRect.BottomRight().x-2, cRect.TopLeft().y);
}



void CAIproject1Dlg::drawLucky(int row, int col)
{
	CClientDC dc(this);
	CBrush cBrush;
	cBrush.CreateSolidBrush(RGB(255, 204, 0));
	dc.SelectObject(cBrush);
	dc.SelectObject(blackPen);

	CRect cRect = getRect(make_pair(row, col));
	auto center = cRect.CenterPoint();
	int width = min(cRect.Height(), cRect.Width()) * 0.8;
	cRect.SetRect(CPoint(center - CPoint(width / 2, width / 2)), center + CPoint(width / 2, width / 2));

	dc.Ellipse(cRect);
	dc.MoveTo(center.x + width / 4 * sin(45/PI), center.y + width / 4 * cos(45/PI));
	dc.AngleArc(center.x, center.y, width/4, 45, 270);

}


void CAIproject1Dlg::drawDest(int row, int col)
{
	CClientDC dc(this);
	CBrush cBrush;
	CRect cRect = getRect(make_pair(row, col));
	cBrush.CreateSolidBrush(RGB(149, 219, 125));
	
	dc.SelectObject(cBrush);
	dc.SelectObject(nullPen);

	dc.Rectangle(cRect);
}

void CAIproject1Dlg::OnBnClickedButtonDisplay()
{
	showResult = !showResult;
	draw();
}


void CAIproject1Dlg::drawResult()
{
	drawMaze();

	auto row = maze->getRow(), col = maze->getCol();
	for (int i = 0; i != row; i++)
	{
		for (int j = 0; j != col; j++)
		{
			drawCellResult(i, j, false);
		}
	}

	auto result = agent->getResult();
	for (auto i : result)
	{
		drawDirection(i.first.first, i.first.second, i.second);
	}
}


void CAIproject1Dlg::OnBnClickedButtonDefault()
{
	Helper::V_TRAP = -1;
	Helper::V_DEST = 1;
	Helper::V_LUCKY = 0.1;
	Helper::PROB_S = 0.6;
	Helper::PROB_L = 0.2;
	Helper::PROB_R = 0.2;
	Helper::DISCOUNT = 0.99;
	Helper::EPSILON = 1;
	Helper::LEARNING_RATE = 0.9;
	initMaze();
	agent->setMaze(maze);
	agent->clearResult();
	draw();
	setStatus(READY);
}


void CAIproject1Dlg::draw()
{
	if (maze == nullptr) return;
	if (showResult) drawResult();
	else drawMaze();
}


void CAIproject1Dlg::drawCellResult(int r, int c, bool localRedraw, Direction d)
{
	if (localRedraw) drawCell(r, c);

	CClientDC dc(this);
	dc.SetTextColor(colorBlue);
	dc.SetBkMode(TRANSPARENT);
	dc.SelectObject(bluePen);
	dc.SelectObject(nullBrush);
	dc.SelectObject(font);

	CString s;
	auto tmp = maze->getPoint(r, c);
	s.Format(L"%.2f", tmp.value + tmp.reward);
	auto rect = getRect(make_pair(r, c));
	dc.Rectangle(&rect);

	if (d != null) drawDirection(r, c, d);

	rect.left++;
	rect.right--;
	rect.top++;
	rect.bottom--;
	if (tmp.type != WALL && tmp.type != UNDEF)
	{
		dc.DrawText(s, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}
	
}


void CAIproject1Dlg::redraw(int r, int c)
{
	CClientDC dc(this);
	dc.SelectObject(whiteBrush);
	dc.SelectObject(whitePen);
	dc.Rectangle((CRect)getRect(make_pair(r, c)));
	if (showResult)
	{
		drawCellResult(r, c);
	}
	else
	{
		drawCell(r, c);
	}
}


void CAIproject1Dlg::drawCell(int r, int c)
{
	if (r == row - 1 && c == col - 1) drawDest(r, c);
	else if (r == 0 && c == 0) drawOrigin(r, c);
	else {
		switch (maze->getPoint(r, c).type)
		{
		case WALL: drawWall(r, c); break;
		case TRAP: drawTrap(r, c); break;
		case LUCKY: drawLucky(r, c); break;
		}
	}
}


void CAIproject1Dlg::OnBnClickedButtonAutorun()
{
	setStatus(RUNNING);
	agent->iteration(15,true);
	showResult = true;
	draw();
	setStatus(RUNNED);
}


void CAIproject1Dlg::OnBnClickedRadio(UINT uID)
{
	algo = uID - IDC_RADIO1;
	agent->setAlgo(algo);
	if (status == RUNNED)
	{
		agent->clearResult();
		maze->clearEst();
		draw();
	}
	setStatus(READY);
}


void CAIproject1Dlg::OnBnClickedButtonSeq(UINT uID)
{
	unsigned times;
	setStatus(RUNNING);
	switch (uID)
	{
	case IDC_BUTTON_SEQ1: times = 1; break;
	case IDC_BUTTON_SEQ10: times = 10; break;
	case IDC_BUTTON_SEQ50: times = 50; break;
	default: throw(-1);
	}
	agent->iteration(times, false);
	showResult = true;
	draw();
	setStatus(RUNNED);
}


void CAIproject1Dlg::OnBnClickedButtonRev(UINT uID)
{
	unsigned times;
	setStatus(RUNNING);
	switch (uID)
	{
	case IDC_BUTTON_REV1: times = 1; break;
	case IDC_BUTTON_REV10: times = 10; break;
	case IDC_BUTTON_REV50: times = 50; break;
	default: throw(-1);
	}
	agent->iteration(times, true);
	showResult = true;
	draw();
	setStatus(RUNNED);
}


void CAIproject1Dlg::drawOrigin(int row, int col)
{
	CClientDC dc(this);
	CBrush cBrush;
	CRect cRect = getRect(make_pair(row, col));
	cBrush.CreateSolidBrush(RGB(255, 216, 104));

	dc.SelectObject(cBrush);
	dc.SelectObject(nullPen);

	dc.Rectangle(cRect);
}


void CAIproject1Dlg::OnBnClickedButtonChange()
{
	ChangeSizeDlg dlg;
	dlg.preset(row, col);
	if (dlg.DoModal() == IDOK)
	{
		int r = _ttoi(dlg.v_row);
		int c = _ttoi(dlg.v_col);
		if (row != r || col != c)
		{
			row = r, col = c;
			auto oldMaze = maze;
			maze = new Maze(r, c);
			agent->setMaze(maze);
			delete oldMaze;
			draw();
			setStatus(READY);
		}
	}
}


void CAIproject1Dlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (point.x > orgX && point.y > orgY && point.x < orgX + length && point.y < orgY + width)
	{
		auto grid = getGrid(make_pair(point.x, point.y));
		if (grid.first == row - 1 && grid.second == col - 1)
		{
			AfxMessageBox(L"不可以改变终点的类型!"); return;
		}
		else if (grid.first == 0 && grid.second == 0)
		{
			AfxMessageBox(L"不可以改变起点的类型!"); return;
		}
		if (status == RUNNED)
		{
			maze->clearEst();
			agent->clearResult();
			setStatus(READY);
			draw();
		}
		auto oldPoint = maze->getPoint(grid);
		if (oldPoint.type == ROAD) typeToSet=WALL;
		else typeToSet=ROAD;
		maze->setCell(grid.first, grid.second, typeToSet);
		redraw(grid.first, grid.second);
	}

	CDialogEx::OnLButtonDown(nFlags, point);
}


void CAIproject1Dlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if ((nFlags & MK_LBUTTON || nFlags & MK_RBUTTON)
		&& point.x > orgX && point.y > orgY && point.x < orgX + length && point.y < orgY + width)
	{
		auto grid = getGrid(make_pair(point.x, point.y));
		if (!(grid.first == 0 && grid.second == 0) && !(grid.first == row - 1 && grid.second == col - 1))
		{
			maze->setCell(grid.first, grid.second, typeToSet);
			redraw(grid.first, grid.second);
		}
	}

	CDialogEx::OnMouseMove(nFlags, point);
}


void CAIproject1Dlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	if (point.x > orgX && point.y > orgY && point.x < orgX + length && point.y < orgY + width)
	{
		auto grid = getGrid(make_pair(point.x, point.y));
		if (grid.first == row - 1 && grid.second == col - 1)
		{
			AfxMessageBox(L"不可以改变终点的类型!"); return;
		}
		else if (grid.first == 0 && grid.second == 0)
		{
			AfxMessageBox(L"不可以改变起点的类型!"); return;
		}
		if (status == RUNNED)
		{
			maze->clearEst();
			agent->clearResult();
			setStatus(READY);
			draw();
		}
		auto oldPoint = maze->getPoint(grid);
		if (oldPoint.type == ROAD) typeToSet=TRAP;
		else if (oldPoint.type == TRAP) typeToSet=LUCKY;
		else typeToSet=ROAD;
		maze->setCell(grid.first, grid.second, typeToSet);
		redraw(grid.first, grid.second);
		
	}

	CDialogEx::OnRButtonDown(nFlags, point);
}


void CAIproject1Dlg::drawDirection(int r, int c, Direction d)
{
	CClientDC dc(this);
	dc.SetTextColor(RGB(242, 71, 30));
	dc.SetBkMode(TRANSPARENT);
	dc.SelectObject(bigFont);

	CString s;

	auto rect = getRect(make_pair(r, c));
	//rect.left-=2;
	//rect.right+=2;
	//rect.top-=2;
	//rect.bottom+=2;

	switch (d)
	{
	case UP: dc.DrawText(L"↑", &rect, DT_LEFT | DT_TOP | DT_SINGLELINE); break;
	case RIGHT: dc.DrawText(L"→", &rect, DT_RIGHT | DT_BOTTOM | DT_SINGLELINE); break;
	case DOWN: dc.DrawText(L"↓", &rect, DT_RIGHT | DT_BOTTOM | DT_SINGLELINE); break;
	case LEFT: dc.DrawText(L"←", &rect, DT_LEFT | DT_TOP | DT_SINGLELINE); break;
	default: throw(-1);
	}
}


void CAIproject1Dlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (m_bar.GetCount() != 0)
	{
		m_bar.SetPaneInfo(0, IDS_BAR_STATUS, SBPS_NORMAL, 150);
		m_bar.SetPaneInfo(1, IDS_BAR_STEP, SBPS_STRETCH, 0);
		RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, IDS_BAR_STEP);
	}
	
	if (m_listRect.GetCount() > 0)
	{
		CRect dlgNow;
		GetWindowRect(&dlgNow);
		POSITION pos = m_listRect.GetHeadPosition();//第一个保存的是对话框的Rect
		CRect dlgSaved;
		dlgSaved = m_listRect.GetNext(pos);
		ScreenToClient(dlgNow);
		double x = dlgNow.Width() * 1.0 / dlgSaved.Width();//根据当前和之前保存的对话框的宽高求比例
		double y = dlgNow.Height() * 1.0 / dlgSaved.Height();
		ClientToScreen(dlgNow);
		CRect childSaved;
		CWnd* pWnd = GetWindow(GW_CHILD);
		while (pWnd)
		{
			if (pos == nullptr) break;
			childSaved = m_listRect.GetNext(pos);//依次获取子窗体的Rect
			childSaved.left = (LONG)(dlgNow.left + (1.0 * childSaved.left - dlgSaved.left) * x);//根据比例调整控件上下左右距离对话框的距离
			childSaved.right = (LONG)(dlgNow.right + (1.0 * childSaved.right - dlgSaved.right) * x);
			childSaved.top = (LONG)(dlgNow.top + (1.0 * childSaved.top - dlgSaved.top) * y);
			childSaved.bottom = (LONG)(dlgNow.bottom + (1.0 * childSaved.bottom - dlgSaved.bottom) * y);
			ScreenToClient(childSaved);
			pWnd->MoveWindow(childSaved);
			CRect rect;/*
			if (IDC_STATIC_KEY == pWnd->GetDlgCtrlID())
			{
				rect.top = 0;
				rect.bottom = childSaved.bottom - childSaved.top;
				rect.left = 0;
				rect.right = childSaved.right - childSaved.left;
				m_flueDlg.MoveWindow(&rect);
				m_flueDlg.ShowWindow(SW_SHOW);
			}*/
			pWnd = pWnd->GetNextWindow();
		}
		CRect rect;
		GetDlgItem(IDC_MAZE)->GetWindowRect(&rect);
		ScreenToClient(&rect);
		orgX = rect.left + 60;
		orgY = rect.top + 50;
		length = rect.Width() - 120;
		width = rect.Height() - 80;
	}
}


void CAIproject1Dlg::setStatus(Status s)
{
	status = s;
	CString str;
	auto tmp = agent->getCount();
	str.Format(L"已正向迭代%d次, 反向迭代%d次, 共%d次", tmp.first, tmp.second, tmp.first + tmp.second);
	m_bar.SetPaneText(1, str);
	if (s == READY)
	{
		m_bar.SetPaneText(0, L"Ready");
	}
	else if (s == RUNNED)
	{
		m_bar.SetPaneText(0, L"Runned");
	}
	else if (s == RUNNING)
	{
		m_bar.SetPaneText(0, L"Running");
	}
}


void CAIproject1Dlg::OnBnClickedButtonChangeArg()
{
	ArgChangeDlg dlg;
	CString s;
	s.Format(L"%.3f", Helper::V_TRAP);
	dlg.v_v_trap = s;
	s.Format(L"%.3f", Helper::V_LUCKY);
	dlg.v_v_lucky = s;
	s.Format(L"%.3f", Helper::V_DEST);
	dlg.v_v_dest = s;
	s.Format(L"%.3f", Helper::PROB_S);
	dlg.v_prob_s = s;
	s.Format(L"%.3f", Helper::PROB_L);
	dlg.v_prob_l = s;
	s.Format(L"%.3f", Helper::DISCOUNT);
	dlg.v_discount = s;
	s.Format(L"%.3f", Helper::EPSILON);
	dlg.v_epsilon = s;
	s.Format(L"%.3f", Helper::LEARNING_RATE);
	dlg.v_learning_rate = s;

	if (dlg.DoModal() == IDOK)
	{
		double tmp1, tmp2;
		Helper::V_TRAP = _ttof(dlg.v_v_trap);
		Helper::V_LUCKY = _ttof(dlg.v_v_lucky);
		Helper::V_DEST = _ttof(dlg.v_v_dest);
		tmp1 = _ttof(dlg.v_prob_s);
		tmp2 = _ttof(dlg.v_prob_l);
		Helper::PROB_S = tmp1;
		Helper::PROB_L = tmp2;
		Helper::PROB_R = 1 - tmp1 - tmp2;
		Helper::DISCOUNT = _ttof(dlg.v_discount);
		Helper::EPSILON = _ttof(dlg.v_epsilon);
		Helper::LEARNING_RATE = _ttof(dlg.v_learning_rate);
		agent->clearResult();
		maze->reload();
		draw();
		setStatus(READY);
	}

}
