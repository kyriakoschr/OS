#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>

#include "myRecordDef.h"

using namespace std;

int main(int argc,char* argv[]){
    float centerx;
    float centery;
    float radius;
    int offset=0;
    int pointscntr=0;
    int numOfrecords;
    bool iflag=false;
    bool oflag=false;
    bool aflag=false;
    bool fflag=false;
    bool nflag=false;
    FILE* input;
    FILE* output;
    int i=1;
    if( (argc!=9)&&(argc!=11)&&(argc!=13)){
        cerr<<"Wrong amount of arguments"<<endl;
        return -1;
    }
    int j=0;
    while (i<argc){
        if(strcmp(argv[i],"-i")==0){
            if(iflag==true){
                cerr<<"Wrong -i argument"<<endl;
                return -1;
            }
            iflag=true;
            input=fopen(argv[i+1],"rb");
        }
        else if(strcmp(argv[i],"-o")==0){
            if(oflag==true){
                cerr<<"Wrong -o argument"<<endl;
                return -1;
            }
            oflag=true;
            output=fopen(argv[i+1],"wb");
        }
        else if(strcmp(argv[i],"-a")==0){
            if(aflag==true){
                cerr<<"Wrong -a arguments"<<endl;
                return -1;
            }
            aflag=true;
            centerx=atof(argv[i+1]);
            centery=atof(argv[i+2]);
            radius=atof(argv[i+3]);
            i+=2;
        }
        else if(strcmp(argv[i],"-f")==0){
            if(fflag==true){
                cerr<<"Wrong -f arguments"<<endl;
                return -1;
            }
            fflag=true;
            offset=atoi(argv[i+1]);
        }
        else if(strcmp(argv[i],"-n")==0){
            if(nflag==true){
                cerr<<"Wrong -n arguments"<<endl;
                return -1;
            }
            nflag=true;
            pointscntr=atoi(argv[i+1]);
        }
        else {
            cerr<<"Wrong arguments"<<endl;
            return -1;
        }
        i+=2;
    }
    long lSize;
    if(pointscntr>0)
        numOfrecords=pointscntr;
    else if(pointscntr==0){
        fseek (input, 0 , SEEK_END);
        lSize = ftell (input);
        rewind (input);
        numOfrecords =((int) lSize/(sizeof(MyRecord)))-(offset/sizeof(MyRecord));
    }
    if((numOfrecords<=0)||(pointscntr<0)||(offset<0)){
        fclose(input);
        fclose(output);
        cerr<<"Wrong offset arguments : "<<offset<<" "<<pointscntr<<" "<<numOfrecords<<endl;
        return -1;
    }
    fseek (input,offset,SEEK_SET);
    for(int j=0;j<numOfrecords;j++){
        MyRecord temp;
        fread(&temp,sizeof(MyRecord),1,input);
        float tempdist=sqrt((temp.x-centerx)*(temp.x-centerx)+(temp.y-centery)*(temp.y-centery));
        if(tempdist<=radius){
            fwrite(&temp,sizeof(MyRecord),1,output);
        }
    }
    fclose(input);
    fclose(output);
    return 1;
}
