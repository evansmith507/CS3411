#include "hmalloc.h"
#include <stdlib.h>
#include <unistd.h>
/*You may include any other relevant headers here.*/


/*Add additional data structures and globals here as needed.*/

/*
//node for linked list of free nodes
typedef struct {
	void* freeSpot;
	freeNode NextSpot;
}freeNode;
*/
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
}

//easy method for setting metaData
void addMetaData(int size, void* pointer){
	int* temp = pointer;
	*temp = size; //place size in first word
	temp++;       //increment pointer to next word
	//TODO:: FIX THIS IF NEEDED
	*temp = (int) (free_list - pointer); //place free list as the next free item 
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
	}else{
		pointer = sbrk(bytesNeeded);
		addMetaData(bytes_to_allocate, pointer); //call meta data function
	}

   return pointer; 
}

/* hcalloc
 * Performs the same function as hmalloc but also clears the allocated 
 * area by setting all bytes to 0.
 */
void *hcalloc(int bytes_to_allocate){
	
	void* pointer = hmalloc(bytes_to_allocate);
	int* temp = (int*) pointer;
	int size = *temp; //get size of allocated size 
	char* pointerData = (char*) (temp + 2); //increment to actual pointer data
	//clear all data 
	for(int i = 0; i < size; i++){
		pointerData[i] = 0; //go through each byte and clear it
	}
	
   return pointer; 
}

/* hfree
 * Responsible for returning the area (pointed to by ptr) to the free
 * pool.
 *    -simply appends the returned area to the beginning of the single
 *     free list.
 */
void hfree(void *ptr){
	//check if pointer is the last one
	int fullSize = *((int*) ptr) + 8;
	if(sbrk(0)==(ptr + fullSize)){ //if breakpoint is equal to last address simply get rid of 
		sbrk(-1 * fullSize);  //deallocate memory of end 
	}else{
		if(free_list == NULL){ //if free_list is empty then set to first one
			free_list = ptr;
			int* reset = (int*) (ptr + 4);
			*reset = NULL;
		}else{				   //if free_list is filled add to list
			void* currentPtr = free_list;
			while((currentPtr+4) != NULL){ //while there is a pointer to the next link
				int size = *((int*) currentPtr);
				currentPtr = currentPtr + size + 8;
			}
			//insert pointer
			int* final = (int*)(currentPtr + 4); //increment to pointer section
			*final = (int) (ptr - currentPtr); //p-n: pointer to next entry relative distance between start of this entry and start of next entry
			//reuse final to make the end of the list NULL
			final = ptr;
			*final = NULL;
		}
	}
}

/* For the bonus credit implement hrealloc. You will need to add a prototype
 * to hmalloc.h for your function.*/

/*You may add additional functions as needed.*/
