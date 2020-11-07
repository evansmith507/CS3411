//Jake Muller CS3411

/* used for debuging purposes
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stddef.h>
*/
#include "bitsy.h" //This header includes prototypes for the proposed bit abstractions
/*Include any additional headers you require*/


/*You may use any global variables/structures that you like*/
void addToBuffer(char* buffer, unsigned int* count, char entry){
	
	buffer[*count % 8] = entry; 
	//int fd = open("Debub.txt", O_RDWR | O_CREAT | O_APPEND);
	//char string[200];
	//sprintf(string, "Put into Buffer: %c \n", buffer[count % 8]);
	//write(fd, string, 26);
	*count = *count + 1;
}

/*
void runBuffer(char* buffer, unsigned int count){
	//char result;
	int fd = open("Debub.txt", O_RDWR | O_CREAT | O_APPEND);
	char string[200];
	
	for(int i = 0; i < 8; i++){
		sprintf(string, "buffer: %c \n", buffer[( count - (i+1) ) % 8]);
		write(fd, string, 11);
	}
	sprintf(string, "\n");
	write(fd, string, 1);
	return;
}
*/

char checkUniqueness(char* buffer, unsigned int count, char byteRead){
	//char result;
	//int fd = open("Debub.txt", O_RDWR | O_CREAT | O_APPEND);
	//char string[200];
	int i = 0;
	for(i = 0; i < 8; i++){

		//sprintf(string, "Compare, input: %c buffer: %c \n", byteRead, buffer[( count - (i+1) ) % 8]);
		//write(fd, string, 29);

		if(buffer[(count - (i+1)) % 8] == byteRead){
			//sprintf(string, "Return %d \n", i);
			//write(fd, string, 10);
			return i;
		}
	}
	//sprintf(string, "\n");
	//write(fd, string, 1);
	return 9;
}





/* main - czy compression implementation
 * Develop a program called czy which compresses the data stream directed 
 * at its standard input and writes the compressed stream to its standard 
 * output.
 *
 * The compression algorithm reads the input one symbol (i.e., byte) at a 
 * time and compares it with each of the 8 bytes previously seen.  It also 
 * checks to see if the following n characters are the same as the current 
 * symbol.  If the byte has been previously seen, it outputs the position of 
 * the previous byte relative to the current position as well as the number 
 * of times it occurs using the format described below. Otherwise, the symbol 
 * is output as is by prefixing it with a binary one.
 *
 * To compile czy: make czy
 * To execute: ./czy < somefile.txt > somefile.encoded
 */
int main(int argc, char *argv[]){
	//The implementation of your encoder should go here.
	//It is recommeded that you implement the bit abstractions in bitsy.c and
	//utilize them to implement your encoder. 
	//If so, do NOT call read/write here. Instead rely exclusively on 
	//readBit, readByte, writeBit, writeByte, and flushWriteBuffer.
	int isLastRepeated = 0;
	char finalReadCharOnRepeated = 0;
	unsigned int count = 0;
	char buffer[8] = {0,0,0,0,0,0,0};
	while(1){ 

		char byteRead = 0;
		if(!isLastRepeated){
			unsigned short readResult = readByte();
			if(readResult == 258){
				break; //break out of while loop as there is nothing left
				//end of file / error
			}
			//convert to char after checking for error
			byteRead = (char) readResult;
		}else{
			byteRead = finalReadCharOnRepeated;
		}
		
		

		//start encoding process 
		char uniqueResult = checkUniqueness(buffer, count, byteRead);
		//runBuffer(buffer, count);
		//************************************************************
		// Infrequent Data Form
		//************************************************************
		if(uniqueResult >= 9){ //if unique to past eight
			//write 1 and symbol
			writeBit(1);
			writeByte(byteRead);
			//add writen byte to previous read buffer
			addToBuffer(buffer, &count, byteRead);
			isLastRepeated = 0;
		//************************************************************
		// Occurred Data Form
		//************************************************************
		}else{ //if isnt unique in the past eight
			//write 0 bit for repeating 
			writeBit(0);

			//check if reapeats 
			unsigned short repeatByte = readByte();
			if(repeatByte == 258){
				writeBit(0);
				//write 3 bit count
				writeBit((0x4 & uniqueResult) >> 2);
				writeBit((0x2 & uniqueResult) >> 1);
				writeBit(0x1 & uniqueResult);
				break; //were done end of file bb
			//************************************************************
			// Repeating Data Form
			//************************************************************
			}else if(byteRead == (char)repeatByte){
				addToBuffer(buffer, &count, repeatByte);//add first item
				addToBuffer(buffer, &count, repeatByte);//add first repeated
				char repeats = 0;
				while(repeats < 8){
					repeatByte = readByte();
					
					if(repeatByte == 258){
						//end of file
						break;
					}
					if((char)repeatByte != byteRead){
						//stoped repeating
						break;
					}
					addToBuffer(buffer, &count, (char) repeatByte);
					repeats++;			
				}
				if(repeats == 8){ //captures edge case where increments a last time if next element still repeats
					repeats = 7;
				}
				//write repeating code 
				writeBit(1);
				//write offset
				writeBit((0x4 & uniqueResult) >> 2);
				writeBit((0x2 & uniqueResult) >> 1);
				writeBit(0x1 & uniqueResult);
				//write count
				writeBit((0x4 & repeats) >> 2);
				writeBit((0x2 & repeats) >> 1);
				writeBit(0x1 & repeats);
				if(repeatByte == 258){
					//end of file reached
					break;
				}else{
					isLastRepeated = 1;
					finalReadCharOnRepeated = repeatByte;
				}
			//************************************************************
			// Non-Repeating Data Form
			//************************************************************
			}else{
				writeBit(0);
				//write 3 bit count
				//int fd = open("Debub.txt", O_RDWR | O_CREAT | O_APPEND);
				//char string[200];
				//sprintf(string, "uniqueResult %x \n", uniqueResult);
				//write(fd, string, 16);
				writeBit((0x4 & uniqueResult) >> 2);
				writeBit((0x2 & uniqueResult) >> 1);
				writeBit(0x1 & uniqueResult);
				addToBuffer(buffer, &count, (char)byteRead);
				
				isLastRepeated = 1;
				finalReadCharOnRepeated = repeatByte;
			}
		}
		//addToBuffer(buffer, count, byteRead); //after dealing with new byte store in buffer
	}
	flushWriteBuffer();
	
	return 0; //exit status. success=0, error=-1
}

