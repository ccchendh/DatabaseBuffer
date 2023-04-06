#ifndef _BUFFER_H_
#define _BUFFER_H_

#include "DataStorage.h"
#include "LRU.h"
#include "Other.h"

#define DEFBUFSIZE 1024

bFrame buf[DEFBUFSIZE];//实际缓冲区

class BMgr
{
public:
	BMgr();
    ~BMgr();
    bFrame& read(int page_id);//供外界对页进行读操作
    void write(int page_id, bFrame input);//供外界对页进行写操作
    int get_read_io_times() { return storage.get_read_io_times(); }
    int get_write_io_times() { return storage.get_write_io_times(); }
    int get_read_hitbuf_times() { return read_hitbuf_times; }
    int get_write_hitbuf_times() { return write_hitbuf_times; }

private:
    int  FixPage(int page_id);
    void FixNewPage(int page_id);
    int  UnfixPage(int page_id);
    int NumFreeFrames();
    int  SelectVictim();
    int  Hash(int  page_id);
    void RemoveBCB(int page_id);
    int RemoveLRUEle();
    void SetDirty(int  frame_id);
    void UnsetDirty(int  frame_id);
    void WriteDirtys();
    void PrintFrame(int frame_id);
    BCB* GetBCBByPage(int page_id);

private:
    DSMgr storage;
    LRU* lru;//LRU队列
	int  ftop[DEFBUFSIZE];//frame->page
	BCB* ptof[DEFBUFSIZE];//page->BCB(frame)
    int read_hitbuf_times;
    int write_hitbuf_times;
};


BMgr::BMgr()
{
    //初始化Hash表
    for (int i = 0; i < DEFBUFSIZE; ++i)
    {
        ftop[i] = -1;
        ptof[i] = NULL;
    }
    lru = new LRU();
    read_hitbuf_times = 0;
    write_hitbuf_times = 0;
}

BMgr::~BMgr()
{
    WriteDirtys();//写回所有脏页

}

bFrame& BMgr::read(int page_id)
{
    int frame_id = FixPage(page_id);
    return buf[frame_id];
}

void BMgr::write(int page_id, bFrame input)
{
    int frame_id = FixPage(page_id);
    strcpy_s(buf[frame_id].field,strlen(input.field)+1, input.field); 
    SetDirty(frame_id);
}

BCB* BMgr::GetBCBByPage(int page_id)
{
    BCB* ptr = ptof[page_id % DEFBUFSIZE];
    while (ptr != NULL)
    {
        if (ptr->page_id == page_id)
            break;
        else
            ptr = ptr->next;
    }
    return ptr;
}

int BMgr::FixPage(int page_id)
{
    int ret_frame_id;
    BCB* ptr = GetBCBByPage(page_id);
    if (ptr==NULL)//未命中，将页调入
    {
        int freeframe_id = NumFreeFrames();
        if (freeframe_id == -1)//缓冲区满，则调用SelectVictim函数淘汰一页
        {
            freeframe_id = SelectVictim();
        }
        BCB* f_ptr = ptof[page_id % DEFBUFSIZE];
        lru->InsertRear(page_id);
        storage.ReadPage(page_id, &(buf[freeframe_id]));
        if (f_ptr == NULL)
        {
            ptof[page_id % DEFBUFSIZE] = new BCB(page_id, freeframe_id);
            ptof[page_id % DEFBUFSIZE]->fix_count++;
        }
        else
        {
            BCB* tmp = new BCB(page_id, freeframe_id);
            tmp->next = f_ptr->next;
            f_ptr->next = tmp;
            tmp->fix_count++;
        }
        ftop[freeframe_id] = page_id;
        ret_frame_id = freeframe_id;
    }
    else//命中
    {
        if(ReadOrWrite)
            write_hitbuf_times++;
        else
            read_hitbuf_times++;
        lru->MoveToRear(page_id);
        ptr->fix_count++;
        ret_frame_id = ptr->frame_id;
    }
    return ret_frame_id;
}

void BMgr::FixNewPage(int page_id)
{
    //
}

int BMgr::UnfixPage(int page_id)
{
    BCB* ptr = GetBCBByPage(page_id);
    ptr->fix_count--;
    return ptr->frame_id;
}

int BMgr::NumFreeFrames()
{
    int ret_frame_id = -1;
    for (int i = DEFBUFSIZE-1; i >= 0; i--)
    {
        if (ftop[i] == -1)
        {
            ret_frame_id = i;
            break;
        }
    }
    return ret_frame_id;
}

int BMgr::SelectVictim()
{
    int page_id = RemoveLRUEle();
    int frame_id = GetBCBByPage(page_id)->frame_id;
    if (GetBCBByPage(page_id)->dirty_bit == true)
    {
        storage.WritePage(frame_id, &buf[frame_id]);
    }
    RemoveBCB(page_id);
    
    return frame_id;
}

void BMgr::RemoveBCB(int page_id)
{
    BCB* ptr = ptof[page_id % DEFBUFSIZE];

    if (ptr->page_id == page_id)
    {
        ptof[page_id % DEFBUFSIZE] = ptof[page_id % DEFBUFSIZE]->next;
        ftop[ptr->frame_id] = -1;
        delete ptr;
    }
    else
    {
        while (ptr->next != NULL)
        {
            if (ptr->next->page_id == page_id)
            {
                BCB* tmp = ptr->next;
                ptr->next = ptr->next->next;
                ftop[tmp->frame_id] = -1;
                delete tmp;
            }
            else
            {
                ptr = ptr->next;
            }
        }
    }
}

int BMgr::RemoveLRUEle()//从LRU队列删除淘汰页
{
    return lru->DeleteHead();
}

int BMgr::Hash(int page_id)//page->frame
{
    BCB* ptr = GetBCBByPage(page_id);
    
    return ptr ? ptr->frame_id : -1;
}

void BMgr::SetDirty(int frame_id)
{
    GetBCBByPage(ftop[frame_id])->dirty_bit = true;
}

void BMgr::UnsetDirty(int frame_id)
{
    GetBCBByPage(ftop[frame_id])->dirty_bit = false;
}

void BMgr::WriteDirtys()//写所有脏页回磁盘
{
    for (int i = 0; i < DEFBUFSIZE; ++i)
    {
        int page_id = ftop[i];
        BCB* ptr = GetBCBByPage(page_id);
        if (page_id != -1)
        {
            if (ptr->dirty_bit == true)
            {
                storage.WritePage(page_id, &buf[ptr->frame_id]);
                ptr->dirty_bit = false;
            }
        }
    }
}

void BMgr::PrintFrame(int frame_id) 
{
    printf("frame_id: %d\ncontent: %s", frame_id, buf[frame_id]);
}



#endif // !_BUFFER_H_
