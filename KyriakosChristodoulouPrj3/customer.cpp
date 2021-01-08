#include <iostream>
#include "tables.h"
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

using namespace std;

bool cointoss(){
    if(rand()%2==0)
        return true;
    else
        return false;
}

int main(int argc,char *argv[]){
    srand(time(NULL));
    int i=1;
    bool sflag=false;
    bool dflag=false;
    bool nflag=false;
    int shmid;
    int customers;
    double time;
    //get input from arguments
    if (argc!=7){
        cerr<<"Wrong arguments"<<endl;
        return -1;
    }
    while (i<argc){
        if(strcmp(argv[i],"-s")==0){
            if(sflag==true){
                cerr<<"Wrong arguments"<<endl;
                return -1;
            }
            sflag=true;
            shmid=atoi(argv[i+1]);
        }
        else if(strcmp(argv[i],"-d")==0){
            if(dflag==true){
                cerr<<"Wrong arguments"<<endl;
                return -1;
            }
            dflag=true;
            time=atof(argv[i+1]);
        }
        else if(strcmp(argv[i],"-n")==0){
            if(nflag==true){
                cerr<<"Wrong arguments"<<endl;
                return -1;
            }
            nflag=true;
            customers=atoi(argv[i+1]);
        }
        else{
            cerr<<"Wrong arguments"<<endl;
            return -1;
        }
        i+=2;
    }
    int err=0,retval;
	if (shmid == -1)
		perror ("Creation");
	else
		printf("Customer Allocated. %d\n",(int)shmid);
	/* Attach the segment */
	Shared_Data *mem;
	mem = (Shared_Data *) shmat(shmid, (void*)0, 0);
	if ((Shared_Data*) mem == (void*)-1)
		perror("Attachment. ");
	else
		printf("Customer Attached. \n");
    mem->bar.array=(node*)((void*)mem+sizeof(Shared_Data)+((mem->two+mem->four+mem->six+mem->eight)*sizeof(Table)));
    mem->hall.tarray=(Table*)((void*)mem+sizeof(Shared_Data));
    mem->hall.farray=(Table*)((void*)mem+sizeof(Shared_Data)+(mem->two*sizeof(Table)));
    mem->hall.sarray=(Table*)((void*)mem+sizeof(Shared_Data)+((mem->two+mem->four)*sizeof(Table)));
    mem->hall.earray=(Table*)((void*)mem+sizeof(Shared_Data)+((mem->two+mem->four+mem->six)*sizeof(Table)));
    retval=sem_wait(&mem->doorinfo.doorq);   //getting in queue
    if(retval==-1)
        perror("Wait. ");
    retval=sem_wait(&mem->doorinfo.door);    //lock doorman struct
    if(retval==-1)
        perror("Wait. ");
    cout<<"Access shared mem "<<getpid()<<endl;
    mem->bar.array=(node*)((void*)mem+sizeof(Shared_Data)+((mem->two+mem->four+mem->six+mem->eight)*sizeof(Table)));
    mem->hall.tarray=(Table*)((void*)mem+sizeof(Shared_Data));
    mem->hall.farray=(Table*)((void*)mem+sizeof(Shared_Data)+(mem->two*sizeof(Table)));
    mem->hall.sarray=(Table*)((void*)mem+sizeof(Shared_Data)+((mem->two+mem->four)*sizeof(Table)));
    mem->hall.earray=(Table*)((void*)mem+sizeof(Shared_Data)+((mem->two+mem->four+mem->six)*sizeof(Table)));

    mem->doorinfo.nextinq.info=getpid();    //setting the data
    mem->doorinfo.nextinq.size=customers;
    mem->doorinfo.custwaits=true;
    retval=sem_post(&mem->doorinfo.door);    //unlock doorman struct
    if(retval==-1)
        perror("Post. ");
    retval=sem_post(&mem->doorinfo.wakeup);  //wakeup doorman
    if(retval==-1)
        perror("Post. ");
    sleep(1);
    cout<<"Customer slept"<<endl;
    retval=sem_wait(&mem->doorinfo.door);    //lock doorman struct
    if(retval==-1)
        perror("Wait. ");
    mem->bar.array=(node*)((void*)mem+sizeof(Shared_Data)+((mem->two+mem->four+mem->six+mem->eight)*sizeof(Table)));
    mem->hall.tarray=(Table*)((void*)mem+sizeof(Shared_Data));
    mem->hall.farray=(Table*)((void*)mem+sizeof(Shared_Data)+(mem->two*sizeof(Table)));
    mem->hall.sarray=(Table*)((void*)mem+sizeof(Shared_Data)+((mem->two+mem->four)*sizeof(Table)));
    mem->hall.earray=(Table*)((void*)mem+sizeof(Shared_Data)+((mem->two+mem->four+mem->six)*sizeof(Table)));

    if(mem->doorinfo.status==AVAILABLETABLE){//available table
        cout<<"option 1 "<<getpid()<<endl;
        mem->bar.array=(node*)((void*)mem+sizeof(Shared_Data)+((mem->two+mem->four+mem->six+mem->eight)*sizeof(Table)));
    mem->hall.tarray=(Table*)((void*)mem+sizeof(Shared_Data));
    mem->hall.farray=(Table*)((void*)mem+sizeof(Shared_Data)+(mem->two*sizeof(Table)));
    mem->hall.sarray=(Table*)((void*)mem+sizeof(Shared_Data)+((mem->two+mem->four)*sizeof(Table)));
    mem->hall.earray=(Table*)((void*)mem+sizeof(Shared_Data)+((mem->two+mem->four+mem->six)*sizeof(Table)));

        cout<<" kathetai o "<<mem->doorinfo.availablet->whosit()<<endl;
        mem->doorinfo.availablet->giveit(getpid());
        cout<<"took table"<<endl;
        //sit
    }
    else if(mem->doorinfo.status==AVAILABLEBAR){
        cout<<"option 2 "<<getpid()<<endl;
        if(cointoss()){     //accept waiting at bar
            mem->doorinfo.acceptbar=true;
            //sleep until your turn comes or you want to leave
        }
        else{               //reject bar
            mem->doorinfo.acceptbar=false;
        }
    }
    cout<<"leaving "<<getpid()<<endl;
    retval=sem_post(&mem->doorinfo.door);    //unlock doorman struct
    if(retval==-1)
        perror("post. ");
    retval=sem_post(&mem->doorinfo.doorq);    //let the next customer on q procceed
    if(retval==-1)
        perror("post. ");
    //detaching
    err = shmdt (( void *) mem ) ;
    err = shmctl(shmid, IPC_RMID, 0);
	if (err == -1)
		perror ("Removal.");
	else
		printf("Removed. %d\n", (int)(err));
	return 0;
}


