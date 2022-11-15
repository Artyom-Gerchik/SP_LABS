//#define _WINSOCK_DEPRECATED_NO_WARNINGS
//#include <iostream> 
//#include <cstdio> 
//#include <cstring> 
//#include <winsock2.h> 
//#pragma comment(lib, "WS2_32.lib")
//using namespace std;
//
//DWORD WINAPI clientReceive(LPVOID lpParam) {
//	char buffer[1024] = { 0 };
//	SOCKET server = *(SOCKET*)lpParam;
//	while (true) {
//		if (recv(server, buffer, sizeof(buffer), 0) == SOCKET_ERROR) {
//			cout << "recv function failed with error: " << WSAGetLastError() << endl;
//			return -1;
//		}
//		if (strcmp(buffer, "exit\n") == 0) {
//			cout << "Server disconnected." << endl;
//			return 1;
//		}
//		cout << "Server: " << buffer << endl;
//		memset(buffer, 0, sizeof(buffer));
//	}
//	return 1;
//}
//
//DWORD WINAPI clientSend(LPVOID lpParam) {
//	char buffer[1024] = { 0 };
//	SOCKET server = *(SOCKET*)lpParam;
//	while (true) {
//		fgets(buffer, 1024, stdin);
//		if (send(server, buffer, sizeof(buffer), 0) == SOCKET_ERROR) {
//			cout << "send failed with error: " << WSAGetLastError() << endl;
//			return -1;
//		}
//		if (strcmp(buffer, "exit") == 0) {
//			cout << "Thank you for using the application" << endl;
//			break;
//		}
//	}
//	return 1;
//}
//
//int main() {
//	WSADATA WSAData;
//	SOCKET server;
//	SOCKADDR_IN addr;
//	WSAStartup(MAKEWORD(2, 0), &WSAData);
//	if ((server = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
//		cout << "Socket creation failed with error: " << WSAGetLastError() << endl;
//		return -1;
//	}
//
//	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
//	addr.sin_family = AF_INET;
//	addr.sin_port = htons(5555);
//	if (connect(server, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR) {
//		cout << "Server connection failed with error: " << WSAGetLastError() << endl;
//		return -1;
//	}
//
//	cout << "Connected to server!" << endl;
//	cout << "Now you can use our live chat application. " << " Enter \"exit\" to disconnect" << endl;
//
//	DWORD tid;
//	HANDLE t1 = CreateThread(NULL, 0, clientReceive, &server, 0, &tid);
//	if (t1 == NULL) cout << "Thread creation error: " << GetLastError();
//	HANDLE t2 = CreateThread(NULL, 0, clientSend, &server, 0, &tid);
//	if (t2 == NULL) cout << "Thread creation error: " << GetLastError();
//
//	WaitForSingleObject(t1, INFINITE);
//	WaitForSingleObject(t2, INFINITE);
//	closesocket(server);
//	WSACleanup();
//}


#include <iostream>
#include <string>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

using namespace std;

void main()
{
	string ipAddress = "127.0.0.1";			// IP Address of the server
	int port = 54000;						// Listening port # on the server

	// Initialize WinSock
	WSAData data;
	WORD ver = MAKEWORD(2, 2);
	int wsResult = WSAStartup(ver, &data);
	if (wsResult != 0)
	{
		cerr << "Can't start Winsock, Err #" << wsResult << endl;
		return;
	}

	// Create socket
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		cerr << "Can't create socket, Err #" << WSAGetLastError() << endl;
		WSACleanup();
		return;
	}

	// Fill in a hint structure
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

	// Connect to server
	int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
	if (connResult == SOCKET_ERROR)
	{
		cerr << "Can't connect to server, Err #" << WSAGetLastError() << endl;
		closesocket(sock);
		WSACleanup();
		return;
	}

	// Do-while loop to send and receive data
	char buf[4096];
	string userInput;

	do
	{
		// Prompt the user for some text
		cout << "> ";
		getline(cin, userInput);

		if (userInput.size() > 0)		// Make sure the user has typed in something
		{
			// Send the text
			int sendResult = send(sock, userInput.c_str(), userInput.size() + 1, 0);
			if (sendResult != SOCKET_ERROR)
			{
				// Wait for response
				ZeroMemory(buf, 4096);
				int bytesReceived = recv(sock, buf, 4096, 0);
				if (bytesReceived > 0)
				{
					// Echo response to console
					cout << "SERVER> " << string(buf, 0, bytesReceived) << endl;
				}
			}
		}

	} while (userInput.size() > 0);

	// Gracefully close down everything
	closesocket(sock);
	WSACleanup();
}
