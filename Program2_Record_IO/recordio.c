#include "recordio.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stddef.h>

/* You must use this struct when constructing records. It is
 * expected that all binary data in the index file will follow
 * this format. Do not modify the struct.
 */

int recordFileDescriptor;
struct record_descriptor
{
	int position; //byte offset relative to the beginning of the data file
	int length;   //length of record in bytes
};

/* rio_open
 * Open data file and index file. If create is requested, open both 
 * the data file and the index file with O_CREAT. If O_RDONLY or
 * O_RDWR is requested, make sure that the index file is present and 
 * return an error otherwise. On success the data file's descriptor
 * should be returned and the file descriptor for the index file
 * must be maintained within the abstraction.
 */
int rio_open(const char *pathname, int flags, mode_t mode){
		int dataFileDescriptor = open(pathname, flags, mode);
		if(dataFileDescriptor == -1){ //check for error
			return -1; //return error if error
		}
		//find record file
		char* filename = (char*) pathname;
		char* indexerFileName = malloc(strlen(".rinx.") + strlen(filename) + 1 ); //
		strcpy(indexerFileName, ".rinx.");
		strcat(indexerFileName, filename); //make new file name
		recordFileDescriptor = open(indexerFileName, flags); //get indexerFileDescriptor
		if(recordFileDescriptor == -1){
			return -1; //return error if error
		}
		return dataFileDescriptor; //return file descriptor
	
}

/* rio_read
 * Allocate a buffer large enough to hold the requested record, read 
 * the record into the buffer and return the pointer to the allocated 
 * area. The I/O result should be returned through the return_value 
 * argument. On success this will be the number of bytes read into
 * the allocated buffer. If any system call returns an error, this
 * should be communicated to the caller through return_value.
 */
void *rio_read(int fd, int *return_value){
	char buff[8];
	int readResult = read(recordFileDescriptor, buff, 8); //get record
	
	if(readResult == -1){ //if recordFile Fails to open throw error
		*return_value = -1;
		return NULL;
	}

	//set up record descriptor
	struct record_descriptor record;
	record.position = *((int*) buff);
	record.length = *((int*) buff + 1);
	char* resultBuff = malloc(record.length +1); 
	//char resultBuff[record.length + 1];
	readResult = read(fd, resultBuff, record.length);
	
	//if read has issues return error
	if(readResult == -1){
		*return_value = -1;
		return NULL;
	}

	if(resultBuff[record.length-1] == '\n'){
		resultBuff[record.length-1] = '\0'; //if last element is a line feed set to null termination so when printing it doenst print a line feed
	}
	resultBuff[record.length] = '\0'; //add null terminator to end of buffer 

	//set return values
	*return_value = readResult;
	return resultBuff; //send back correct buffer
}

/* rio_write
 * Write a new record. If appending to the file, create a record_descriptor 
 * and fill in the values. Write the record_descriptor to the index file and 
 * the supplied data to the data file for the requested length. If updating 
 * an existing record, read the record_descriptor, check to see if the new 
 * record fits in the allocated area and rewrite. Return an error otherwise.
 */
//WRITE NEEDS BIG WORK
int rio_write(int fd, const void*buf, int count){
	struct record_descriptor newRecord;
	newRecord.length = count;
	//find current position
	long int currentOffset = lseek(recordFileDescriptor, 0, SEEK_CUR); //save current offset for later
	newRecord.position = lseek(recordFileDescriptor, 0, SEEK_END);	//get the end of the file
	//reset seek
	lseek(recordFileDescriptor, currentOffset, SEEK_SET);

	//write to data file
	int writeReturn = write(fd, buf, count);
	if(writeReturn == -1){
		return -1;
	}

	//write to record file
	writeReturn = write(recordFileDescriptor, &newRecord, sizeof(newRecord));	
	if(writeReturn == -1){
		return -1;
	}
	return writeReturn; //replace with correct return value
}

/* rio_lseek
 * Seek both files (data and index files) to the beginning of the requested 
 * record so that the next I/O is performed at the requested position. The
 * offset argument is in terms of records not bytes (relative to whence).
 * whence assumes the same values as lseek whence argument.
 *
 */
int rio_lseek(int fd, int offset, int whence){

	return -1; //replace with correct return value
}

/* rio_close
 * Close both files. Even though a single descriptor is passed along, 
 * your abstraction must close the other file as well. It is suggested 
 * that you return the descriptor obtained by opening the data file 
 * to the user and keep the index file descriptor number in the 
 * abstraction and associate them.
 */
int rio_close(int fd){
	int closeReturn = close(fd);
	if(closeReturn == -1){
		return -1;
	}
	closeReturn = close(recordFileDescriptor);
	if(closeReturn == -1 ){
		return -1;
	}
	return 0; //replace with correct return value
}
