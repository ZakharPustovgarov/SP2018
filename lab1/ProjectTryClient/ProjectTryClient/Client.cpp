#define _CRT_SECURE_NO_WARNINGS 
#pragma comment( lib, "shlwapi.lib" )
#include <shlwapi.h>
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
#include <fstream>



//	C:\VS\source\repos\Project3\Debug\Project3.exe -c "\\.\pipe\MyPipe" -d "C:\example" -r
const char *srcpath = "C:\\ServerProgVaultClient\\";
const char *metapath = "C:\\ServerProgMetas\\";
const char *pipename1 = "\\\\.\\pipe\\New_pipe";
using namespace std;

struct Metadata
{
	char dnum[100];
	char aname[100];
	char dfname[200];
};

void PrintMainMenu()
{
	cout << "Выберите раздел:" << "\n";
	cout << "1. Add file" << "\n";
	cout << "2. Change \ update file" << "\n";
	cout << "3. Change \ update file's metadata" << "\n";
	cout << "4. Open file" << "\n";
	cout << "5. View full list of files" << "\n";
	cout << "0. Exit" << "\n";
	cout << "> ";
}

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "Russian");
	char user = 0;
	DWORD  cbRead, cbWritten ,dwMode;
	HANDLE pipe;
	BOOL   fSuccess = false;
	WIN32_FIND_DATA fdat;
	HANDLE hdata;
	Metadata mdata;
	*mdata.aname = { 0 };
	*mdata.dfname = { 0 };
	*mdata.dnum = { 0 };
	ifstream fin;
	ofstream fout;
	bool ch = true;
	int usr;
	char str1[1000] = { 0 };
	char str2[1000] = { 0 };
	char buff[1000] = { 0 };
	char fext[10] = { 0 };
	size_t result = 0;
	char * buffer = 0;
	long size = 0;

	
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
			Sleep(1000);
			return -1;
		}

		// All pipe instances are busy, so wait for 20 seconds. 

		if (!WaitNamedPipe(pipename1, 20000))
		{
			cout << "Could not open pipe: 20 second wait timed out.";
			Sleep(1000);
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
	
		// Read client requests from the pipe. This simplistic code only allows messages
		// up to BUFSIZE characters in length.

		do
		{
			try
			{
				PrintMainMenu();
				cin >> user;
				
				usr = atoi(&user);
				fSuccess = WriteFile(
					pipe,        // handle to pipe 
					&user,     // buffer to write from 
					sizeof(user), // number of bytes to write 
					&cbWritten,   // number of bytes written 
					NULL);        // not overlapped I/O 

				if (!fSuccess)
				{
					cout << "InstanceThread WriteFile failed, GLE=" << GetLastError() << "\n";
					break;
				}
				 
				switch (usr)
				{
					case 1:
						ch = true;
						cout << "Enter a name of new file:\n";
						cin >> str2;
						cout << "\n";

						fSuccess = WriteFile(pipe,        // handle to pipe 
							str2,     // buffer to write from 
							sizeof(str2), // number of bytes to write 
							&cbWritten,   // number of bytes written 
							NULL);        // not overlapped I/O 
						if (!fSuccess)
						{
							cout << "InstanceThread WriteFile failed, GLE=" << GetLastError() << "\n";
							break;
						}

						cout << "Enter a path to file (only English letters in a path):\n";
						cin >> str1;
						cout << "\n";						

						strcat(fext, PathFindExtensionA(str1));
						fSuccess = WriteFile(pipe, &fext, sizeof(fext), &cbWritten, NULL);
						if (!fSuccess)
						{
							cout << "InstanceThread WriteFile failed, GLE=" << GetLastError() << "\n";
							break;
						}
						
						fSuccess = ReadFile(pipe, &ch, 1, &cbRead, NULL);
						if (!fSuccess)
						{
							cout << "InstanceThread ReadFile failed, GLE=" << GetLastError() << "\n";
							break;
						}
						if (ch == false)
						{
							cout << "Error: file with this name and extension already exist.\nUse 'Change \ update file' to rewrite file that already exist\n";
							ch = true;
							*str1 = { 0 };
							*str2 = { 0 };
							*fext = { 0 };
							break;
						}
						
						cout << "Enter a number of document (use underscore '_' instead of space ' '):\n";
						cin >> mdata.dnum;
						cout << "\n";
						cout << "Enter a author of document (use underscore '_' instead of space ' '):\n";
						cin >> mdata.aname;
						cout << "\n";
						cout << "Enter a full name of document (use underscore '_' instead of space ' '):\n";
						cin >> mdata.dfname;
						cout << "\n";

						// определяем размер файла
						fin.open(str1, ios_base::binary | ios_base::ate);
						if (fin.is_open() != 1) cout << "Error: file isn't open";
						size = fin.tellg();
						fin.close();

						buffer = (char*)malloc(sizeof(char) * size); 
						if (buffer == NULL)
						{
							fputs("Ошибка памяти", stderr);
							exit(2);
						}
						
						fin.open(str1, ios_base::binary | ios_base::in);
						if (fin.is_open() != 1) cout << "Error: file isn't open";
						fin.read(buffer, size);
						fin.close();

						//содержимое файла теперь находится в буфере
						
						cout << size << "\n";
						fSuccess = WriteFile(pipe, &size, sizeof(size), &cbWritten, NULL);
						if (!fSuccess)
						{
							cout << "InstanceThread WriteFile failed, GLE=" << GetLastError() << "\n";
							break;
						}

						fSuccess = WriteFile(pipe,buffer, (sizeof(char) * size),&cbWritten,NULL);
						if (!fSuccess)
						{
							cout << "InstanceThread WriteFile failed, GLE=" << GetLastError() << "\n";
							break;
						}

						fSuccess = WriteFile(pipe, &mdata, sizeof(mdata), &cbWritten, NULL);
						if (!fSuccess)
						{
							cout << "InstanceThread WriteFile failed, GLE=" << GetLastError() << "\n";
							break;
						}

						*str1 = { 0 };
						*str2 = { 0 };
						*fext = { 0 };
						*buffer = { 0 };
						size = 0;
						*mdata.aname = { 0 };
						*mdata.dfname = { 0 };
						*mdata.dnum = { 0 };
					
						break;

					case 2:
						ch = true;
						cout << "Enter a name of file that you want to change:\n";
						cin >> str2;
						cout << "\n";

						fSuccess = WriteFile(pipe,str2,sizeof(str2),&cbWritten,NULL);  
						if (!fSuccess)
						{
							cout << "InstanceThread WriteFile failed, GLE=" << GetLastError() << "\n";
							break;
						}

						cout << "Enter a path to file (only English letters in a path):\n";
						cin >> str1;
						strcat(fext, PathFindExtensionA(str1));

						fSuccess = WriteFile(pipe, &fext, sizeof(fext), &cbWritten, NULL);
						if (!fSuccess)
						{
							cout << "InstanceThread WriteFile failed, GLE=" << GetLastError() << "\n";
							break;
						}

						fSuccess = ReadFile(pipe, &ch, 1, &cbRead, NULL);
						if (!fSuccess)
						{
							cout << "InstanceThread ReadFile failed, GLE=" << GetLastError() << "\n";
							break;
						}
						if (ch == true)
						{
							cout << "Error: file with this name and extension was not found.\nUse 'View full list of files' to get list of all files on server and try again\n";
							*str1 = { 0 };
							*str2 = { 0 };
							*fext = { 0 };
							break;
						}

						// определяем размер файла
						fin.open(str1, ios_base::binary | ios_base::ate);
						if (fin.is_open() != 1) cout << "Error: file isn't open";
						size = fin.tellg();
						fin.close();

						buffer = (char*)malloc(sizeof(char) * size);
						if (buffer == NULL)
						{
							cout << "Ошибка памяти:" << stderr << "\n";
							exit(2);
						}

						fin.open(str1, ios_base::binary | ios_base::in);
						if (fin.is_open() != 1) cout << "Error: file isn't open";
						fin.read(buffer, size);
						fin.close();

						//содержимое файла теперь находится в буфере
						cout << size << "\n";
						fSuccess = WriteFile(pipe, &size, sizeof(size), &cbWritten, NULL);
						if (!fSuccess)
						{
							cout << "InstanceThread WriteFile failed, GLE=" << GetLastError() << "\n";
							break;
						}

						fSuccess = WriteFile(pipe, buffer, (sizeof(char) * size), &cbWritten, NULL);
						if (!fSuccess)
						{
							cout << "InstanceThread WriteFile failed, GLE=" << GetLastError() << "\n";
							break;
						}

						*str1 = { 0 };
						*str2 = { 0 };
						*fext = { 0 };
						ch = true;
						break;

					case 3:
						ch = true;
						cout << "Enter a name and extension of file whose metadata you want to change:\n";
						cin >> str2;
						cout << "\n";

						fSuccess = WriteFile(pipe, str2, sizeof(str2), &cbWritten, NULL);
						if (!fSuccess)
						{
							cout << "InstanceThread WriteFile failed, GLE=" << GetLastError() << "\n";
							break;
						}

						fSuccess = ReadFile(pipe, &ch, 1, &cbRead, NULL);
						if (!fSuccess)
						{
							cout << "InstanceThread ReadFile failed, GLE=" << GetLastError() << "\n";
							break;
						}
						if (ch == true)
						{
							cout << "Error: file with this name and extension was not found.\nUse 'View full list of files' to get list of all files on server and try again\n";
							*str1 = { 0 };
							*str2 = { 0 };
							*fext = { 0 };
							break;
						}


						cout << "Enter a number of document (use underscore '_' instead of space ' '):\n";
						cin >> mdata.dnum;
						cout << "\n";
						cout << "Enter a author of document (use underscore '_' instead of space ' '):\n";
						cin >> mdata.aname;
						cout << "\n";
						cout << "Enter a full name of document (use underscore '_' instead of space ' '):\n";
						cin >> mdata.dfname;
						cout << "\n";

						fSuccess = WriteFile(pipe, &mdata, sizeof(mdata), &cbWritten, NULL);
						if (!fSuccess)
						{
							cout << "InstanceThread WriteFile failed, GLE=" << GetLastError() << "\n";
							break;
						}

						*str1 = { 0 };
						*str2 = { 0 };
						*fext = { 0 };
						*mdata.aname = { 0 };
						*mdata.dfname = { 0 };
						*mdata.dnum = { 0 };
						ch = true;
						break;

					case 4:
						ch = true;
						cout << "Enter a name and extension of file whose metadata you want to change:\n";
						cin >> str2;
						cout << "\n";

						fSuccess = WriteFile(pipe, str2, sizeof(str2), &cbWritten, NULL);
						if (!fSuccess)
						{
							cout << "InstanceThread WriteFile failed, GLE=" << GetLastError() << "\n";
							break;
						}

						fSuccess = ReadFile(pipe, &ch, 1, &cbRead, NULL);
						if (!fSuccess)
						{
							cout << "InstanceThread ReadFile failed, GLE=" << GetLastError() << "\n";
							break;
						}
						if (ch == true)
						{
							cout << "Error: file with this name and extension was not found.\nUse 'View full list of files' to get list of all files on server and try again\n";
							*str1 = { 0 };
							*str2 = { 0 };
							*fext = { 0 };
							break;
						}

						CreateDirectoryA(srcpath, NULL);

						fSuccess = ReadFile(pipe, &size, 4, &cbRead, NULL);
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


						fSuccess = ReadFile(pipe, buffer, (sizeof(char) * size), &cbRead, NULL);
						if (!fSuccess)
						{
							cout << "1-3. InstanceThread ReadFile failed, GLE=" << GetLastError() << "\n";
							break;
						}
						else
						{
							cout << "1-3. ReadDone!\n";
						}

						cout << size <<"\n";
						strcat(buff, srcpath);
						strcat(buff, str2);
						fout.open(buff, ios_base::out | ios_base::binary);
						if (fout.is_open() != 1) cout << "Error: file isn't open\n";
						fout.write(buffer, size);
						fout.close();
						ShellExecute(NULL, "open", buff, NULL, NULL, SW_SHOWNORMAL);

						*str1 = { 0 };
						*str2 = { 0 };
						*fext = { 0 };
						*buff = { 0 };
						*buffer = { 0 };
						size = 0;
						ch = true;
						break;

					case 5:

						do
						{
							*str1 = NULL;
							fSuccess = ReadFile(pipe, str1, 1000, &cbRead, NULL);
							if (!fSuccess)
							{
								cout << "InstanceThread ReadFile failed, GLE=" << GetLastError() << "\n";
								break;
							}
							
							cout << str1 << "\n";
							
						} while (strcmp(str1,"End of list\n") != 0);
						break;	
				}

			}
			catch (char er)
			{
				cout << "Error:" << er << "\n";
			}
			

		} while (usr != 0);
	
	
	printf("Done!");
	
}


