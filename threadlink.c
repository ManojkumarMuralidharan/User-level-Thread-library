#ifndef FILE_FOO_SEEN
#define FILE_FOO_SEEN


#include <ucontext.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
struct Threadlist{
 int id;
 int pid;
 int child_count;
 int exit_status;
 int wait_status;
 int active;
 int join;
 int join_count;
 ucontext_t context;
 struct Threadlist * next;
};
////////////////////////////////

struct zombiequeue{

    int id;
    struct zombiequeue * next;
};

static struct zombiequeue * zombiequeuehead=NULL ;




struct zombiequeue * zombieqfrontp =NULL ;
struct zombiequeue * zombieqrearp = NULL ;


void pushtozombiequeue(int id){

struct zombiequeue * temp;
temp=(struct zombiequeue*)malloc(sizeof(struct zombiequeue));		
temp->id=id;
temp->next=NULL;
	
	if(zombieqfrontp ==NULL && zombieqrearp ==NULL){
	

	zombieqfrontp=temp;
	zombieqrearp=temp;	
	//queuehead=temp;


	}
	else{
	
	zombieqrearp->next=temp;
	zombieqrearp=temp;	
	
	}



}



int popfromzombiequeue(){

int temp;
	if(zombieqfrontp==NULL && zombieqrearp==NULL){
// List is empty
	return -1;
	}
	else if(zombieqfrontp==zombieqrearp){
	temp=zombieqfrontp->id;	
	zombieqfrontp=NULL;
	zombieqrearp=NULL;
	return temp;
	}
	else{
	temp=zombieqfrontp->id;
	zombieqfrontp=zombieqfrontp->next;
	return temp;
	}

}





/////////////////////////////

struct Threadlist * Threadlisthead;

//initlist(){

//Threadlist

//}
int addchild(struct Threadlist child){
	struct Threadlist* tmp,* lastchild;
	//printf("1");
	if(Threadlisthead == NULL){
		//printf("2");
		Threadlisthead = (struct Threadlist*)malloc(sizeof(struct Threadlist));
		if(Threadlisthead==NULL){
		printf("\n Cannot allocate memory Program terminating \n");
		exit(0);
		}
		Threadlisthead->id=child.id;
Threadlisthead->pid=child.pid;
Threadlisthead->child_count=child.child_count;
Threadlisthead->active=child.active;
Threadlisthead->context=child.context;
Threadlisthead->exit_status=child.exit_status;
Threadlisthead->wait_status=child.wait_status;
Threadlisthead->join_count=child.join_count;
Threadlisthead->join=child.join;
Threadlisthead->next=NULL;
		//Threadlisthead=child;
		//thread_count=thread_count+1;
		//Threadlisthead->id=thread_count;
		return 1;
		}
		else
		{
		 //struct Threadlist* child;
		 //child=(struct Threadlist*)malloc(sizeof(struct Threadlist));
		 //thread_count++;
		 //child->id=thread_count;
		 tmp=Threadlisthead;
			 while(tmp->next!=NULL){
			  tmp=tmp->next;
			 }


		lastchild= (struct Threadlist*)malloc(sizeof(struct Threadlist));
		if(lastchild==NULL){
		printf("\n Cannot allocate memory Program terminating \n");
		exit(0);
		}
tmp->next=lastchild;
		lastchild->id=child.id;
lastchild->pid=child.pid;
lastchild->child_count=child.child_count;
lastchild->active=child.active;
lastchild->context=child.context;

lastchild->exit_status=child.exit_status;
lastchild->wait_status=child.wait_status;
lastchild->join_count=child.join_count;

lastchild->join=child.join;
lastchild->next=NULL;
		//	 tmp->next=child;
		 return 1;
	}

	return 0;

}
void setchildjoins(int id){

struct Threadlist *temp=Threadlisthead;

if(temp!=NULL){

while(temp!=NULL){
if(temp->pid==id){
temp->join=1;
}
temp=temp->next;
}

}


}

void deletechild(int id){
struct Threadlist *deletenode = Threadlisthead;
//added newly
struct Threadlist *delete;
struct Threadlist *prevnode = NULL;
//int id = child->id;
	if(deletenode==NULL){

	//No nodes to delte

	}
	else
	{
		if(deletenode->id==id){
		delete=Threadlisthead;
		Threadlisthead= Threadlisthead->next;
		free(delete);
		}
		else{
			prevnode=deletenode;
				while(deletenode!=NULL){
					if(deletenode->id==id){
			                     prevnode->next=deletenode->next;
					     break;
					}else{
						prevnode=deletenode;						
						deletenode=deletenode->next;	
						
					}
				}


			
		}

	}
}




struct Threadlist * findnode(id){
struct Threadlist * homenode = Threadlisthead;
do{
if(homenode->id==id)
{
return homenode;
}
else
homenode=homenode->next;
}while(homenode!=NULL);

return NULL;


}



void deletezombies(int id){
//parent id
struct Threadlist * node, *temp  ;

while(id!=1){
node=findnode(id);
	if(node->exit_status==1&&node->child_count==0){
	pushtozombiequeue(id);
	id=node->pid;
	temp=findnode(id);
	temp->child_count=temp->child_count-1;
	}
/*check this logic
	else if(node->wait_status==1&&node->child_count==0){
	pushintoreadyqueue(id);	
	break;
	}*/
	else
	break;
}

id=popfromzombiequeue();
while(id!=-1){
temp=findnode(id);
//free(temp->context.uc_stack.ss_sp);
deletechild(id);
free(temp);
id=popfromzombiequeue();

}




}

int updatenode(int id,ucontext_t updatecontext){
struct Threadlist * homenode = Threadlisthead;
do{
if(homenode->id==id)
{
homenode->context=updatecontext;
return 1;
}
else
homenode=homenode->next;
}while(homenode!=NULL);

return -1;


}

int findactive(void){
struct Threadlist * homenode = Threadlisthead;
int id;
while(homenode!=NULL){
id=homenode->id;
//printf("hi");
if(homenode->active==1)
{
return id;
}
else
homenode=homenode->next;
}

return -1;


}
void clearactive(void){
struct Threadlist * homenode = Threadlisthead;
do{
if(homenode->active==1)
{
homenode->active=0;
return ;
}
else
homenode=homenode->next;
}while(homenode!=NULL);

return ;


}
void printll(){

//printf("3");
struct Threadlist * tmp=Threadlisthead;
if(tmp!=NULL){
printf("%d---",tmp->id);
while(tmp->next!=NULL){
tmp=tmp->next;
printf(" %d ->",tmp->id);

}
}
else
printf("tlist rmpty");



}

/*
int main(){

struct Threadlist * tmp1 = (struct Threadlist*)malloc(sizeof(struct Threadlist));
struct Threadlist * tmp2 = (struct Threadlist*)malloc(sizeof(struct Threadlist));
struct Threadlist * tmp3 =(struct Threadlist*)malloc(sizeof(struct Threadlist));
struct Threadlist * tmp4 = (struct Threadlist*)malloc(sizeof(struct Threadlist));

tmp1->id=1;
tmp2->id=2;
tmp3->id=3;
tmp2->active=1;
//tmp4->id=4;

printll();
addchild(tmp1);
printf("\n");
printll();
addchild(tmp2);
printf("\n");
printll();
addchild(tmp3);
printf("\n");
printll();
deletechild(3);
printf("\n");
printll();
printf("active =%d",findactive());
//tmp4=findnode(3);
//if(tmp4==NULL){
//printf("Not FOund");
//}
//else
//printf("%d",tmp4->id);





}*/


#endif
