#ifndef _DATA_STORAGE_H_
#define _DATA_STORAGE_H_

#include "File.h"
#include "Other.h"


//存储管理类，类内包含一个目录式文件
class DSMgr
{
public:
	DSMgr();
	void OpenFile(const char* filename = "data.dbf");
	void CloseFile();
	void ReadPage(int page_id, bFrame* frame);//读文件页
	void WritePage(int page_id, bFrame* frame);//写文件页
	int get_read_io_times() { return read_io_times; }
	int get_write_io_times() { return write_io_times; }
private:
	void Seek(int offset, int pos);//移动文件指针
	FILE* GetFile();
	void IncNumPages();
	int GetNumPages();
	void SetUse(int index, int use_bit);
	int GetUse(int index);
private:
	File* curfile;
	int numPages;//页数量
	int read_io_times;
	int write_io_times;

};


DSMgr::DSMgr()
{
	curfile = new File();
	OpenFile();
	numPages = MAXPAGES;
	read_io_times = 0;
	write_io_times = 0;
}
void DSMgr::OpenFile(const char* filename)
{
	curfile->OpenFile(filename);
}
void DSMgr::CloseFile()
{
	curfile->CloseFile();
}
void DSMgr::ReadPage(int page_id, bFrame* frame)
{
	read_io_times++;
	int offset = (curfile->pages)[page_id] * PAGESIZE;
	Seek(curfile->data_start_pos, offset);
	fread(frame, sizeof(bFrame), 1, curfile->file);

}
void DSMgr::WritePage(int page_id, bFrame* frame)
{
	write_io_times++;
	int offset = (curfile->pages)[page_id] * PAGESIZE;
	Seek(curfile->data_start_pos, offset);
	fwrite(frame, sizeof(bFrame), 1, curfile->file);
}
void DSMgr::Seek(int offset, int pos)
{
	fseek(curfile->file, pos + offset, 0);
}
FILE* DSMgr::GetFile() {
	return curfile->file;
}
void DSMgr::IncNumPages()
{
	numPages++;
}
int DSMgr::GetNumPages()
{
	return numPages;
}
void DSMgr::SetUse(int index, int use_bit)
{
	//
}
int DSMgr::GetUse(int index)
{
	return 0;
}



#endif // !_DATA_STORAGE_H_
