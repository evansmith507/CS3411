#include "recordio.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>


//NOTES - 

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
	
	if(argc <= 1){
		printf("./%s <some_record_file_name> \n", argv[0]);
		exit(0);
	}

	int fd = rio_open(argv[1], O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO);
	if(fd == -1){
		printf("Open Fail \n");
		exit(0);
	}

		printf("Data File                          \n");
		printf("--------------------------------------------------- \n");
		int readReturn;
		char* stringReturn;
		int position = 0;
		while(1){
			stringReturn = rio_read(fd, &readReturn);
			if(readReturn < 0){
				exit(0);
			}else if(readReturn == 0){
				break;
			}else{
				//print String
				printf("%-35s", stringReturn);
				//print index data 
				//https://cdn.discordapp.com/attachments/755913763827351692/764722500713185380/Capture.PNG
				//printf("%d,%d ", position, readReturn); //emails from tino state that this is not needed?? Please enable if otherwise
				printf("\n");
				position = position + readReturn; //updates position as if they were right behind each other
			}

		}
	rio_close(fd);
	
	return 0;
	
} 