#include  <sys/types.h>
#include  <sys/stat.h>
#include  <unistd.h>
#include  <dirent.h>
#include  <stdio.h>
#include  <cstdlib>
#include  <cstring>
#include  <errno.h>
#include <sys/time.h>
#include <time.h>
#include <utime.h>

#include <cstdlib>
#include <cstring>
#include <iostream>

#include "block.h"
#include "BlockHandler.h"

using namespace std;

int globalCounter = 2;
int size=1024;
char* memory=new char[BLOCK_SIZE*size];

void doubleME(){  //double size of memory
    if(globalCounter==size){
        char* mem=new char[BLOCK_SIZE*size*2];
        memset(mem,0,size*BLOCK_SIZE*2);
        memcpy(mem,memory,size*BLOCK_SIZE);
        delete[] memory;
        memory=mem;
        size*=2;
    }
}

char* token(char* fullName){ //return the last part of a filepath
    char* pch;
    char* ret;
    char temp[128];
    strcpy(temp,fullName);
    pch=strtok(temp,"/");
    ret=temp;
    while(pch!=NULL){
        ret=pch;
        pch=strtok(NULL,"/");
    }
    char* retval=new char[128];
    strcpy(retval,ret);
    return retval;
}

void listRecursive(char *dirname, int parent) { //create memory
    DIR *dp;
    struct dirent *entry;
    MetadataBlock* myMDB =new MetadataBlock;
    memset(myMDB, 0, sizeof(MetadataBlock));
    char *newname;
    MetadataBlock* myparent=new MetadataBlock;
    memset(myparent, 0, sizeof(MetadataBlock));
    struct stat mybuf;
    int files = 0;
    if ((dp = opendir(dirname)) == NULL) {
        if(errno!=ENOTDIR){
            cout<<dirname<<endl;
            perror("opendir:");
            delete myparent;
            delete myMDB;
            return;
        }
        else { //in case of a file is given as an argument
            memcpy(myparent,(char*)(memory+parent*BLOCK_SIZE),BLOCK_SIZE);
            myparent->childrenblocknum[myparent->children]=globalCounter;
            myparent->children++;
            int tempparent=parent;
            memcpy((char*)(memory+tempparent*BLOCK_SIZE),myparent,BLOCK_SIZE);
            FILE* fp;
            myMDB->parentblocknum = parent;
            myMDB->isdirectory = 0;
            fp = fopen(dirname, "r");
            stat(dirname, &mybuf);
            memcpy(&myMDB->mybuf,&mybuf,sizeof(struct stat));
            myMDB->bytesOfLast=mybuf.st_size%BLOCK_SIZE;
            int blocksNUM=mybuf.st_size/BLOCK_SIZE;
            strcpy(myMDB->d_name,dirname);
            myMDB->datablocknum=globalCounter+1;
            memcpy((MetadataBlock*)(memory+globalCounter*BLOCK_SIZE),myMDB,BLOCK_SIZE);
            globalCounter++;
            doubleME();

            for(int i=0;i<blocksNUM;i++){
                DataBlock* tempData=new DataBlock;
                char buffer[BLOCK_SIZE];
                fread(buffer,BLOCK_SIZE,1,fp);
                memcpy(tempData->bytes,buffer,BLOCK_SIZE);
                memcpy((DataBlock *) (memory+globalCounter*BLOCK_SIZE),  tempData ,BLOCK_SIZE);
                globalCounter++;
                doubleME();
                delete tempData;
            }
            fclose(fp);
            delete myparent;
            delete myMDB;
            return;
        }
    }
    else if(parent == 1){ //create child-dir of root
        memcpy(myparent,(char*)(memory+parent*BLOCK_SIZE),BLOCK_SIZE);
        myparent->childrenblocknum[myparent->children]=globalCounter;
        myparent->children++;
        int tempparent=parent;
        parent = globalCounter;
        memcpy((char*)(memory+tempparent*BLOCK_SIZE),myparent,BLOCK_SIZE);

        strcpy(myMDB->d_name,dirname);
        myMDB->parentblocknum = tempparent;
        myMDB->isdirectory = 1;
        stat(dirname, &mybuf);
        memcpy(&myMDB->mybuf,&mybuf,sizeof(struct stat));
        memcpy((char*)(memory+globalCounter*BLOCK_SIZE),myMDB,BLOCK_SIZE);
        globalCounter++;
        doubleME();
    }
    while ((entry = readdir(dp)) != NULL) {
        if (entry->d_ino == 0) {
            continue;
        }
        if (strcmp(entry->d_name, "..") == 0 || strcmp(entry->d_name, ".") == 0) {
            continue;
        }
        if (entry->d_type == DT_BLK) {
            continue;
        }
        if (entry->d_type == DT_CHR) {
            continue;
        }
        if (entry->d_type == DT_FIFO) {
            continue;
        }
        if (entry->d_type == DT_SOCK) {
            continue;
        }
        newname = (char *) malloc(strlen(dirname) + strlen(entry ->d_name) + 2);
        strcpy(newname, dirname);
        if(newname[strlen(dirname)-1]!='/')
            strcat(newname, "/");
        strcat(newname, entry->d_name);
        stat(newname, &mybuf);
        strcpy(myMDB->d_name,newname);
        memcpy(&myMDB->mybuf,&mybuf,sizeof(struct stat));
        myMDB->parentblocknum=parent;
        memcpy(myparent,(char*)(memory+parent*BLOCK_SIZE),BLOCK_SIZE);
        myparent->childrenblocknum[myparent->children]=globalCounter;
        myparent->children++;
        memcpy((char*)(memory+parent*BLOCK_SIZE),myparent,BLOCK_SIZE); //Update parent block in memory
        if (entry->d_type == DT_DIR) { //Directory
            myMDB->isdirectory=1;
            memcpy((MetadataBlock*)(memory+globalCounter*BLOCK_SIZE),myMDB,BLOCK_SIZE); //Save MetadataBlock in memory
            globalCounter++;
            doubleME();
            listRecursive(newname,globalCounter-1);
        }
        if (entry->d_type == DT_REG) { //File
            int blocksNUM=0;
            if(mybuf.st_size%BLOCK_SIZE==0){
                blocksNUM=mybuf.st_size/BLOCK_SIZE;
                myMDB->bytesOfLast=0;
            }
            else{
                blocksNUM=mybuf.st_size/BLOCK_SIZE+1;
                myMDB->bytesOfLast=mybuf.st_size%BLOCK_SIZE;
            }
            myMDB->isdirectory=0;
            myMDB->datablocknum=globalCounter+1;
            memcpy((MetadataBlock*)(memory+globalCounter*BLOCK_SIZE),myMDB,BLOCK_SIZE); //Save MetadataBlock in memory
            globalCounter++;
            doubleME();
            FILE* fp=fopen(newname,"r");
            for(int i=0;i<blocksNUM;i++){
                DataBlock* tempData=new DataBlock;
                char buffer[BLOCK_SIZE];
                fread(buffer,BLOCK_SIZE,1,fp);
                memcpy(tempData->bytes,buffer,BLOCK_SIZE);
                memcpy((DataBlock *) (memory+globalCounter*BLOCK_SIZE),  tempData ,BLOCK_SIZE); // Save DataBlock in memory
                globalCounter++;
                doubleME();
                delete tempData;
            }
            fclose(fp);
        }
        free(newname);
        newname = NULL;
    }
    closedir(dp);
    delete myparent;
    delete myMDB;
}

