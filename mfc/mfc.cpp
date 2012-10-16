/*
	mfc lib's linked automatically, just set subsystem stupid.
*/

#include <afxwin.h>
#include <afxext.h>
#include <Commctrl.h>
#include <math.h>
#include "mfc.h"
#include "resource.h"

BEGIN_MESSAGE_MAP(CMainWindow, CFrameWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_TIMER()
	ON_COMMAND(ID_FILE_CLOSE, OnFileClose)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_MESSAGE(WM_ASYNC_IO, OnAsyncIo)
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

char *get_file(char *file_name)
{
	FILE	*file;
	char	*buffer;
	int	fSize, bRead;

	file = fopen(file_name, "rb");
	if (file == NULL)
		return 0;
	fseek(file, 0, SEEK_END);
	fSize = ftell(file);
	fseek(file, 0, SEEK_SET);
	buffer = (char *) malloc( fSize * sizeof(char) + 1 );
	bRead = (int)fread(buffer, sizeof(char), fSize, file);
	if (bRead != fSize)
		return 0;
	fclose(file);
	buffer[fSize] = '\0';
	return buffer;
}

void CMainWindow::initialize_gdi()
{
		red_brush[0] = CreateSolidBrush(RGB(127,0,0));
		red_brush[1] = CreateSolidBrush(RGB(191,0,0));
		red_brush[2] = CreateSolidBrush(RGB(255,0,0));

		blue_brush[0] = CreateSolidBrush(RGB(0,0,127));
		blue_brush[1] = CreateSolidBrush(RGB(0,0,191));
		blue_brush[2] = CreateSolidBrush(RGB(0,0,255));

		orange_brush[0] = CreateSolidBrush(RGB(127,63,0));
		orange_brush[1] = CreateSolidBrush(RGB(191,95,0));
		orange_brush[2] = CreateSolidBrush(RGB(255,127,0));

		white_brush[0] = CreateSolidBrush(RGB(127,127,127));
		white_brush[1] = CreateSolidBrush(RGB(191,191,191));
		white_brush[2] = CreateSolidBrush(RGB(200,200,200));

		red_pen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
		blue_pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
}

void CMainWindow::initialize_data()
{
	char	*data;

	data = (char *)get_file("db.bin");
	if (data == NULL)
	{
		MessageBox("Unable to load db.bin", "Error!", 0);
		PostMessage(WM_DESTROY, 0, 0);
	}

	header = (dbh_t *)data;
	ref = (ref_t *)(data + sizeof(dbh_t));

	node = (node_t *)get_file("graph.bin");
	if (node == NULL)
	{
		MessageBox("Unable to load graph.bin", "Error!", 0);
		PostMessage(WM_DESTROY, 0, 0);
	}

	if (node)
	{
		graph.load(node, header->num_refpoints);
	}

	memset(list, -1, sizeof(position_t) * 32);
	list[0].x = 1;
	list[0].y = 1;
	list[0].color = 2;
	list[0].intensity = 3;
	list_size = 1;


	x_click = 0.0f;
	y_click = 0.0f;

	x_active = 0.0f;
	y_active = 0.0f;

}

void CMainWindow::OnTimer(UINT nTimerID)
{
	int erase = 0;

	for (int i = 0; i < list_size; i++)
	{
		if (list[i].intensity > 0)
		{
			list[i].intensity--;
			erase = 1;
		}
		else
		{
			list[i].intensity = -1;
			list_size--;
			for(int j = i; j < list_size; j++)
			{
				list[j] = list[j+1];
			}
		}
	}

//	if (erase)
	{
		Invalidate();
		UpdateWindow();
	}
}


void CMainWindow::draw_window(CPaintDC &dc)
{
	RECT		rect = {cxClient * 0.4f, 0, cxClient - 15, cyClient * 0.3f};
	char		buffer[1024];
	int		i, j;
	static		int *path = NULL;
	static		int start = 0;
	static		int end = 286;
	static		int path_length = 0;

	for(i = 0; i < header->num_refpoints; i++)
	{
		float	x = (ref[i].x + header->xoffset) * cxClient / header->width;
		float	y = cyClient - (ref[i].y + header->yoffset + 1) * cyClient / header->height;

		dc.SelectObject(GetStockObject(NULL_PEN));
		dc.SelectObject(GetStockObject(BLACK_BRUSH));
		for(j = 0; j < list_size; j++)
		{
			if (ref[i].x == list[j].x && ref[i].y == list[j].y)
			{
				if (list[j].intensity == 3)
				{
					switch(list[j].color)
					{
					case 0:
						dc.SelectObject(red_brush[2]);
						break;
					case 1:
						dc.SelectObject(blue_brush[2]);
						break;
					case 2:
						dc.SelectObject(orange_brush[2]);
						break;
					case 3:
						dc.SelectObject(white_brush[2]);
						break;
					}
					break;
				}
				else if (list[j].intensity == 2)
				{
					switch(list[j].color)
					{
					case 0:
						dc.SelectObject(red_brush[1]);
						break;
					case 1:
						dc.SelectObject(blue_brush[1]);
						break;
					case 2:
						dc.SelectObject(orange_brush[1]);
						break;
					case 3:
						dc.SelectObject(white_brush[1]);
						break;
					}
					break;
				}
				else if (list[j].intensity == 1)
				{
					switch(list[j].color)
					{
					case 0:
						dc.SelectObject(red_brush[0]);
						break;
					case 1:
						dc.SelectObject(blue_brush[0]);
						break;
					case 2:
						dc.SelectObject(orange_brush[0]);
						break;
					case 3:
						dc.SelectObject(white_brush[0]);
						break;
					}
					break;
				}
			}
		}

		for(j = 0; j < path_length; j++)
		{
			if (i == path[j])
				dc.SelectObject(blue_brush[2]);
		}


		if (x_click > x && x_click < x + 15.0f)
		{
			if (y_click > y && y_click < y + 15.0f)
			{
				sprintf(buffer, "Reference points: (%d,%d) has %d cells\n", ref[i].x, ref[i].y, ref[i].num_cells);
				for(j = 0; j < ref[i].num_cells; j++)
				{
					sprintf(&buffer[strlen(buffer)], "    cell %d: %s %s S=%3.2f N=%3.2f N=%d\n", j+1, ref[i].cell[j].address, ref[i].cell[j].essid, ref[i].cell[j].signal, ref[i].cell[j].noise, ref[i].cell[j].num_samples);
				}
				dc.DrawText(buffer, strlen(buffer), &rect, 0);
				dc.SelectObject(red_pen);

				if (end != i)
				{
					if (node)
					{
						end = i;
						path = graph.astar_path(ref, start, end, &path_length);
					}
				}
			}
		}

		if (x_active > x && x_active < x + 15.0f)
		{
			if (y_active > y && y_active < y + 15.0f)
			{
				dc.SelectObject(blue_pen);

				if (start != i)
				{
					if (node)
					{
						start = i;
						path = graph.astar_path(ref, start, end, &path_length);
					}
				}
			}
		}


//		float plus_size = 200.0f / header->width;
		dc.Rectangle(x + 5.0f, y, x + 10.0f, y + 15.0f);
		dc.Rectangle(x, y + 5.0f, x + 15.0f, y + 10.0f);
/*
		float plus_size = 200.0f / header->width;
		dc.Rectangle(x + plus_size * 0.5f, y, x + plus_size, y + plus_size * 1.5f);
		dc.Rectangle(x, y + plus_size * 0.5f, x + plus_size * 1.5f, y + plus_size);
*/
	}
	sprintf(buffer, "%d points", list_size);
	dc.TextOut(5, cyClient - 15, buffer, strlen(buffer));
}



BOOL CApp::InitInstance()
{
	m_pMainWnd = new CMainWindow;
	m_pMainWnd->ShowWindow(m_nCmdShow);
	m_pMainWnd->UpdateWindow();
	InitCommonControls();
	return TRUE;
}

CMainWindow::CMainWindow()
{
	Create(NULL, _T("iwlist_plot"));
	menu.LoadMenu("mfcMenu");
	SetMenu(&menu);
/*
	toolbar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP |
		CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	toolbar.LoadToolBar("mfcToolBar");
	toolbar.LoadBitmap("mfcToolBarBitmap");
*/
	initialize_gdi();
	initialize_data();
	SetTimer(1, 1000, NULL);

	WSAStartup(MAKEWORD(2,0), &WSAData);
	sock = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock == -1)
		MessageBox("Failed to create socket. Program will not recieve packets.", "Socket Error", 0);

	memset(&servaddr, 0, sizeof(struct sockaddr_in));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);
	bind(sock, (sockaddr *)&servaddr, sizeof(struct sockaddr_in));
	WSAAsyncSelect(sock, m_hWnd, WM_ASYNC_IO, FD_READ);
}

