#include <windows.h> 
#include <stdio.h>
#include <conio.h>
#include <tchar.h>

#define BUFSIZE 512

int _tmain()
{
	HANDLE hPipe;
	TCHAR  chBuf[BUFSIZE];
	BOOL   fSuccess = FALSE;
	DWORD  cbRead, cbToWrite, cbWritten, dwMode;
	LPCTSTR lpszPipename = TEXT("\\\\.\\pipe\\mynamedpipe");

	BOOL STATE = TRUE;
	char messageFromConsole[512];

	while (1)
	{
		hPipe = CreateFile(
			lpszPipename,   // pipe name 
			GENERIC_READ |  // read and write access 
			GENERIC_WRITE,
			0,              // no sharing 
			NULL,           // default security attributes
			OPEN_EXISTING,  // opens existing pipe 
			0,              // default attributes 
			NULL);          // no template file 

	  // Break if the pipe handle is valid. 

		if (hPipe != INVALID_HANDLE_VALUE)
			break;

		// Exit if an error other than ERROR_PIPE_BUSY occurs. 

		if (GetLastError() != ERROR_PIPE_BUSY)
		{
			_tprintf(TEXT("Could not open pipe. GLE=%d\n"), GetLastError());
			return -1;
		}

		// All pipe instances are busy, so wait for 20 seconds. 

		if (!WaitNamedPipe(lpszPipename, 20000))
		{
			printf("Could not open pipe: 20 second wait timed out.");
			return -1;
		}
	}

	// The pipe connected; change to message-read mode. 

	dwMode = PIPE_READMODE_MESSAGE;
	fSuccess = SetNamedPipeHandleState(
		hPipe,    // pipe handle 
		&dwMode,  // new pipe mode 
		NULL,     // don't set maximum bytes 
		NULL);    // don't set maximum time 
	if (!fSuccess)
	{
		_tprintf(TEXT("SetNamedPipeHandleState failed. GLE=%d\n"), GetLastError());
		return -1;
	}

	// Send a message to the pipe server. 
	while (1) {
		if (STATE) {
			while (1)
			{

				printf("Message To Server: ");
				gets_s(messageFromConsole);

				LPTSTR message = LPTSTR(messageFromConsole);
				// printf("qweqwew   %s", nigger);
				DWORD messageWeight = (lstrlen(message) + 1) * sizeof(TCHAR);

				if (!WriteFile(
					hPipe,              // pipe handle 
					message,             // message 
					messageWeight,            // message length 
					&cbWritten,         // bytes written 
					NULL)) {
					break;
				}

				STATE = FALSE;
				break;
			}
		}
		else if (!STATE) {
			while (1)
			{
				fSuccess = ReadFile(
					hPipe,    // pipe handle 
					chBuf,    // buffer to receive reply 
					BUFSIZE * sizeof(TCHAR),  // size of buffer 
					&cbRead,  // number of bytes read 
					NULL);    // not overlapped 

				if (!fSuccess && GetLastError() != ERROR_MORE_DATA) {

				}
				else {

					printf("Message From Server: %s\n", chBuf);
				}
				STATE = TRUE;
				break;
			}
		}
	}

	if (!fSuccess)
	{
		_tprintf(TEXT("ReadFile from pipe failed. GLE=%d\n"), GetLastError());
		return -1;
	}

	printf("\n<End of message, press ENTER to terminate connection and exit>");
	_getch();

	CloseHandle(hPipe);

	return 0;
}