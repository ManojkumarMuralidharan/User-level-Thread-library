#ifndef FILE_FOO_SEEN
#define FILE_FOO_SEEN

#include <ucontext.h>
#include <malloc.h>
#include <stdio.h>
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

struct Threadlist * Threadlisthead;

int addchild(struct Threadlist child);
void deletechild(int id);

struct Threadlist* findnode(int id);

int updatenode(int id,ucontext_t *updatecontext);

int findactive(void);
void clearactive(void);
void printll();

#endif