CMainWindow::~CMainWindow()
{
	WSACleanup();
}

LRESULT CMainWindow::OnAsyncIo(WPARAM wParam, LPARAM lParam)
{
	WORD			wEvent, wError;
	struct sockaddr_in	clientaddr;
	int			socklen;
	int			data[3];
	char			buffer[80];

	wEvent = WSAGETSELECTEVENT(lParam);	
	wError = WSAGETSELECTERROR(lParam);

	switch (wEvent)
	{
	case FD_READ:
		recvfrom(sock, (char *)&data, 8, 0, (struct sockaddr *)&clientaddr, &socklen);
		sprintf(buffer, "(%d,%d)", data[0], data[1]);
		MessageBox(buffer, buffer, 0);
		list[list_size].x = data[0];
		list[list_size].y = data[1];
		list[list_size].color = data[2];
		list[list_size].intensity = 3;
		list_size++;
		break;
	}
	return 0;
}


void CMainWindow::OnPaint()
{
	CPaintDC dc(this);
    
	CRect rect;
	GetClientRect(&rect);
	cxClient = rect.right;
	cyClient = rect.bottom;
	draw_window(dc);
}

void CMainWindow::OnLButtonDown(UINT nFlags, CPoint point)
{
	x_click = point.x;
	y_click = point.y;
	Invalidate();
	UpdateWindow();
}

