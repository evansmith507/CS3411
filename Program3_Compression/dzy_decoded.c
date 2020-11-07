/*
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
	char result;
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

/* main - dzy de-compression implementation
 * This program decompresses a compressed stream directed at its standard input 
 * and writes decompressed data to its standard output.
 *
 * To compile dzy: make dzy
 * To execute: ./dzy < somefile.encoded > somefile_decoded.txt
 */
int main(int argc, char *argv[]){
	//The implementation of your decoder should go here.
	
	//It is recommeded that you implement the bit abstractions in bitsy.c and
	//utilize them to implement your decoder.
	//If so, do NOT call read/write here. Instead rely exclusively on 
	//readBit, readByte, writeBit, writeByte, and flushWriteBuffer.
	char buffer[8] = {0,0,0,0,0,0,0,0};
	unsigned int count = 0;
	unsigned short byteRead = 0;
	unsigned short bitRead = 0;
	while(1){
		bitRead = readBit();
		if(bitRead == 258){
			//end of file
			break;
		}

		if(bitRead == 1){	//if first bit is one then iregular and symbol needs to be output
			//runBuffer(buffer, count);
			byteRead = readByte();
			if(byteRead == 258){
				//end of file
				break;
			}
			writeByte(byteRead);
			addToBuffer(buffer, &count, byteRead);
			//runBuffer(buffer, count);
		}else{
			bitRead = readBit();
			if(bitRead == 258){
				//end of file //bad time
				break;
			}

			if(bitRead == 0){ //
				//runBuffer(buffer, count);
				//grab number to shift by
				unsigned short shift = 0;
				shift = readBit();
				shift = shift << 1;
				shift = shift | readBit();
				shift = shift << 1;
				shift = shift | readBit();
				//if it causes issues check shift 
				
				writeByte(buffer[(count - (shift+1)) % 8]);
				addToBuffer(buffer, &count, buffer[(count - (shift+1)) % 8]);

			}else if(bitRead == 1){
				//runBuffer(buffer, count);
				unsigned short shift = 0;
				shift = readBit();
				shift = shift << 1;
				shift = shift | readBit();
				shift = shift << 1;
				shift = shift | readBit();
				char repeatingByte = buffer[(count - (shift+1)) % 8];
				writeByte(repeatingByte);
				addToBuffer(buffer, &count, repeatingByte);
				
				//read number of repeats
				shift = 0;
				shift = readBit();
				shift = shift << 1;
				shift = shift | readBit();
				shift = shift << 1;
				shift = shift | readBit();
				int i = 0;
				for(i = 0; i <= shift /*&& i < 7*/; i++){
					writeByte(repeatingByte);
					addToBuffer(buffer, &count, repeatingByte);
				}

			}else{
				return -1;
			}
		}
	//ttttt
	//oooooooooooooooooooooooooooooooooohhhhhhhhhhhhhhhhhhhh
		
	}
	return 0; //exit status. success=0, error=-1
}
