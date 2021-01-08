#include "tables.h"

using namespace std;

Table::Table(){
    available=UNUSED;
    capacity=0;
}

void Hall::initHall(int tw,int fou,int si,int eigh){
    two=tw;
    four=fou;
    six=si;
    eight=eigh;
    for(int i=0;i<two;i++){
        tarray[i].setcap(2);
    }
    for(int i=0;i<four;i++){
        farray[i].setcap(4);
    }
    for(int i=0;i<six;i++){
        sarray[i].setcap(6);
    }
    for(int i=0;i<eight;i++){
        earray[i].setcap(8);
    }
    if( sem_init ( &(hallg) ,1 ,1) == -1)
        perror("Init. ");
    /*tarray=tar;
    farray=far;
    sarray=sar;
    earray=ear;*/
}

/*
Hall::~Hall(){
    for(int i=0;i<size;i++)
        delete tarray[i];
    delete[] tarray;
}
*/
Table* Hall::request(int capacity){
    cout<<"Hall access is ok"<<endl;
    for(int i=0;i<two;i++)
        cout<<"test "<<tarray[i].cap()<<endl;
    if (capacity==1||capacity==2){
        for(int i=0;i<two;i++)
            if(tarray[i].isavailable())
                return &tarray[i];
        for(int i=0;i<four;i++)
            if(farray[i].isavailable())
                return &farray[i];
        for(int i=0;i<six;i++)
            if(sarray[i].isavailable())
                return &sarray[i];
        for(int i=0;i<eight;i++)
            if(earray[i].isavailable())
                return &earray[i];
        return NULL;
    }
    else if (capacity==4||capacity==3){
        for(int i=0;i<four;i++)
            if(farray[i].isavailable())
                return &farray[i];
        for(int i=0;i<six;i++)
            if(sarray[i].isavailable())
                return &sarray[i];
        for(int i=0;i<eight;i++)
            if(earray[i].isavailable())
                return &earray[i];
        return NULL;
    }
    else if (capacity==5||capacity==6){
        for(int i=0;i<six;i++)
            if(sarray[i].isavailable())
                return &sarray[i];
        for(int i=0;i<eight;i++)
            if(earray[i].isavailable())
                return &earray[i];
        return NULL;
    }
    else if (capacity==7||capacity==8){
        for(int i=0;i<eight;i++)
            if(earray[i].isavailable())
                return &earray[i];
        return NULL;
    }
    return NULL;
}
