#ifndef MY_THREAD
#define MY_THREAD

#include <ucontext.h>
#ifndef FILE_FOO_SEEN
#include "threadlink.h"
#endif

#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>

typedef void * MyThread;

typedef int MySemaphore;

static struct semaphorequeue * semaphoresqhead;
static int semaphore_count=0;

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
static struct readyqueue * queuehead=NULL ;

/* The Pointer to the head of the blockedqueue */

static struct blockedqueue * blockedqueuehead=NULL ;



struct readyqueue * readyqfrontp =NULL ;
struct readyqueue * readyqrearp = NULL ;


struct blockedqueue * blockedqfrontp =NULL ;
struct blockedqueue * blockedqrearp = NULL ;



void pushtoreadyqueue(int id){

	struct readyqueue * temp;
	temp=(struct readyqueue*)malloc(sizeof(struct readyqueue));		
	temp->id=id;
	temp->next=NULL;
	
		if(readyqfrontp ==NULL && readyqrearp ==NULL){
			readyqfrontp=temp;
			readyqrearp=temp;	
		}
		else{
		readyqrearp->next=temp;
		readyqrearp=temp;	
		}
}

int popfromreadyqueue(){

	int temp;
	if(readyqfrontp==NULL && readyqrearp==NULL){
	// List is empty
	return -1;
	}
	else if(readyqfrontp==readyqrearp){
	temp=readyqfrontp->id;	
	readyqfrontp=NULL;
	readyqrearp=NULL;
	return temp;
	}
	else{
	temp=readyqfrontp->id;
	readyqfrontp=readyqfrontp->next;
	return temp;
	}

}

///////////////////////
//
// blocked list
//////////////////////


int pushintoblockedqueue(int id){

struct blockedqueue * tmp;
struct blockedqueue * nextelement;

	if(blockedqueuehead==NULL){
	blockedqueuehead=(struct blockedqueue *)malloc(sizeof(struct blockedqueue));
	blockedqueuehead->id=id;
	blockedqueuehead->next=NULL;
	return 1;
	}

	else
	{
	tmp=blockedqueuehead;
	nextelement=(struct blockedqueue *)malloc(sizeof(struct blockedqueue));
	nextelement->id=id;
	nextelement->next=NULL;
	while(tmp->next!=NULL){
		tmp=tmp->next;
	}
	tmp->next=nextelement;
	return 1;
	}
return -1;
}

void deletefromblockedqueue(int id){

struct blockedqueue * tmp=blockedqueuehead;
if(tmp!=NULL){

if(tmp->id==id){
blockedqueuehead=blockedqueuehead->next;
}

else
   { 
	while(tmp->next!=NULL){
	if(tmp->next->id==id)
	 {
	 tmp->next=tmp->next->next;
         return ;
	 }
	else
	tmp=tmp->next;
   	}
   }

}

}

int findinblockedqueue(int id){

struct blockedqueue * tmp=blockedqueuehead;

	if(tmp==NULL){
	 return -1;
	}

	else{

		while(tmp!=NULL){
			if(tmp->id==id)
				return 1;
			else
				tmp=tmp->next;
			}
	return -1;
	}

}

////////////////////////
//
//My Thread functions
///////////////////////

MyThread MyThreadCreate(void(*start_funct)(void*), void *args)
{
//Threadlist
struct Threadlist * parent,* returnid;
struct Threadlist child;
ucontext_t childcontext;
int result;
parent=findnode(findactive());
getcontext(&childcontext);
childcontext.uc_link=0;       
childcontext.uc_stack.ss_sp = malloc (8*1064);
if(childcontext.uc_stack.ss_sp==NULL){
printf("\n Cannot allocate memory Program terminating \n");
exit(0);
}
childcontext.uc_stack.ss_size = 8*1064;
childcontext.uc_stack.ss_flags = 0;
makecontext(&childcontext, (void(*)(void))start_funct,1,args);

       child.id=++thread_count;
       child.child_count=0;
       child.active=0;
       child.join=0;
       child.join_count=0;
       child.wait_status=0;
       child.context=childcontext;
       child.pid=parent->id;
       parent->child_count=parent->child_count+1;

      //add child to linkled info
      result=addchild(child);
      if(result!=-1){
      //push to ready queue
      pushtoreadyqueue(child.id);
      }
      
returnid=findnode(child.id);
return (void *)&returnid->id;


}

// Yield invoking thread
 void MyThreadYield(void)
{
int id;
ucontext_t savecontext,newcontext;
struct Threadlist * current_thread;
struct Threadlist * next_thread;

id=findactive();
current_thread=findnode(id);

//pop from ready queue
pushtoreadyqueue(id);

id=popfromreadyqueue();
if(id!=-1){
//set active to 1
next_thread=findnode(id);

current_thread->active=0;
next_thread->active=1;

//send old context back to queue
getcontext(&savecontext);
current_thread->context=savecontext;
newcontext=next_thread->context;

//swap to new context 
swapcontext(&current_thread->context,&next_thread->context);
}
return;

}


