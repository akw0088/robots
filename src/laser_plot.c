#include <windows.h>

#include "include.h"

#define M_PI 3.141592653589793238462643

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

/*
// angular range degrees 239.77f
Protocol (SCIP): 2.0 
Serial Number: H0805583 
Model: URG-04LX 
Vendor: Hokuyo Automatic Co.,Ltd. 
Firmware: 3.3.00 
Angular Step Resolution: 1024 
Min Measurement Step: 44 
Max Measurement Step: 725 
Min Distance: 20 
Max Distance Step: 5600 
Reviewing laser data
*/

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char *szCmdLine, int iCmdShow)
{
     static TCHAR szAppName[] = TEXT("laser_plot");
     HWND         hwnd;
     MSG          msg;
     WNDCLASS     wndclass;
     
     wndclass.style         = CS_HREDRAW | CS_VREDRAW;
     wndclass.lpfnWndProc   = WndProc;
     wndclass.cbClsExtra    = 0;
     wndclass.cbWndExtra    = 0;
     wndclass.hInstance     = hInstance;
     wndclass.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
     wndclass.hCursor       = LoadCursor(NULL, IDC_ARROW);
     wndclass.hbrBackground =(HBRUSH) GetStockObject(WHITE_BRUSH);
     wndclass.lpszMenuName  = NULL;
     wndclass.lpszClassName = szAppName;
          
     if (!RegisterClass(&wndclass))
     {
          MessageBox(NULL, TEXT("RegisterClass() Failed"), szAppName, MB_ICONERROR);
          return 0;
     }
     
     hwnd = CreateWindow(szAppName, TEXT("laser plot"),
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          NULL, NULL, hInstance, NULL);
     
     ShowWindow(hwnd, iCmdShow);
     UpdateWindow(hwnd);
     
     while(GetMessage(&msg, NULL, 0, 0))
     {
          TranslateMessage(&msg);
          DispatchMessage(&msg);
     }
     return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int  cxClient, cyClient;
	static laserscan_t *scan;
	static int num_scan;
	static int xmouse = 0;
	static int ymouse = 0;
	static HBRUSH brush[5];
	static float zoom = 1.0;
	static float x_shift = 0.0f;
	static float y_shift = 0.0f;
	static int rotate_shift = 0;
	COLORREF ref;

	switch(message)
	{
	case WM_CREATE:
	{
		char *data = get_file("laser.bin");
		num_scan = *((int *)data);
		scan = (laserscan_t *)(data + 4);
		SetTimer(hwnd, 3, 1000, NULL);
		return 0;
	}

	case WM_MOUSEMOVE:
		xmouse = LOWORD(lParam) - cxClient / 2;
		ymouse = HIWORD(lParam) - cyClient / 2;
		return 0;
	case WM_MOUSEWHEEL:
	{
		short int distance = HIWORD(wParam);
		short int flag = LOWORD(wParam);


		if (flag & MK_SHIFT)
		{
			if ( distance > 0)
				x_shift += 0.1f;
			else
				x_shift -= 0.1f;
		}
		else if (flag & MK_CONTROL)
		{
			if ( distance > 0)
				y_shift += 0.1f;
			else
				y_shift -= 0.1f;
		}
		else if (flag & MK_RBUTTON)
		{
			if ( distance > 0)
				rotate_shift++;
			else
				rotate_shift--;
		}
		else
		{
			if ( distance > 0)
				zoom += 0.1f;
			else
				zoom -= 0.1f;
		}

		if (zoom <= 0.0)
			zoom = 0.1f;
		return 0;
	}

	case WM_SIZE:
		cyClient = LOWORD(lParam);
		cxClient = HIWORD(lParam);
		return 0;

	case WM_TIMER:
		InvalidateRect(hwnd, NULL, TRUE);
		return 0;
          
	case WM_PAINT:
	{
		POINT		apt[4096];
		HDC		hdc;
		PAINTSTRUCT	ps;
		int		i, j, k;
		int		size = MIN(cxClient, cyClient);
		char		xstr[80];
		char		ystr[80];
		char		rstr[80];

		sprintf(xstr, "x shift %f", x_shift);
		sprintf(ystr, "y shift %f", y_shift);
		sprintf(rstr, "r shift %d", rotate_shift);

		hdc = BeginPaint(hwnd, &ps);

		for(j = 0; j < num_scan; j++)
		{
			for(i = 0; i < 681; i++)
			{
				float hw = 1;
				float x = scan[j].point[i].x;
				float y = scan[j].point[i].y;

				// rotate points by heading
				float sin_val = sin((scan[j].heading + rotate_shift) * (M_PI / 180.0) );
				float cos_val = cos((scan[j].heading + rotate_shift) * (M_PI / 180.0) );

				x = x * cos_val - y * sin_val;
				y = x * sin_val + y * cos_val;

				apt[i].x = x * size * zoom + (cxClient / 2.0) + scan[j].y * ( -x_shift * zoom * cyClient / 32.0) + xmouse;
				apt[i].y = y * size * zoom + (cyClient / 2.0) + scan[j].x * ( -y_shift * zoom * cyClient / 32.0) + ymouse;
				Ellipse(hdc, apt[i].x - hw, apt[i].y + hw, apt[i].x + hw, apt[i].y - hw);
			}
		}



		TextOut(hdc, 5, 5, xstr, lstrlen(xstr));
		TextOut(hdc, 5, 25, ystr, lstrlen(ystr));
		TextOut(hdc, 5, 45, rstr, lstrlen(rstr));
		EndPaint(hwnd, &ps);
		return 0;
	}
          
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}
