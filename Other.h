#ifndef _OTHER_H_
#define _OTHER_H_


#define FRAMESIZE 4096

//Frame
struct bFrame
{
	char field[FRAMESIZE];
};

//buffer控制块
struct BCB
{
	BCB(int page_id, int frame_id) :page_id(page_id), frame_id(frame_id), fix_count(0), dirty_bit(0), next(NULL) {}
	int page_id;
	int frame_id;
	int fix_count;
	int dirty_bit;
	BCB* next;
};

//读操作还是写操作
int ReadOrWrite;



#define PAGESIZE 4096

struct page {
    int page_id;
    int page_size;
    //其他

};


#endif // !_OTHER_H_
