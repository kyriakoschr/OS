#include "hasht.h"
#include <iomanip>

int htsize;

int main(int argc, char* argv[]){
    srand(time(NULL));
    char* filename=NULL;
    char* configfile=NULL;                                          //mexri ti grammi 151 ginetai elegxos parametrwn
    if(argc==7){
        if(strcmp(argv[1],"-b")==0){
            if(strcmp(argv[3],"-l")==0){
                if(strcmp(argv[5],"-c")==0){
                    htsize=atoi(argv[2]);
                    filename=new char[strlen(argv[4])];
                    strcpy(filename,argv[4]);
                    configfile=new char[strlen(argv[6])];
                    strcpy(configfile,argv[6]);
                }
                else{
                    cout << "Wrong input"<<endl;
                    return -1;
                }
            }
            else if(strcmp(argv[3],"-c")==0){
                if(strcmp(argv[5],"-l")==0){
                    htsize=atoi(argv[2]);
                    filename=new char[strlen(argv[6])];
                    strcpy(filename,argv[6]);
                    configfile=new char[strlen(argv[4])];
                    strcpy(configfile,argv[4]);
                }
                else{
                    cout << "Wrong input"<<endl;
                    return -1;
                }
            }
            else {
                cout << "Wrong input"<<endl;
                return -1;
            }
        }
        else if(strcmp(argv[1],"-l")==0){
            if(strcmp(argv[3],"-b")==0){
                if(strcmp(argv[5],"-c")==0){
                    htsize=atoi(argv[4]);
                    filename=new char[strlen(argv[2])];
                    strcpy(filename,argv[2]);
                    configfile=new char[strlen(argv[6])];
                    strcpy(configfile,argv[6]);
                }
                else{
                    cout << "Wrong input"<<endl;
                    return -1;
                }
            }
            else if(strcmp(argv[3],"-c")==0){
                if(strcmp(argv[5],"-b")==0){
                    htsize=atoi(argv[6]);
                    filename=new char[strlen(argv[2])];
                    strcpy(filename,argv[2]);
                    configfile=new char[strlen(argv[4])];
                    strcpy(configfile,argv[4]);
                }
                else{
                    cout << "Wrong input"<<endl;
                    return -1;
                }
            }
            else {
                cout << "Wrong input"<<endl;
                return -1;
            }
        }
        else if(strcmp(argv[1],"-c")==0){
            if(strcmp(argv[3],"-l")==0){
                if(strcmp(argv[5],"-b")==0){
                    htsize=atoi(argv[6]);
                    filename=new char[strlen(argv[4])];
                    strcpy(filename,argv[4]);
                    configfile=new char[strlen(argv[2])];
                    strcpy(configfile,argv[2]);
                }
                else{
                    cout << "Wrong input"<<endl;
                    return -1;
                }
            }
            else if(strcmp(argv[3],"-b")==0){
                if(strcmp(argv[5],"-l")==0){
                    htsize=atoi(argv[4]);
                    filename=new char[strlen(argv[6])];
                    strcpy(filename,argv[6]);
                    configfile=new char[strlen(argv[2])];
                    strcpy(configfile,argv[2]);
                }
                else{
                    cout << "Wrong input"<<endl;
                    return -1;
                }
            }
            else {
                cout << "Wrong input"<<endl;
                return -1;
            }
        }
        else{
            cout << "Wrong input"<<endl;
            return -1;
        }
    }
    else if(argc==5){
        if((strcmp(argv[1],"-l")==0)&&(strcmp(argv[3],"-b")==0)){
            htsize=atoi(argv[4]);
            filename=new char[strlen(argv[2])];
            strcpy(filename,argv[2]);
        }
        else if((strcmp(argv[1],"-c")==0)&&(strcmp(argv[3],"-b")==0)){
            htsize=atoi(argv[4]);
            configfile=new char[strlen(argv[2])];
            strcpy(configfile,argv[2]);
        }
        else if(strcmp(argv[1],"-b")==0){
            if(strcmp(argv[3],"-l")==0){
                htsize=atoi(argv[2]);
                filename=new char[strlen(argv[4])];
                strcpy(filename,argv[4]);
            }
            else if(strcmp(argv[3],"-c")==0){
                htsize=atoi(argv[2]);
                configfile=new char[strlen(argv[4])];
                strcpy(configfile,argv[4]);
            }
            else{
                cout << "Wrong input"<<endl;
                return -1;
            }
        }

    }
    else if(argc==3)
        if ( strcmp(argv[1],"-b")==0)
            htsize=atoi(argv[2]);
        else{
            cout << "Wrong input"<<endl;
            return -1;
    }
    else {
        cout << "Wrong input"<<endl;
        return -1;
    }
    SkipList* skiplist=new SkipList;
    HashTable* hasht=new HashTable(htsize);
    FILE * fp;
    char * line = NULL;
    char* token;
    size_t len = 0;
    ssize_t read;
    if(filename!=NULL){                         //gia diavasma apo arxeio
        fp = fopen(filename, "r");
        if (fp == NULL){
            delete skiplist;
            delete hasht;
            exit(EXIT_FAILURE);
        }
        while ((read = getline(&line, &len, fp)) != -1) {
            cerr<<line;
            char* token;
            token = strtok (line," \n");
            if (token != NULL){
                if(strcmp(token,"i")==0){
                    int id=atoi(strtok(NULL, " \n"));
                    if ((id<MinValue)||(id>=MaxValue))
                        continue;
                    char* lname=strtok(NULL, " \n");
                    char* fname=strtok(NULL, " \n");
                    double gpa=atof(strtok(NULL, " \n"));
                    if((gpa>10)||(gpa<0))
                        continue;
                    int noc=atoi(strtok(NULL, " \n"));
                    char* department=(strtok(NULL, " \n"));
                    if ((noc<0)||(noc>52))
                        continue;
                    int postc=atoi(strtok(NULL, " \n"));
                    token=strtok(NULL, " \n");
                    if(token != NULL){
                        cerr<<"Wrong command"<<endl;
                        continue;
                    }
                    else{
                        Account* temp=skiplist->insertNode(id,lname,fname,gpa,noc,department,postc);
                        if(temp!=NULL)
                            hasht->addNode(temp);
                    }
                }
                else if(strcmp(token,"q")==0){
                    int id=atoi(strtok(NULL, " \n"));
                    token=strtok(NULL, " \n");
                    if(token != NULL){
                        cerr<<"Wrong command"<<endl;
                        continue;
                    }
                    else{
                        SkipListNode* temp=skiplist->findStud(id);
                        if(temp==NULL)
                            cerr<<"Not found"<<endl;
                        else
                            temp->getAcc()->print();
                    }
                }
                else if(strcmp(token,"m")==0){
                    int id=atoi(strtok(NULL, " \n"));
                    double gpa=atof(strtok(NULL, " \n"));
                    if((gpa>10)||(gpa<0))
                        continue;
                    int noc=atoi(strtok(NULL, " \n"));
                    if ((noc<0)||(noc>52))
                        continue;
                    token=strtok(NULL, " \n");
                    if(token != NULL){
                        cerr<<"Wrong command"<<endl;
                        continue;
                    }
                    else
                        if(skiplist->modify(id,gpa,noc)==1)
                            skiplist->findStud(id)->getAcc()->print();
                        else
                            cerr<<"Not found"<<endl;
                }
                else if(strcmp(token,"d")==0){
                    int id=atoi(strtok(NULL, " \n"));
                    token=strtok(NULL, " \n");
                    if(token != NULL){
                        cerr<<"Wrong command"<<endl;
                        continue;
                    }
                    else{
                        SkipListNode* temp=skiplist->findStud(id);
                        if(temp!=NULL){
                            temp->getAcc()->print();
                            hasht->deleteNode(temp->getAcc());
                            skiplist->deleteNode(id);
                        }
                        else{
                            cerr<<"Not found"<<endl;
                        }
                    }
                }
                else if(strcmp(token,"ra")==0){
                    int id1=atoi(strtok(NULL, " \n"));
                    int id2=atoi(strtok(NULL, " \n"));
                    token=strtok(NULL, " \n");
                    if(token != NULL){
                        cerr<<"Wrong command"<<endl;
                        continue;
                    }
                    else{
                        cout<<setprecision(2)<<fixed<<skiplist->raverage(id1,id2)<<"\%"<<endl;
                    }
                }
                else if(strcmp(token,"a")==0){
                    int pc=atoi(strtok(NULL, " \n"));
                    token=strtok(NULL, " \n");
                    if(token != NULL){
                        cerr<<"Wrong command"<<endl;
                        continue;
                    }
                    else{
                        cout<<setprecision(2)<<fixed<<hasht->average(pc)<<"\%"<<endl;
                    }
                }
                else if(strcmp(token,"ta")==0){
                    int k=atoi(strtok(NULL, " \n"));
                    int pc=atoi(strtok(NULL, " \n"));
                    token=strtok(NULL, " \n");
                    if(token != NULL){
                        cerr<<"Wrong command"<<endl;
                        continue;
                    }
                    else
                        hasht->taverage(k,pc);
                }
                else if(strcmp(token,"b")==0){
                    int k=atoi(strtok(NULL, " \n"));
                    token=strtok(NULL, " \n");
                    if(token != NULL){
                        cerr<<"Wrong command"<<endl;
                        continue;
                    }
                    else
                        skiplist->bottom(k);
                }
                else if(strcmp(token,"ct")==0){
                    int pc=atoi(strtok(NULL, " \n"));
                    char* department=strtok(NULL, " \n");
                    token=strtok(NULL, " \n");
                    if(token != NULL){
                        cerr<<"Wrong command"<<endl;
                        continue;
                    }
                    else{
                        int result=hasht->coursestotake(pc,department);
                        if(result>0)
                            cout<<result<<endl;
                        else
                            cerr<<"Not found"<<endl;
                    }
                }
                else if(strcmp(token,"f")==0){
                    double gpa=atof(strtok(NULL, " \n"));
                    token=strtok(NULL, " \n");
                    if(token != NULL){
                        cerr<<"Wrong command"<<endl;
                        continue;
                    }
                    else
                        skiplist->findGPA(gpa);
                }
                else if(strcmp(token,"p")==0){
                    int pc=atoi(strtok(NULL, " \n"));
                    token=strtok(NULL, " \n");
                    if(token != NULL){
                        cerr<<"Wrong command"<<endl;
                        continue;
                    }
                    else{
                        cout<<setprecision(2)<<fixed<<hasht->studentsinpc(pc)*100.00/skiplist->fetchCounter()<<"\%"<<endl;
                    }
                }
                else if(strcmp(token,"pe")==0){
                    token=NULL;
                    token=strtok(NULL, " \n");
                    if(token != NULL){
                        cerr<<"Wrong command"<<endl;
                        continue;
                    }
                    else{
                        int* temparray=hasht->percentiles();
                        for(int i=MINPOSTCODE;i<MAXPOSTCODE+1;i++)
                            if(temparray[i-MINPOSTCODE]>0)
                                cout<<i<<": "<<setprecision(2)<<fixed<<temparray[i-MINPOSTCODE]*100.00/skiplist->fetchCounter()<<"\%"<<endl;
                            delete[] temparray;
                    }
                }
                else if(strcmp(token,"e")==0){
                    token=strtok(NULL, " \n");
                    if(token != NULL){
                        cerr<<"Wrong command"<<endl;
                        continue;
                    }
                    else{
                        delete hasht;
                        delete skiplist;
                        fclose(fp);
                        if (line)
                            free(line);
                        exit(0);
                    }
                }
                else{
                    continue;
                }
            }
            else
                cerr<<"Wrong command"<<endl;

        }
        fclose(fp);
        if (line)
            free(line);
        exit(-1);
    }
    else{                                               //gia diavasma apo to terminal
        char input[100];
        scanf ("%[^\n]%*c", input);
        while(1){
            cerr<<input<<endl;
            char* token;
            token = strtok (input," \n");
            if (token != NULL){
                if(strcmp(token,"i")==0){
                    int id=atoi(strtok(NULL, " \n"));
                    if ((id<=MinValue)||(id>=MaxValue)){
                        scanf ("%[^\n]%*c", input);
                        continue;
                    }
                    char* lname=strtok(NULL, " \n");
                    char* fname=strtok(NULL, " \n");
                    double gpa=atof(strtok(NULL, " \n"));
                    if((gpa>10)||(gpa<0)){
                        scanf ("%[^\n]%*c", input);
                        continue;
                    }
                    int noc=atoi(strtok(NULL, " \n"));
                    char* department=(strtok(NULL, " \n"));
                    if ((noc<0)||(noc>52)){
                        scanf ("%[^\n]%*c", input);
                        continue;
                    }
                    int postc=atoi(strtok(NULL, " \n"));
                    token=strtok(NULL, " \n");
                    if(token != NULL){
                        cerr<<"Wrong command"<<endl;
                        scanf ("%[^\n]%*c", input);
                        continue;
                    }
                    else{
                        Account* temp=skiplist->insertNode(id,lname,fname,gpa,noc,department,postc);
                        if(temp!=NULL)
                            hasht->addNode(temp);
                    }
                }
                else if(strcmp(token,"q")==0){
                    int id=atoi(strtok(NULL, " \n"));
                    token=strtok(NULL, " \n");
                    if(token != NULL){
                        cerr<<"Wrong command"<<endl;
                        scanf ("%[^\n]%*c", input);
                        continue;
                    }
                    else{
                        SkipListNode* temp=skiplist->findStud(id);
                        if(temp==NULL)
                            cerr<<"Not found"<<endl;
                        else
                            temp->getAcc()->print();
                    }
                }
                else if(strcmp(token,"m")==0){
                    int id=atoi(strtok(NULL, " \n"));
                    double gpa=atof(strtok(NULL, " \n"));
                    if((gpa>10)||(gpa<0)){
                        scanf ("%[^\n]%*c", input);
                        continue;
                    }
                    int noc=atoi(strtok(NULL, " \n"));
                    if ((noc<0)||(noc>52)){
                        scanf ("%[^\n]%*c", input);
                        continue;
                    }
                    token=strtok(NULL, " \n");
                    if(token != NULL){
                        cerr<<"Wrong command"<<endl;
                        scanf ("%[^\n]%*c", input);
                        continue;
                    }
                    else
                        if(skiplist->modify(id,gpa,noc)==1)
                            skiplist->findStud(id)->getAcc()->print();
                        else
                            cerr<<"Not found"<<endl;
                }
                else if(strcmp(token,"d")==0){
                    int id=atoi(strtok(NULL, " \n"));
                    token=strtok(NULL, " \n");
                    if(token != NULL){
                        cerr<<"Wrong command"<<endl;
                        scanf ("%[^\n]%*c", input);
                        continue;
                    }
                    else{
                        SkipListNode* temp=skiplist->findStud(id);
                        if(temp!=NULL){
                            temp->getAcc()->print();
                            hasht->deleteNode(temp->getAcc());
                            skiplist->deleteNode(id);
                        }
                        else{
                            cerr<<"Not found"<<endl;
                        }
                    }
                }
                else if(strcmp(token,"ra")==0){
                    int id1=atoi(strtok(NULL, " \n"));
                    int id2=atoi(strtok(NULL, " \n"));
                    token=strtok(NULL, " \n");
                    if(token != NULL){
                        cerr<<"Wrong command"<<endl;
                        scanf ("%[^\n]%*c", input);
                        continue;
                    }
                    else{
                        cout<<setprecision(2)<<fixed<<skiplist->raverage(id1,id2)<<"\%"<<endl;
                    }
                }
                else if(strcmp(token,"a")==0){
                    int pc=atoi(strtok(NULL, " \n"));
                    token=strtok(NULL, " \n");
                    if(token != NULL){
                        cerr<<"Wrong command"<<endl;
                        scanf ("%[^\n]%*c", input);
                        continue;
                    }
                    else{
                        cout<<setprecision(2)<<fixed<<hasht->average(pc)<<"\%"<<endl;
                    }
                }
                else if(strcmp(token,"ta")==0){
                    int k=atoi(strtok(NULL, " \n"));
                    int pc=atoi(strtok(NULL, " \n"));
                    token=strtok(NULL, " \n");
                    if(token != NULL){
                        cerr<<"Wrong command"<<endl;
                        scanf ("%[^\n]%*c", input);
                        continue;
                    }
                    else
                        hasht->taverage(k,pc);
                }
                else if(strcmp(token,"b")==0){
                    int k=atoi(strtok(NULL, " \n"));
                    token=strtok(NULL, " \n");
                    if(token != NULL){
                        cerr<<"Wrong command"<<endl;
                        scanf ("%[^\n]%*c", input);
                        continue;
                    }
                    else
                        skiplist->bottom(k);
                }
                else if(strcmp(token,"ct")==0){
                    int pc=atoi(strtok(NULL, " \n"));
                    char* department=strtok(NULL, " \n");
                    token=strtok(NULL, " \n");
                    if(token != NULL){
                        cerr<<"Wrong command"<<endl;
                        scanf ("%[^\n]%*c", input);
                        continue;
                    }
                    else{
                        int result=hasht->coursestotake(pc,department);
                        if(result>0)
                            cout<<result<<endl;
                        else
                            cerr<<"Not found"<<endl;
                    }
                }
                else if(strcmp(token,"f")==0){
                    double gpa=atof(strtok(NULL, " \n"));
                    token=strtok(NULL, " \n");
                    if(token != NULL){
                        cerr<<"Wrong command"<<endl;
                        scanf ("%[^\n]%*c", input);
                        continue;
                    }
                    else
                        skiplist->findGPA(gpa);
                }
                else if(strcmp(token,"p")==0){
                    int pc=atoi(strtok(NULL, " \n"));
                    token=strtok(NULL, " \n");
                    if(token != NULL){
                        cerr<<"Wrong command"<<endl;
                        scanf ("%[^\n]%*c", input);
                        continue;
                    }
                    else{
                        cout<<setprecision(2)<<fixed<<hasht->studentsinpc(pc)*100.00/skiplist->fetchCounter()<<"\%"<<endl;
                    }
                }
                else if(strcmp(token,"pe")==0){
                    token=NULL;
                    token=strtok(NULL, " \n");
                    if(token != NULL){
                        cerr<<"Wrong command"<<endl;
                        scanf ("%[^\n]%*c", input);
                        continue;
                    }
                    else{
                        int* temparray=hasht->percentiles();
                        for(int i=MINPOSTCODE;i<MAXPOSTCODE+1;i++)
                            if(temparray[i-MINPOSTCODE]>0)
                                cout<<i<<": "<<setprecision(2)<<fixed<<temparray[i-MINPOSTCODE]*100.00/skiplist->fetchCounter()<<"\%"<<endl;
                            delete[] temparray;
                    }
                }
                else if(strcmp(token,"e")==0){
                    token=strtok(NULL, " \n");
                    if(token != NULL){
                        cerr<<"Wrong command"<<endl;
                        scanf ("%[^\n]%*c", input);
                        continue;
                    }
                    else{
                        delete hasht;
                        delete skiplist;
                        exit(0);
                    }
                }
                else{
                    cerr<<"Wrong command"<<endl;
                    scanf ("%[^\n]%*c", input);
                    continue;
                }
            }
            else
                cerr<<"Wrong command"<<endl;
        }

    }
}
