#ifndef BAR_H
#define BAR_H
#include <sys/types.h>
#include <semaphore.h>
#include <stdio.h>
#include <iostream>
//#define barcap 10

using namespace std;

struct node {
    pid_t info;
    int size;
    //struct node *next;
};

class Queue{
    private:
        //node *rear;
        //node *front;
        int max;
        int counter;
        int people;
    public:
        sem_t barg;
        void initQueue(int);
        node *array;
        ~Queue();
        void push(pid_t data,int s);
        pid_t pop(int cap);
        bool is_empty();
        bool is_full(int);
};

#endif // BAR_H
