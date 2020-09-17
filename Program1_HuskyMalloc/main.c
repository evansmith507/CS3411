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
	
	//test 1 test traverse and basic functions
	void* first32 = hmalloc(32);
	void* second20 = hmalloc(20);
	void* thrid16 = hmalloc(16);
	hfree(second20);
	hfree(first32);
	hfree(thrid16);
	//should line free_list => 16 -> 32 -> 20

	//test 2 test that bigger memory sections are overpassed
	/*
	void* first = hmalloc(4);
	hfree(first);
	void* second = hmalloc(10);
	*/

	//test 3 remove head from freelist
	/*
	void* first = hmalloc(1);
	void* second = hmalloc(2);
	void* third = hmalloc(30);
	hfree(first);
	hfree(second);
	hfree(third);
	void* test = hmalloc(20);
	*/

	//test 4 remove last element 
	/*
	void* first = hmalloc(1);
	void* second = hmalloc(2);
	void* third = hmalloc(30);
	hfree(third);
	hfree(first);
	hfree(second);
	void* test = hmalloc(20);
	*/

	//test 5 remove only element
	/*
	void* third = hmalloc(30);
	hfree(third);
	void* test = hmalloc(20);
	//printf("test: %x \n", test);
	*/

	//test 6 test calloc
	/*
	void* first = hmalloc(10);
	hfree(first);
	char* thing = hcalloc(5);
	for(int i = 0; i < 10; i++){ //test full 10 secion despite not acttually being that large
		printf("thing[%d] has value %X \n", i, thing[i]);
	}
	*/	

	//test 7 reallocate exact amount
	
	//void* third = hmalloc(30);
	//hfree(third);
	//void* re = hmalloc(30);
	

	traverse();
}
