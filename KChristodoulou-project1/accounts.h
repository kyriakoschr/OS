#ifndef ACCOUNTS_H
#define ACCOUNTS_H

#include <iostream>
#include <cstring>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "list.h"
#define MaxLevel 4
#define MaxValue 1000000
#define MinValue 99999

using namespace std;

class Lista;
class ListNode;

class Account{
	private:
		int studid;
		char* firstname;
		char* lastname;
		double gpa;
		int numofcourses;
		char* deprt;
		int postcode;
	public:
		Account(const int,char*,char*,const double,const int,char*,const int);
		~Account();
		double getGPA(){return gpa;};
		int getPC(){return postcode;};
		char* getDPRT(){return deprt;};
		int getID(){return studid;};
		int getNOC(){return numofcourses;};
		void print(){cout<<studid<<" "<<lastname<<" "<<firstname<<" "<<gpa<<" "<<numofcourses<<" "<<deprt<<" "<<postcode<<endl;};
		void modify(const double newGPA,const int newNumofcourses){gpa=newGPA;numofcourses=newNumofcourses;};
};

class SkipListNode{
	private:
		int key;
		Account* ptr;
		SkipListNode** forwardtbl;
	public:
		SkipListNode(SkipListNode*,const int,Account*);
		SkipListNode(const int);
		int getkey(){return key;};
		SkipListNode* getFT(const int pos){if((pos<MaxLevel)&&(pos>-1)) return forwardtbl[pos]; else {cout<<"Wrong array position"<<endl; return this;}};
		~SkipListNode();
		SkipListNode(SkipListNode*);
		void updateFT(SkipListNode**,const int);
		void setFT(const int pos,SkipListNode* temp){forwardtbl[pos]=temp;};
		Account* getAcc(){return ptr;};
};

class SkipList{
	private:
		SkipListNode* First;
		SkipListNode* Last;
		int counter;
	public:
		SkipList();
		~SkipList();
		SkipListNode* fetchfirst(){return First;};
		SkipListNode* fetchlast(){return Last;};
		int fetchCounter(){return counter;};
		Account* insertNode(const int,char*,char*,const double,const int,char*,const int);
		SkipListNode* findStud(const int);
		int modify(const int,const double,const int);
		void deleteNode(const int);
		double raverage(const int,const int);
		void bottom(const int);
		void findGPA(const double);

};

void bubbleSort(Account**,const int);


#endif
