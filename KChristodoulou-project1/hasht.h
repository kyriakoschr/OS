#include <iostream>
#include "accounts.h"
#define MAXPOSTCODE 100000
#define MINPOSTCODE 9999

extern int htsize;

class HashTableCell{
	private:
		Lista lista;
	public:
        void addNode(Account* input){lista.addNode(input);input->print();};
        void deleteNode(Account* input){lista.deleteNode(input);};
        double getaverage(const int);
        ListNode* getlisthead(){return lista.fetchFirst();};
        int getlistcntr(){return lista.fetchCounter();};
};

class HashTable{
	private:
		HashTableCell** HTable;
	public:
		HashTable(const int);
		~HashTable();
        void addNode(Account*);
        void deleteNode(Account*);
        double average(const int);
        void taverage(const int,const int);
        int coursestotake(const int,char*);
        int studentsinpc(const int);
        int* percentiles();
};

