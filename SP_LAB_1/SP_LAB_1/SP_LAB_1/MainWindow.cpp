#ifndef UNICODE
#define UNICODE
#endif 

#define ID_GUESSBUTTON 1
#define ID_HINTBUTTON 2

#define ID_PASSWORDEDIT 3

#define IDPASSWORDSTATIC 4


#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int hintCounter = 0;
wchar_t  hintString[200] = { 0 };

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
		L"Gimme a password",            // Window text
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
	static HWND passwordEdit;

	const wchar_t* passwordToGuess = L"123456";

	switch (uMsg)
	{
	case WM_CREATE:

		passwordEdit = CreateWindowW(L"Edit", NULL,
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_PASSWORD,
			25,  //  x
			25,  //  y
			200, //  length
			20,  //  width
			hwnd, (HMENU)ID_PASSWORDEDIT,
			NULL, NULL);

		CreateWindowW(L"Button", L"Guess",
			WS_VISIBLE | WS_CHILD,
			230,
			25,
			80,
			20,
			hwnd, (HMENU)ID_GUESSBUTTON,
			NULL, NULL);

		CreateWindowW(L"Button", L"Hint",
			WS_VISIBLE | WS_CHILD,
			230,
			50,
			80,
			20,
			hwnd, (HMENU)ID_HINTBUTTON,
			NULL, NULL);

		CreateWindowW(L"Static", L"Password",
			WS_CHILD | WS_VISIBLE | SS_LEFT,
			25,
			5,
			70,
			20,
			hwnd, (HMENU)IDPASSWORDSTATIC, NULL, NULL);
		break;

		break;

	case WM_COMMAND:

		if (LOWORD(wParam) == ID_GUESSBUTTON) {

			int passwordGuessLen = GetWindowTextLengthW(passwordEdit) + 1;
			wchar_t* passwordGuessFromUser = new wchar_t[passwordGuessLen];

			GetWindowTextW(passwordEdit, passwordGuessFromUser, passwordGuessLen);

			if (wcscmp(passwordGuessFromUser, passwordToGuess) == 0) {
				MessageBox(hwnd, L"You're goddamn right", L"You're goddamn right", MB_OK);
			}
			else {
				MessageBeep(MB_OK);
				MessageBox(hwnd, L"Wrong Password", L"Wrong Password", MB_OK);
			}

			delete passwordGuessFromUser;

		}
		else if (LOWORD(wParam) == ID_HINTBUTTON) {
			if (hintCounter <= wcslen(passwordToGuess)) {
				for (int index = 0; index <= hintCounter; index++) {
					hintString[index] = passwordToGuess[index];
				}
				hintCounter++;
				SetWindowTextW(hwnd, hintString);
			}

		}

		break;

	case WM_CLOSE:

		if (MessageBox(hwnd, L"Really quit?", L"Quit Window", MB_OKCANCEL) == IDOK)
		{
			DestroyWindow(hwnd);
		}

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