#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <cstdio>
#include <iostream>
#include <vector>
#include <string>
#include <conio.h>

#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383

void QueryKey(HKEY hKey)
{
	TCHAR    achKey[MAX_KEY_LENGTH];   // buffer for subkey name
	DWORD    cbName;                   // size of name string 
	TCHAR    achClass[MAX_PATH] = TEXT("");  // buffer for class name 
	DWORD    cchClassName = MAX_PATH;  // size of class string 
	DWORD    cSubKeys = 0;               // number of subkeys 
	DWORD    cbMaxSubKey;              // longest subkey size 
	DWORD    cchMaxClass;              // longest class string 
	DWORD    cValues;              // number of values for key 
	DWORD    cchMaxValue;          // longest value name 
	DWORD    cbMaxValueData;       // longest value data 
	DWORD    cbSecurityDescriptor; // size of security descriptor 
	FILETIME ftLastWriteTime;      // last write time 

	DWORD i, retCode;

	TCHAR  achValue[MAX_VALUE_NAME];
	DWORD cchValue = MAX_VALUE_NAME;

	// Get the class name and the value count. 
	retCode = RegQueryInfoKey(
		hKey,                    // key handle 
		achClass,                // buffer for class name 
		&cchClassName,           // size of class string 
		NULL,                    // reserved 
		&cSubKeys,               // number of subkeys 
		&cbMaxSubKey,            // longest subkey size 
		&cchMaxClass,            // longest class string 
		&cValues,                // number of values for this key 
		&cchMaxValue,            // longest value name 
		&cbMaxValueData,         // longest value data 
		&cbSecurityDescriptor,   // security descriptor 
		&ftLastWriteTime);       // last write time 

	// Enumerate the subkeys, until RegEnumKeyEx fails.

	if (cSubKeys)
	{
		printf("\nNumber of subkeys: %d\n", cSubKeys);

		for (i = 0; i < cSubKeys; i++)
		{
			cbName = MAX_KEY_LENGTH;
			retCode = RegEnumKeyEx(hKey, i,
				achKey,
				&cbName,
				NULL,
				NULL,
				NULL,
				&ftLastWriteTime);
			if (retCode == ERROR_SUCCESS)
			{
				_tprintf(TEXT("(%d) %s\n"), i + 1, achKey);
			}
		}
	}

	// Enumerate the key values. 

	BYTE* buffer = new BYTE[cbMaxValueData];
	ZeroMemory(buffer, cbMaxValueData);

	if (cValues)
	{
		printf("\nNumber of values: %d\n", cValues);

		for (i = 0, retCode = ERROR_SUCCESS; i < cValues; i++)
		{
			cchValue = MAX_VALUE_NAME;
			achValue[0] = '\0';
			retCode = RegEnumValue(hKey, i,
				achValue,
				&cchValue,
				NULL,
				NULL,
				NULL,
				NULL);

			if (retCode == ERROR_SUCCESS)
			{

				DWORD lpData = cbMaxValueData;
				buffer[0] = '\0';
				LONG dwRes = RegQueryValueEx(hKey, achValue, 0, NULL, buffer, &lpData);
				_tprintf(TEXT("(%d) %s : %s\n"), i + 1, achValue, buffer);
			}
		}
	}
	delete[] buffer;
}


int _tmain(int argc, _TCHAR* argv[])
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleTextAttribute(hConsole, 12);

	std::cout << "You are in area of HKEY_LOCAL_MACHINE" << std::endl;
	std::cout << "Try something like this: SOFTWARE\\" << std::endl;

	SetConsoleTextAttribute(hConsole, 10);

	QueryKey(HKEY_LOCAL_MACHINE);
	std::cout << std::endl;

	while (true)
	{
		SetConsoleTextAttribute(hConsole, 11);
		std::string input;
		std::getline(std::cin, input);
		std::cout << std::endl;

		if (input == "") {
			system("cls");
			SetConsoleTextAttribute(hConsole, 12);

			std::cout << "You are in area of HKEY_LOCAL_MACHINE" << std::endl;
			std::cout << "Try something like this: SOFTWARE\\" << std::endl;

			SetConsoleTextAttribute(hConsole, 10);

			QueryKey(HKEY_LOCAL_MACHINE);
			std::cout << std::endl;
		}
		else {

			std::wstring temp = std::wstring(input.begin(), input.end());

			LPCWSTR wideString = temp.c_str();

			HKEY hKey;
			LONG dwRegOPenKey = RegOpenKeyEx(HKEY_LOCAL_MACHINE, wideString, 0, KEY_READ, &hKey);
			if (dwRegOPenKey == ERROR_SUCCESS) {
				SetConsoleTextAttribute(hConsole, 12);
				printf("RegOpenKeyEx succeeded, error code %d\n", GetLastError());
				std::cout << std::endl << input << ": " << std::endl;
				SetConsoleTextAttribute(hConsole, 10);
				QueryKey(hKey);
				std::cout << std::endl;


			}
			else {
				printf("RegOpenKeyEx failed, error code %d\n", dwRegOPenKey);
			}
			RegCloseKey(hKey);
		}
	}
	return 0;
}