#include "bar.h"

void Queue::initQueue(int cap){
    //rear = NULL;
    //front = NULL;
    //array=arr;
    for (int i=0;i<cap;i++){
        array[i].info=0;
        //array[i].next=NULL;
        array[i].size=0;
    }
    max=cap;
    people=0;
    counter=0;
    if( sem_init ( &(barg) ,1 ,1) ==-1 )
        perror("Init. ");
}

void Queue::push(pid_t data,int s){
    array[counter].info=data;
    array[counter].size=s;
    //node *temp = new node;
    //temp->info = data;
    //temp->size=s;
    //temp->next = NULL;
    /*if(front == NULL){
        front = temp;
    }else{
        //rear->next = temp;
        rear=array[counter];
    }*/
    //rear = temp;
    counter++;
    people+=s;
}

pid_t Queue::pop(int cap){
    pid_t result;
    //cout<<"in pop "<<front->info<<endl;
/*    if(front->size<=cap){
        temp=front;
        result=temp->info;
        front=front->next;
        delete temp;
        for(int i=1;i<counter;i++)
            array[i-1]=array[i];
        counter--;
        rear=array[counter-1];
        return result;
    }*/
    for(int i=0;i<counter;i++){
        if(array[i].size<=cap){
            result=array[i].info;
            people-=array[i].size;
            for(int j=i+1;j<counter;j++)
                array[j-1]=array[j];
            counter--;
            return result;
        }
    }
    return 0;
}

bool Queue::is_empty()
{
    if(counter == 0){
        return true;
    }else{
        return false;
    }
}

bool Queue::is_full(int cap)
{
    if((max-people )>=cap){
        return true;
    }else{
        return false;
    }
}

Queue::~Queue(){
   // delete[] array;
}
