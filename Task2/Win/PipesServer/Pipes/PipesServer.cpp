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
#include <Windows.h>
#include <Winbase.h.> 
#include <conio.h>
// C:\Users\deads\source\repos\Pipes\Debug\Pipes.exe -c "\\.\pipe\MyPipe"
int dircheckrec(char* full)
{
	int sum = 0;
	char fname[1000] = { 0 };
	strcat(fname, full);
	strcat(fname, "\\*");
	printf(fname);
	printf("\n");
	WIN32_FIND_DATA fdat;
	HANDLE hdata = FindFirstFileA(fname, &fdat);
	FindNextFile(hdata, &fdat);
	if (hdata != INVALID_HANDLE_VALUE)
	{
		while (FindNextFile(hdata, &fdat) != 0)
		{
			printf("%s", fdat.cFileName);
			printf("\n");
			if (fdat.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				char fullname[1000] = { 0 };
				char* name = fdat.cFileName;
				strcat(fullname, full);
				strcat(fullname, "\\");
				strcat(fullname, name);
				printf(fullname);
				printf("\n");
				printf("----------------\n");
				int papka = dircheckrec(fullname);
				sum += papka;

			}
			else
			{
				int csize = fdat.nFileSizeLow;
				printf("<- Size = %d \n", csize);
				sum += csize;
			}
		}
	}
	printf("-------------------------------------\n");
	FindClose(hdata);
	return sum;
}
int dircheck(char* full)
{
	int sum = 0;
	char fname[1000] = { 0 };
	strcat(fname, full);
	strcat(fname, "\\*");
	printf(fname);
	printf("\n");
	WIN32_FIND_DATA fdat;
	HANDLE hdata = FindFirstFileA(fname, &fdat);
	FindNextFile(hdata, &fdat);
	if (hdata != INVALID_HANDLE_VALUE)
	{
		while (FindNextFile(hdata, &fdat) != 0)
		{
			printf("%s", fdat.cFileName);
			printf("\n");
			if (fdat.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
			}
			else
			{
				int csize = fdat.nFileSizeLow;
				printf("<- Size = %d \n", csize);
				sum += csize;
			}
		}
	}
	FindClose(hdata);
	return sum;
}
int main(int argc, char *argv[])
{
	int sum = 0;
	int	cn = 0;
	bool r = false;
	char d[1000] = { 0 };
	LPCTSTR c = 0;
	DWORD  cbRead;
	DWORD  cbWritten;
	char buf[50] = { '\0' };
	std::cout << "There are " << argc << " arguments:\n";
	for (int count = 0; count < argc; count++)
	{
		std::cout << count << " " << argv[count] << '\n';
	
		if (strcmp(argv[count], "-c") == 0 || strcmp(argv[count], "--channel_name") == 0)
		{
			cn = count + 1;
		}
	}
	strcat(buf, argv[cn]);
	c = buf;
		printf_s("\nc = '%s'\n", c);
		HANDLE pipe =
			CreateNamedPipeA(c, PIPE_ACCESS_DUPLEX,PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,PIPE_UNLIMITED_INSTANCES,512, 512, 5000, NULL);
	
		if (pipe == INVALID_HANDLE_VALUE)
		{
			printf("Can't create named pipe (%d)\n", GetLastError());
			return 1;
		}
		printf("Wait client\n");
		if (ConnectNamedPipe(pipe, NULL))
		{
			printf("Client connected\n");
			
			printf("\nConnect to server\n");
		}
		ReadFile(pipe, d, 512, &cbRead, NULL);
		ReadFile(pipe, &r, 512, &cbRead, NULL);
		printf_s("\nd = %s\n", d);
		if (r == true) printf("\nr  = true");
		if (r == true)
		{
			sum = dircheckrec(d);
		}
		else 
		{
			sum = dircheck(d);
		}
		WriteFile(pipe, &sum, sizeof(sum) + 1, &cbWritten, NULL);
		printf("Done!");
		CloseHandle(pipe);
		_getch();
}