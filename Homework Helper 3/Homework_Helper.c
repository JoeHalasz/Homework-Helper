
















#include <stdio.h>


// BELOW THIS IS STUFF TO MAKE THE WINDOW

#include <windows.h>


HWND hwnd;
LRESULT CALLBACK WndProc(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam);

HWND children[1024];
int numChildren = 0;

int WIDTH = 450;
int HEIGHT = 650;



RECT editSize = {0,0,0,0};

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	editSize.left = 1920 - WIDTH - 25;
	editSize.top = 25;
	editSize.right = editSize.left + WIDTH;
	editSize.bottom = editSize.top + HEIGHT;

  	MSG         Msg;
    WNDCLASSEX  WndClsEx = {0};

    WndClsEx.cbSize        = sizeof (WNDCLASSEX);
    WndClsEx.style         = CS_HREDRAW | CS_VREDRAW;
    WndClsEx.lpfnWndProc   = WndProc;
    WndClsEx.hInstance     = hInstance;
    WndClsEx.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    WndClsEx.lpszClassName = "ClassName";
    WndClsEx.hIconSm       = LoadIcon(hInstance, IDI_APPLICATION);

    RegisterClassEx(&WndClsEx);

    hwnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW,
                          "ClassName",
                          "Homework Helper",
                          WS_OVERLAPPEDWINDOW,
                          1920 - WIDTH - 25,
                          25,
                          WIDTH,
                          HEIGHT,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hwnd, nShowCmd);
    UpdateWindow(hwnd);

    while(GetMessage(&Msg, NULL, 0, 0))
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
	return 0;
}

#define ID_Edit1  200 


BOOL CALLBACK EnumChildProc(HWND hwndChild, LPARAM lParam)
{
    int idChild;
    idChild = GetWindowLong(hwndChild, GWL_ID);
    LPRECT rcParent;
    rcParent = (LPRECT)lParam;

    if (idChild == ID_Edit1) {

        //Calculate the change ratio
        double cxRate = rcParent->right * 1.0 / WIDTH; //884 is width of client area
        double cyRate = rcParent->bottom * 1.0 / HEIGHT; //641 is height of client area

        LONG newRight = editSize.left * cxRate;
        LONG newTop = editSize.top * cyRate;
        LONG newWidth = editSize.right * cxRate;
        LONG newHeight = editSize.bottom * cyRate;

        MoveWindow(hwndChild, newRight, newTop, newWidth, newHeight, TRUE);

        // Make sure the child window is visible. 
        ShowWindow(hwndChild, SW_SHOW);
    }
    return TRUE;
}

int first = 0;
int BUTTON_WIDTH = 120;

