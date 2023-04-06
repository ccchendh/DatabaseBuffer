#ifndef _LRU_H_
#define _LRU_H_

#include<stdio.h>

//LRU�ڵ�
struct LRUNode
{
	LRUNode* next;
	int page_id;

	LRUNode(LRUNode* ptr, int page) { next = ptr; page_id = page; }
};

//LRU����
class LRU
{
private:
	LRUNode* head;
	LRUNode* rear;
public:
	LRU();
	void MoveToRear(int page_id);//page_id�������򽫶�Ӧ�ڵ�����LRU���н�β��ʹ֮����滻
	int DeleteHead();//�Ƴ�������δ�����ʵĽڵ�
	void InsertRear(int page_id);//�������뻺�����Ľڵ����LRU����
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

