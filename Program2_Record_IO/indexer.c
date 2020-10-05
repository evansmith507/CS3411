
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

	return 0;
}
