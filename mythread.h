#ifndef MY_THREAD
#define MY_THREAD


#ifndef FILE_FOO_SEEN
#include "threadlink.h"
#endif

#include <ucontext.h>
#include <stdio.h>
#include <stdlib.h>


typedef void * MyThread;

typedef int MySemaphore;


static struct semaphorequeue * semaphoresqhead;
static int semaphore_count;


////////////////////////
// 
// semaphores
//
///////////////////////

struct semaphorequeue{
int semid;
struct semaphores *semaphore;
struct semaphorequeue *next;
};

struct semaphores{

int semid;
int semvalue;
struct sem_list *list;
};

struct sem_list{
int process_id;
struct sem_list * next;

};



struct blockedqueue{
	int id;
	struct blockedqueue * next;
}; 

static int thread_count=0;

/////////////////////
//
// ready queue methods
//
/////////////////////

struct readyqueue{

    int id;
    struct readyqueue * next;
};

/* The Pointer to the head of the ready queue */
static struct readyqueue * queuehead ;

/* The Pointer to the head of the blockedqueue */

static struct blockedqueue * blockedqueuehead;



struct readyqueue * readyqfrontp;
struct readyqueue * readyqrearp ;


struct blockedqueue * blockedqfrontp ;
struct blockedqueue * blockedqrearp ;



void pushtoreadyqueue(int id);

int popfromreadyqueue();

///////////////////////
//
// blocked list
//////////////////////


int pushintoblockedqueue(int id);

void deletefromblockedqueue(int id);

int findinblockedqueue(int id);

////////////////////////
//
//My Thread functions
///////////////////////

MyThread MyThreadCreate(void(*start_funct)(void*), void *args);

// Yield invoking thread
 void MyThreadYield(void);


// Join with a child thread
 int MyThreadJoin(MyThread thread);

// Join with all children
void MyThreadJoinAll(void);

void MyThreadExit(void);

// ****** CALLS ONLY FOR UNIX PROCESS ****** 
// Create the "main" thread
MyThread MyThreadInit(void(*start_funct)(void*), void *args);
    
// Start running the "main" thread
void MyThreadRun(void);


MySemaphore MySemaphoreInit(int initialValue);


struct semaphores * findsemaphore(int id);


void addtosem_list(struct semaphores* sem,int thread_id);


void deletefromsem_list(struct semaphores* sem,int id,int thread_id);

void deletefromsemqueue(int sem_id);


int lengthofsemlist(struct semaphores *sem);

int removefromsemlist(struct semaphores *sem);

void MySemaphoreSignal(int semid);

void MySemaphoreWait(int semid);

int MySemaphoreDestroy(MySemaphore sem);


#endif
