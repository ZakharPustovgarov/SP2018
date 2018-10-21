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
//	C:\Users\deads\source\repos\PipesClient\Debug\PipesClient.exe -c "\\.\pipe\MyPipe" -d "C:\examples" -r

int main(int argc, char* argv[])
{
	int sum = 0;
	int	cn;
	int dn;
	bool r = false;
	char d[1000] = {0};
	LPCTSTR c = 0;
	DWORD  cbWritten;
	DWORD  cbRead;
	char   szBuf[256];
	std::cout << "There are " << argc << " arguments:\n";
	for (int count = 0; count < argc; count++)
	{
		std::cout << count << " " << argv[count] << '\n';

		if (strcmp(argv[count], "-c") == 0 || strcmp(argv[count], "--channel_name") == 0)
		{
			cn = count + 1;
			c = argv[cn];
		}
		if (strcmp(argv[count], "-d") == 0 || strcmp(argv[count], "--directory_path") == 0)
		{
			dn = count + 1;
			strcat(d, argv[dn]);
		}
		if (strcmp(argv[count], "-r") == 0)
		{
			r = true;
		}
	}
	if (r == true)printf("\nr  = true");
	printf_s("\nd = %s", d);
	printf("\nConnect to server\n");
	HANDLE pipe = CreateFileA(c, GENERIC_READ | GENERIC_WRITE,
		0, NULL, OPEN_EXISTING, 0, NULL);
	printf("Connected\n");
	
	

	WriteFile(pipe,d, strlen(d) + 1, &cbWritten, NULL);
	WriteFile(pipe, &r, sizeof(r) + 1, &cbWritten, NULL);
	ReadFile(pipe, &sum, 512, &cbRead, NULL);
	printf("\n sum = %d \n",sum);
	WaitForSingleObject(pipe, INFINITE);
	printf("Done!");
	_getch();
}