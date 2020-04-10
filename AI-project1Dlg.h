
// AI-project1Dlg.h: 头文件
//

#pragma once

// CAIproject1Dlg 对话框
class CAIproject1Dlg : public CDialogEx
{
// 构造
public:
	CAIproject1Dlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_AIPROJECT1_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    // 在指定格点画墙
    void drawWall(int row, int col);
    void drawMaze();
    afx_msg void OnBnClickedButtonReset();
    void drawTrap(int row, int col);
    CPen blackPen;
	CBrush blackBrush;
	CPen *nullPen;
	CBrush whiteBrush;
	void drawLucky(int row, int col);
	void drawDest(int row, int col);
	CPen whitePen;
//	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
//	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnBnClickedButtonDisplay();
    CBrush* nullBrush;
	void drawResult();
	afx_msg void OnBnClickedButtonDefault();
	void draw();
	CPen bluePen;
	COLORREF colorBlue;
	void drawCellResult(int row, int col);
	void redraw(int row, int col);
	void drawCell(int row, int col);
	afx_msg void OnBnClickedButtonAutorun();
	afx_msg void OnBnClickedRadio(UINT uID);
	afx_msg void OnBnClickedButtonSeq(UINT uID);
	afx_msg void OnBnClickedButtonRev(UINT uID);
	void drawOrigin(int row, int col);
	afx_msg void OnBnClickedButtonChange();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
};
