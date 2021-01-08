#include "accounts.h"

using namespace std;

void bubbleSort( Account** AcArray, const int counter ){ //sort the array descending
    for (int i = 0;i<counter-1;i++)
        for (int j=i+1; j<counter; j++)
            if (AcArray[i]->getGPA() < AcArray[j]->getGPA()){
                Account* temp=AcArray[i];
                AcArray[i]=AcArray[j];
                AcArray[j]=temp;
            }
}

int randomLevel(){
    return rand()%MaxLevel;
}

SkipList::~SkipList(){
    for(int i=0;i<counter;i++)
        deleteNode(First->getFT(0)->getAcc()->getID());
}

Account::~Account(){
    delete[] firstname;
    delete[] lastname;
    delete[] deprt;
}

Account::Account(const int Id,char* lname,char* fname,const double grades,const int lessons,char* depart,const int postc){
	studid=Id;
	lastname=new char[strlen(lname)];
	strcpy(lastname,lname);
	firstname=new char[strlen(fname)];
	strcpy(firstname,fname);
	gpa=grades;
	numofcourses=lessons;
	deprt=new char[strlen(depart)];
	strcpy(deprt,depart);
	postcode=postc;
}

SkipListNode::SkipListNode(SkipListNode* last,const int level,Account* newacc){
	ptr=newacc;
	forwardtbl=new SkipListNode*[MaxLevel];
    key=newacc->getID();
	for(int i=0;i<level+1;i++)
        forwardtbl[i]=last;
    for(int i=level+1;i<MaxLevel;i++)
        forwardtbl[i]=NULL;

}

SkipListNode::SkipListNode(const int maxid){        //constructor of the tail
    ptr=NULL;
    key=maxid;
    forwardtbl=new SkipListNode*[MaxLevel];
    for(int i=0;i<MaxLevel;i++)
        forwardtbl[i]=NULL;
}

SkipListNode::SkipListNode(SkipListNode* last){ //constructor for the head of the skip list
    ptr=NULL;
    forwardtbl=new SkipListNode*[MaxLevel];
    key=MinValue;
    for(int i=0;i<MaxLevel;i++)
        forwardtbl[i]=last;
}

SkipListNode::~SkipListNode(){  //destructor
    if(ptr!=NULL)
        delete ptr;
    for(int i=0;i<MaxLevel;i++)
        forwardtbl[i]=NULL;
    delete[] forwardtbl;
}

SkipList::SkipList(){           //first constructs the tail and then the head, sets counter to 0
    Last=new SkipListNode(MaxValue);
    First=new SkipListNode(Last);
    counter=0;
}

void SkipListNode::updateFT(SkipListNode** newFT,const int lvl){    //updates the forward table
    for(int i=0;i<lvl;i++)
        forwardtbl[i]=newFT[i]->getFT(i);
}

Account* SkipList::insertNode(const int Id, char* lname, char* fname,const double grades,const int lessons, char* depart,const int postc){
    Account* newacc=new Account(Id,lname,fname,grades,lessons,depart,postc); //allocates memory for the new account
    SkipListNode* update[MaxLevel];
    SkipListNode* x=fetchfirst();                   //following the algorithm frome the notes
    for(int i=MaxLevel-1;i>-1;i--){
        while (x->getFT(i)->getkey()<Id)
            x=x->getFT(i);
        update[i]=x;
    }
    x=x->getFT(0);
    if(x->getkey()==Id){             //if the id already exists return NULL and free the memory allocated before
        delete newacc;
        return NULL;
    }
    else{
        int lvl=randomLevel();
        SkipListNode* newAcc=new SkipListNode(fetchlast(),lvl,newacc);  //create a skiplist node for the new node
        counter++;
        newAcc->updateFT(update,lvl+1); //update its forward table
        for(int i=0;i<lvl+1;i++)
            update[i]->setFT(i,newAcc); //update the pointers of the others
    }
    return newacc;  //return the account pointer so that the hash table will use it to insert
}

