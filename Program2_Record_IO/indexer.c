//Jake Muller

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
/* You must use this struct when constructing records. It is
 * expected that all binary data in the index file will follow
 * this format. Do not modify the struct.
 */
struct record_descriptor
{
	int position; //byte offset relative to the beginning of the data file
	int length;   //length of record in bytes
};

/* main - indexer
 * Develop a single standalone program called indexer which creates an 
 * index file for a text file whose title is supplied as an argument to 
 * the program. This program should read the text file from beginning 
 * till end, find the beginning and ending of each line (ending with 
 * the newline character), create a descriptor and write the descriptor 
 * to the created index file. The program should not modify the text 
 * file which is supplied as an argument.
 * 
 * "make indexer" will compile this source into indexer
 * To run your program: ./indexer <some_text_file>
 */
int main(int argc, char *argv[]){
	/* The index file for record i/o is named by following the 
	 * convention .rinx.<data file name> (i.e., if the data file
	 * is named myfile.txt, its index will be .rinx.myfile.txt). 
	 * This convention conveniently hides the index files while
	 * permitting easy access to them based on the data file name. 
	 */
	printf("argc: %d \n", argc);
	if(argc <= 1){ //no argument found
		printf("No argurment present, \n To run program: %s <filename> \n", argv[0]);
		exit(1);
		return 0;
	}
	

	//open and create files
	int binaryFileHandle = open(argv[1], O_RDONLY);
	printf("opened \n");
	printf("file name: %s \n", argv[1]);
	char* filename = argv[1];
	char* indexerFileName = malloc(strlen(".rinx.") + strlen(filename) + 1 ); //
	strcpy(indexerFileName, ".rinx.");
	strcat(indexerFileName, filename); //make new file name
	printf("New file Name is: %s \n", indexerFileName);
	int recordFileHandle = open(indexerFileName, O_RDWR | O_CREAT | O_APPEND);
	int readReturn;
	int writeReturn;
	unsigned int recordFileIndexSize = 0;
	
	while(1){
		
		//find size of read
		char Sizebuff[1];
		int size = 0; 
		while(1){
			readReturn = read(binaryFileHandle, Sizebuff, 1);
			if(Sizebuff[0] == '\n'){
				printf("found line feed \n");
				size++; // keeping track of line feed may cause problems (btw it totally does)
				break;
			}
			if(readReturn == 0){
				break;
			}
			size++;
		}

		char* buff[size];
		lseek(binaryFileHandle, recordFileIndexSize, SEEK_SET); //reset read point
		readReturn = read(binaryFileHandle, buff, size);
		
		//check for error/end of file
		if(readReturn < 0){ //error returned
			printf("Error reading: \n");
			break;
		}else if(readReturn == 0){ //no bytes read (aka end of file)
			break;
		}
		

		//make new record
		struct record_descriptor newRecord;
		newRecord.length = readReturn;							//
		newRecord.position = recordFileIndexSize;				//set postion
		recordFileIndexSize = recordFileIndexSize + readReturn; //update record index file position

		//write new record_descriptor to file
		writeReturn = write(recordFileHandle, &newRecord, sizeof(newRecord)); //&newRecord may cause issues as its just a pointer to it
		//char lineCarragebuff[1] = '\n';
		printf("writen bytes: %d \n", writeReturn);
		//int eff = write(recordFileHandle, lineCarragebuff, 1); 
		printf("What was writen %d, %d \n", *((int*)(&newRecord)), *((int*)(&newRecord) + 1));
		printf("Record Descriptor: Lenght: %d : Position: %d \n", newRecord.length, newRecord.position);
	}
	

	return 0;
}
