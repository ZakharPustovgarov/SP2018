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
using namespace std;
// -w -i 5 "notepad.exe"
int main(int argc, char *argv[])
{
	bool w = false;
	int in = 1;
	int an = NULL;
	int arg = argc;
		cout << "There are " << argc << " arguments:\n";
		for (int count = 0; count < argc; count++)
		{
			cout << count << " " << argv[count] << '\n';
			if (strcmp(argv[count],"-w") == 0 || strcmp(argv[count], "--wait") == 0)
			{
				w = true;
			}
			if (strcmp(argv[count], "-i") == 0 || strcmp(argv[count], "--instances") == 0)
			{
				in = atoi(argv[count + 1]);;
			}

			if (strcmp(argv[count], "-a") == 0 || strcmp(argv[count], "--arguments") == 0)
			{
				an = count+1;	
			}
		}
	if (w==true)printf("\nw = true");
	cout << "\nin = %d" << in << "\n";
	cout << "\nan = %d" << an << "\n";
	cout << "\n %s" << argv[an] << "\n";
	int t = atoi(getenv("MAX_INSTANCES"));
	if ( t < in) in = t;
	cout <<  "\n t = %d" << t << "\n";
	char str[1000] = { 0 };
	strcat(str, argv[argc - 1]);
	strcat(str, " ");
	strcat(str, argv[an]);
	cout << "\n";
	cout << str;
	int termstat;
	PROCESS_INFORMATION pInfo;
	STARTUPINFO startupInfo;
	HANDLE* handles = new HANDLE[in];
	ZeroMemory(&startupInfo, sizeof(startupInfo));
	startupInfo.cb = sizeof(startupInfo);
	for (int i=0;i<in;i++)
	{
		if (CreateProcess(NULL, str, NULL, NULL, FALSE, 0, NULL, NULL, &startupInfo, &pInfo)) printf("\n Done!");
		else ("Error!");
		handles[i] = pInfo.hProcess;
	}
	
		WaitForMultipleObjects(in, handles, true, INFINITE);
		cout << "\nMission complete!";
		delete handles;
		CloseHandle(pInfo.hProcess);
		CloseHandle(pInfo.hThread);
	return 0;
		
}

