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
#include <fstream>


const char *srcpath = "C:\\ServerProgVault\\";
const char *metapath = "C:\\ServerProgMetas\\";
const char *pipename = "\\\\.\\pipe\\New_pipe";
using namespace std;

 struct Metadata
{
	 char dnum[100];
	 char aname[100];
	 char dfname[200];
 };

DWORD WINAPI InstanceThread(LPVOID lpvParam)
// This routine is a thread processing function to read from and reply to a client
// via the open pipe connection passed from the main loop. Note this allows
// the main loop to continue executing, potentially creating more threads of
// of this procedure to run concurrently, depending on the number of incoming
// client connections.
{
	DWORD  cbWritten = 0;
	DWORD  cbRead = 0;

	BOOL fSuccess = FALSE;
	HANDLE hPipe = NULL;
	WIN32_FIND_DATA fdat;
	HANDLE hdata;
	Metadata mdata; 
	*mdata.aname = { 0 };
	*mdata.dfname = { 0 };
	*mdata.dnum = { 0 };

	CreateDirectoryA(srcpath, NULL);
	CreateDirectoryA(metapath, NULL);
	setlocale(LC_ALL, "Russian");
	char user;
	int usr;
	char str1[1000] = { 0 };
	char str2[1000] = { 0 };
	char buff[1000] = { 0 };
	char buff1[1000] = { 0 };
	char fext[10] = { 0 };
	char * buffer = 0;
	long size = 0;
	ofstream fout;
	ifstream fin;
	bool ch = true;


	// Do some extra error checking since the app will keep running even if this
	// thread fails.

	if (lpvParam == NULL)
	{
		printf("\nERROR - Pipe Server Failure:\n");
		printf("   InstanceThread got an unexpected NULL value in lpvParam.\n");
		printf("   InstanceThread exitting.\n");
		return -1;
	}


	// Print verbose messages. In production code, this should be for debugging only.
	printf("InstanceThread created, receiving and processing messages.\n");

	// The thread's parameter is a handle to a pipe object instance. 

	hPipe = (HANDLE)lpvParam;

	// Loop until done reading

	///////////////-w -i 5 -a "C:\ServerProgVault\asdfasdf.txt" notepad.exe
	do
	{
		fSuccess = ReadFile(hPipe, &user, 512, &cbRead, NULL);
		if (!fSuccess)
		{
			cout << "0. InstanceThread ReadFile failed, GLE=" << GetLastError() << "\n";
			break;
		}
		else
		{
			cout << "0. ReadDone!\n";
		}
		try
		{
			usr = atoi(&user);
			switch (usr)
			{
			case 1:
				ch = true;
				fSuccess = ReadFile(hPipe, str2, 1000, &cbRead, NULL);
				if (!fSuccess)
				{
					cout << "1-1.InstanceThread ReadFile failed, GLE=" << GetLastError() << "\n";
					break;
				}
				else
				{
					cout << "1-1. ReadDone!\n";
				}

				fSuccess = ReadFile(hPipe, fext, 10, &cbRead, NULL);
				if (!fSuccess)
				{
					cout << "1-extension.InstanceThread ReadFile failed, GLE=" << GetLastError() << "\n";
					break;
				}
				else
				{
					cout << "1-extension. ReadDone!\n";
				}

				strcat(buff1, srcpath);
				strcat(buff1, "*.*");
				strcat(buff, str2);
				strcat(buff, fext);
				hdata = FindFirstFileA(buff1, &fdat);
				if (hdata != INVALID_HANDLE_VALUE)
				{
					while (FindNextFile(hdata, &fdat) != 0)
					{
						cout << "\n" << fdat.cFileName << " ? " << buff << "\n";
						if (strcmp(fdat.cFileName, buff) == 0)
						{
							cout << "Equal..\n";
							ch = false;
							break;
						}
						else
						{
							cout << "Not equal!\n";
						}
					}
				}
				FindClose(hdata);
				*buff1 = { 0 };
				*buff = { 0 };
				WriteFile(hPipe, &ch, 1, &cbWritten, NULL);
				if (ch == false)
				{	
					cout << "Bad things happens\n";
					*str2 = { 0 };
					*fext = { 0 };
					size = 0;
					break;
				}
				
				strcat(buff, srcpath);
				strcat(buff, str2);
				strcat(buff, fext);
				
				fSuccess = ReadFile(hPipe, &size, 4, &cbRead, NULL);
				if (!fSuccess)
				{
					cout << "1-2. InstanceThread ReadFile failed, GLE=" << GetLastError() << "\n";
					break;
				}
				else
				{
					cout << "1-2. ReadDone!\n";
				}

				cout << size << "\n";
				buffer = (char*)malloc(sizeof(char) * size); // выделить память для хранения содержимого файла
				if (buffer == NULL)
				{
					fputs("Ошибка памяти", stderr);
					exit(2);
				}


				fSuccess = ReadFile(hPipe, buffer, (sizeof(char) * size), &cbRead, NULL);
				if (!fSuccess)
				{
					cout << "1-3. InstanceThread ReadFile failed, GLE=" << GetLastError() << "\n";
					break;
				}
				else
				{
					cout << "1-3. ReadDone!\n";
				}

				fSuccess = ReadFile(hPipe, &mdata, sizeof(mdata), &cbRead, NULL);
				if (!fSuccess)
				{
					cout << "1-4. InstanceThread ReadFile failed, GLE=" << GetLastError() << "\n";
					break;
				}
				else
				{
					cout << "1-4. ReadDone!\n";
				}

				fout.open(buff, ios_base::out | ios_base::binary);
				if (fout.is_open() != 1) cout << "Error: file isn't open\n";
				fout.write(buffer, size);
				fout.close();

				strcat(buff1, metapath);
				strcat(buff1, "_");
				strcat(buff1, str2);
				strcat(buff1, fext);
				strcat(buff1, ".txt");
				strcat(mdata.aname, "\n");
				strcat(mdata.dfname, "\n");
				strcat(mdata.dnum, "\n");

				fout.open(buff1, ios_base::out );
				if (fout.is_open() != 1) cout << "Error: file isn't open";
				fout << mdata.dnum;
				fout << mdata.aname;
				fout << mdata.dfname;
				fout.close();

				*buff = { 0 };
				*buff1 = { 0 };
				*str2 = { 0 };
				*fext = { 0 };
				size = 0;
				*mdata.aname = { 0 };
				*mdata.dfname = { 0 };
				*mdata.dnum = { 0 };
				break;

			case 2:
				ch = true;
				fSuccess = ReadFile(hPipe, str2, 1000, &cbRead, NULL);
				if (!fSuccess)
				{
					cout << "1-1.InstanceThread ReadFile failed, GLE=" << GetLastError() << "\n";
					break;
				}
				else
				{
					cout << "1-1. ReadDone!\n";
				}

				fSuccess = ReadFile(hPipe, fext, 10, &cbRead, NULL);
				if (!fSuccess)
				{
					cout << "1-extension.InstanceThread ReadFile failed, GLE=" << GetLastError() << "\n";
					break;
				}
				else
				{
					cout << "1-extension. ReadDone!\n";
				}

				strcat(buff1, srcpath);
				strcat(buff1, "*.*");
				strcat(buff, str2);
				strcat(buff, fext);
				hdata = FindFirstFileA(buff1, &fdat);
				if (hdata != INVALID_HANDLE_VALUE)
				{
					while (FindNextFile(hdata, &fdat) != 0)
					{
						cout << "\n" << fdat.cFileName << " ? " << buff << "\n";
						if (strcmp(fdat.cFileName, buff) == 0)
						{
							cout << "Equal..\n";
							ch = false;
							break;
						}
						else
						{
							cout << "Not equal!\n";
						}
					}
				}
				FindClose(hdata);
				*buff1 = { 0 };
				*buff = { 0 };
				WriteFile(hPipe, &ch, 1, &cbWritten, NULL);
				if (ch == true)
				{
					cout << "Bad things happens\n";
					*str2 = { 0 };
					*fext = { 0 };
					size = 0;
					break;
				}

				strcat(buff, srcpath);
				strcat(buff, str2);
				strcat(buff, fext);

				fSuccess = ReadFile(hPipe, &size, 4, &cbRead, NULL);
				if (!fSuccess)
				{
					cout << "1-2. InstanceThread ReadFile failed, GLE=" << GetLastError() << "\n";
					break;
				}
				else
				{
					cout << "1-2. ReadDone!\n";
				}

				cout << size << "\n";
				buffer = (char*)malloc(sizeof(char) * size); // выделить память для хранения содержимого файла
				if (buffer == NULL)
				{
					fputs("Ошибка памяти", stderr);
					exit(2);
				}


				fSuccess = ReadFile(hPipe, buffer, (sizeof(char) * size), &cbRead, NULL);
				if (!fSuccess)
				{
					cout << "1-3. InstanceThread ReadFile failed, GLE=" << GetLastError() << "\n";
					break;
				}
				else
				{
					cout << "1-3. ReadDone!\n";
				}
				fout.open(buff, ios_base::out | ios_base::binary);
				if (fout.is_open() != 1) cout << "Error: file isn't open\n";
				fout.write(buffer, size);
				fout.close();
				*buff = { 0 };
				*buff1 = { 0 };
				*str2 = { 0 };
				*fext = { 0 };
				size = 0;
				ch = true;
				break;

			case 3:
				ch = true;
				fSuccess = ReadFile(hPipe, str2, 1000, &cbRead, NULL);
				if (!fSuccess)
				{
					cout << "1-1.InstanceThread ReadFile failed, GLE=" << GetLastError() << "\n";
					break;
				}
				else
				{
					cout << "1-1. ReadDone!\n";
				}

				strcat(buff1, srcpath);
				strcat(buff1, "*.*");
				hdata = FindFirstFileA(buff1, &fdat);
				if (hdata != INVALID_HANDLE_VALUE)
				{
					while (FindNextFile(hdata, &fdat) != 0)
					{
						cout << "\n" << fdat.cFileName << " ? " << str2 << "\n";
						if (strcmp(fdat.cFileName, str2) == 0)
						{
							cout << "Equal..\n";
							ch = false;
							break;
						}
						else
						{
							cout << "Not equal!\n";
						}
					}
				}
				FindClose(hdata);
				*buff1 = { 0 };
				WriteFile(hPipe, &ch, 1, &cbWritten, NULL);
				if (ch == true)
				{
					cout << "Bad things happens\n";
					*str2 = { 0 };
					*fext = { 0 };
					size = 0;
					break;
				}


				fSuccess = ReadFile(hPipe, &mdata, sizeof(mdata), &cbRead, NULL);
				if (!fSuccess)
				{
					cout << "1-4. InstanceThread ReadFile failed, GLE=" << GetLastError() << "\n";
					break;
				}
				else
				{
					cout << "1-4. ReadDone!\n";
				}

				strcat(buff1, metapath);
				strcat(buff1, "_");
				strcat(buff1, str2);
				strcat(buff1, ".txt");
				strcat(mdata.aname, "\n");
				strcat(mdata.dfname, "\n");
				strcat(mdata.dnum, "\n");

				fout.open(buff1, ios_base::out);
				if (fout.is_open() != 1) cout << "Error: file isn't open";
				fout << mdata.dnum;
				fout << mdata.aname;
				fout << mdata.dfname;
				fout.close();

				*buff1 = { 0 };
				*str2 = { 0 };
				*mdata.aname = { 0 };
				*mdata.dfname = { 0 };
				*mdata.dnum = { 0 };
				ch = true;

				break;
			
			case 4:
				ch = true;
				fSuccess = ReadFile(hPipe, str2, 1000, &cbRead, NULL);
				if (!fSuccess)
				{
					cout << "1-1.InstanceThread ReadFile failed, GLE=" << GetLastError() << "\n";
					break;
				}
				else
				{
					cout << "1-1. ReadDone!\n";
				}

				strcat(buff1, srcpath);
				strcat(buff1, "*.*");
				hdata = FindFirstFileA(buff1, &fdat);
				if (hdata != INVALID_HANDLE_VALUE)
				{
					while (FindNextFile(hdata, &fdat) != 0)
					{
						cout << "\n" << fdat.cFileName << " ? " << str2 << "\n";
						if (strcmp(fdat.cFileName, str2) == 0)
						{
							cout << "Equal..\n";
							ch = false;
							break;
						}
						else
						{
							cout << "Not equal!\n";
						}
					}
				}
				FindClose(hdata);
				*buff1 = { 0 };
				WriteFile(hPipe, &ch, 1, &cbWritten, NULL);
				if (ch == true)
				{
					cout << "Bad things happens\n";
					*str2 = { 0 };
					*fext = { 0 };
					size = 0;
					break;
				}
				// определяем размер файла
				strcat(buff1, srcpath);
				strcat(buff1, str2);
				fin.open(buff1, ios_base::binary | ios_base::ate);
				if (fin.is_open() != 1) cout << "Error: file isn't open";
				size = fin.tellg();
				fin.close();

				buffer = (char*)malloc(sizeof(char) * size);
				if (buffer == NULL)
				{
					fputs("Ошибка памяти", stderr);
					exit(2);
				}

				fin.open(buff1, ios_base::binary | ios_base::in);
				if (fin.is_open() != 1) cout << "Error: file isn't open";
				fin.read(buffer, size);
				fin.close();

				//содержимое файла теперь находится в буфере

				cout << size << "\n";
				fSuccess = WriteFile(hPipe, &size, 4, &cbWritten, NULL);
				if (!fSuccess)
				{
					cout << "InstanceThread WriteFile failed, GLE=" << GetLastError() << "\n";
					break;
				}

				fSuccess = WriteFile(hPipe, buffer, (sizeof(char) * size), &cbWritten, NULL);
				if (!fSuccess)
				{
					cout << "InstanceThread WriteFile failed, GLE=" << GetLastError() << "\n";
					break;
				}

				*str1 = { 0 };
				*str2 = { 0 };
				*fext = { 0 };
				*buffer = { 0 };
				*buff1 = { 0 };
				size = 0;
				ch = true;
				break;

			case 5:
				strcat(buff1, srcpath);
				strcat(buff1, "*.*");
				hdata = FindFirstFileA(buff1, &fdat);
	
				cout << buff1 << "\n";
				cout << fdat.cFileName << "\n";
				if (hdata != INVALID_HANDLE_VALUE)
				{   
					while (FindNextFile(hdata, &fdat) != 0)
					{
						cout << fdat.cFileName << "\n";
						fSuccess = WriteFile(hPipe, &fdat.cFileName, 1000, &cbWritten, NULL);

						if (!fSuccess)
						{
							cout << "5-1. InstanceThread WriteFile failed, GLE=" << GetLastError() << "\n";
							break;
						}
						cout << "5-1. Write done!\n";
					}
				}
				FindClose(hdata);
				fSuccess = WriteFile(hPipe, "End of list\n", 1000, &cbWritten, NULL);

				if (!fSuccess)
				{
					cout << "5-2. InstanceThread WriteFile failed, GLE=" << GetLastError() << "\n";
					break;
				}
				*buff1 = { 0 };
				break;
			}

		}
		catch (char er)
		{
			cout << "Ошибка:" << er << "\n";
		}

	} while (usr != 0);

	// Flush the pipe to allow the client to read the pipe's contents 
	// before disconnecting. Then disconnect the pipe, and close the 
	// handle to this pipe instance. 

	FlushFileBuffers(hPipe);
	DisconnectNamedPipe(hPipe);
	CloseHandle(hPipe);


	printf("InstanceThread exitting.\n");
	return 1;
}


int main(int argc, char *argv[])
{
	DWORD  dwThreadId = 0;

	for (;;)
	{
		HANDLE pipe =
			CreateNamedPipeA(pipename, PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, 512, 512, 5000, NULL);
		if (pipe == INVALID_HANDLE_VALUE)
		{
			printf(TEXT("CreateNamedPipe failed, GLE=%d.\n"), GetLastError());
			return -1;
		}

		printf("Wait client\n");
		if (ConnectNamedPipe(pipe, NULL))
		{
			printf("Client connected\n");

			HANDLE thread;
			thread = CreateThread(
				NULL,              // no security attribute 
				0,                 // default stack size 
				InstanceThread,    // thread proc
				(LPVOID)pipe,    // thread parameter 
				0,                 // not suspended 
				&dwThreadId);      // returns thread ID 

			if (thread == NULL)
			{
				printf(TEXT("CreateThread failed, GLE=%d.\n"), GetLastError());
				return -1;
			}
			else CloseHandle(thread);

		}
		else
			// The client could not connect, so close the pipe. 
			CloseHandle(pipe);
	}
	return 0;

}