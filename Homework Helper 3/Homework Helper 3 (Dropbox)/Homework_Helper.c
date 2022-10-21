
#include <windows.h>
#include <shlobj.h>

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>


#define try bool __HadError=false;
#define catch(x) ExitJmp:if(__HadError)
#define throw(x) {__HadError=true;goto ExitJmp;}

HWND hwnd;

HWND windowElements[1024];

int NUM_ITEMS = 1;
BOOL loaded = FALSE;
HBRUSH bkbrush = NULL;

char* days[] = {"monday", "tuesday", "wednesday", "thursday", "friday", "saturday", "sunday"};
char* daysCaps[] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};

char* months[] = {"january", "february", "march", "april", "may", "june", "july", "august", "september", "october", "november", "december"};
char* monthsCaps[] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

int BACKGROUND_COLOR[3];

void eraseTextBox(){
	SetWindowText(windowElements[3], "");
}


char* getPath(){
  char* path = calloc(MAX_PATH, sizeof(char));
  HRESULT result = SHGetFolderPath(NULL, 0, NULL, 0, path);
  int i = 0;
  char c = path[i];
  int lastFound = -1;
  for (;c != '\0';i++){
    if (c == '\\'){
      lastFound = i;
    }
    c = path[i];
  }

  path[lastFound] = '\0';
  printf("[%s]\n", path);
  strcat(path, "Documents/Homework-Helper/Homework Helper 3/Homework Helper 3 (Dropbox)/rclone");
  return path;
}


int saveToCloud(){
  char* path = getPath();
  printf("[%s]\n", path);
  SetCurrentDirectory(path);
  WinExec("rclone copy HomeworkList.txt HomeworkHelper:HomeworkHelper/", SW_HIDE);
  
  free(path);

  return 0;
}


int loadFromCloud(){
  char* path = getPath();
  char* currentPath = calloc(MAX_PATH, sizeof(char));
  printf("[%s]\n", path);
  SetCurrentDirectory(path);
  GetCurrentDirectory(MAX_PATH, currentPath);
  printf("Current Directory is [%s]\n", currentPath);
  WinExec("rclone copy HomeworkHelper:HomeworkHelper/homeworkList.txt ./", SW_HIDE);
  sleep(3);
  free(path);

  return 0;
}


void save(){

  char text[1024];
	
  GetWindowText(windowElements[0], text, 1024);
	
  FILE *f;  

	f = fopen("homeworkList.txt", "w+");
	
  fputs(text, f);
	
  fclose(f);
  
  printf("Saved to file.\n", text);
}


void load(){
  TCHAR text[1024*1024] = TEXT("");
  char c ;
  FILE *f;

  loadFromCloud();

  f = fopen("homeworkList.txt", "r+");
  c = fgetc(f);
  while (c != EOF){
    if (c == '\n'){
      NUM_ITEMS++;
    }
    strncat(text, &c, 1);
    c = fgetc(f);
  } 

  fgets(text, 1024*1024, (FILE*)f);
	
  if (SetWindowText(windowElements[0], text) == TRUE){
    printf("Loaded from file. [%s]\n", text);
  }
  else{
    printf("Failed to insert text\n");
  }

  fclose(f);
  
  

}

char* getThisMonth(char* thisMonth){
	
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);

	return monthsCaps[tm.tm_mon];
}

int CheckInput(TCHAR typed[], char parts[4][1024]){
	
	char* pch = NULL;
  	pch = strtok(typed, " ");
	char intCounter[10] = "";

	int partPlace = 0;

    while (pch != NULL)
    {

		// if we already found a day of the week increment
		if (partPlace > 0){
			partPlace += 1;
		}

		if (partPlace > 3){
			break;
		}

		// check if this word is a day of the week
		if (partPlace == 0){
			// for ( ; *pch; ++pch) *pch = tolower(*pch); // make it lower case
			for (int i = 0; i < 7; i++){
				if (strcmp(pch, days[i]) == 0 || strcmp(pch, daysCaps[i]) == 0){
					partPlace += 1;
					break;
				}
			}
		}

		// check months
		if (partPlace == 3){
			int i;
			for (i = 0; i < 13; i++){
				if (strcmp(pch, months[i]) == 0 || strcmp(pch, monthsCaps[i]) == 0){
					break;
				}
				if (i == 11){
					printf("Thats not a real month");
					return FALSE;
				}
			}
			strcat(parts[partPlace], monthsCaps[i]);			
		}
		else{
			strcat(parts[partPlace], pch);
			if (partPlace == 0){
				strcat(parts[partPlace], " ");
			}
		}
		
		

        pch = strtok(NULL, " ");	
    }
		
	if (partPlace < 2){
		printf("Add format incorrect\n");
		return FALSE;
	}

	return TRUE;
}