int recWriteCounter = 0;

void recWrite(MetadataBlock* MDB,FILE* fd){ // flag: x without search
    cout << "recWrite: " << recWriteCounter++ << endl;
    struct utimbuf times;
    times.actime=MDB->mybuf.st_atim.tv_sec;
    times.modtime=MDB->mybuf.st_mtim.tv_sec;
    if(MDB->isdirectory==1){
        if(mkdir(MDB->d_name,MDB->mybuf.st_mode)!=0){
            perror("directory");
        }
        for(int i=0;i<MDB->children;i++){
            fseek(fd,MDB->childrenblocknum[i]*BLOCK_SIZE,SEEK_SET);
            char buffer[BLOCK_SIZE];
            fread(buffer,BLOCK_SIZE,1,fd);
            MetadataBlock* temp=new MetadataBlock;
            memcpy(temp,buffer,BLOCK_SIZE);
            recWrite(temp,fd);
            utime(MDB->d_name,&times);
            chown(MDB->d_name,MDB->mybuf.st_uid,MDB->mybuf.st_gid);
            delete temp;
        }
    }else{ // file
        FILE* newfd=fopen(MDB->d_name,"w");
        if(newfd==NULL){
            perror("creating");
            exit(1);
        }
        chmod(MDB->d_name,MDB->mybuf.st_mode);
        int i;
        for(i=0;i<MDB->mybuf.st_size/BLOCK_SIZE;i++){
            fseek(fd,(MDB->datablocknum+i)*BLOCK_SIZE,SEEK_SET);
            char buffer[BLOCK_SIZE];
            fread(buffer,BLOCK_SIZE,1,fd);
            fseek(newfd,i*BLOCK_SIZE,SEEK_SET);
            fwrite(buffer,BLOCK_SIZE,1,newfd);
        }
        if(MDB->mybuf.st_size%BLOCK_SIZE!=0 && MDB->mybuf.st_size > BLOCK_SIZE){
            fseek(fd,(MDB->datablocknum+i)*BLOCK_SIZE,SEEK_SET);
            char buffer[MDB->bytesOfLast];
            fread(buffer,MDB->bytesOfLast,1,fd);
            fseek(newfd,i*BLOCK_SIZE,SEEK_SET);
            fwrite(buffer,MDB->bytesOfLast,1,newfd);
        }
        else if (MDB->mybuf.st_size < BLOCK_SIZE){
          // only one block has to be allocated that has less size than Block_Size
          fseek(fd,(MDB->datablocknum+i)*BLOCK_SIZE,SEEK_SET);
          char buffer[BLOCK_SIZE];
          fread(buffer,MDB->bytesOfLast,1,fd);
          fseek(newfd,i*BLOCK_SIZE,SEEK_SET);
          fwrite(buffer,MDB->bytesOfLast,1,newfd);
        }
        fclose(newfd);
    }
    utime(MDB->d_name,&times);
    chown(MDB->d_name,MDB->mybuf.st_uid,MDB->mybuf.st_gid);
}

