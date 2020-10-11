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
	
		/*
	int fd = open(".rinx.Test.txt", O_RDWR);
	char buff[8]; 
	lseek(fd, 0, SEEK_SET);
	int result = read(fd, buff, 8);
	if(result == -1){
		printf("BAD \n");
	}
	printf("Hex Buffer: %X %X %X %X %X %X %X %X \n", buff[0], buff[1],buff[2],buff[3],buff[4],buff[5],buff[6],buff[7] );
	struct record_descriptor returnRecord;
	returnRecord.position = *((int*) buff);
	returnRecord.length = *((int*) buff + 1);
	printf("Record Descriptor: Lenght: %d : Position: %d \n", returnRecord.length, returnRecord.position);
	*/
	



	if(argc <= 1){
		printf("No Argument \n");
		exit(0);
	}

	int fd = rio_open(argv[1], O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO);
	if(fd == -1){
		printf("Open Fail \n");
		exit(0);
	}
	int readResult;
	for(int i = 0; i < 2; i++){
		char* row = rio_read(fd, &readResult);
		printf("Bytes Read: %d \n", readResult);
		printf("Read: %s \n", row);
	}
	
	
	char* str = "No THIS"; //7 characters (8 with null pointer) 
	int writeResult = rio_write(fd, str, 8);
	printf("WriteResult: %d \n", writeResult);

	
	printf("Continue Reading from start\n\n");
	rio_lseek(fd, 0, SEEK_SET); //reset 
	for(int i = 0; i < 5; i++){
		char* nrow = rio_read(fd, &readResult);
		printf("Bytes Read: %d \n", readResult);
		printf("Read: %s \n", nrow);
	}
	
	str = "This on is gonna be reallly long to test this"; //7 characters (8 with null pointer) 
	writeResult = rio_write(fd, str, strlen(str)+2);
	printf("WriteResult: %d \n", writeResult);

	printf("Continue Reading from start\n\n");
	rio_lseek(fd, 0, SEEK_SET); //reset 
	for(int i = 0; i < 5; i++){
		char* nrow = rio_read(fd, &readResult);
		printf("Bytes Read: %d \n", readResult);
		printf("Read: %s \n", nrow);
	}

	
	
	
	return 0;
}
