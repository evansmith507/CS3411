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
			printf("fail on data open:\n");
			return -1; //return error if error
		}
		//find record file
		char* filename = (char*) pathname;
		char* indexerFileName = malloc(strlen(".rinx.") + strlen(filename) ); //
		strcpy(indexerFileName, ".rinx.");
		strcat(indexerFileName, filename); //make new file name
		printf("file name: %s \n", indexerFileName);
		recordFileDescriptor = open(indexerFileName, flags, mode); //get indexerFileDescriptor
		if(recordFileDescriptor == -1){
			printf("fail on record open:\n");
			return -1; //return error if error
		}
		free(indexerFileName); //free indexerFileName
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
	}else if(readResult == 0){ //if end of file is found and no more records return (SHOULDNT MOVE POSTION)
		*return_value = 0;
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

	//THIS GETS RID OF LINE FEEDS AT THE END OF READS - SUPPOSEDLY THEY CAN STAY
	//if(resultBuff[record.length-1] == '\n'){
	//		resultBuff[record.length-1] = '\0'; //if last element is a line feed set to null termination so when printing it doenst print a line feed
	//}
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

	//get current position  
	char buff[8];
	int readReturn = read(recordFileDescriptor, buff, sizeof(struct record_descriptor));
	int writeReturn = -1;
	if(readReturn == -1){ //if recordFile Fails to read
		printf("record file read fail: within write");
		return -1;
	}else if(readReturn == 0 ){ //create new record
		struct record_descriptor newRecord;
		newRecord.length = count;
		int postion = lseek(fd, 0, SEEK_CUR); //get current position
		newRecord.position = postion; //hopfully this works

		//write both files
		 writeReturn = write(fd, buf, count); //write data file
		if(writeReturn == -1){
			return -1; //error writing
		}
		writeReturn = write(recordFileDescriptor, &newRecord, sizeof(newRecord));	//write record file
		if(writeReturn == -1){
			return -1; //error writing
		}

	}else{ //replace record
		
		int length = *((int*) buff + 1); //get length of record.
		if(count > length){ //if new buffer is longer than old buffer return error
			printf("Buffer is longer than allocated space can hold: Bytes requested: %d, Bytes Avalible: %d \n", count, length);
			return -1;
		}

		//write new data
		writeReturn = write(fd, buf, count);
		if(writeReturn == -1){
			return -1;
		}
		int leftover = length - count;
		if(leftover > 0){ //if there is extra buffer space open clear it out
			char* leftBuff = calloc(leftover, sizeof(char));
			writeReturn = write(fd, leftBuff, leftover); //clear rest of record and postion properly
			free(leftBuff);
		} 
	}

	return count; 
}

/* rio_lseek
 * Seek both files (data and index files) to the beginning of the requested 
 * record so that the next I/O is performed at the requested position. The
 * offset argument is in terms of records not bytes (relative to whence).
 * whence assumes the same values as lseek whence argument.
 *
 */
int rio_lseek(int fd, int offset, int whence){
	int position = -1;
	if(whence == SEEK_SET){
		position = whence;
		lseek(recordFileDescriptor, whence * sizeof(struct record_descriptor), SEEK_SET); //set postion
		//find data file poistion
		char* buff[8];
		int readResult = read(recordFileDescriptor, buff, 8);
		if(readResult == -1){
			return -1; //error reading
		}
		int dataPosition = *((int*) buff); //get the data position
		//send record file back to proper spot
		lseek(recordFileDescriptor, whence * sizeof(struct record_descriptor), SEEK_SET); //set postion
		//send data file to proper spot
		lseek(fd, dataPosition, SEEK_SET);
	}else if(whence == SEEK_END){
		printf("NOT DONE YET \n");
	}else if(whence == SEEK_CUR){ 
		printf("NOT DONE YET \n");
	}else{
		return -1;
	}
	return position; //replace with correct return value
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
