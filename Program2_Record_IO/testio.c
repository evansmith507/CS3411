#include "recordio.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

/* Develop a test program called testio which includes recordio.h and 
 * is linked against recordio.o. This program should expect a single 
 * argument which supplies a file name. The program should rio_open 
 * the supplied argument and report any errors to the user. Upon a 
 * successful open it should execute a series of rio_read statements, 
 * read the file one record at a time and write each record to the 
 * standard output as shown below.
 *
 * Data File                          Index file
 * ---------------------------------------------------
 *  Systems                            0, 8
 *  programming is cool.               8, 20
 *
 * "make testio" will compile this souce into testio
 * To execute your program: ./testio <some_record_file_name>
 */
struct record_descriptor
{
	int position; //byte offset relative to the beginning of the data file
	int length;   //length of record in bytes
};




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
	
	int fd = rio_open(argv[1], O_RDWR, 777);
	int readResult;
	for(int i = 0; i < 2; i++){
		char* row = rio_read(fd, &readResult);
		printf("Bytes Read: %d \n", readResult);
		printf("Read: %s \n", row);
	}
	
	char* str = "Just Wrote This Lol"; //19 characters (20 with null pointer) 
	int writeResult = rio_write(fd, str, 20);
	printf("WriteResult: %d \n", writeResult);


	printf("Continue Reading \n");

	for(int i = 0; i < 2; i++){
		char* nrow = rio_read(fd, &readResult);
		printf("Bytes Read: %d \n", readResult);
		printf("Read: %s \n", nrow);
	}


	
	
	return 0;
	
} 