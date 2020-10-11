#include "recordio.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
/* You can use this main function to implement tests that utilize
 * the recordio functions as you implement. This file will not be
 * graded. 
 * "make recordio" will compile the recordio library into a .o file,
 * but it will also produce an executable named recordio which uses
 * the main function declared here.
 */

int main(int argc, char *argv[]){
	



	if(argc <= 1){
		printf("No Argument \n");
		exit(0);
	}

	int fd = rio_open(argv[1], O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
	if(fd == -1){
		printf("Open Fail \n");
		exit(0);
	}

	
	//int readResult;
	//for(int i = 0; i < 3; i++){
	//	char* row = rio_read(fd, &readResult);
	//	printf("Bytes Read: %d \n", readResult);
	//	printf("Read: %s \n", row);
	//}

	

	//--------------------------------
	// Test Writes on new folder
	//--------------------------------
	
	rio_write(fd, "THis Is COol", 12);
	rio_write(fd, "THis Is rad", 11);
	rio_write(fd, "THis Is dumb", 12);
	rio_lseek(fd, 1, SEEK_SET);
	rio_write(fd, "exactly", 7);

	rio_lseek(fd, 0, SEEK_SET);
	int readResult;
	int i = 0;
	for(i = 0; i < 3; i++){
		char* row = rio_read(fd, &readResult);
		printf("Bytes Read: %d \n", readResult);
		printf("Read: %s \n", row);
	}


	//-------------------------------
	// Test Opening two files at once ->(./testio <NewFile> , <File with existing .rinx. file>)
	//-------------------------------
	int fd2 = rio_open(argv[2], O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
	if(fd == -1){
		printf("Open Fail \n");
		exit(0);
	}
	
	for(i = 0; i < 3; i++){
		char* row = rio_read(fd2, &readResult);
		printf("Bytes Read: %d \n", readResult);
		printf("Read: %s \n", row);
	}


	rio_close(fd);
	rio_close(fd2);
	return 0;
}