void SkipList::deleteNode(const int ID){
    SkipListNode* update[MaxLevel];                                     //following the algorithm frome the notes
    SkipListNode* x=fetchfirst();
    for(int i=MaxLevel-1;i>-1;i--){
        while(x->getFT(i)->getkey()<ID)
            x=x->getFT(i);
        update[i]=x;
    }
    x=x->getFT(0);
    if (x->getkey()==ID){
        for(int i=0;i<MaxLevel;i++){
            if(update[i]->getFT(i)!=x)
                break;
            update[i]->setFT(i,x->getFT(i));
        }
    if (x!=NULL)
        delete x;
        counter--;
    }
}

SkipListNode* SkipList::findStud(const int ID){
    SkipListNode* x=fetchfirst();
    for (int i= MaxLevel-1;i>-1;i--)
        while (x->getFT(i)->getkey()<ID)
            x=x->getFT(i);
    x=x->getFT(0);
    if(x->getkey()==ID)
        return x;
    else
        return NULL;
}

int SkipList::modify(const int ID,const double newGPA,const int newLessons){
    Account* target=findStud(ID)->getAcc();
    if (target==NULL)
        return 0;
    else
        target->modify(newGPA,newLessons);
    return 1;
}

double SkipList::raverage(const int ID1,const int ID2){
    SkipListNode* target1=findStud(ID1);
    SkipListNode* target2=findStud(ID2);    //if there are no accounts with the given ids
    bool found=true;                        //search in that range for the closer to them
    if (target1==NULL){
        found=false;
        for(int i=1;i<ID2-ID1+1;i++){
            target1=findStud(ID1+i);
            if(target1!=NULL){
                found=true;
                break;
            }
        }
    }
    if (found==false)
        return -1;
    if (target2==NULL){
        found=false;
        for(int i=1;i<ID2-ID1+1;i++){
            target2=findStud(ID2-i);
            if(target2!=NULL){
                found=true;
                break;
            }
        }
    }
    if (found==false)   //if didnt found abort
        return -1;
    int temp=ID1;
    double sum=0;
    int counter=0;
    while(temp<=ID2){   //else calculate the average
        sum+=target1->getAcc()->getGPA();
        counter++;
        target1=target1->getFT(0);
        temp=target1->getkey();
    }
    return sum/counter;
}

void SkipList::bottom(const int k){
    Account** arr=new Account*[k];  //use an array of k positions
    SkipListNode* temp=First->getFT(0); //and check every account in the skiplist
    for(int i=0;i<k;i++){
        arr[i]=temp->getAcc();          //for the first k accounts they are going in the array anyway
        temp=temp->getFT(0);
    }
    bubbleSort(arr,k);                  //sort them
    for(int i=0;i<counter-k;i++){       //and then if there is an account with gpa smaller than the bigger exchange and sort again
        if(temp->getAcc()->getGPA()<arr[0]->getGPA()){
            arr[0]=temp->getAcc();
            bubbleSort(arr,k);
        }
        temp=temp->getFT(0);
    }
    for(int i=0;i<k;i++)
        arr[i]->print();
}

void SkipList::findGPA(const double minGPA){
    int maxCTT=-1;
    Lista templist;             // a temporary list with the accounts to be printed
    Account* tempacc;
    SkipListNode* temp=First->getFT(0); //takes the first account in the skiplist(with the smaller ID)
    if(temp!=NULL)
        tempacc=temp->getAcc();
    for(int i=0;i<counter;i++){
        if(tempacc->getGPA()>minGPA){      //if has gpa greater than the given
             if(tempacc->getNOC()==maxCTT)  //check if has the same number of courses to take as the max
                templist.addNode(tempacc);  //if so add it to the list
            else if(tempacc->getNOC()>maxCTT){  //else check if has more courses to take
                templist.emptyList();           //if so empty the list, add it to the list and change the max
                templist.addNode(tempacc);
                maxCTT=tempacc->getNOC();
            }
        }
        temp=temp->getFT(0);            //get the next account in the list
        if(temp!=NULL)
            tempacc=temp->getAcc();
    }
    templist.print();                      //print the list
    cout<<maxCTT<<endl;                     //print the max couses to take
    templist.emptyList();               //free the memory used
}