// Join with a child thread
 int MyThreadJoin(MyThread thread)
{
int next_id;
int *tid=(int *)thread;
int id=*tid;
struct Threadlist * child,*active,*next_active;
child=findnode(id);
active=findnode(findactive());
if(child==NULL){

return -1;

}
else if(child->pid==active->id){

if(active->join_count==0)
active->wait_status=1;

active->join_count=active->join_count+1;
child->join=1;
pushintoblockedqueue(active->id);
active->active=0;
next_id=popfromreadyqueue();

/// check for deadlock if no process in ready queue
next_active=findnode(next_id);
next_active->active=1;
swapcontext(&active->context,&next_active->context); 
return 1;


}else if(child->pid!=active->id){

return -1;
}
else if(child->id==active->id){
printf("\nDeadlock condition\n");
return -1;
}



}

// Join with all children
 void MyThreadJoinAll(void)
{
int id;
struct Threadlist * parent,*active;
parent=findnode(findactive());
parent->join_count=parent->child_count;
parent->wait_status=1;
parent->active=0;
setchildjoins(parent->id);
pushintoblockedqueue(parent->id);
id=popfromreadyqueue();
active=findnode(id);
active->active=1;

swapcontext(&parent->context,&active->context); 

} 

void MyThreadExit(void)
{
int id,parent_id,child_id,deleteid,grandparent;
struct Threadlist * parent, *child,*zombies;
ucontext_t *temp;
ucontext_t parentcontext,tempcontext;
struct readyqueue *temp1=readyqfrontp;

child=findnode(findactive());
temp1=readyqfrontp;

//identify parent
parent_id=child->pid;
child_id=child->id;
parent=findnode(parent_id);

//decrement parent child count

	child->active=0;

	if(parent->wait_status==1){

		if(child->join==1){
		parent->join_count=parent->join_count-1;
		}
		if(parent->join_count==0)
		parent->wait_status=0;
		if(parent->wait_status==0&&parent->exit_status==0)
		pushtoreadyqueue(parent->id);

	}
	if(child->child_count>0){
	child->exit_status=1;

	}if(child->child_count==0){
		parent->child_count=(parent->child_count)-1;
		if(parent->child_count==0&&parent->exit_status==1)
		deletezombies(parent->id);
		else
		deletechild(child->id);

		}
		
id=popfromreadyqueue();
				if(id==-1){
				parent=Threadlisthead;
				parent->active=1;
				free(child);
				swapcontext(&child->context,&parent->context);		
				}
				if(id!=-1)
				{
				parent=findnode(id);
				parent->active=1;
				//free(child);
				swapcontext(&tempcontext,&parent->context);
		
				}
		





}

// ****** CALLS ONLY FOR UNIX PROCESS ****** 
// Create the "main" thread
MyThread MyThreadInit(void(*start_funct)(void*), void *args)
{

int result;

ucontext_t childcontext;
ucontext_t parentcontext;
struct Threadlist * parentnode;

struct Threadlist child,parent; 

getcontext(&parentcontext);
parent.context=parentcontext;

parent.id=++thread_count;
parent.child_count=0;
parent.active=1;
parent.pid=0;
parent.exit_status=1;
//added in linked list to get information
result=addchild(parent);
//add to blocked queue


result=findactive();

	if (result!=-1){	
	//add to ready queue 
	getcontext(&childcontext);

	childcontext.uc_stack.ss_sp=malloc(8*1064);
	if(childcontext.uc_stack.ss_sp==NULL){
	printf("\n Cannot allocate memory Program terminating \n");
	exit(0);
	}
	childcontext.uc_stack.ss_size=8*1064;
	childcontext.uc_link=0;
        childcontext.uc_stack.ss_flags=0;

	makecontext(&childcontext, (void(*)(void))start_funct,1,args);
	// Put context info into Child Thread Identifier	
	child.context=childcontext;
        child.id=++thread_count;
	child.active=0;
        child.pid=parent.id;
	child.child_count=0;
	child.join_count=0;
	child.join=0;
	child.wait_status=0;
	child.exit_status=0;
        // update childcount
	parentnode=findnode(parent.id);
 	//adding 1 to child count 
	parentnode->child_count=parentnode->child_count+1;

	//add child to linkled info
	result=addchild(child);

		if(result!=-1){
		//push to ready queue
		pushtoreadyqueue(child.id);
		}
	}


}
    
// Start running the "main" thread
void MyThreadRun(void)
{

//pop from ready queue and take for execution
int id=popfromreadyqueue();
ucontext_t temp;
struct Threadlist * parent = (struct Threadlist *)malloc(sizeof(struct Threadlist));
struct Threadlist * child = (struct Threadlist *)malloc(sizeof(struct Threadlist));

parent=findnode(findactive());

parent->active=0;

//get element from ready queue
child=findnode(id);
child->active=1;

temp=child->context;
swapcontext(&parent->context,&child->context);

}



