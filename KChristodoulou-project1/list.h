#ifndef LIST_H
#define LIST_H

#include "accounts.h"

using namespace std;

class Account;

class ListNode{
	private:
		Account* ptr;
		ListNode* Next;
	public:
		ListNode(Account*);
		~ListNode();
		ListNode* fetchNext(){return Next;};
		void addNext(Account* newobj){Next=new ListNode(newobj);};
		Account* fetchptr(){return ptr;};
		void deleteNext();
};

class Lista{
	private:
		ListNode* First;
		int counter;
	public:
		Lista(){First=NULL;counter=0;};
		~Lista(){if(counter>0) emptyList();};
		void addNode(Account*);
		void deleteNode(Account*);
		ListNode* fetchFirst(){return First;};
		int fetchCounter(){return counter;};
		void emptyList();
		void print();
};

#endif
