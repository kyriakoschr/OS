#ifndef TABLES_H
#define TABLES_H
#include <iostream>
#include <sys/types.h>
#include <stdio.h>
#include "bar.h"
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#define UNUSED 0
#define FULL 0
#define AVAILABLEBAR 1
#define AVAILABLETABLE 2
/*#define two 2
#define four 4
#define six 6
#define eight 8*/

using namespace std;

class Table{
    private:
        int capacity;
        pid_t available;
    public:
        Table();
        int cap(){return capacity;};
        void setcap(int cap){capacity=cap;};
        void giveit(pid_t customer){available=customer;};
        pid_t whosit(){return available;};
        bool isavailable(){if (available>0) return false;else return true;};
        void freeme(){available=UNUSED;};
};

class Hall{
    private:
        int size;

    public:
        sem_t hallg;
        int two;
        int four;
        int six;
        int eight;
        Table *tarray;
        Table *farray;
        Table *sarray;
        Table *earray;
        void initHall(int,int,int,int);
        Table* request(int capacity);
};


struct Doorman{
        sem_t doorq;
        sem_t door;
        sem_t wakeup;
        sem_t cust;
        node nextinq;
        int status;
        Table* availablet;
        bool acceptbar;
        bool custwaits;
        //int custwaits;
};

struct Shared_Data{
        int two;
        int four;
        int six;
        int eight;
        Hall hall;
        Queue bar;
        struct Doorman doorinfo;
};


#endif // TABLES_H

