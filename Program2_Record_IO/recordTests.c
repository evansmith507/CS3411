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
	


	rio_close(fd);
	return 0;
}
