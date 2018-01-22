#include "stdafx.h"
#include "WinApiWithoutClasses.h"

#define MAX_LOADSTRING 100
#define ID_TIMER 101

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];


RECT coordinateBuffer[4097];
RECT symbolRect;
RECT clientRect;
int cursor;
HDC hdc;
bool isCursorVisible;

int textBufferCount;
WCHAR textBuffer[4096];
int indexBuffer;
int symbolCount;

//! Переменная хранящая цвет текста.
COLORREF colorText;

UINT TimerID = 0;


ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void				handleInsertSymbol(HWND, WCHAR);
void				windowUpdate(HWND);
void				cursorTimer(HWND);
void				moveCursor(Direction, HWND);
void				handleKey(Key, HWND);
void				handleMouseClick(int, int, HWND);
//////////////////////////////////////////////////////
void				deleteCursor(HWND);
void				clearScreen(HWND);
void				drawModel(HWND);
WCHAR				getSymbolFromTextBuffer(int);
void				clearCoordinates(int);
void				insertSymbol(WCHAR);
void				drawCursor(HWND);
void				flashCursor(HWND);
void				deleteSymbol();
void				setCursorToHome();
void				setCursorToEnd();
void				setCursorFromCoordinates(int, int);
///////////////////////////////////////////////////////
void				cursorRight();
void				cursorLeft();
void				cursorUp();
void				cursorDown();



int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_WINAPIWITHOUTCLASSES, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINAPIWITHOUTCLASSES));

	MSG msg;

	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}


ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PEN));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_WINAPIWITHOUTCLASSES);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_PEN));

	return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance;

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	TimerID = SetTimer(hWnd, ID_TIMER, 700, NULL); //для курсора

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}



LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message)
	{
	case WM_CREATE: //при создании
	{
		symbolCount = 0;
		indexBuffer = 0;
		textBufferCount = 4096;
		for (int i = 0; i < textBufferCount; i++)
		{
			textBuffer[i] = NULL;
		}


		symbolRect = { 5, 5, 25, 25 }; //{left, top, right, bottom}
		coordinateBuffer[0] = symbolRect;
		colorText = colorText = RGB(255, 0, 0);
		isCursorVisible = true;


		break;
	}


	 case WM_COMMAND:
	{
	int wmId = LOWORD(wParam);
	switch (wmId)
	{
	case IDM_ABOUT:
	DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
	break;
	case IDM_EXIT:
	DestroyWindow(hWnd);
	break;
	default:
	return DefWindowProc(hWnd, message, wParam, lParam);
	}
	}
	break;


	case WM_PAINT: //перерисовка
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		windowUpdate(hWnd);
		EndPaint(hWnd, &ps);

	}
	break;



	case WM_KEYDOWN: //клавиатура
	{
		if (wParam == VK_RIGHT)
		{
			moveCursor(RIGHT, hWnd);
			break;
		}

		if (wParam == VK_LEFT)
		{
			moveCursor(LEFT, hWnd);
			break;
		}

		if (wParam == VK_UP)
		{
			moveCursor(UP, hWnd);
			break;
		}

		if (wParam == VK_DOWN)
		{
			moveCursor(DOWN, hWnd);
			break;
		}

		if (wParam == VK_HOME)
		{
			handleKey(HOME, hWnd);
			break;
		}

		if (wParam == VK_END)
		{
			handleKey(END, hWnd);
			break;
		}

		if (wParam == VK_DELETE)
		{
			handleKey(DEL, hWnd);
			break;
		}
		break;
	}





	case WM_CHAR: //ввод символов
	{
		if (wParam == VK_RETURN) //ввод
		{
			handleInsertSymbol(hWnd, wParam);
			break;
		}

			if (wParam == VK_TAB)
		{
		break;
		}

			if (wParam == VK_ESCAPE)
		{
		break;
		}

		if (wParam == VK_BACK) //стереть предыдущий
		{
			handleKey(BACKSPASE, hWnd);
			break;
		}

		handleInsertSymbol(hWnd, wParam);

		break;
	}





	case WM_LBUTTONDOWN: //мышка
	{
		handleMouseClick(GET_X_LPARAM(lParam) / 20, GET_Y_LPARAM(lParam) / 20, hWnd);
		break;
	}





	case WM_TIMER:  //таймер курсора
	{
		cursorTimer(hWnd);
		break;
	}




	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
UNREFERENCED_PARAMETER(lParam);
switch (message)
{
case WM_INITDIALOG:
return (INT_PTR)TRUE;

case WM_COMMAND:
if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
{
EndDialog(hDlg, LOWORD(wParam));
return (INT_PTR)TRUE;
}
break;
}
return (INT_PTR)FALSE;
}

void handleInsertSymbol(HWND hwnd, WCHAR wParam)
{
	deleteCursor(hwnd);
	clearScreen(hwnd);
	insertSymbol(wParam); //вставка символа в текстовый буфер
	drawModel(hwnd);
	drawCursor(hwnd);
}

