#include <iostream>
#include "tables.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>

using namespace std;

int main(int argc,char *argv[]){
    int i=1;
    bool sflag=false;
    bool dflag=false;
    int shmid,retval;
    double time;
    //get input from arguments
    if (argc!=5){
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
        else{
            cerr<<"Wrong arguments"<<endl;
            return -1;
        }
        i+=2;
    }
    int err=0;
	if (shmid == -1)
		perror ("Creation");
	else
		printf("Doorman Allocated. %d\n",(int)shmid);
	/* Attach the segment */
	Shared_Data *mem;
	mem = (Shared_Data *) shmat(shmid, (void*)0, 0);
	if ((Shared_Data*) mem == (void*)-1)
		perror("Attachment. \n");
	else
		printf("Doorman Attached. \n");
    while(1){
        mem->bar.array=(node*)((void*)mem+sizeof(Shared_Data)+((mem->two+mem->four+mem->six+mem->eight)*sizeof(Table)));
        mem->hall.tarray=(Table*)((void*)mem+sizeof(Shared_Data));
        mem->hall.farray=(Table*)((void*)mem+sizeof(Shared_Data)+(mem->two*sizeof(Table)));
        mem->hall.sarray=(Table*)((void*)mem+sizeof(Shared_Data)+((mem->two+mem->four)*sizeof(Table)));
        mem->hall.earray=(Table*)((void*)mem+sizeof(Shared_Data)+((mem->two+mem->four+mem->six)*sizeof(Table)));
        retval=sem_wait(&mem->doorinfo.wakeup);  //wake up doorman
        cout<<"Doorman woke up"<<endl;
        if(retval==-1)
            perror("Wait. ");
        //if customer want to leave move as more as you can from bar to tables
        retval=sem_wait(&mem->doorinfo.door);  //lock doorman struct
        if(retval==-1)
            perror("Wait. ");
        mem->bar.array=(node*)((void*)mem+sizeof(Shared_Data)+((mem->two+mem->four+mem->six+mem->eight)*sizeof(Table)));
        mem->hall.tarray=(Table*)((void*)mem+sizeof(Shared_Data));
        mem->hall.farray=(Table*)((void*)mem+sizeof(Shared_Data)+(mem->two*sizeof(Table)));
        mem->hall.sarray=(Table*)((void*)mem+sizeof(Shared_Data)+((mem->two+mem->four)*sizeof(Table)));
        mem->hall.earray=(Table*)((void*)mem+sizeof(Shared_Data)+((mem->two+mem->four+mem->six)*sizeof(Table)));

        if(mem->doorinfo.custwaits==true){      //customer on entrance
            cout<<"Doorman: customer waiting"<<endl;
            retval=sem_wait(&mem->hall.hallg);   //lock hall
            if(retval==-1)
                perror("Wait. ");
            retval=sem_wait(&mem->bar.barg);     //lock bar
            if(retval==-1)
                perror("Wait. ");

            cout<<"Doorman: going to search for table "<<(mem->doorinfo.nextinq.size)<<endl;
            Table* ret=mem->hall.request(mem->doorinfo.nextinq.size); //search for a table
            cout<<"searched"<<endl;
            if(ret==NULL){                  //no table found
                if(!mem->bar.is_full(mem->doorinfo.nextinq.size)){    //if bar is available suggest to customer
                    mem->doorinfo.status=AVAILABLEBAR;
                    retval=sem_post(&mem->doorinfo.door);
                    if(retval==-1)
                        perror("Wait. ");
                    mem->bar.array=(node*)((node*)mem+sizeof(Shared_Data)+((mem->two+mem->four+mem->six+mem->eight)*sizeof(Table)));
                    sleep(1);
                    retval=sem_wait(&mem->doorinfo.door);
                    if(retval==-1)
                        perror("Wait. ");
                    mem->bar.array=(node*)((void*)mem+sizeof(Shared_Data)+((mem->two+mem->four+mem->six+mem->eight)*sizeof(Table)));
                    mem->hall.tarray=(Table*)((void*)mem+sizeof(Shared_Data));
                    mem->hall.farray=(Table*)((void*)mem+sizeof(Shared_Data)+(mem->two*sizeof(Table)));
                    mem->hall.sarray=(Table*)((void*)mem+sizeof(Shared_Data)+((mem->two+mem->four)*sizeof(Table)));
                    mem->hall.earray=(Table*)((void*)mem+sizeof(Shared_Data)+((mem->two+mem->four+mem->six)*sizeof(Table)));
                    if(mem->doorinfo.acceptbar){
                        mem->bar.push(mem->doorinfo.nextinq.info,mem->doorinfo.nextinq.size);
                    }
                }
                else{
                    mem->doorinfo.status=FULL;
                }
            }
            else{
                cout<<"Doorman found table"<<endl;
                mem->doorinfo.status=AVAILABLETABLE;
                mem->doorinfo.availablet=ret;
            }
            cout<<"Searched 2"<<endl;
            mem->doorinfo.custwaits=false;
            mem->doorinfo.nextinq.info=0;
            mem->doorinfo.nextinq.size=0;
            retval=sem_post(&mem->hall.hallg);   //unlock hall
            if(retval==-1)
                perror("Wait. ");
            retval=sem_post(&mem->bar.barg);     //unlock bar
            if(retval==-1)
                perror("Wait. ");
            retval=sem_post(&mem->doorinfo.door); //unlock doorman struct
            if(retval==-1)
                perror("Wait. ");
        }
        cout<<"Doorman going to sleep"<<endl;
    }
    //detaching
    err = shmdt (( void *) mem ) ;
    err = shmctl(shmid, IPC_RMID, 0);
	if (err == -1)
		perror ("Removal.");
	else
		printf("Removed. %d\n", (int)(err));
	return 0;
}

