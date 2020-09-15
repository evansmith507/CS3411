#include <stdio.h>
#include <unistd.h>
#include "hmalloc.h"

/*You may include any other relevant headers here.*/


/*	main()
 *	Use this function to develop tests for hmalloc. You should not 
 *	implement any of the hmalloc functionality here. That should be
 *	done in hmalloc.c
 *	This file will not be graded. When grading I will replace main 
 *	with my own implementation for testing.*/
int main(int argc, char *argv[]){
	int temp = 8;
	void* pointer = &temp;
	printf(" pointer 1: %p\n", pointer);
	pointer = pointer + 2;
	printf(" pointer 2: %p\n", pointer);
	//temp = (int) pointer;
	//printf(" temp : 0x%x", temp);
	

	//printf(" hmalloc return %p \n", hmalloc(5));
	
	void* point = sbrk(32);
	int test = ( (point + 32) == sbrk(0) );
	printf("%d \n", test);
	printf("%p \n", sbrk(0));
	return 1;
}
