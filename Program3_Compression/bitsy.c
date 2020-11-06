#include "bitsy.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stddef.h>
/* Add any global variables or structures you need.*/

/* readByte
 * Abstraction to read a byte.
 * Relys on readBit.
 */
unsigned short readByte(){ 
	/* This function should not call read() directly.
	 * If we are buffering data in readBit, we dont want to skip over
	 * that data by calling read again. Instead, call readBit and 
	 * construct a byte one bit at a type. This also allows us to read
	 * 8 bits that are not necessarily byte alligned. */

	//Do NOT call read here. Instead rely on readBit.
	char readByte = 0;
	for(int i = 0; i < 8; i++){
		unsigned short result = readBit();
		//printf("bit read at i=%d : %d \n", i, result);
		if(result > 255){
			//printf("returning on no read \n");
			return 258;
			//read end of file
		}else{
			readByte = result | readByte; //set bit
			if(i != 7){
			readByte = readByte << 1;	   //shift bit
			}
		}
	}
	//printf("\n");
	//I suggest returning a unique value greater than the max byte value
	//to communicate end of file. We need this since 0 is a valid byte value
	//so we need another way to communicate eof. These functions are typed
	//as short to allow us to return a value greater than the max byte value.
	//printf("readByte %x \n", readByte);
	return readByte; //placeholder
}

/* readBit
 * Abstraction to read a bit.
 * */
char byteBuff = 0;
char avalibleBits = 0;
unsigned short readBit(){
	/* This function is responsible for reading the next bit on the
	 * input stream from stdin (fd = 0). To accomplish this, keep a 
	 * byte sized buffer. Each time read bit is called, use bitwise
	 * operations to extract the next bit and return it to the caller.
	 * Once the entire buffered byte has been read the next byte from 
	 * the file. Once eof is reached, return a unique value > 255
	 */	
	if(avalibleBits == 0){
		int readReturn = read(0, &byteBuff, 1);
		//printf("read: %c, readReturn: %d \n", byteBuff, readReturn);
		if(readReturn == -1){
			//error
		}
		if(readReturn == 0){ //if nothing is read return high number
			return 258; //number above 255 
		}
		avalibleBits = 8;
	}
	
	if(avalibleBits > 0 && avalibleBits < 9){
		//extract bit
		unsigned short resultBit = 0x80 & byteBuff; //extract last bit
		byteBuff = byteBuff << 1; //shift bit to next bit
		avalibleBits--; //decrease avaliable bits
		return resultBit >> 7;
	}else{
		//ERROR TIME
	}
	//You will need to call read here

	//I suggest returning a unique value greater than the max byte value
	//to communicate end of file. We need this since 0 is a valid byte value
	//so we need another way to communicate eof. These functions are typed
	//as short to allow us to return a value greater than the max byte value.
	return 256; //placeholder
}

/* writeByte
 * Abstraction to write a byte.
 */
void writeByte(unsigned char byt){
	/* Use writeBit to write each bit of byt one at a time. Using writeBit
	 * abstracts away byte boundaries in the output.*/
	char byteUsed = byt;
	//call 8 bit writes 
	for(int i = 0; i < 8; i++){
		unsigned char bit = (0x80 & byteUsed) >> 7;
		byteUsed = byteUsed << 1;	//shift to next bit
		writeBit(bit); 	//send bit through
	}
}

/* writeBit
 * Abstraction to write a single bit.
 */
char writeBuffer;
int bitsNeeded = 8;
void writeBit(unsigned char bit){
	/* Keep a byte sized buffer. Each time this function is called, insert the 
	 * new bit into the buffer. Once 8 bits are buffered, write the full byte
	 * to stdout (fd=1).
	 */
	writeBuffer = writeBuffer << 1;	//shift bit to prepair for next bit
	writeBuffer = writeBuffer | bit;	//add bit to byte
	bitsNeeded--;	//subtract bits needed
	if(bitsNeeded < 1){
		int writeReturn = write(1, &writeBuffer, 1);
		bitsNeeded = 8;
		if(writeReturn == -1){
			//error
		}
	}
	//You will need to call write here eventually.
}

/* flushWriteBuffer
 * Helper to write out remaining contents of a buffer after padding empty bits
 * with 1s.
 */
void flushWriteBuffer(){
	if(bitsNeeded == 8){ //if nothing to write skip
		//printf("nothing to print \n");
		return;
	}else{
		//flush byte
		int bitsNeededTrack = bitsNeeded;
		while(bitsNeededTrack > 0){
			//printf("flushing\n");
			writeBit(1);
			bitsNeededTrack--;
		}
	
	}
	/* This will be utilized when finishing your encoding. It may be that some bits
	 * are still buffered and have not been written to stdout. Call this function 
	 * which should do the following: Determine if any buffered bits have yet to be 
	 * written. Pad remaining bits in the byte with 1s. Write byte to stdout
	 */

}