void recWriteX(MetadataBlock* MDB,FILE* fd){ // flag: x with specific search
    char bufferS[1024];
    getcwd(bufferS,1024);

    char* tempS;
    struct utimbuf times;
    times.actime=MDB->mybuf.st_atim.tv_sec;
    times.modtime=MDB->mybuf.st_mtim.tv_sec;
    if(MDB->isdirectory==1){
        tempS=token(MDB->d_name);
        if(mkdir(tempS,MDB->mybuf.st_mode)!=0){
            perror("directory");
        }
        for(int i=0;i<MDB->children;i++){
            fseek(fd,MDB->childrenblocknum[i]*BLOCK_SIZE,SEEK_SET);
            char buffer[BLOCK_SIZE];
            fread(buffer,BLOCK_SIZE,1,fd);
            MetadataBlock* temp=new MetadataBlock;
            memcpy(temp,buffer,BLOCK_SIZE);
            char WDB[1024];
            getcwd(WDB,1024);
            chdir(tempS);
            utime(tempS,&times);
            chown(tempS,MDB->mybuf.st_uid,MDB->mybuf.st_gid);
            recWriteX(temp,fd);
            chdir(WDB);
            delete temp;
        }
    }else{ // file
        tempS=token(MDB->d_name);
        FILE* newfd=fopen(tempS,"w");
        if(newfd==NULL){
            perror("creating");
            exit(1);
        }
        chmod(tempS,MDB->mybuf.st_mode);
        int i;
        for(i=0;i<MDB->mybuf.st_size/BLOCK_SIZE;i++){
            fseek(fd,(MDB->datablocknum+i)*BLOCK_SIZE,SEEK_SET);
            char buffer[BLOCK_SIZE];
            fread(buffer,BLOCK_SIZE,1,fd);
            fseek(newfd,i*BLOCK_SIZE,SEEK_SET);
            fwrite(buffer,BLOCK_SIZE,1,newfd);
        }
        if(MDB->mybuf.st_size%BLOCK_SIZE!=0 && MDB->mybuf.st_size > BLOCK_SIZE){
            fseek(fd,(MDB->datablocknum+i)*BLOCK_SIZE,SEEK_SET);
            char buffer[MDB->bytesOfLast];
            fread(buffer,MDB->bytesOfLast,1,fd);
            fseek(newfd,i*BLOCK_SIZE,SEEK_SET);
            fwrite(buffer,MDB->bytesOfLast,1,newfd);
        }
        else if (MDB->mybuf.st_size < BLOCK_SIZE){
          // only one block has to be allocated that has less size than Block_Size
          fseek(fd,(MDB->datablocknum+i)*BLOCK_SIZE,SEEK_SET);
          char buffer[BLOCK_SIZE];
          fread(buffer,MDB->bytesOfLast,1,fd);
          fseek(newfd,i*BLOCK_SIZE,SEEK_SET);
          fwrite(buffer,MDB->bytesOfLast,1,newfd);
        }
        fclose(newfd);
    }
    utime(tempS,&times);
    chown(tempS,MDB->mybuf.st_uid,MDB->mybuf.st_gid);
    delete[] tempS;
}