MySemaphore MySemaphoreInit(int initialValue){

struct semaphores * sem;
struct semaphorequeue *head;
head=semaphoresqhead;

sem=(struct semaphores *)malloc(sizeof(struct semaphores));
sem->semid=semaphore_count++;
sem->semvalue=initialValue;
sem->list=NULL;
//Create a semaphore. Set the initial value to initialValue, which must be non-negative. A positive initial value has the same effect as invoking MySemaphoreSignal the same number of times.
	if(semaphoresqhead==NULL){
	semaphoresqhead=(struct semaphorequeue *)malloc(sizeof(struct semaphorequeue));	
	semaphoresqhead->semid=sem->semid;
	semaphoresqhead->semaphore=sem;
	semaphoresqhead->next=NULL;
	}
	else{
	while(head->next!=NULL){
	head=head->next;
	}
	head->next=(struct semaphorequeue *)malloc(sizeof(struct semaphorequeue));
	head->next->semid=sem->semid;
	head->next->semaphore=sem;
	head->next->next=NULL;
	}

return sem->semid;
}


struct semaphores * findsemaphore(int id){

	struct semaphorequeue * head=semaphoresqhead;

	while(head!=NULL){
		if(head->semid==id)
		return head->semaphore;
		else
		head=head->next;
	}
return NULL;

}


void addtosem_list(struct semaphores* sem,int thread_id){

struct sem_list *list;

	if(sem->list==NULL){
	sem->list=(struct sem_list *)malloc(sizeof(struct sem_list));
	sem->list->process_id=thread_id;
	sem->list->next=NULL;
	}
	else{
	list=sem->list;
		while(list->next!=NULL){
		list=list->next;
		}
	list->next=(struct sem_list *)malloc(sizeof(struct sem_list)); 	 
	list->next->process_id=thread_id;
	list->next->next=NULL;
	}

}


void deletefromsem_list(struct semaphores* sem,int id,int thread_id){
/*struct sem_list *list;
list=sem->list;
if(list!=NULL){
if(list->process_id==thread_id){
sem->list=sem->list->next;
return;}
}
	while(list!=NULL){
		if(list->next->process_id==thread_id){
		list->next=list->next->next;
		break;
		}else
		list=list->next;
	}
*/
}

void deletefromsemqueue(int sem_id){
struct semaphorequeue *head;
head=semaphoresqhead;
if(head!=NULL){
	if(head->semid==sem_id){
	semaphoresqhead=semaphoresqhead->next;
	return;
	}
	while(head->next!=NULL){
	if(head->next->semid==sem_id){
	head->next=head->next->next;
	break;
	}
	head=head->next;
	}
   }
}

int lengthofsemlist(struct semaphores *sem){
int length=0;

struct sem_list *list;
list=sem->list;

while(list!=NULL){
list=list->next;
length++;
}

return length;

}

int removefromsemlist(struct semaphores *sem){

struct sem_list *list;
int id;

	list=sem->list;
	if(list!=NULL){
	id=list->process_id;	
	sem->list=list->next;
	return id;
	}
	else 
	return -1;
	


}




//wait
void MySemaphoreWait(int semid){

struct semaphores *sem;
struct Threadlist * thread, * next_thread;
int id,thread_id;
sem=findsemaphore(semid);
if(sem==NULL){
	printf("\n %d - No such semaphore exists\n",semid);
	exit(0);
	}else{
thread_id=findactive();
	if(sem->semvalue>0)
	{
	sem->semvalue=sem->semvalue-1;
	}
	else{
		
		thread=findnode(thread_id);	
		//push into bloacked state
		thread->active=0;	
		//add wait status
		addtosem_list(sem,thread_id);
		int id=popfromreadyqueue();
		if(id!=-1){
		next_thread=findnode(id);
		next_thread->active=1;
		swapcontext(&thread->context,&next_thread->context);	
		}
		else if(id==-1){
		next_thread=Threadlisthead;
		next_thread->active=1;
		setcontext(&next_thread->context);	
		}
	}
}	

}

//signal
void MySemaphoreSignal(int semid){
	struct semaphores *sem;
	int id,thread_id;
	sem=findsemaphore(semid);
	if(sem==NULL){
	printf("\n %d - No such semaphore exists\n",semid);
	exit(0);
	}else{
	thread_id=findactive();
	//deletefromsem_list(thread_id);

	if(lengthofsemlist(sem)==0){
	sem->semvalue=sem->semvalue+1;
	}
	else
	{
	id=removefromsemlist(sem);
	pushtoreadyqueue(id);
	}
  }
}

int MySemaphoreDestroy(MySemaphore sem){
int id=sem;
int length;
struct semaphores *semdestroy;
semdestroy=findsemaphore(id);

	if(semdestroy==NULL){
	printf("\n %d - No such semaphore exists\n",sem);
	exit(0);
	}

	if(semdestroy!=NULL){
	length=lengthofsemlist(semdestroy);
	if(length>0){
	return -1;
	}
	else{
	//deletefromsemlist(semdestroy);
	deletefromsemqueue(id);
	return 0;
	}


	}

}


#endif
