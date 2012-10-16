#include "include.h"
#include "graph.h"

#define WM_ASYNC_IO (WM_USER + 1)

class CApp : public CWinApp
{
public:
	virtual BOOL InitInstance();
};


class CMainWindow : public CFrameWnd
{
public:
	CMainWindow();
	void initialize_gdi();
	void initialize_data();
	void draw_window(CPaintDC &dc);
	~CMainWindow();

protected:
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk (UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg LRESULT OnAsyncIo(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTimer(UINT nTimerID);
	afx_msg void OnFileNew();
	afx_msg void OnFileOpen();
	afx_msg void OnFileSave();
	afx_msg void OnFileClose();
	afx_msg void OnEditCut();
	afx_msg void OnEditCopy();
	afx_msg void OnEditPaste();
	afx_msg void OnFilePrint();
	afx_msg void OnAppAbout();

	DECLARE_MESSAGE_MAP()
private:
	CMenu		menu;
//	CToolBar	toolbar;
	HBRUSH		red_brush[3], blue_brush[3], orange_brush[3], white_brush[3];
	HPEN		red_pen;
	HPEN		blue_pen;
	Graph		graph;
	node_t		*node;
	dbh_t		*header;
	ref_t		*ref;
	position_t	list[32];
	int		list_size;
	int		cxClient;
	int		cyClient;
	float		x_active;
	float		y_active;
	float		x_click;
	float		y_click;
	WSADATA		WSAData;
	int		sock;
	struct sockaddr_in	servaddr;
};