void windowUpdate(HWND hwnd)
{
	deleteCursor(hwnd);
	clearScreen(hwnd);
	drawModel(hwnd);
	drawCursor(hwnd);
}

void cursorTimer(HWND hwnd)
{
	flashCursor(hwnd);
}

void moveCursor(Direction direction, HWND hwnd)
{
	if (direction == RIGHT) {
		deleteCursor(hwnd);
		cursorRight();
		drawCursor(hwnd);
		return;
	}

	if (direction == LEFT) {
		deleteCursor(hwnd);
		cursorLeft();
		drawCursor(hwnd);
		return;
	}

	if (direction == UP)
	{
		deleteCursor(hwnd);
		cursorUp();
		drawCursor(hwnd);
		return;
	}

	if (direction == DOWN)
	{
		deleteCursor(hwnd);
		cursorDown();
		drawCursor(hwnd);
		return;
	}
}

void handleKey(Key key, HWND hwnd)
{
	if (key == HOME)
	{
		deleteCursor(hwnd);
		setCursorToHome();
		drawCursor(hwnd);
		return;
	}
	if (key == END)
	{
		deleteCursor(hwnd);
		setCursorToEnd();
		drawCursor(hwnd);
		return;
	}

	if (key == DEL)
	{
		if (indexBuffer >= symbolCount)
		{
			return;
		}
		deleteCursor(hwnd);
		clearScreen(hwnd);
		deleteSymbol();
		drawModel(hwnd);
		drawCursor(hwnd);
		return;
	}

	if (key == BACKSPASE)
	{
		if (indexBuffer == 0)
		{
			return;
		}
		deleteCursor(hwnd);
		clearScreen(hwnd);
		cursorLeft();
		deleteSymbol();
		drawModel(hwnd);
		drawCursor(hwnd);
		return;

	}
}

void handleMouseClick(int x, int y, HWND hwnd)
{
	deleteCursor(hwnd);
	setCursorFromCoordinates(x, y);
	drawCursor(hwnd);
}

//////////////////////////////////////////////////////////

void deleteCursor(HWND hwnd)
{
	hdc = GetDC(hwnd);
	cursor = indexBuffer;
	symbolRect = { coordinateBuffer[cursor].left,coordinateBuffer[cursor].top,coordinateBuffer[cursor].left + 1,coordinateBuffer[cursor].bottom };
	FillRect(hdc, &symbolRect, (HBRUSH)(COLOR_WINDOW + 1));
	ReleaseDC(hwnd, hdc);
}

void clearScreen(HWND hwnd)
{
	int length = symbolCount;
	for (int i = 0; i < length + 1; i++)
	{
		FillRect(hdc, &coordinateBuffer[i], (HBRUSH)(COLOR_WINDOW + 1));
	}
}

void drawModel(HWND hwnd)
{
	hdc = GetDC(hwnd);
	GetClientRect(hwnd, &clientRect);
	int length = symbolCount; //получаем кол-во символов
	WCHAR symbol;
	clearScreen(hwnd);
	clearCoordinates(length);
	symbolRect = { 5, 5, 25, 25 };
	coordinateBuffer[0] = symbolRect;
	for (int i = 0; i < length; i++)
	{
		symbol = getSymbolFromTextBuffer(i);
		if (symbol == (char)13)
		{
			symbolRect.left = 5;
			symbolRect.right = 25;
			symbolRect.bottom += 20;
			symbolRect.top += 20;
			coordinateBuffer[i + 1] = symbolRect;
			continue;
		}
		FillRect(hdc, &symbolRect, (HBRUSH)(COLOR_WINDOW + 1));
		DrawText(hdc, (LPWSTR)(&symbol), 1, &symbolRect, DT_CENTER);
		symbolRect.left += 20;
		symbolRect.right += 20;
		if (symbolRect.right >= clientRect.right)
		{
			symbolRect.left = 5;
			symbolRect.right = 25;
			symbolRect.bottom += 20;
			symbolRect.top += 20;
		}
		coordinateBuffer[i + 1] = symbolRect;
	}
	ReleaseDC(hwnd, hdc);
}

WCHAR getSymbolFromTextBuffer(int i)
{
	return textBuffer[i];
}

void clearCoordinates(int length) {
	for (int i = 0; i < length; i++)
	{
		coordinateBuffer[i] = { 0,0,0,0 };
	}
}

void insertSymbol(WCHAR symbol) //вставка символа в текстовый буфер//
{
	if (indexBuffer == symbolCount) //если курсор на последнем символе
	{
		textBuffer[indexBuffer] = symbol; //то записать в конец введенный
		++indexBuffer; //и увеличить кол-во символов и позицию курсора сдвинуть
		++symbolCount;
	}
	else  //если курсор не в конце
	{
		for (int i = symbolCount + 1; i > indexBuffer; i--) //то сдвигаем символы после курсора на 1
		{
			textBuffer[i] = textBuffer[i - 1]; //чтобы вставить в это место новый символ
		}
		textBuffer[indexBuffer] = symbol;
		++indexBuffer;
		++symbolCount;
	}
}

