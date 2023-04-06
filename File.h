#ifndef _FILE_H_
#define _FILE_H_

#include <stdio.h>
#include "Other.h"

#define MAXPAGES 50000

class File {

public:
	FILE* file;
	int pages[MAXPAGES];//��ȡ�ļ�Ŀ¼
	int data_start_pos;//�ļ�������ʼ��ַ

public:
	void OpenFile(const char* filename);//���ļ�
	void CloseFile();//�ر��ļ�
};

void File::OpenFile(const char* filename) 
{
	fopen_s(&file, "E:/Homework/Advanced Database/Lab2/data.dbf", "rb+");
	fread(&data_start_pos, sizeof(int), 1, file);
	fread(pages, sizeof(int), 50000, file);
}

void File::CloseFile() 
{
	fclose(file);
}

#endif // !_FILE_H_
