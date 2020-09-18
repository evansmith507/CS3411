#include <stdio.h>
#include <unistd.h>
#include "hmalloc.h"

/*You may include any other relevant headers here.*/
//Jake Muller
//CS3411 FA20
//9/17/2020

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

	//test 6 test calloc (thing[10] should equal 0xA) *all over good test -> tests malloc, free, and callac
	/*
	char* first = (char*) hmalloc(10);
	first[5] = 0x4;
	first[9] = 0x5;
	first[10] = 0xA; //put something after to check calloc
	
	printf("first: %p \n", first);
	int i = 0;
	for(i = 0; i < 11; i++){ //test full 10 secion despite not acttually being that large
		printf("first [%d] has value %X \n", i, first[i]);
	}
	hfree(first);
	char* thing = hcalloc(5);
	
	printf("thing: %p \n", thing);
	i = 0;
	for(i = 0; i < 11; i++){ //test full 10 secion despite not acttually being that large
		printf("thing[%d] has value %X \n", i, thing[i]);
	}
	*/

	//test 7 reallocate exact amount
	
	//void* third = hmalloc(30);
	//hfree(third);
	//void* re = hmalloc(30);
	

	traverse();
	return 0;
}
