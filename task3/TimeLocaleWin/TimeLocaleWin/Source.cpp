#define _CRT_SECURE_NO_WARNINGS 
#include <stdlib.h> 
#include <windows.h> 
#include <cstdio> 
#include <stdio.h> 
#include <iostream> 
#include <cstdlib> 
#include <conio.h>  
#include <string> 
#include <sstream> 
#include <time.h>
#include <Windows.h.> 
#include <vector>
#include <algorithm>
using namespace std;
int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "Russian");
	if (strcmp(argv[1], "sub") != 0 && strcmp(argv[1], "add") != 0 && strcmp(argv[1], "diff") != 0)
	{
		printf("Error: invalid arguments");
		_getch();
		return -1;
	}

	bool add = false;
	bool sub = false;
	bool diff = false;
	char* vari = 0;
	int val = 0;
	time_t t;
	t = time(&t);
	tm *localt = localtime(&t);
	tm *time1 = localtime(&t);
	tm *time2 = localtime(&t);
	time_t td1;
	time_t td2;
	
	char buff[100];

	for (int count = 0; count < argc; count++)
	{
		if (strcmp(argv[count], "add") == 0)
		{
			add = true;
		}
		if (strcmp(argv[count], "sub") == 0)
		{
			sub = true;
		}
		if (strcmp(argv[count], "add") == 0 || strcmp(argv[count], "sub") == 0)
		{
			if (strcmp(argv[count + 1], "day") != 0 &&
				strcmp(argv[count + 1], "hour") != 0 &&
				strcmp(argv[count + 1], "min") != 0 &&
				strcmp(argv[count + 1], "sec") != 0)
			{
				printf("Error: invalid arguments for add/sub");
				_getch();
				return -1;
			}
			vari = argv[count + 1];
			val = atoi(argv[count + 2]);
		}

		if (strcmp(argv[count], "diff") == 0)
		{
			diff = true;

			string t1 = argv[count + 1];
			string t2 = argv[count + 2];
			if (std::count(t1.begin(), t1.end(), ':'))
			{

			}
			else {
				printf("Error: invalid arguments for diff: wrong separator");
				_getch();
				return -1;
			}
			replace(t1.begin(), t1.end(), ':', ' ');  
			replace(t2.begin(), t2.end(), ':', ' ');
			vector<int> array1;
			vector<int> array2;
			stringstream ss1(t1);
			int temp1;
			while (ss1 >> temp1)
				array1.push_back(temp1);
			cout << array1.at(0) << " " << array1.at(1) << " " << array1.at(2) << "\n";
			stringstream ss2(t2);
			int temp2;
			while (ss2 >> temp2)
				array2.push_back(temp2);
			cout << array2.at(0) << " " << array2.at(1) << " " << array2.at(2) << "\n";
			if (array1.at(0) > 23 || array2.at(0) > 23 || array1.at(1) > 59 || array2.at(1) > 59 || array1.at(2) > 59 || array2.at(2) > 59 ||
				array1.at(0) < 0 || array2.at(0) < 0 || array1.at(1) < 0 || array2.at(1) < 0 || array1.at(2) < 0 || array2.at(2) < 0)
			{
				printf("Error: invalid arguments for diff: wrong hours, minutes or seconds values");
				_getch();
				return -1;
			}
			time1->tm_hour = array1.at(0);
			time1->tm_min = array1.at(1);
			time1->tm_sec = array1.at(2);
			td1 = mktime(time1);
			cout << time1->tm_hour << " " << time1->tm_min << " " << time1->tm_sec << "\n";
			time2->tm_hour = array2.at(0);
			time2->tm_min = array2.at(1);
			time2->tm_sec = array2.at(2);
			td2 = mktime(time2);
			cout << time2->tm_hour << " " << time2->tm_min << " " << time2->tm_sec << "\n";
		}
	}

	

	if (strcmp(argv[1], "sub") == 0 || strcmp(argv[1], "add") == 0)
	{ 
		strftime(buff, sizeof(buff), "%H:%M:%S %d.%m.%Y", localt);
		printf("original local time: %s\n", buff);
		if (add == true)
		{
			if (strcmp(vari, "day") == 0) localt->tm_mday += val;
			if (strcmp(vari, "hour") == 0) localt->tm_hour += val;
			if (strcmp(vari, "min") == 0) localt->tm_min += val;
			if (strcmp(vari, "sec") == 0) localt->tm_sec += val;

		}

		if (sub == true)
		{
			if (strcmp(vari, "day") == 0) localt->tm_mday -= val;
			if (strcmp(vari, "hour") == 0) localt->tm_hour -= val;
			if (strcmp(vari, "min") == 0) localt->tm_min -= val;
			if (strcmp(vari, "sec") == 0) localt->tm_sec -= val;
		}

		time_t conv = mktime(localt);
		localt = localtime(&conv);

		strftime(buff, sizeof(buff), "%H:%M:%S %d.%m.%Y", localt);
		printf("changed local time: %s\n", buff);

	}

	if (diff == true)
	{
		double differtime = difftime(td1, td2);
		differtime = abs(differtime);
		printf("\ndifference in %6.0f seconds.\n", differtime);
	}

	_getch();
}