int spaceCounter = 0;

void recPrint(MetadataBlock* MDB,FILE* fd){
  for (int i=0; i<spaceCounter; i++){
    cout << "  ";
  }
  char tempS[128];
  char* tempSS= token(MDB->d_name);
  strcpy (tempS,tempSS);
  cout << tempS << endl;
  delete[] tempSS;
  if (MDB->children > 0){
    spaceCounter++;
    for (int i = 0; i<MDB->children; i++){
      for (int j=0; j<spaceCounter; j++){
        cout << "  ";
      }
      fseek(fd,MDB->childrenblocknum[i]*BLOCK_SIZE,SEEK_SET);
      char buffer[BLOCK_SIZE];
      fread(buffer,BLOCK_SIZE,1,fd);
      MetadataBlock* temp=new MetadataBlock;
      memcpy(temp,buffer,BLOCK_SIZE);
      recPrint(temp,fd);
      delete temp;
    }
    spaceCounter--;
  }
}

void printPerms(struct stat fileStat){
    if (S_ISDIR(fileStat.st_mode)) cout << "d"; else cout << "-";
    if (fileStat.st_mode & S_IRUSR) cout << "r"; else cout << "-";
    if (fileStat.st_mode & S_IWUSR) cout << "w"; else cout << "-";
    if (fileStat.st_mode & S_IXUSR) cout << "x"; else cout << "-";
    if (fileStat.st_mode & S_IRGRP) cout << "r"; else cout << "-";
    if (fileStat.st_mode & S_IWGRP) cout << "w"; else cout << "-";
    if (fileStat.st_mode & S_IXGRP) cout << "x"; else cout << "-";
    if (fileStat.st_mode & S_IROTH) cout << "r"; else cout << "-";
    if (fileStat.st_mode & S_IWOTH) cout << "w"; else cout << "-";
    if (fileStat.st_mode & S_IXOTH) cout << "x"; else cout << "-";
}

