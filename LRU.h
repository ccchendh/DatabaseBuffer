#ifndef _LRU_H_
#define _LRU_H_

#include<stdio.h>

//LRU节点
struct LRUNode
{
	LRUNode* next;
	int page_id;

	LRUNode(LRUNode* ptr, int page) { next = ptr; page_id = page; }
};

//LRU链队
class LRU
{
private:
	LRUNode* head;
	LRUNode* rear;
public:
	LRU();
	void MoveToRear(int page_id);//page_id被访问则将对应节点移至LRU队列结尾，使之最后被替换
	int DeleteHead();//移除最近最久未被访问的节点
	void InsertRear(int page_id);//将新移入缓冲区的节点加入LRU队列
	~LRU();
};

LRU::LRU()
{
    head = NULL;
    rear = NULL;
}

void LRU::MoveToRear(int page_id)
{
    if (head == rear||rear->page_id==page_id)
        return;

    if (head->page_id == page_id)
    {
        int deleted_page_id = DeleteHead();
        InsertRear(deleted_page_id);
    }
    else
    {
        LRUNode* ptr = head;
        while (ptr->next->page_id != page_id)
        {
            ptr = ptr->next;
        }
        if (ptr->next->next != NULL)
        {
            LRUNode* tmp = ptr->next;
            ptr->next = tmp->next;
            InsertRear(page_id);
            delete tmp;
        }
    }


}

int LRU::DeleteHead()
{
    if ((head == rear) && (head == NULL))
    {
        return -1;
    }
    else if (rear == head)
    {
        LRUNode* tmp = head;
        int page_id = tmp->page_id;
        head = NULL;
        rear = NULL;
        delete tmp;
        return page_id;
    }
    else
    {
        LRUNode* tmp = head;
        head = head->next;
        int page_id = tmp->page_id;
        delete tmp;
        return page_id;
    }
}

void LRU::InsertRear(int page_id)
{
    if ((head == rear) && (head == NULL))
    {
        head = new LRUNode(NULL, page_id);
        rear = head;
    }
    else
    {
        rear->next = new LRUNode(NULL, page_id);
        rear = rear->next;
    }
}

LRU::~LRU()
{
    while (DeleteHead() != -1);
}

#endif // !_LRU_H_