void CMainWindow::OnLButtonDblClk (UINT nFlags, CPoint point)
{
	x_active = point.x;
	y_active = point.y;
	Invalidate();
	UpdateWindow();
}


void CMainWindow::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{
	case VK_LEFT:
		x_click -= (float)cxClient / header->width;
		Invalidate();
		UpdateWindow();
		break;
	case VK_RIGHT:
		x_click += (float)cxClient / header->width;
		Invalidate();
		UpdateWindow();
		break;
	case VK_UP:
		y_click -= (float)cyClient / header->height;
		Invalidate();
		UpdateWindow();
		break;
	case VK_DOWN:
		y_click += (float)cyClient / header->height;
		Invalidate();
		UpdateWindow();
		break;
	}
}


void CMainWindow::OnFileNew()
{
    MessageBox("Clicked File->New");
}

void CMainWindow::OnFileOpen()
{
    MessageBox("Clicked File->OnFileOpen");
}

void CMainWindow::OnFileSave()
{
    MessageBox("Clicked File->OnFileSave");
}

void CMainWindow::OnFilePrint()
{
    MessageBox("Clicked File->OnFilePrint");
}

void CMainWindow::OnFileClose()
{
    MessageBox("Clicked File->OnFileClose");
}

void CMainWindow::OnEditCut()
{
    MessageBox("Clicked File->OnEditCut");
}

void CMainWindow::OnEditCopy()
{
    MessageBox("Clicked File->OnEditCopy");
}

void CMainWindow::OnEditPaste()
{
    MessageBox("Clicked File->OnEditPaste");
}

void CMainWindow::OnAppAbout()
{
    MessageBox("Clicked File->OnAppAbout");
}

// entry point
CApp	app;