int AddButtonPressed(){
	printf("add button pressed\n");	
	
	TCHAR text[1024*1024] = TEXT("");
	TCHAR typed[1024] = TEXT("");
	GetWindowText(windowElements[0], text, 1024*1024);
	GetWindowText(windowElements[3], typed, 1024*1024);

	if (strcmp(typed, "") == 0 || strcmp(typed, " ") == 0){
		return FALSE;
	}


	char parts[4][1024] = {"", "", "", ""};
	if (!CheckInput(typed, parts)){
		return FALSE;
	}

	char numItems[10] = "";
	
	strcat(text, numItems);
	strcat(text, ". ");
	strcat(text, parts[0]);
	strcat(text, parts[1]);
	strcat(text, ", ");
	char thisMonth[100];
	if (strcmp(parts[3], "") == 0){
		strcat(text, getThisMonth(thisMonth));
	}
	else{
		strcat(text, parts[3]);
	}
	strcat(text, " ");
	strcat(text, parts[2]);
  	strcat(text, "\n");
	
	
	
	if (SetWindowText(windowElements[0], text) == TRUE){
		save();
		NUM_ITEMS++;
		eraseTextBox();
		return TRUE;
	}

	return FALSE;
}

void RemoveNumberFromFront(char* str, int strlen){
	char c = ' ';
	BOOL foundSpace = FALSE;
	BOOL removed = FALSE;
	int i, j;
	j = 0;
	int lastNotSpace = 0;
	BOOL lastWasSpace = FALSE;

	for (i = 0; i < strlen; i++){
		c = str[i];
		if (foundSpace && c != ' '){
			removed = TRUE;
		}
		if (c == ' ' && c != '\0'){
			foundSpace = TRUE;
			if (!lastWasSpace){
				lastWasSpace = TRUE;
				lastNotSpace = i-1;
			}
		}
		else{
			lastWasSpace = false;
			lastNotSpace = -1;
		}
		if (removed){
			str[j++] = c;
			str[i] = ' ';
		}
	}

	if (lastNotSpace != -1){
		str[lastNotSpace+1] = '\0';
	}
}

int RemoveButtonPressed(){
	printf("remove button pressed\n");

	int counter = 0;
	int intTyped;

	TCHAR text[1024] = TEXT("");
	TCHAR typed[1024] = TEXT("");
	TCHAR newText[1024] = TEXT("");
	GetWindowText(windowElements[0], text, 1024*1024);
	GetWindowText(windowElements[3], typed, 1024*1024);
	
	
	try{
		intTyped = atoi(typed);
	}
	catch(){
		printf("Tried to remove without a number\n");
		return FALSE;
	}


	char* pch = NULL;
	char helper[1024];
    pch = strtok(text, "\n");
	
	
	char intCounter[10] = "";

    while (pch != NULL)
    {
		counter++;
		if (counter == intTyped){
			intTyped = -1;
			counter--;
			pch = strtok(NULL, "\n");	
			NUM_ITEMS--;
			eraseTextBox();
			continue;
		}
		strcpy(helper, pch);
		helper[strlen(pch)] = '\0';
		
		sprintf(intCounter, "%d", counter);
		RemoveNumberFromFront(helper, strlen(pch));

		strcat(newText, intCounter);
		strcat(newText, ". ");
		strcat(newText, helper);
    	strcat(newText, "\n");
		
        pch = strtok(NULL, "\n");	
    }

	if (SetWindowText(windowElements[0], newText) == TRUE){
		save();
		return TRUE;
	}

	return TRUE;

}


