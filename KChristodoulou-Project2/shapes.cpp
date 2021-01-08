#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <poll.h>
#include <unistd.h>
#include "myRecordDef.h"

using namespace std;

int main(int argc,char* argv[]){
    int i=1;
    bool iflag=false;
    bool wflag=false;
    bool dflag=false;
    long lSize;
    FILE* input;
    int workers,numofrecords;
    DIR *directory;
    char* dir;
    char* inputf;
    if (argc!=7){
        cerr<<"Wrong arguments"<<endl;
        return -1;
    }
    while (i<argc){
        if(strcmp(argv[i],"-i")==0){
            if(iflag==true){
                cerr<<"Wrong arguments"<<endl;
                return -1;
            }
            iflag=true;
            input=fopen(argv[i+1],"rb");
            inputf=new char[strlen(argv[i+1])];
            strcpy(inputf,argv[i+1]);
        }
        else if(strcmp(argv[i],"-w")==0){
            if(wflag==true){
                cerr<<"Wrong arguments"<<endl;
                return -1;
            }
            wflag=true;
            workers=atoi(argv[i+1]);
        }
        else if(strcmp(argv[i],"-d")==0){
            if(dflag==true){
                cerr<<"Wrong arguments"<<endl;
                return -1;
            }
            dflag=true;
            directory=opendir(argv[i+1]);
            dir=new char[strlen(argv[i+1])];
            strcpy(dir,argv[i+1]);
            if(directory==NULL)
                if (mkdir(argv[i+1],0777)==-1){
                    cerr<<"Directory creation: "<<strerror(errno)<<endl;
                    return -1;
                }
        }
        else{
            cerr<<"Wrong arguments"<<endl;
            return -1;
        }
        i+=2;
    }
    fseek (input, 0 , SEEK_END);
    lSize = ftell (input);
    rewind (input);
    numofrecords = (int) lSize/(sizeof(MyRecord))+1;
    int commands=0;
    bool first;
    while(1){
        int chandlers=0;
        char buffer[256];
        cout<<"input: ";
        char* end_str;
        scanf ("%[^\n]%*c", buffer);
        if(strcmp(buffer,"exit")==0)
            break;
        if(buffer[strlen(buffer)-1]!=';'){
            cerr<<"Wrong input"<<endl;
            continue;
        }
        first=true;
        buffer[strlen(buffer)-1]='\0';
        char* token = strtok_r(buffer, ",",&end_str);
        char gnuplotscript[40];
        sprintf(gnuplotscript,"%d_script.gnuplot",commands);
        FILE* gpfd=fopen(gnuplotscript,"a");
        fprintf(gpfd,"set terminal png\nset output \"./%d_image.png\"\nplot ",commands);
        fclose(gpfd);
        while( token != NULL ){
            char args[7][15];
            char* end_token;
            char buffer2[256];
            strcpy(buffer2,token);
            buffer2[strlen(token)]='\0';
            int argsc=0;
            char* token2=strtok_r(buffer2," ",&end_token);
            int i=0;
            while(token2!=NULL){
                argsc++;
                strcpy(args[i],token2);
                token2=strtok_r(NULL," ",&end_token);
                i++;
            }
            if((strcmp(args[0],"circle")==0)||(strcmp(args[0],"square")==0)){
                if(argsc!=5){
                    cerr<<"Wrong amount of arguments circle/square"<<endl;
                    token = strtok_r(NULL, ";",&end_str);
                    remove(gnuplotscript);
                    continue;
                }
            }
            else if((strcmp(args[0],"ellipse")==0)||(strcmp(args[0],"ring")==0)||(strcmp(args[0],"semicircle")==0)){
                if(argsc!=6){
                    cerr<<"Wrong amount of arguments ellipse/ring/semicircle"<<endl;
                    token = strtok_r(NULL, ";",&end_str);
                    remove(gnuplotscript);
                    continue;
                }
            }
            else{
                cerr<<"Wrong first argument"<<endl;
                token = strtok_r(NULL, ";",&end_str);
                remove(gnuplotscript);
                continue;
            }
            char colour[strlen(args[argsc-1])];
            strcpy(colour,args[argsc-1]);
            if(fork()==0){                                  //handler part
                FILE* gpfd2=fopen(gnuplotscript,"a");
                if(!first)
                    fprintf(gpfd2,",\\\n\"./%s/%d.txt\" notitle with points pointsize 0.5 linecolor rgb \"%s\"",dir,getpid(),colour);
                else
                    fprintf(gpfd2,"\\\n\"./%s/%d.txt\" notitle with points pointsize 0.5 linecolor rgb \"%s\"",dir,getpid(),colour);
                fclose(gpfd2);
                struct pollfd polltbl[workers];
                char* fargs[15];
                for(int i=0;i<14;i++){
                    fargs[i]=new char[100];
                }
                strcpy(fargs[0],args[0]);
                strcpy(fargs[1],"-i");
                strcpy(fargs[2],inputf);
                strcpy(fargs[3],"-o");
                strcpy(fargs[5],"-f");
                strcpy(fargs[7],"-n");
                strcpy(fargs[9],"-a");
                int last=10;
                for(int i=1;i<argsc-1;i++){
                    strcpy(fargs[i+9],args[i]);
                    last++;
                }
                fargs[last]=NULL;
                int points[workers];
                for(int i=0;i<workers;i++){
                    points[i]=numofrecords/workers;
                }
                for(int i=0;i<numofrecords%workers;i++){
                    points[i]=points[i]+1;
                }
                for(int i=0;i<workers;i++){
                    char fifo[40];
                    sprintf(fifo,"%s/%d_w%d.fifo",dir,getpid(),i);
                    strcpy(fargs[4],fifo);
                    if(mkfifo(fifo,0666)<0){
                        cerr<<"Make fifo: "<<strerror(errno)<<endl;
                        remove(gnuplotscript);
                        exit(-1);
                    }
                    polltbl[i].fd=open(fifo,O_RDONLY|O_NONBLOCK,0666);
                    polltbl[i].events=POLLIN;
                    polltbl[i].revents=0;
                    if(polltbl[i].fd==-1){
                        cerr<<"Open fifo: "<<strerror(errno)<<endl;
                        exit(-1);
                    }
                    char offs[8];
                    int offsd=0;
                    for(int j=0;j<i;j++)
                        offsd+=points[j];
                    sprintf(offs,"%ld",offsd*sizeof(MyRecord));
                    strcpy(fargs[6],offs);
                    char ptread[8];
                    sprintf(ptread,"%d",points[i]);
                    strcpy(fargs[8],ptread);
                    if(fork()==0){                  //call the utility
                        if(execv(fargs[0],fargs)==-1){
                            cerr<<"execv: "<<strerror(errno)<<endl;
                            remove(gnuplotscript);
                            commands--;
                            exit(-1);
                        }
                    }
                }
                char tomaster[40];
                sprintf(tomaster,"%s/%d.txt",dir,getpid());
                FILE* fd=fopen(tomaster,"w");
                if(fd==NULL){
                    cerr<<strerror(errno)<<endl;
                    exit(-1);
                }
                int tempworkers=workers;
                while(poll(polltbl,workers,-1)>0){
                    for(int i=0;i<workers;i++){
                        if(polltbl[i].fd==-1)
                            continue;
                        if((polltbl[i].revents&POLLIN )==POLLIN){
                            MyRecord temp;
                            while(read(polltbl[i].fd,&temp,sizeof(MyRecord))>0)
                                fprintf(fd,"%.2f %.2f\n",temp.x,temp.y);
                            polltbl[i].events=POLLIN;
                            polltbl[i].revents=0;
                        }
                        else if((polltbl[i].revents&POLLHUP )==POLLHUP){
                            polltbl[i].fd=-1;
                            tempworkers--;
                            if(tempworkers==0)
                                break;
                        }
                    }
                    if(tempworkers==0)
                        break;
                }
                int hstatus;
                pid_t wpid;
                int tworkers=workers;
                while(tworkers>0){
                    wpid = wait(&hstatus);
                    --tworkers;
                }
                for(int i=0;i<workers;i++){
                    char fifo[40];
                    sprintf(fifo,"%s/%d_w%d.fifo",dir,getpid(),i);
                    close(polltbl[i].fd);
                    if(remove(fifo)!=0)
                        cerr<<"Deleting file: "<<strerror(errno)<<endl;
                }
                fclose(fd);
                int tempid=getpid();
                for(int i=0;i<15;i++)
                    delete[] fargs[i];
                delete[] fargs;
                exit(1);
            }
            else{ //master
                chandlers++;
                if (first)
                    first=false;
                token = strtok_r(NULL, ",",&end_str);
            }
        }
        int status;
        pid_t hpid;
        int counter=chandlers;
        for(int i=0;i<counter;i++)
            hpid = wait(&status);
        if(fork()==0){
            if(execl("/usr/bin/gnuplot","/usr/bin/gnuplot", gnuplotscript, (char*) NULL)==-1){
                cerr<<"execl: "<<strerror(errno)<<endl;
                exit(-1);
            }
        }
        else{
            int status;
            pid_t gppid;
            gppid = wait(&status);
            remove(gnuplotscript);
        }
        commands++;
    }
    fclose(input);
    closedir(directory);
    delete[] dir;
    delete[] inputf;
}
