#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <math.h>
#include "Buffer.h"

using namespace std;

void createFile();//创建一个目录式文件

void run();//运行模拟程序

void PrintIOInfo(int read_op_times, int write_op_times, int read_hitbuf_times, \
    int write_hitbuf_times, int read_io_times, int write_io_times);//打印IO信息


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
    //将数据起始页号写入文件起始位置。
    fwrite(&pos, sizeof(int), 1, fp);
    //将文件目录写入文件
    fwrite(dir, sizeof(int), MAXPAGES, fp);
    //将文件分成MAXPAGES个PAGESIZE大小的页
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
    while (!file.eof())//读入trace文件每一行，获取操作
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
    cout << "缓冲区大小：" <<  DEFBUFSIZE << endl;
    cout << "读操作总次数：" << read_op_times << endl;
    cout << "写操作总次数：" << write_op_times << endl;
    cout << "读命中次数：" << read_hitbuf_times << endl;
    cout << "写命中次数：" << write_hitbuf_times << endl;
    cout << "读IO次数：" << read_io_times << endl;
    cout << "写IO次数：" << write_io_times << endl;
    cout << "IO总次数：" << total_io << endl;
    cout << "命中总次数：" << total_hitbuf << endl;
    cout << "命中概率：" << 100 * hitrate << " % " << endl;
}