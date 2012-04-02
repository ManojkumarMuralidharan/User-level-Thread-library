all:
	gcc -m32 -c threadlink.c
	gcc -m32 -c mythread.c
	ar rcs mythread.a mythread.o threadlink.o