int MoveButtonPressed(){
	printf("move button pressed\n");
	
	int counter = 0;
	int intTyped;
	int intTyped2;

	TCHAR saveText[1024] = TEXT("");

	TCHAR text[1024] = TEXT("");
	TCHAR textCopy[1024] = TEXT("");
	TCHAR typed[1024] = TEXT("");
	TCHAR newText[1024] = TEXT("");
	GetWindowText(windowElements[0], text, 1024*1024);
	GetWindowText(windowElements[3], typed, 1024*1024);
	
	strcpy(textCopy, text);
	
	char* pch = NULL;
    pch = strtok(typed, " ");
	char intCounter[10] = "";

	try{
		intTyped = atoi(pch);
		pch = strtok(NULL, " ");
		intTyped2 = atoi(pch);
	}
	catch(){
		printf("User typed something wrong");
		return false;
	}

	if (intTyped >= NUM_ITEMS || intTyped <= 0 || intTyped2 >= NUM_ITEMS || intTyped2 <= 0 || intTyped == intTyped2){
		printf("Tried to move with an invalid number\n");
		return FALSE;
	}

	pch = strtok(text, "\n");
	char* helper = calloc(1024, sizeof(char));

	while (pch != NULL)
    {
		counter++;
		
		strcpy(helper, pch);
		helper[strlen(pch)] = '\0';

		if (counter == intTyped){
			RemoveNumberFromFront(helper, strlen(pch));
			strcpy(saveText, helper);
		}
		pch = strtok(NULL, "\n");


	}

	counter = 0;

	pch = strtok(textCopy, "\n");

	BOOL wasHere = FALSE;
	
    while (pch != NULL)
    {
		counter++;
		sprintf(intCounter, "%d", counter);

		if (intTyped < intTyped2){ // user wants to move it down
			if (counter == intTyped){
				intTyped = -2;
				counter--;
				pch = strtok(NULL, "\n");
			}
			else if (counter == intTyped2){
				intTyped2 = -2;
				
				strcat(newText, intCounter);
				strcat(newText, ". ");
				strcat(newText, saveText);
				strcat(newText, "\n");
				wasHere = TRUE;
			}
			else{
				strcpy(helper, pch);
				helper[strlen(pch)] = '\0';
				RemoveNumberFromFront(helper, strlen(pch));
				
				strcat(newText, intCounter);
				strcat(newText, ". ");
				strcat(newText, helper);
				strcat(newText, "\n");
				
				pch = strtok(NULL, "\n");
			}
		}
		else{ // user wants to move it up
			wasHere = TRUE;
			if (counter == intTyped+1){
				intTyped = -2;
				counter--;
				pch = strtok(NULL, "\n");
			}
			else if (counter == intTyped2){
				intTyped2 = -2;
				
				strcat(newText, intCounter);
				strcat(newText, ". ");
				strcat(newText, saveText);
				strcat(newText, "\n");
			}
			else{
				strcpy(helper, pch);
				helper[strlen(pch)] = '\0';
				RemoveNumberFromFront(helper, strlen(pch));
				
				strcat(newText, intCounter);
				strcat(newText, ". ");
				strcat(newText, helper);
				strcat(newText, "\n");
				
				pch = strtok(NULL, "\n");
			}
		}		
    }
	if (!wasHere){
		counter++;
		sprintf(intCounter, "%d", counter);

		strcat(newText, intCounter);
		strcat(newText, ". ");
		strcat(newText, saveText);
		strcat(newText, "\n");
	}

	free(helper);
	
	if (SetWindowText(windowElements[0], newText) == TRUE){
		save();
		eraseTextBox();
		return TRUE;
	}
	return FALSE;
}



// BELOW THIS IS STUFF TO MAKE THE WINDOW





LRESULT CALLBACK WndProc(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam);


int numChildren = 0;

int WIDTH = 475;
int HEIGHT = 650;



