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
	


	
	return 0;
	
} 