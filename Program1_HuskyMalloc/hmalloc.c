#include "hmalloc.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
/*You may include any other relevant headers here.*/
//Jake Muller
//CS3411 FA20
//9/17/2020

/*Add additional data structures and globals here as needed.*/
void* free_list = NULL;

/* traverse
 * Start at the free list head, visit and print the length of each
 * area in the free pool. Each entry should be printed on a new line.
 */
void traverse(){
   /* Printing format:
	 * "Index: %d, Address: %08x, Length: %d\n"
	 *    -Index is the position in the free list for the current entry. 
	 *     0 for the head and so on
	 *    -Address is the pointer to the beginning of the area.
	 *    -Length is the length in bytes of the free area.
	 */
	if(free_list == NULL){	return;	}

	void* current = free_list;
	
	int index = 0;
	
	while(1){
		printf("Index: %d, Address: %08x, Length: %d\n", index, (unsigned int)(current+8), *((int*)(current))); //print info
		if(*((int*)(current+4)) == 0 ){ //if there is nothing else break
			//printf("done\n");
			break; 
		} 
		current = current + *((int*)(current + 4)); //set to next element
		index++;
	}
}

//easy method for setting metaData
void addMetaData(int size, void* pointer){
	int* temp = pointer;
	temp[0] = size; //place size in first word
	temp++;       //increment pointer to next word
	temp[0] = 0; //set offset to NULL
}

/* hmalloc
 * Allocation implementation.
 *    -will not allocate an initial pool from the system and will not 
 *     maintain a bin structure.
 *    -permitted to extend the program break by as many as user 
 *     requested bytes (plus length information).
 *    -keeps a single free list which will be a linked list of 
 *     previously allocated and freed memory areas.
 *    -traverses the free list to see if there is a previously freed
 *     memory area that is at least as big as bytes_to_allocate. If
 *     there is one, it is removed from the free list and returned 
 *     to the user.
 */
void *hmalloc(int bytes_to_allocate){
		
	
	int bytesNeeded = bytes_to_allocate + 8; //add 8 bytes for meta data
	void* pointer;
	if(free_list != NULL){ //if there are no free nodes
		//TODO: grab section from meta data and look for big enough memory
		void* currentNode = free_list;
		void* previous = NULL;
		while(*((int*)currentNode) < bytes_to_allocate){
			//printf("currentNode Offset: %d \n", *((int*)(currentNode+4)));
			if(*((int*)(currentNode+4)) == 0){  //if offset to next == NULL break
				//printf("got here \n");
				break;
			}else{
				//printf("went here instead \n");
				previous = currentNode; //keep track of last node
				currentNode = currentNode +  *((int*)(currentNode + 4)); //set to next node
			}
		}
		if(*((int*)currentNode) < bytes_to_allocate){ //if current node is still not big enough allocate mem normaly
			pointer = sbrk(bytesNeeded);
			addMetaData(bytes_to_allocate, pointer); //call meta data function
		}else{
			pointer = currentNode; //set return to current node found that was big enough
			if(*((int*)(currentNode + 4)) == 0 && previous == NULL){ //if its the only value
				//printf("remove only value \n");
				free_list = NULL;
			}else if(previous == NULL){ //if its the head
				//printf("remove head value \n");
				free_list = currentNode +  *((int*)(currentNode + 4)); //send next value
			}else if(*((int*)(currentNode + 4)) == 0){ //if it the last value
				//printf("remove last value \n");
				*((int*)(previous + 4)) = 0;
			}else{
				//printf("restructure \n");
				*((int*)(previous + 4)) = *((int*)(previous + 4)) + *((int*)(currentNode + 4));
			}
		}
		//printf("end malloc, freeList: %p \n", free_list);
		return pointer + 8;
	}else{
		//printf("new allocate \n");
		pointer = sbrk(bytesNeeded);
		addMetaData(bytes_to_allocate, pointer); //call meta data function
	}

   return pointer+8; //hide metadata
}

/* hcalloc
 * Performs the same function as hmalloc but also clears the allocated 
 * area by setting all bytes to 0.
 */
void *hcalloc(int bytes_to_allocate){
	
	void* pointer = hmalloc(bytes_to_allocate);
	int* temp = (int*) (pointer-8); //set temp to data with metadata
	int size = *temp; //get size of allocated size 
	char* pointerData = (char*) (temp + 2); //increment to actual pointer data
	//clear all data 
	int i = 0;
	for(i = 0; i < size; i++){
		pointerData[i] = 0; //go through each byte and clear it
	}
	
   return pointer; //pointer should already be corret location
}

/* hfree
 * Responsible for returning the area (pointed to by ptr) to the free
 * pool.
 *    -simply appends the returned area to the beginning of the single
 *     free list.
 */
void hfree(void *ptr){
	//check if pointer is the last one
	ptr = ptr - 8; //bring back metadata
	//int fullSize = *((int*) ptr) + 8;
	//if(sbrk(0)==(ptr + fullSize)){ //if breakpoint is equal to last address simply get rid of 
	//	sbrk(-1 * fullSize);  //deallocate memory off end 
	//}else{
		if(free_list == NULL){ //if free_list is empty then set to first one
			free_list = ptr;
			int* reset = (int*) (ptr + 4);
			*reset = 0;
		}else{				   //if free_list is filled add to front of the list
			void* currentPrt = ptr;
			int* currentIntPrt = ptr;
			int offset = free_list - ptr; //get offset to freeList
			currentIntPrt[1] = offset; //set offset of currentIntPrt to proper postion
			free_list = currentPrt; //set currentptr to new head
		}
	//}
}

/* For the bonus credit implement hrealloc. You will need to add a prototype
 * to hmalloc.h for your function.*/

/*You may add additional functions as needed.*/
