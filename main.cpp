#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <math.h>
#include "Buffer.h"

using namespace std;

void createFile();//����һ��Ŀ¼ʽ�ļ�

void run();//����ģ�����

void PrintIOInfo(int read_op_times, int write_op_times, int read_hitbuf_times, \
    int write_hitbuf_times, int read_io_times, int write_io_times);//��ӡIO��Ϣ


int main()
{
    createFile();

    run();

    return 0;
}

void createFile()
{
    FILE* fp;
    fopen_s(&fp, "E:/Homework/Advanced Database/Lab2/data.dbf", "wb+");
    int dirsize = MAXPAGES * sizeof(int);
    int pagesize = PAGESIZE;
    int pos = (sizeof(int) + dirsize) / PAGESIZE + (((sizeof(int) + dirsize) % PAGESIZE) == 0 ? 0 : 1);
    int* dir = new int[MAXPAGES];
    for (int i = 0; i < MAXPAGES; i++)
    {
        dir[i] = i;
    }
    char* page = new char[PAGESIZE];
    //��������ʼҳ��д���ļ���ʼλ�á�
    fwrite(&pos, sizeof(int), 1, fp);
    //���ļ�Ŀ¼д���ļ�
    fwrite(dir, sizeof(int), MAXPAGES, fp);
    //���ļ��ֳ�MAXPAGES��PAGESIZE��С��ҳ
    for (int i = 0; i < MAXPAGES; i++) 
    {
        page[0] = i;
        fwrite(page, sizeof(page), 1, fp);
    }
    fclose(fp);
    delete[] dir;
    delete[] page;
}

void run()
{

    BMgr buffer;
    int read_op_times = 0;
    int write_op_times = 0;
    string trace;
    ifstream file("E:/Homework/Advanced Database/Lab2/data-5w-50w-zipf.txt");

    bFrame input;
    strcpy_s(input.field,28, "writing--------------------");
    while (!file.eof())//����trace�ļ�ÿһ�У���ȡ����
    {
        getline(file, trace);
        int operation = stoi(trace.substr(0, 1));
        int page_id = stoi(trace.substr(2));
        if (operation == 0)
        {
            ReadOrWrite = 0;
            ++read_op_times;
            buffer.read(page_id);
        }
        else
        {
            ReadOrWrite = 1;
            ++write_op_times;
            buffer.write(page_id, input);
        }
    }

    file.close();


    PrintIOInfo(read_op_times, write_op_times, buffer.get_read_hitbuf_times(), \
                buffer.get_write_hitbuf_times(), buffer.get_read_io_times(), buffer.get_write_io_times());
}

void PrintIOInfo(int read_op_times, int write_op_times, \
                 int read_hitbuf_times, int write_hitbuf_times, int read_io_times, int write_io_times)
{
    int total_op = read_op_times + write_op_times;
    int total_io = read_io_times + write_io_times;
    int total_hitbuf = read_hitbuf_times + write_hitbuf_times;
    float hitrate = 1.0 * total_hitbuf / total_op;
    cout << "��������С��" <<  DEFBUFSIZE << endl;
    cout << "�������ܴ�����" << read_op_times << endl;
    cout << "д�����ܴ�����" << write_op_times << endl;
    cout << "�����д�����" << read_hitbuf_times << endl;
    cout << "д���д�����" << write_hitbuf_times << endl;
    cout << "��IO������" << read_io_times << endl;
    cout << "дIO������" << write_io_times << endl;
    cout << "IO�ܴ�����" << total_io << endl;
    cout << "�����ܴ�����" << total_hitbuf << endl;
    cout << "���и��ʣ�" << 100 * hitrate << " % " << endl;
}