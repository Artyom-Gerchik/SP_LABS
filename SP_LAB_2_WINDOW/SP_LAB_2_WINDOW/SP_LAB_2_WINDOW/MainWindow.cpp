#ifndef UNICODE
#define UNICODE
#endif 

#define ID_INCREASEBUTTON 1
#define ID_QUITBUTTON 3

#define ID_COUNTEREDIT 2

#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fstream> 
#include <string>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void saveValueToFile(int value);
int getValueFromFile();
bool isFileExist(const char* fileName);


char fileName[] = "vars.txt";

int counter;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	// Register the window class.
	const wchar_t CLASS_NAME[] = L"Main Window Class";

	WNDCLASS wc = { };

	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;

	RegisterClass(&wc);

	// Create the window.

	HWND hwnd = CreateWindowEx(
		0,                              // Optional window styles.
		CLASS_NAME,                     // Window class
		L"Main",                        // Window text
		WS_OVERLAPPED | WS_SYSMENU,     // Window style

		// Size and position
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		345,
		200,

		NULL,       // Parent window    
		NULL,       // Menu
		hInstance,  // Instance handle
		NULL        // Additional application data
	);

	if (hwnd == NULL)
	{
		return 0;
	}

	ShowWindow(hwnd, nCmdShow);

	// Run the message loop.

	MSG msg = { };
	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HWND counterEdit;

	switch (uMsg)
	{
	case WM_CREATE:

		if (isFileExist(fileName)) {
			counter = getValueFromFile();
		}
		else {
			counter = 0;
		}

		wchar_t initial[256];
		wsprintfW(initial, L"%d", counter);

		counterEdit = CreateWindowW(L"Edit", initial,
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY,
			25,  //  x
			25,  //  y
			200, //  length
			20,  //  width
			hwnd, (HMENU)ID_COUNTEREDIT,
			NULL, NULL);

		CreateWindowW(L"Button", L"Increase",
			WS_VISIBLE | WS_CHILD,
			230,
			25,
			80,
			20,
			hwnd, (HMENU)ID_INCREASEBUTTON,
			NULL, NULL);

		CreateWindowW(L"Button", L"QUIT",
			WS_VISIBLE | WS_CHILD,
			230,
			50,
			80,
			20,
			hwnd, (HMENU)ID_QUITBUTTON,
			NULL, NULL);

		break;

	case WM_COMMAND:

		if (LOWORD(wParam) == ID_INCREASEBUTTON) {
			counter++;

			wchar_t next[256];
			wsprintfW(next, L"%d", counter);

			SetWindowTextW(counterEdit, next);

			

		}
		else if(LOWORD(wParam) == ID_QUITBUTTON) {
			remove("vars.txt");
			PostQuitMessage(0);
		}
		break;

	case WM_CLOSE:

		saveValueToFile(counter);

		STARTUPINFO si;
		::memset(&si, 0, sizeof(si));
		si.cb = sizeof(si);
		PROCESS_INFORMATION pi;
		::CreateProcess(NULL, ::GetCommandLine(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);

		DestroyWindow(hwnd);

		/*if (MessageBox(hwnd, L"Really quit?", L"Quit Window", MB_OKCANCEL) == IDOK)
		{
			DestroyWindow(hwnd);
		}*/

		return 0;

	case WM_DESTROY:

		PostQuitMessage(0);
		
		return 0;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
		EndPaint(hwnd, &ps);
	}
	return 0;

	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void saveValueToFile(int value) {
	std::ofstream outfile(fileName);
	outfile << value << std::endl;
	outfile.close();
}

int getValueFromFile() {
	int inputValue;
	std::ifstream infile(fileName);
	infile >> inputValue;
	return inputValue;
}

bool isFileExist(const char* fileName)
{
	std::ifstream infile(fileName);
	return infile.good();
}