#define _CRT_SECURE_NO_WARNINGS 
#include <stdlib.h>
#include <windows.h>
#include <cstdio>
#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include <conio.h>
#include <process.h>
#include <string>
#include <sstream> 
#include <Winbase.h.>
#include <shellapi.h>


//	C:\VS\source\repos\Project3\Debug\Project3.exe -c "\\.\pipe\MyPipe" -d "C:\example" -r
const char *srcpath1 = "C:\\ServerProgVault\\";
const char *pipename1 = "\\.\pipe\MyPipe";
using namespace std;

void PrintMainMenu()
{
	cout << "Выберите раздел:" << "\n";
	cout << "1. Добавить файл" << "\n";
	cout << "2. Обновить файл" << "\n";
	cout << "3. Посмотреть файл" << "\n";
	cout << "4. Удалить файл" << "\n";
	cout << "5. Посмотреть полный список файлов" << "\n";
	cout << "0. Выход" << "\n";
	cout << "> ";
}

int main1(int argc, char* argv[])
{
	setlocale(LC_ALL, "Russian");
	char* user = 0;
	DWORD  cbRead, cbWritten ,dwMode;
	HANDLE pipe;
	BOOL   fSuccess = false;
	char   szBuf[256];
	int usr;
	char str1[200] = { 0 };
	char str2[200] = { 0 };
	char buff[1000] = { 0 };

	
	cout << "\nConnect to server\n";
	while (1)
	{
		pipe = CreateFileA(pipename1, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);   // no template file 


		if (pipe != INVALID_HANDLE_VALUE)
			break;

		// Exit if an error other than ERROR_PIPE_BUSY occurs. 

		if (GetLastError() != ERROR_PIPE_BUSY)
		{
			cout << "Could not open pipe. GLE=%d\n" << GetLastError();
			return -1;
		}

		// All pipe instances are busy, so wait for 20 seconds. 

		if (!WaitNamedPipe(pipename1, 20000))
		{
			cout << "Could not open pipe: 20 second wait timed out.";
			return -1;
		}

		cout << "Connected\n";
	}
	Sleep(1000);

	/*if (!fSuccess)
	{
		printf(TEXT("SetNamedPipeHandleState failed. GLE=%d\n"), GetLastError());
		return -1;
	}*/
	while (1)
	{
		// Read client requests from the pipe. This simplistic code only allows messages
		// up to BUFSIZE characters in length.

		do
		{
			try
			{
				PrintMainMenu();
				cin >> user;
				usr = atoi(user);
				fSuccess = WriteFile(
					pipe,        // handle to pipe 
					user,     // buffer to write from 
					sizeof(user) + 1, // number of bytes to write 
					&cbWritten,   // number of bytes written 
					NULL);        // not overlapped I/O 

				if (!fSuccess)
				{
					printf(TEXT("InstanceThread WriteFile failed, GLE=%d.\n"), GetLastError());
					break;
				}
				switch (usr)
				{
					case 1:
						cout << "Vvedite path to file:\n";
						cin >> str1;
						cout << "Vvedite new(or old) file name:\n";
						cin >> str2;
						cout << str1 << "\n" << str2;
						strcat(buff, srcpath1);
						strcat(buff, str2);
						cout << "\n";
						fSuccess = WriteFile(pipe,        // handle to pipe 
							str1,     // buffer to write from 
							sizeof(str1) + 1, // number of bytes to write 
							&cbWritten,   // number of bytes written 
							NULL);        // not overlapped I/O 
						if (!fSuccess)
						{
							printf(TEXT("InstanceThread WriteFile failed, GLE=%d.\n"), GetLastError());
							break;
						}
						fSuccess = WriteFile(pipe,        // handle to pipe 
							buff,     // buffer to write from 
							sizeof(buff) + 1, // number of bytes to write 
							&cbWritten,   // number of bytes written 
							NULL);        // not overlapped I/O 
						if (!fSuccess)
						{
							printf(TEXT("InstanceThread WriteFile failed, GLE=%d.\n"), GetLastError());
							break;
						}
						*buff = { 0 };
						*str1 = { 0 };
						break;

				}

			}
			catch (char er)
			{
				cout << "Ошибка:" << er << "\n";
			}

		} while (user != 0);
		// Write the reply to the pipe. 
		
	}
	
	printf("Done!");
	_getch();
}