RECT editSize = {0,0,0,0};

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{

  BACKGROUND_COLOR[0] = 60;
  BACKGROUND_COLOR[1] = 60;
  BACKGROUND_COLOR[2] = 60;

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
int ADD_BUTTON_ID = 101;
int REMOVE_BUTTON_ID = 102;
int MOVE_BUTTON_ID = 103;


LRESULT CALLBACK WndProc(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	HWND static hwndTextBox, hwndTitle, hwndButtonAdd, hwndButtonRemove, hwndButtonMove, hwndHelpMessage, hwndHomeworkList;
	RECT rcClient;
	RECT MainRect;
	int width = WIDTH;
	int height = HEIGHT;
	RECT rc;
	int counter = 0;

  switch(Msg) 
	{
		case WM_SIZE:
			GetWindowRect(hwnd, &MainRect);
			width = MainRect.right - MainRect.left;
			height = MainRect.bottom - MainRect.top;

			// homework list
			SetWindowPos(windowElements[counter++], hwnd, 20, 80, width - 40, height-260, SWP_SHOWWINDOW | SWP_NOZORDER | SWP_FRAMECHANGED);
			// help message
			SetWindowPos(windowElements[counter++], hwnd, 20, height-175, width - 60, 40, SWP_SHOWWINDOW | SWP_NOZORDER | SWP_FRAMECHANGED);
			// title
			SetWindowPos(windowElements[counter++], hwnd, (width/2) - 150, 20, 300, 40, SWP_SHOWWINDOW | SWP_NOZORDER | SWP_FRAMECHANGED);
			// text box
			SetWindowPos(windowElements[counter++], hwnd, 20, height-145, width - 60, 40, SWP_SHOWWINDOW | SWP_NOZORDER | SWP_FRAMECHANGED);
			// add button
			SetWindowPos(windowElements[counter++], hwnd, 20, height-100, BUTTON_WIDTH, 40, SWP_SHOWWINDOW | SWP_NOZORDER | SWP_FRAMECHANGED);
			// remove button
			SetWindowPos(windowElements[counter++], hwnd, (width/2)-(BUTTON_WIDTH/2)-10, height-100, BUTTON_WIDTH, 40, SWP_SHOWWINDOW | SWP_NOZORDER | SWP_FRAMECHANGED);
			// move button
			SetWindowPos(windowElements[counter++], hwnd, (width - BUTTON_WIDTH) - 40, height-100, BUTTON_WIDTH, 40, SWP_SHOWWINDOW | SWP_NOZORDER | SWP_FRAMECHANGED);
			
		case WM_CREATE:

			hwndHomeworkList = CreateWindow( // homework list
				"STATIC", "",WS_CHILD, 
				0,0,0,0, // pos is created by the WM_SIZE above
				hwnd, (HMENU) 1, NULL, NULL
			);
			windowElements[numChildren++] = hwndHomeworkList;
      
			if (!loaded){
				load();
				loaded = TRUE;
			}

			hwndHelpMessage = CreateWindow( // help message
				"STATIC", TEXT("Name  Weekday  Day_number  Month(blank if n/a)"),WS_CHILD, 
				0,0,0,0, // pos is created by the WM_SIZE above
				hwnd, (HMENU) 1,NULL, NULL
			);
			windowElements[numChildren++] = hwndHelpMessage;
	
			hwndTitle = CreateWindow( // title at the top 
				"STATIC", "Homework Helper",WS_CHILD, 
				0,0,0,0, // pos is created by the WM_SIZE above
				hwnd, (HMENU) 1,NULL, NULL
			);
			windowElements[numChildren++] = hwndTitle;

			hwndTextBox = CreateWindow( // text box
				"edit", NULL,WS_CHILD | WS_BORDER  | ES_MULTILINE | ES_AUTOHSCROLL,
				0,0,0,0, // pos is created by the WM_SIZE above
				hwnd, (HMENU) 1,NULL, NULL
			);
			windowElements[numChildren++] = hwndTextBox;

      SetForegroundWindow(windowElements[3]);

			hwndButtonAdd = CreateWindow( // add button
				"button", "Add",WS_CHILD, 
				0,0,0,0, // pos is created by the WM_SIZE above
				hwnd, (HMENU) ADD_BUTTON_ID,NULL, NULL
			);
			windowElements[numChildren++] = hwndButtonAdd;

			hwndButtonRemove = CreateWindow( // remove button
			"button", "Remove",WS_CHILD, 
				0,0,0,0, // pos is created by the WM_SIZE above
				hwnd, (HMENU) REMOVE_BUTTON_ID,NULL, NULL
			);
			windowElements[numChildren++] = hwndButtonRemove;

			hwndButtonMove = CreateWindow( // move button
				"button", "Move",WS_CHILD, 
				0,0,0,0, // pos is created by the WM_SIZE above
				hwnd, (HMENU) MOVE_BUTTON_ID,NULL, NULL
			);
			windowElements[numChildren++] = hwndButtonMove;
			
			
			// create a font for the title and text box
			LOGFONT logfont; 
			ZeroMemory(&logfont, sizeof(LOGFONT));
			logfont.lfHeight = -35;
      		logfont.lfWeight = 700;
			HFONT hFont = CreateFontIndirect(&logfont);
			ZeroMemory(&logfont, sizeof(LOGFONT));
			logfont.lfHeight = -17; 
			HFONT hFont2 = CreateFontIndirect(&logfont);
			ZeroMemory(&logfont, sizeof(LOGFONT));
			logfont.lfHeight = -23;
			HFONT hFont3 = CreateFontIndirect(&logfont);
			
			// set fonts
			SendMessage(hwndTextBox, WM_SETFONT, (WPARAM)hFont3, TRUE);
			SendMessage(hwndTitle, WM_SETFONT, (WPARAM)hFont, TRUE);
			SendMessage(hwndHelpMessage, WM_SETFONT, (WPARAM)hFont2, TRUE);
			SendMessage(hwndButtonAdd, WM_SETFONT, (WPARAM)hFont2, TRUE);
			SendMessage(hwndButtonRemove, WM_SETFONT, (WPARAM)hFont2, TRUE);
			SendMessage(hwndButtonMove, WM_SETFONT, (WPARAM)hFont2, TRUE);
			SendMessage(hwndHomeworkList, WM_SETFONT, (WPARAM)hFont3, TRUE);

			// set text and background colors
			SendMessage(hwndTitle, WM_CTLCOLORSTATIC, (WPARAM)RGB(0,0,0), TRUE);
			SendMessage(hwndButtonAdd, WM_CTLCOLORSTATIC, (WPARAM)RGB(0,0,0), TRUE);
			SendMessage(hwndButtonRemove, WM_CTLCOLORSTATIC, (WPARAM)RGB(0,0,0), TRUE);
			SendMessage(hwndButtonMove, WM_CTLCOLORSTATIC, (WPARAM)RGB(0,0,0), TRUE);
			SendMessage(hwndHelpMessage, WM_CTLCOLORSTATIC, (WPARAM)RGB(0,0,0), TRUE);			
			break;

		case WM_CTLCOLORSTATIC: // set background color for the title
			SetBkColor((HDC)wParam, RGB(60, 60, 60));
			SetTextColor((HDC)wParam, RGB(255,255,255));
			return (LRESULT)CreateSolidBrush(RGB(60, 60, 60));
    case WM_CTLCOLORDLG: // set background color
			SetBkColor((HDC)wParam, RGB(255, 255, 255));
			return (LRESULT)CreateSolidBrush(RGB(255, 255, 255));
		case WM_ERASEBKGND:{
			HDC hdc = (HDC)(wParam); 
			RECT rc;
			GetClientRect(hwnd, &rc); 
			HBRUSH brush = CreateSolidBrush(RGB(BACKGROUND_COLOR[0], BACKGROUND_COLOR[1], BACKGROUND_COLOR[2]));// grey
			FillRect(hdc, &rc, brush);
			DeleteObject(brush); 
		}
		case WM_COMMAND:
			// TODO responds to button click
			if (LOWORD(wParam) == ADD_BUTTON_ID) { // add button
				AddButtonPressed();
        		SetFocus(windowElements[3]);
			}
			else if (LOWORD(wParam) == REMOVE_BUTTON_ID) { // remove button
				RemoveButtonPressed();
        		SetFocus(windowElements[3]);
			}
			else if (LOWORD(wParam) == MOVE_BUTTON_ID) { // move button
				MoveButtonPressed();
        		SetFocus(windowElements[3]);
			}
      
      
			break;
		case WM_DESTROY:
      save();
      saveToCloud();
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hwnd, Msg, wParam, lParam);
  }
  return 0;
}