LRESULT CALLBACK WndProc(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	HWND static hwndTextBox, hwndTitle, hwndButtonAdd, hwndButtonRemove, hwndButtonMove, hwndHelpMessage;
	RECT rcClient;
	RECT MainRect;
	int width = WIDTH;
	int height = HEIGHT;
	RECT rc;
    switch(Msg) 
	{
		case WM_SIZE:
			GetWindowRect(hwnd, &MainRect);
			width = MainRect.right - MainRect.left;
			height = MainRect.bottom - MainRect.top;
			// help message
			SetWindowPos(children[0], hwnd, 20, height-175, width - 60, 40, SWP_SHOWWINDOW | SWP_NOZORDER | SWP_FRAMECHANGED);
			// title
			SetWindowPos(children[1], hwnd, (width/2) - 150, 20, 300, 40, SWP_SHOWWINDOW | SWP_NOZORDER | SWP_FRAMECHANGED);
			// text box
			SetWindowPos(children[2], hwnd, 20, height-145, width - 60, 40, SWP_SHOWWINDOW | SWP_NOZORDER | SWP_FRAMECHANGED);
			// add button
			SetWindowPos(children[3], hwnd, 20, height-100, BUTTON_WIDTH, 40, SWP_SHOWWINDOW | SWP_NOZORDER | SWP_FRAMECHANGED);
			// remove button
			SetWindowPos(children[4], hwnd, (width/2)-(BUTTON_WIDTH/2)-10, height-100, BUTTON_WIDTH, 40, SWP_SHOWWINDOW | SWP_NOZORDER | SWP_FRAMECHANGED);
			// move button
			SetWindowPos(children[5], hwnd, (width - BUTTON_WIDTH) - 40, height-100, BUTTON_WIDTH, 40, SWP_SHOWWINDOW | SWP_NOZORDER | SWP_FRAMECHANGED);
			
		case WM_CREATE:

			hwndHelpMessage = CreateWindow( // help message
				"STATIC", TEXT("Name  Weekday  Day_number  Month(blank if n/a)"),WS_CHILD, 
				0,0,0,0, // pos is created by the WM_SIZE above
				hwnd, (HMENU) 1,NULL, NULL
			);
			children[numChildren++] = hwndHelpMessage;
	
			hwndTitle = CreateWindow( // title at the top 
				"STATIC", "Homework Helper",WS_CHILD, 
				0,0,0,0, // pos is created by the WM_SIZE above
				hwnd, (HMENU) 1,NULL, NULL
			);
			children[numChildren++] = hwndTitle;

			hwndTextBox = CreateWindow( // text box
				TEXT("edit"), NULL,WS_CHILD | WS_BORDER, 
				0,0,0,0, // pos is created by the WM_SIZE above
				hwnd, (HMENU) 1,NULL, NULL
			);
			children[numChildren++] = hwndTextBox;

			hwndButtonAdd = CreateWindow( // add button
				TEXT("button"), TEXT("Add"),WS_CHILD, 
				0,0,0,0, // pos is created by the WM_SIZE above
				hwnd, (HMENU) 2,NULL, NULL
			);
			children[numChildren++] = hwndButtonAdd;

			hwndButtonRemove = CreateWindow( // remove button
				TEXT("button"), TEXT("Remove"),WS_CHILD, 
				0,0,0,0, // pos is created by the WM_SIZE above
				hwnd, (HMENU) 2,NULL, NULL
			);
			children[numChildren++] = hwndButtonRemove;

			hwndButtonMove = CreateWindow( // move button
				TEXT("button"), TEXT("Move"),WS_CHILD, 
				0,0,0,0, // pos is created by the WM_SIZE above
				hwnd, (HMENU) 2,NULL, NULL
			);
			children[numChildren++] = hwndButtonMove;
			
			
			// create a font for the title and text box
			LOGFONT logfont; 
			ZeroMemory(&logfont, sizeof(LOGFONT));
			logfont.lfHeight = -35; 
			HFONT hFont = CreateFontIndirect(&logfont);
			ZeroMemory(&logfont, sizeof(LOGFONT));
			logfont.lfHeight = -17; 
			HFONT hFont2 = CreateFontIndirect(&logfont);
			ZeroMemory(&logfont, sizeof(LOGFONT));
			logfont.lfHeight = -30;
			HFONT hFont3 = CreateFontIndirect(&logfont);

			SendMessage(hwndTextBox, WM_SETFONT, (WPARAM)hFont3, TRUE); // set font for text box
			
			// set fonts
			SendMessage(hwndTitle, WM_SETFONT, (WPARAM)hFont, TRUE);
			SendMessage(hwndHelpMessage, WM_SETFONT, (WPARAM)hFont2, TRUE);
			SendMessage(hwndButtonAdd, WM_SETFONT, (WPARAM)hFont2, TRUE);
			SendMessage(hwndButtonRemove, WM_SETFONT, (WPARAM)hFont2, TRUE);
			SendMessage(hwndButtonMove, WM_SETFONT, (WPARAM)hFont2, TRUE);

			// set text and background colors
			SendMessage(hwndTitle, WM_CTLCOLORSTATIC, (WPARAM)RGB(0,0,0), TRUE);
			SendMessage(hwndButtonAdd, WM_CTLCOLORSTATIC, (WPARAM)RGB(0,0,0), TRUE);
			SendMessage(hwndButtonRemove, WM_CTLCOLORSTATIC, (WPARAM)RGB(0,0,0), TRUE);
			SendMessage(hwndButtonMove, WM_CTLCOLORSTATIC, (WPARAM)RGB(0,0,0), TRUE);
			SendMessage(hwndHelpMessage, WM_CTLCOLORSTATIC, (WPARAM)RGB(0,0,0), TRUE);
			// SendMessage(hwndButtonAdd, WM_CTLCOLORSTATIC, (WPARAM)RGB(0,0,0), TRUE);
			
			break;

		case WM_CTLCOLORSTATIC: // set background color for the title
                SetBkColor((HDC)wParam, RGB(60, 60, 60));
                SetTextColor((HDC)wParam, RGB(255,255,255));
                return (LRESULT)CreateSolidBrush(RGB(60, 60, 60));
		case WM_ERASEBKGND: {
			HDC hdc = (HDC)(wParam); 
			RECT rc; 
			GetClientRect(hwnd, &rc); 
			HBRUSH brush = CreateSolidBrush(RGB(60, 60, 60));// grey
			FillRect(hdc, &rc, brush); 
			DeleteObject(brush); // Free the created brush: see note below!
			return TRUE;
		}
		case WM_COMMAND:
			// TODO responds to button click
			// if (HIWORD(wParam) == BN_CLICKED) {
			// 	const int maxtextlength = 30;
			// 	TCHAR textvalue[100] = TEXT("");
			// 	//gets value of textbox and stores in  'textvalue'
			// 	GetWindowText(hwndTextBox, textvalue, maxtextlength);
			// 	//changes text in static box to value in 'textvalue'
			// 	SetWindowText(hwndStatic, textvalue);
			// }
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hwnd, Msg, wParam, lParam);
    }
    return 0;
}



