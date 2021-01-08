#include <iostream>
#include "tables.h"
#include <stdio.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <fstream>
#include <stdlib.h>

using namespace std;

int main(int argc,char *argv[]){
    int i=1;
    bool nflag=false;
    bool lflag=false;
    bool dflag=false;
    char* inputf;
    int customers;
    double dtime;
    //get input from arguments
    if (argc!=7){
        cerr<<"Wrong arguments"<<endl;
        return -1;
    }
    while (i<argc){
        if(strcmp(argv[i],"-l")==0){
            if(lflag==true){
                cerr<<"Wrong arguments"<<endl;
                return -1;
            }
            lflag=true;
            inputf=new char[strlen(argv[i+1])];
            strncpy(inputf,argv[i+1],strlen(argv[i+1]));
        }
        else if(strcmp(argv[i],"-n")==0){
            if(nflag==true){
                cerr<<"Wrong arguments"<<endl;
                return -1;
            }
            nflag=true;
            customers=atoi(argv[i+1]);
        }
        else if(strcmp(argv[i],"-d")==0){
            if(dflag==true){
                cerr<<"Wrong arguments"<<endl;
                return -1;
            }
            dflag=true;
            dtime=atof(argv[i+1]);
        }
        else{
            cerr<<"Wrong arguments"<<endl;
            return -1;
        }
        i+=2;
    }
    //open the config file and take the parameters in consideration
    string buffer;
    ifstream infile(inputf);
    int two,four,six,eight,bar,waiters;
    while(infile>>buffer){
        if(buffer=="2theseo"){
            infile>>two;
            cout<<"two "<<two<<endl;
        }
        else if(buffer=="4theseo"){
            infile>>four;
            cout<<"four "<<four<<endl;
        }
        else if(buffer=="6theseo"){
            infile>>six;
            cout<<"six "<<six<<endl;
        }
        else if(buffer=="8theseo"){
            infile>>eight;
            cout<<"eight "<<eight<<endl;
        }
        else if(buffer=="bar"){
            infile>>bar;
            cout<<"bar "<<bar<<endl;
        }
        else if(buffer=="waiters"){
            infile>>waiters;
            cout<<"waiters "<<waiters<<endl;
        }
    }
    int id=0, err=0, retval;
	/* Make shared memroy segment */
	id = shmget(IPC_PRIVATE,sizeof(Shared_Data)+((two+four+six+eight)*sizeof(Table))+((bar)*sizeof(node)),0666);
	if (id == -1)
		perror ("Creation");
	else
		printf("Restaurant Allocated. %d\n",(int)id);
	/* Attach the segment */
	Shared_Data *mem;
	mem = (Shared_Data *) shmat(id, (void*)0, 0);
	if ((Shared_Data*) mem == (void*)-1)
		perror("Attachment.");
	else
		printf("Restaurant Attached. \n");
    mem->four=four;
    mem->eight=eight;
    mem->six=six;
    mem->two=two;
    mem->bar.array=(node*)((void*)mem+sizeof(Shared_Data)+((two+four+six+eight)*sizeof(Table)));
    mem->hall.tarray=(Table*)((void*)mem+sizeof(Shared_Data));
    mem->hall.farray=(Table*)((void*)mem+sizeof(Shared_Data)+(two*sizeof(Table)));
    mem->hall.sarray=(Table*)((void*)mem+sizeof(Shared_Data)+((two+four)*sizeof(Table)));
    mem->hall.earray=(Table*)((void*)mem+sizeof(Shared_Data)+((two+four+six)*sizeof(Table)));
    mem->bar.initQueue(bar);
    cout<<"here ok"<<endl;
    mem->hall.initHall(two,four,six,eight);
    retval = sem_init ( &(mem->doorinfo.doorq) ,1 ,1);
    if(retval==-1)
        perror("Init. ");
    retval = sem_init ( &(mem->doorinfo.door) ,1 ,1);
    if(retval==-1)
        perror("Init. ");
    retval = sem_init ( &(mem->doorinfo.wakeup) ,1 ,0);
    if(retval==-1)
        perror("Init. ");
    /*STARTforking*/
    cout<<" test "<<mem->hall.request(8)->cap()<<endl;
    pid_t child=fork();
    if(child==0){
        char* fargs[6];
        for(int i=0;i<5;i++){
            fargs[i]=new char[100];
        }
        fargs[5]=NULL;
        snprintf(fargs[0], 100, "%s", "doorman");
        snprintf(fargs[1], 100, "%s", "-d");
        snprintf(fargs[2], 100, "%f", dtime);
        snprintf(fargs[3], 100, "%s", "-s");
        snprintf(fargs[4], 100, "%d", id);
        if(execv(fargs[0],fargs)==-1){
            cerr<<"execv doorman: "<<strerror(errno)<<endl;
            exit(-1);
        }
    }
    else{
        cout<<"Doorman created "<<child<<endl;
    }
    for(int j=0;j<customers;j++){
        child=fork();
        if(child==0){
            srand(getpid());
            char* fargs[8];
            for(int i=0;i<7;i++){
                fargs[i]=new char[100];
            }
            fargs[7]=NULL;
            snprintf(fargs[0], 100, "%s", "customer");
            snprintf(fargs[1], 100, "%s", "-d");
            snprintf(fargs[2], 100, "%f", dtime);
            snprintf(fargs[3], 100, "%s", "-s");
            snprintf(fargs[4], 100, "%d", id);
            snprintf(fargs[5], 100, "%s", "-n");
            snprintf(fargs[6], 100, "%d", rand()%8);
            if(execv(fargs[0],fargs)==-1){
                cerr<<"execv customer "<<j<<": "<<strerror(errno)<<endl;
                exit(-1);
            }
        }
        else{
            cout<<"Customer created "<<child<<endl;
        }
    }
    int status;
    for(int i=0;i<customers+1;i++)
        pid_t cpid = wait(&status);
    //detach and destroy
    err = shmdt (( void *) mem ) ;
    err = shmctl(id, IPC_RMID, 0);
	if (err == -1)
		perror ("Removal.");
	else
		printf("Removed. %d\n", (int)(err));
	return 0;
}