void drawCursor(HWND hwnd)
{
	hdc = GetDC(hwnd);
	cursor = indexBuffer;
	MoveToEx(hdc, coordinateBuffer[cursor].left, coordinateBuffer[cursor].top, NULL);
	LineTo(hdc, coordinateBuffer[cursor].left, coordinateBuffer[cursor].bottom);
	ReleaseDC(hwnd, hdc);
}

void flashCursor(HWND hwnd)
{
	hdc = GetDC(hwnd);
	deleteCursor(hwnd);
	if (isCursorVisible) {
		drawCursor(hwnd);
	}
	isCursorVisible = !isCursorVisible;
	ReleaseDC(hwnd, hdc);
}

void deleteSymbol()
{
	for (int i = indexBuffer; i < symbolCount - 1; i++)
	{
		textBuffer[i] = textBuffer[i + 1];
	}
	textBuffer[symbolCount - 1] = NULL;
	symbolCount--;
}

void setCursorToHome()
{
	int thissymbolCount = symbolCount;
	cursor = indexBuffer;
	while (coordinateBuffer[cursor].left != 5 && coordinateBuffer[cursor].right != 25 && cursor != thissymbolCount + 1)
	{
		--cursor;
	}
	indexBuffer = cursor;
}

void setCursorToEnd()
{
	cursor = indexBuffer;
	int thissymbolCount = symbolCount;
	RECT newCursorRect = coordinateBuffer[cursor];
	newCursorRect.bottom += 20;
	newCursorRect.top += 20;
	while (coordinateBuffer[cursor].bottom != newCursorRect.bottom && cursor != thissymbolCount + 1)
	{
		++cursor;
	}
	--cursor;
	indexBuffer = cursor;
}

void setCursorFromCoordinates(int x, int y)
{
	cursor = 0;
	int thissymbolCount = symbolCount;
	RECT newCursorRect;
	newCursorRect.right = 20 * x + 25;
	newCursorRect.left = 20 * x + 5;
	newCursorRect.bottom = 20 * y + 25;
	newCursorRect.top = 20 * y + 5;
	while (cursor != thissymbolCount)
	{
		if (newCursorRect.top == coordinateBuffer[cursor].top && newCursorRect.bottom == coordinateBuffer[cursor].bottom)
		{
			break;
		}
		++cursor;
	}
	while (cursor != symbolCount)
	{
		if (newCursorRect.right == coordinateBuffer[cursor].right && newCursorRect.left == coordinateBuffer[cursor].left)
		{
			indexBuffer = cursor;
			return;
		}
		if (newCursorRect.top < coordinateBuffer[cursor].top && newCursorRect.bottom < coordinateBuffer[cursor].bottom)
		{
			--cursor;
			indexBuffer = cursor;
			return;
		}
		++cursor;
	}
	indexBuffer = cursor;
}

/////////////////////////////////////////////////////////

void cursorRight()
{
	cursor = indexBuffer;
	if (coordinateBuffer[cursor + 1].top == 0 && coordinateBuffer[cursor + 1].bottom == 0 && coordinateBuffer[cursor + 1].left == 0 && coordinateBuffer[cursor + 1].right == 0)
	{
		return;
	}
	indexBuffer = ++cursor;
}

void cursorLeft() //просто подвинем его влево на 1 знак
{
	cursor = indexBuffer;
	if (cursor == 0)
	{
		return;
	}
	indexBuffer = --cursor;
}

void cursorUp()
{
	cursor = indexBuffer;
	int thissymbolCount = symbolCount;
	RECT newCursorRect = coordinateBuffer[cursor];
	newCursorRect.bottom -= 20;
	newCursorRect.top -= 20;
	if (newCursorRect.top < 5) {
		return;
	}
	while (newCursorRect.top != coordinateBuffer[cursor].top && cursor != thissymbolCount + 1)
	{
		--cursor;
	}
	if (coordinateBuffer[cursor].right < newCursorRect.right)
	{
		indexBuffer = cursor;
		return;
	}
	while (!EqualRect(&newCursorRect, &coordinateBuffer[cursor]) && cursor != symbolCount + 1)
	{
		--cursor;
	}
	indexBuffer = cursor;
}

void cursorDown()
{
	cursor = indexBuffer;
	int thissymbolCount = symbolCount;
	RECT newCursorRect = coordinateBuffer[cursor];
	newCursorRect.bottom += 20;
	newCursorRect.top += 20;
	if (newCursorRect.top > coordinateBuffer[thissymbolCount].top)
	{
		return;
	}
	while ((!EqualRect(&newCursorRect, &coordinateBuffer[cursor])) && coordinateBuffer[cursor].top != newCursorRect.top + 20 && cursor != symbolCount)
	{
		++cursor;
	}
	if (EqualRect(&newCursorRect, &coordinateBuffer[cursor]))
	{
		indexBuffer = cursor;
		return;
	}
	indexBuffer = --cursor;
}


/////////////////////////////////////////////////////////
