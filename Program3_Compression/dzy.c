#include "bitsy.h" //This header includes prototypes for the proposed bit abstractions
/*Include any additional headers you require*/

/*You may use any global variables/structures that you like*/

void addToBuffer(char* buffer, unsigned int count, char entry){
	buffer[count % 8] = entry; 
	count++;
}


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
	char buffer[8];
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
			byteRead = readByte();
			if(byteRead == 258){
				//end of file
				break;
			}
			writeByte(byteRead);
			addToBuffer(buffer, count, byteRead);
		}else{
			bitRead = readBit();
			if(bitRead == 258){
				//end of file //bad time
				break;
			}

			if(bitRead == 0){ //
				//grab number to shift by
				unsigned short shift = 0;
				shift = readBit();
				shift = shift << 1;
				shift = shift | readBit();
				shift = shift << 1;
				shift = shift | readBit();
				//if it causes issues check shift 
				writeByte(buffer[count - shift]);
				addToBuffer(buffer, count, buffer[count - shift]);

			}else if(bitRead == 1){

				unsigned short shift = 0;
				shift = readBit();
				shift = shift << 1;
				shift = shift | readBit();
				shift = shift << 1;
				shift = shift | readBit();
				char repeatingByte = buffer[count - shift];
				writeByte(repeatingByte);
				addToBuffer(buffer, count, repeatingByte);
				
				//read number of repeats
				shift = 0;
				shift = readBit();
				shift = shift << 1;
				shift = shift | readBit();
				shift = shift << 1;
				shift = shift | readBit();
				for(int i = 0; i < shift; i++){
					writeByte(repeatingByte);
					addToBuffer(buffer, count, repeatingByte);
				}

			}else{
				return -1;
			}
		}

		
	}
	return 0; //exit status. success=0, error=-1
}