int recMetaprintCounter = 0;

void recMetaprint(MetadataBlock* MDB, FILE* fd){
    printPerms(MDB->mybuf);
    char buff[20];
    struct tm * timeinfo;
    timeinfo = localtime (&(MDB->mybuf.st_mtime));
    strftime(buff, 20, "%b %d %H:%M", timeinfo);
    cout << "   u: " << MDB->mybuf.st_uid << "   g: " << MDB->mybuf.st_gid << "   " << MDB->mybuf.st_size << " bytes   " << buff << "    " << MDB->d_name << endl;
    if (MDB->children > 0){
        for (int i = 0; i<MDB->children; i++){
            fseek(fd,MDB->childrenblocknum[i]*BLOCK_SIZE,SEEK_SET);
            char buffer[BLOCK_SIZE];
            fread(buffer,BLOCK_SIZE,1,fd);
            MetadataBlock* temp=new MetadataBlock;
            memcpy(temp,buffer,BLOCK_SIZE);
            recMetaprint(temp,fd);
            delete temp;
        }
    }
}

void recQuery(MetadataBlock* MDB,FILE* fd,int counter,char** FilesList,MetadataBlock** ReturnValues){
    for(int i=0;i<counter;i++){
        if(ReturnValues[i]!=NULL)
            continue;
        char* tempS=token(MDB->d_name);
        if(strcmp(tempS,FilesList[i])==0){
            MetadataBlock* finalMDB=new MetadataBlock;
            memcpy(finalMDB,MDB,BLOCK_SIZE);
            ReturnValues[i]=finalMDB;
        }
        delete[] tempS;
    }
    for(int i=0;i<MDB->children;i++){
        fseek(fd,MDB->childrenblocknum[i]*BLOCK_SIZE,SEEK_SET);
        char buffer[BLOCK_SIZE];
        fread(buffer,BLOCK_SIZE,1,fd);
        MetadataBlock* temp=new MetadataBlock;
        memcpy(temp,buffer,BLOCK_SIZE);
        recQuery(temp,fd,counter,FilesList,ReturnValues);
        delete temp;
    }
}

