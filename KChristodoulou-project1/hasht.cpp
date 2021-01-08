#include "hasht.h"

using namespace std;

int hashf(const int input){
    return input%htsize;
}

HashTable::HashTable(const int HTSize){
	HTable=new HashTableCell*[HTSize];
	for(int i=0;i<htsize;i++)
		HTable[i]=new HashTableCell();
}

void HashTable::addNode(Account* input){    //adding the account in the correct bucket
    HTable[hashf(input->getPC())]->addNode(input);
}

void HashTable::deleteNode(Account* input){ //deleting the account from the correct bucket
    HTable[hashf(input->getPC())]->deleteNode(input);
}

double HashTable::average(const int postcode){
    return HTable[hashf(postcode)]->getaverage(postcode);
}

double HashTableCell::getaverage(const int postc){
    int counter=0;
    double sum =0;
    ListNode* temp=lista.fetchFirst();
    for (int i=0;i<lista.fetchCounter();i++){
        if(temp->fetchptr()->getPC()==postc){
            counter++;
            sum+=temp->fetchptr()->getGPA();
        }
        temp=temp->fetchNext();
    }
    return sum/counter;
}

void HashTable::taverage(const int k,const int postc){
    Account** arr=new Account*[k];  //allocates an array for the best k
    int counter=0;
    HashTableCell* bucket=HTable[hashf(postc)];
    ListNode* temp=bucket->getlisthead();
    bool firsttime=false;
    for(int i=0;i<bucket->getlistcntr();i++){
        if(temp->fetchptr()->getPC()==postc)
            if(counter<k){  //add the first k that have the right postcode
                arr[counter]=temp->fetchptr();
                counter++;
            }
            else{   //the first time an extra account occurs sort the array
                if(firsttime==false){
                    bubbleSort(arr,counter);
                    firsttime=true;
                }
                if(arr[k-1]->getGPA()<temp->fetchptr()->getGPA()){  //the next time it will occur will be sorted
                    arr[k-1]=temp->fetchptr();                      //then compare with the last item and if a change occurs resort
                    bubbleSort(arr,counter);
                }
            }
        temp=temp->fetchNext();                             //fetch next
    }
    bubbleSort(arr,counter);                                //sort for the   last time
    for(int i=0;i<counter;i++)
        arr[i]->print();                                    //and print them
}

int HashTable::coursestotake(const int postc,char* depart){
    HashTableCell* bucket=HTable[hashf(postc)];             //get in the right bucket
    int sum=0;
    ListNode* temp=bucket->getlisthead();                   //and check all its entries
    for(int i =0;i<bucket->getlistcntr();i++){
        int postcode=temp->fetchptr()->getPC();
        char* deprt=temp->fetchptr()->getDPRT();
        if((postcode==postc)&&(strcmp(deprt,depart)==0)){   //if one has the right postcode and department
            sum+=temp->fetchptr()->getNOC();                //add to the sum and print it
            temp->fetchptr()->print();
        }
        temp=temp->fetchNext();
    }
    return sum;                                             //return the sum

}

int HashTable::studentsinpc(const int postc){               //the same as above but it just returns how many students have that postcode
    HashTableCell* bucket=HTable[hashf(postc)];
    int sum=0;
    ListNode* temp=bucket->getlisthead();
    for(int i=0;i<bucket->getlistcntr();i++){
        if(temp->fetchptr()->getPC()==postc)
            sum++;
        temp=temp->fetchNext();
    }
    return sum;

}

HashTable::~HashTable(){
	for(int i=0;i<htsize;i++)
		delete HTable[i];
	delete[] HTable;
}

int* HashTable::percentiles(){                          //creates an array of integers with positions as many as the eligible postcodes
    int* temparray=new int[MAXPOSTCODE-MINPOSTCODE];
    for(int i=0;i<htsize;i++){
        HashTableCell* bucket=HTable[i];                //for each account add one in its postcode counter
        ListNode* temp=bucket->getlisthead();
        for(int j=0;j<bucket->getlistcntr();j++){
            temparray[temp->fetchptr()->getPC()-MINPOSTCODE]++;
            temp=temp->fetchNext();
        }
    }
    return temparray;    //return that array
}

