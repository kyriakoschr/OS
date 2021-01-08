#include "list.h"

using namespace std;

ListNode::ListNode(Account* object){
	ptr=object;
	Next=NULL;
}

ListNode::~ListNode(){
    ptr=NULL;
    Next=NULL;
}

void Lista::addNode(Account* periexomeno){
	ListNode* temp=First;
	if(counter==0){
		First=new ListNode(periexomeno);
		counter++;
	}
	else if (counter==1){
		First->addNext(periexomeno);
		counter++;
	}
	else{
		for(int i=0;i<counter-1;i++)
			temp=temp->fetchNext();
		temp->addNext(periexomeno);
		counter++;
	}
}

void Lista::deleteNode(Account* target){
	ListNode* temp=First;
	if(First->fetchptr()==target){
		First=First->fetchNext();
		counter--;
		delete temp;
	}
	else{
		for(int i=0;i<counter-1;i++)
			if(temp->fetchNext()->fetchptr()==target){
				temp->deleteNext();
				counter--;
				return;
			}
			else
				temp=temp->fetchNext();
		if(temp->fetchNext()->fetchptr()==target){
			temp->deleteNext();
			counter--;
		}
	}
}

void ListNode::deleteNext(){
	ListNode* temp=Next;
	Next=temp->fetchNext();
	delete temp;
}

void Lista::emptyList(){
    for(int i=0;i<counter-1;i++)
        First->deleteNext();
    delete First;
    First=NULL;
    counter=0;
}

void Lista::print(){
	ListNode* temp=First;
	for (int i=0;i<counter;i++){
		temp->fetchptr()->print();
		temp=temp->fetchNext();
	}
}