int main(int argc, char** argv) {
    int i=1;
    int numoffiles=0;
    char** listoffiles=NULL;
    bool cflag=false;
    bool aflag=false;
    bool xflag=false;
    bool mflag=false;
    bool dflag=false;
    bool pflag=false;
    bool jflag=false;
    bool qflag=false;
    bool endofflags=false;
    struct dirent *entry;
    char *newname;
    struct stat mybuf;
    char* outputf=NULL;
    while (i<argc){
        if(strcmp(argv[i],"-c")==0){
            if(cflag==true){
                cerr<<"Wrong arguments"<<endl;
                return -1;
            }
            cflag=true;
        }
        else if(strcmp(argv[i],"-a")==0){
            if(aflag==true){
                cerr<<"Wrong arguments"<<endl;
                return -1;
            }
            aflag=true;
        }
        else if(strcmp(argv[i],"-x")==0){
            if(xflag==true){
                cerr<<"Wrong arguments"<<endl;
                return -1;
            }
            xflag=true;
        }
        else if(strcmp(argv[i],"-m")==0){
            if(mflag==true){
                cerr<<"Wrong arguments"<<endl;
                return -1;
            }
            mflag=true;
        }
        else if(strcmp(argv[i],"-d")==0){
            if(dflag==true){
                cerr<<"Wrong arguments"<<endl;
                return -1;
            }
            dflag=true;
        }
        else if(strcmp(argv[i],"-p")==0){
            if(pflag==true){
                cerr<<"Wrong arguments"<<endl;
                return -1;
            }
            pflag=true;
        }
        else if(strcmp(argv[i],"-q")==0){
            if(qflag==true){
                cerr<<"Wrong arguments"<<endl;
                return -1;
            }
            qflag=true;
        }
        else if(strcmp(argv[i],"-j")==0){
            if(jflag==true){
                cerr<<"Wrong arguments"<<endl;
                return -1;
            }
            jflag=true;
        }
        else{
            if(endofflags==false){
                outputf=new char[strlen(argv[i])+1];
                strcpy(outputf,argv[i]);
                cout << "Output file: " << outputf << " -- argv[" << i << "]" << endl;
                endofflags=true;
            }
            else if(argv[i][0]=='-'){
                cerr<<"Wrong arguments"<<endl;
                return -1;
            }
            else{
                listoffiles=new char*[argc-i];
                numoffiles=argc-i;
                for(int j=i;j<argc;j++){
                    listoffiles[j-i]=new char[strlen(argv[j])+1];
                    strcpy(listoffiles[j-i],argv[j]);
                    listoffiles[j-i][strlen(argv[j])]='\0';
                }
                break;
            }
        }
        i++;
    }
    if(jflag){
        if(!aflag&&!cflag){
            cerr<<"Wrong arguments"<<endl;
            for(int i=0;i<numoffiles;i++){
                delete[] listoffiles[i];
                listoffiles[i]=NULL;
            }
            delete[] listoffiles;
            delete[] outputf;
            return -1;
        }
    } else if((outputf==NULL)&&(endofflags==false)){
        cerr<<"Wrong arguments"<<endl;
        for(int i=0;i<numoffiles;i++){
            delete[] listoffiles[i];
            listoffiles[i]=NULL;
        }
        delete[] listoffiles;
        return -1;
    }
    int counterForFiles[numoffiles];
    cout << "Input files: " << numoffiles<< endl;
    for(int i=0;i<numoffiles;i++){
        counterForFiles[i] = 0;
        if (listoffiles[i][0] == '.' && listoffiles[i][1] == '/') {
            listoffiles[i]++;
            listoffiles[i]++;
            counterForFiles[i]+=2;
        }
        else if (listoffiles[i][0] == '/') {
            listoffiles[i]++;
            counterForFiles[i]++;
        }
        cout<<"File "<<i<<": "<<listoffiles[i]<<endl;
    }
    if(cflag){
        BlockHandler * handler = new BlockHandler(outputf, false);
        HeaderBlock* headerBlock=new HeaderBlock;
        MetadataBlock* root=new MetadataBlock;
        memset(headerBlock, 0, BLOCK_SIZE);
        memset(root, 0, BLOCK_SIZE);
        headerBlock->firstblocknum = 1;
        headerBlock->gzip_enabled = jflag;
        headerBlock->blocksCntr = 0;
        root->parentblocknum = 0;
        root->isdirectory = 1;
        root->nextblocknum = 0;
        root->children = 0 ;
        root->datablocknum = 0;
        strcpy(root->d_name, "archiveroot");
        memcpy((MetadataBlock*)(memory),headerBlock,BLOCK_SIZE);
        memcpy((MetadataBlock*)(memory+BLOCK_SIZE),root,BLOCK_SIZE);
        for(int i=0;i<numoffiles;i++){
            listRecursive(listoffiles[i],1);
        }
        headerBlock->blocksCntr=globalCounter;
        memcpy((MetadataBlock*)(memory),headerBlock,BLOCK_SIZE);
        MetadataBlock * tempParent = new MetadataBlock;
        memcpy (tempParent, (char*) (memory + BLOCK_SIZE), BLOCK_SIZE);
        fseek(handler->retFD(),0,SEEK_SET);
        for(int i=0;i<globalCounter;i++){
            fwrite(memory+(i*BLOCK_SIZE),BLOCK_SIZE,1,handler->retFD());
        }
        delete handler;
        delete headerBlock;
        delete root;
        delete tempParent;
        cout << "Create finished." << endl;
    }
    else if(aflag){
        FILE* fd=fopen(outputf,"r+");
        if(fd==NULL){
            perror("append:");
            for(int i=0;i<numoffiles;i++){
                for(int j=0;j<counterForFiles[i];j++){
                    listoffiles[i]--;
                }
                delete[] listoffiles[i];
                listoffiles[i]=NULL;
            }
            delete[] listoffiles;
            exit(1);
        }
        HeaderBlock* tempHeader = new HeaderBlock;
        MetadataBlock* root=new MetadataBlock;
        char buffer[BLOCK_SIZE];
        fread(buffer,BLOCK_SIZE,1,fd);
        memcpy(tempHeader,buffer,BLOCK_SIZE);
        globalCounter=tempHeader->blocksCntr;
        int oldCntr=tempHeader->blocksCntr;
        size=tempHeader->blocksCntr;
        delete[] memory;
        memory=new char[BLOCK_SIZE*size*2];
        size+=size;
        fread(buffer,BLOCK_SIZE,1,fd);
        memcpy(root,buffer,BLOCK_SIZE);
        memcpy((MetadataBlock*)(memory+BLOCK_SIZE),root,BLOCK_SIZE);
        for(int i=root->children;i<numoffiles+root->children;i++){
            listRecursive(listoffiles[i-root->children],1);
        }
        MetadataBlock * tempParent = new MetadataBlock;
        memcpy (tempParent, (char*) (memory + BLOCK_SIZE), BLOCK_SIZE);

        tempHeader->blocksCntr=globalCounter;
        fseek(fd,0,SEEK_SET);
        fwrite(tempHeader,BLOCK_SIZE,1,fd);
        fwrite(tempParent,BLOCK_SIZE,1,fd);
        fseek(fd,oldCntr*BLOCK_SIZE,SEEK_SET);
        for(int i=oldCntr;i<globalCounter;i++){
            fwrite(memory+(i*BLOCK_SIZE),BLOCK_SIZE,1,fd);
        }
        delete root;
        delete tempHeader;
        delete tempParent;
        fclose(fd);
        cout << "Append finished" << endl;
    }
    else if(xflag){
        FILE* fd = fopen(outputf,"r");
        if (fd == NULL) {
            perror("Opening");
            exit(1);
        }
        MetadataBlock* root = new MetadataBlock;
        MetadataBlock* temp = new MetadataBlock;
        char buffer[BLOCK_SIZE];
        fseek(fd,BLOCK_SIZE,SEEK_SET);
        fread(buffer,BLOCK_SIZE,1,fd);
        memcpy(root,buffer,BLOCK_SIZE);
        char** FilesList = listoffiles;
        MetadataBlock** ReturnValues = new MetadataBlock*[numoffiles];
        for(int i=0;i<numoffiles;i++)
            ReturnValues[i]=NULL;
        if(numoffiles>0){
            for(int i=0;i<root->children;i++){
                char buffer[BLOCK_SIZE];
                fseek(fd,root->childrenblocknum[i]*BLOCK_SIZE,SEEK_SET);
                fread(buffer,BLOCK_SIZE,1,fd);
                memcpy(temp,buffer,BLOCK_SIZE);
                recQuery(temp,fd,numoffiles,FilesList,ReturnValues);
            }
            for(int i=0;i<numoffiles;i++){
                if(ReturnValues[i]==NULL){
                    cout<<listoffiles[i]<<" was not found!"<<endl;
                }else{
                    recWriteX(ReturnValues[i],fd);
                }
                delete ReturnValues[i];
            }
        }else{
            for(int i=0;i<root->children;i++){
                char buffer[BLOCK_SIZE];
                fseek(fd,root->childrenblocknum[i]*BLOCK_SIZE,SEEK_SET);
                fread(buffer,BLOCK_SIZE,1,fd);
                memcpy(temp,buffer,BLOCK_SIZE);
                recWrite(temp,fd);
            }
        }
        delete root;
        delete temp;
        delete[] ReturnValues;
        delete[] FilesList;
        fclose(fd);
    }
    else if (jflag){
      cout << "Gzip flag is true!" << endl;
    }
    else if (mflag){
        FILE* fd = fopen(outputf,"r");
        if (fd == NULL) {
            perror("Opening");
            exit(1);
        }
        MetadataBlock* temp = new MetadataBlock;
        MetadataBlock* root = new MetadataBlock;
        char buffer[BLOCK_SIZE];
        fseek(fd,BLOCK_SIZE,SEEK_SET);
        fread(buffer,BLOCK_SIZE,1,fd);
        memcpy(root,buffer,BLOCK_SIZE);
        for(int i=0;i<root->children;i++){
            char buffer[BLOCK_SIZE];
            fseek(fd,root->childrenblocknum[i]*BLOCK_SIZE,SEEK_SET);
            fread(buffer,BLOCK_SIZE,1,fd);
            memcpy(temp,buffer,BLOCK_SIZE);
            recMetaprint(temp,fd);
        }
        fclose(fd);
        delete temp;
        delete root;
    }
    else if (pflag){
      FILE* fd = fopen(outputf,"r");
      if (fd == NULL) {
          perror("Opening");
          exit(1);
      }
      MetadataBlock* temp = new MetadataBlock;
      MetadataBlock* root = new MetadataBlock;
      char buffer[BLOCK_SIZE];
      fseek(fd,BLOCK_SIZE,SEEK_SET);
      fread(buffer,BLOCK_SIZE,1,fd);
      memcpy(root,buffer,BLOCK_SIZE);
      for(int i=0;i<root->children;i++){
          char buffer[BLOCK_SIZE];
          fseek(fd,root->childrenblocknum[i]*BLOCK_SIZE,SEEK_SET);
          fread(buffer,BLOCK_SIZE,1,fd);
          memcpy(temp,buffer,BLOCK_SIZE);
          recPrint(temp,fd);
      }
      fclose(fd);
      delete temp;
      delete root;
    }
    else if (dflag){
      cout << "Delete flag is true!" << endl;
    }
    else if (qflag){
        FILE* fd = fopen(outputf,"r");
        if (fd == NULL) {
            perror("Opening");
            exit(1);
        }
        MetadataBlock* root = new MetadataBlock;
        MetadataBlock* temp = new MetadataBlock;
        char buffer[BLOCK_SIZE];
        fseek(fd,BLOCK_SIZE,SEEK_SET);
        fread(buffer,BLOCK_SIZE,1,fd);
        memcpy(root,buffer,BLOCK_SIZE);
        char** FilesList = listoffiles;
        MetadataBlock** ReturnValues = new MetadataBlock*[numoffiles];
        for(int i=0;i<numoffiles;i++)
            ReturnValues[i]=NULL;
        for(int i=0;i<root->children;i++){
            char buffer[BLOCK_SIZE];
            fseek(fd,root->childrenblocknum[i]*BLOCK_SIZE,SEEK_SET);
            fread(buffer,BLOCK_SIZE,1,fd);
            memcpy(temp,buffer,BLOCK_SIZE);
            recQuery(temp,fd,numoffiles,FilesList,ReturnValues);
        }
        cout<<"Query results:"<<endl;
        for(int i=0;i<numoffiles;i++){
            if(ReturnValues[i]!=NULL){
                cout << i+1 << ". " << listoffiles[i] << " was found at " << ReturnValues[i]->d_name <<endl;
                delete ReturnValues[i];
            }
            else
                cout << i+1 << ". " << listoffiles[i] << " was not found." <<endl;
        }
        delete[] ReturnValues;
        delete[] FilesList;
        delete root;
        delete temp;
        fclose(fd);
    }
    delete[] memory;
    for(int i=0;i<numoffiles;i++){
        for(int j=0;j<counterForFiles[i];j++){
            listoffiles[i]--;
        }
        delete[] listoffiles[i];
        listoffiles[i]=NULL;
    }
    delete[] listoffiles;
    delete[] outputf;

    return 0;
}
