/* Add any includes needed*/
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <signal.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <strings.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

/*Opcodes for RPC calls*/
#define open_call   1 
#define close_call  2
#define read_call   3
#define write_call  4
#define seek_call   5
#define pipe_call   6
#define dup2_call   7



/* In this project, we will develop a mini Remote Procedure Call (RPC) 
 * based system consisting of a em server and a client. Using the remote 
 * procedures supplied by the server our client program will be able to 
 * open files and perform computations on the server. 
 *
 * The server should open a socket and listen on an available port. The 
 * server program upon starting should print the port number it is using 
 * on the standard output 
 * (Print only the port number with no additional formatting. You may use printf \%d). 
 * This port number is then manually passed as a command line argument to 
 * the client to establish the connection. In order to implement the RPC, 
 * the server and the client should communicate through a TCP socket. It 
 * is allowed to fork a child for each open connection and delagate the 
 * handling to the child.
 */
void c_open(int conn){
	int error = 0;
	int length = 0;
	int flags = 0;
	int mode = 0;
	char tempbuff[4] = " ";

	if(read(conn, tempbuff, 2) == -1) { perror("EXTRACTING ERROR\n"); }
	length = (tempbuff[0] << 8) | (tempbuff[1]);
	//printf("length: %d \n", length);
	
	char* pathname = calloc(1, length);
	
	if(read(conn, pathname, length) == -1){ perror("EXTRACTING ERROR\n"); };
	//printf("pathname: %s \n", pathname);
	
	

	if(read(conn, tempbuff, 4) == -1){ perror("EXTRACTING ERROR\n"); }
	flags = (tempbuff[0] << 24) | (tempbuff[1] << 16) | (tempbuff[2] << 8) | tempbuff[3];
	//printf("flags: %X \n", flags);
	
	
	if(read(conn, tempbuff, 4) == -1){ perror("EXTRACTING ERROR\n"); }
	mode = (tempbuff[0] << 24) | (tempbuff[1] << 16) | (tempbuff[2] << 8) | tempbuff[3];
	
	/*
	printf("Real:   0x");
	for(int i = 0; i < length; i++){
		printf("%X ", pathname[i]);
	}
	printf("\n");

	char* sample = "Test.txt";
	printf("Sample: 0x");
	for(int i = 0; i < 9; i++){
		printf("%X ", sample[i]);
	}
	printf("\n");
	*/


	//printf("Open attempt: %s %x %x \n", pathname, flags, mode);
	int fd = open(pathname, flags, mode);
	//printf("real open %d \n", fd);
	
	error = errno;
	//int test = open("Test.txt", 2, 0);
	//printf("Test Open: %d \n", test);
	
	char result[8] = {" "};
	int L = 0;
	//pharse fd return
	result[L++] =	(fd >> 24) & 0xff;
	result[L++] =	(fd >> 16) & 0xff;
	result[L++] =	(fd >>  8) & 0xff;
	result[L++] =	(fd      ) & 0xff;
	//pharse error
	
	result[L++] =	(error >> 24) & 0xff;
	result[L++] =	(error >> 16) & 0xff;
	result[L++] =	(error >>  8) & 0xff;
	result[L++] =	(error      ) & 0xff;

	write(conn, result, 8);
	free(pathname); //free malloced item

}

void c_close(int conn){
	int fd = 0;
	int error = 0;
	char tempbuff[4] = " ";
	if(read(conn, tempbuff, 4) == -1) { perror("EXTRACTING ERROR\n"); }
	fd = (tempbuff[0] << 24) | (tempbuff[1] << 16) | (tempbuff[2] << 8) | tempbuff[3];

	int closeResult = close(fd);

	char result[8] = " ";
	int L = 0;
	result[L++]  = (closeResult >> 24) & 0xff;
	result[L++]  = (closeResult >> 16) & 0xff;
	result[L++]  = (closeResult >>  8) & 0xff;
	result[L++]  = (closeResult      ) & 0xff;

	error = errno;
	result[L++]  = (error >> 24) & 0xff;
	result[L++]  = (error >> 16) & 0xff;
	result[L++]  = (error >>  8) & 0xff;
	result[L++]  = (error      ) & 0xff;

	write(conn, result, 8);

}

void c_read(int conn){
	int fd = 0;
	int error = 0;
	int count = 0;
	char tempbuff[4] = " ";
	//get fd
	if(read(conn, tempbuff, 4) == -1) {perror("EXTRACTING ERROR\n"); }
	fd = (tempbuff[0] << 24) | (tempbuff[1] << 16) | (tempbuff[2] << 8) | tempbuff[3];
	//get count
	if(read(conn, tempbuff, 4) == -1) {perror("EXTRACTING ERROR\n"); }
	count = (tempbuff[0] << 24) | (tempbuff[1] << 16) | (tempbuff[2] << 8) | tempbuff[3];

	char* readData = calloc(1, count);
	int readReturn = read(fd, readData, count);
	if(readReturn == -1){
		perror("READING ERROR\n");
		//error time
	}
	char* result = malloc(8 + readReturn);

	int L = 0;
	//set up readreturn for 
	result[L++] = (readReturn  >> 24) & 0xff;
	result[L++] = (readReturn  >> 16) & 0xff;
	result[L++] = (readReturn  >>  8) & 0xff;
	result[L++] = (readReturn       ) & 0xff;
	//printf("Read Return: %d\n", readReturn);
	//pharse error
	error = errno;
	result[L++] =	(error >> 24) & 0xff;
	result[L++] =	(error >> 16) & 0xff;
	result[L++] =	(error >>  8) & 0xff;
	result[L++] =	(error      ) & 0xff;
	//add read data
	int i = 0;
	for(i = 0; i < readReturn; i++){
		result[L++] = readData[i];
	}
	//printf("Read from fd: %s \n", readData);
	write(conn, result, readReturn + 8);

	free(readData);
	free(result);

}

void c_write(int conn){
	int fd = 0;
	int error = 0;
	int count = 0;
	char* writeData = malloc(count);
	char tempbuff[4] = " ";
	//read fd
	if(read(conn, tempbuff, 4) == -1) { perror("EXTRACTING ERROR\n"); }
	fd = (tempbuff[0] << 24) | (tempbuff[1] << 16) | (tempbuff[2] << 8) | tempbuff[3];
	//read count
	if(read(conn, tempbuff, 4) == -1) { perror("EXTRACTING ERROR\n"); }
	count = (tempbuff[0] << 24) | (tempbuff[1] << 16) | (tempbuff[2] << 8) | tempbuff[3];
	//read data
	if(read(conn, writeData, count) == -1) { perror("EXTRACTING ERROR\n"); }

	//printf("write info: %x %x %s \n", fd, count, writeData);
	int writeReturn = write(fd, writeData, count);

	char result[8] = {" "};
	int L = 0;
	//pharse fd return
	result[L++] =  (writeReturn >> 24) & 0xff;
	result[L++] =  (writeReturn >> 16) & 0xff;
	result[L++] =  (writeReturn >>  8) & 0xff;
	result[L++] =  (writeReturn      ) & 0xff;
	//pharse error
	error = errno;
	result[L++] =  (error >> 24) & 0xff;
	result[L++] =  (error >> 16) & 0xff;
	result[L++] =  (error >>  8) & 0xff;
	result[L++] =  (error      ) & 0xff;


	write(conn, result, 8); //return data 
	
	free(writeData);
}

void c_lseek(int conn){
	int fd = 0;
	int offset = 0;
	int whence = 0;
	int error = 0;
	char tempbuff[4] = " ";
	//get fd
	if(read(conn, tempbuff, 4) == -1) { perror("EXTRACTING ERROR\n"); }
	fd = (tempbuff[0] << 24) | (tempbuff[1] << 16) | (tempbuff[2] << 8) | tempbuff[3];
	//printf("Lseek fd: %d \n", fd);
	//get offset
	if(read(conn, tempbuff, 4) == -1) { perror("EXTRACTING ERROR\n"); }
	offset = (tempbuff[0] << 24) | (tempbuff[1] << 16) | (tempbuff[2] << 8) | tempbuff[3];
	//printf("Lseek offset: %d \n", offset);
	//get whence
	if(read(conn, tempbuff, 4) == -1) { perror("EXTRACTING ERROR\n"); }
	whence = (tempbuff[0] << 24) | (tempbuff[1] << 16) | (tempbuff[2] << 8) | tempbuff[3];
	//printf("Lseek whence: %X \n", whence);
	//printf("seek info: %x %x %x \n", fd, offset, whence);

	int seekResponse = lseek(fd, offset, whence);
	error = errno;
	char result[8] = " ";
	//write seek return
	int L = 0;
	result[L++] = (seekResponse >> 24) & 0xff;
	result[L++] = (seekResponse >> 16) & 0xff;
	result[L++] = (seekResponse >>  8) & 0xff;
	result[L++] = (seekResponse      ) & 0xff;
	//write error
	result[L++] = (error >> 24) & 0xff;
	result[L++] = (error >> 16) & 0xff;
	result[L++] = (error >>  8) & 0xff;
	result[L++] = (error      ) & 0xff;

	write(conn, result, 8);

}

void c_pipe(int conn){
	int pipefd[2];
	int error = 0;
	int pipeReturn = pipe(pipefd);
	//printf("pipefd[0]: %d, pipefd[1]: %d \n", pipefd[0], pipefd[1]);
	error = errno;
	char result[16] = " ";
	int L = 0;
	//store pipereturn
	result[L++] = (pipeReturn >> 24) & 0xff;
	result[L++] = (pipeReturn >> 16) & 0xff;
	result[L++] = (pipeReturn >>  8) & 0xff;
	result[L++] = (pipeReturn      ) & 0xff;
	//store error
	result[L++] = (error >> 24) & 0xff;
	result[L++] = (error >> 16) & 0xff;
	result[L++] = (error >>  8) & 0xff;
	result[L++] = (error      ) & 0xff;
	//store first pipefd
	result[L++] = (pipefd[0] >> 24) & 0xff;
	result[L++] = (pipefd[0] >> 16) & 0xff;
	result[L++] = (pipefd[0] >>  8) & 0xff;
	result[L++] = (pipefd[0]      ) & 0xff;
	//store second pipefd
	result[L++] = (pipefd[1] >> 24) & 0xff;
	result[L++] = (pipefd[1] >> 16) & 0xff;
	result[L++] = (pipefd[1] >>  8) & 0xff;
	result[L++] = (pipefd[1]      ) & 0xff;

	write(conn, result, 16);

}

void c_dup2(int conn){
	int oldfd = 0;
	int newfd = 0;
	int error = 0;
	char tempbuff[4] = " ";
	
	if(read(conn, tempbuff, 4) == -1) { perror("EXTRACTING ERROR\n"); }
	oldfd = (tempbuff[0] << 24) | (tempbuff[1] << 16) | (tempbuff[2] << 8) | tempbuff[3];

	if(read(conn, tempbuff, 4) == -1) { perror("EXTRACTING ERROR\n"); }
	newfd = (tempbuff[0] << 24) | (tempbuff[1] << 16) | (tempbuff[2] << 8) | tempbuff[3];
	
	int dupReturn = dup2(oldfd, newfd);
	//printf("dup2: old %d new %d, dup2 return: %d\n ",oldfd, newfd, dupReturn);
	error = errno;
	char result[8] = " ";
	int L = 0;
	//write dup return
	result[L++] = (dupReturn >> 24) & 0xff;
	result[L++] = (dupReturn >> 16) & 0xff;
	result[L++] = (dupReturn >>  8) & 0xff;
	result[L++] = (dupReturn      ) & 0xff;
	//write error
	result[L++] = (error >> 24) & 0xff;
	result[L++] = (error >> 16) & 0xff;
	result[L++] = (error >>  8) & 0xff;
	result[L++] = (error      ) & 0xff;


	write(conn, result, 8);

}




/* main - server implementation
 */
int main(int argc, char *argv[]){
	int listener, conn, length; 
	struct sockaddr_in s1, s2;
	//set up socket
	listener = socket(AF_INET, SOCK_STREAM, 0);
	bzero((char*) &s1, sizeof(s1));
	s1.sin_family = (short) AF_INET;
	s1.sin_addr.s_addr = htonl(INADDR_ANY);
	s1.sin_port = htonl(0);

	//bind socket
	bind(listener, (struct sockaddr*) &s1, sizeof(s1));
	length = sizeof(s1);
	getsockname(listener, (struct sockaddr*) &s1, (socklen_t*) &length);

	//print port number
	printf("%d\n", ntohs(s1.sin_port));
	//set up listen
	
	listen(listener, 5);
	//printf("listening \n");
	length = sizeof(s2);


	//pid_t child_pid = -1;
	//int status = 0;

	while(1){
		//printf("waiting for accept \n");
		conn = accept(listener, (struct sockaddr*) &s2, (socklen_t*) &length);
		//printf("Connection Accepted: %d \n", conn);
		
		if(fork() == 0){
			char type = 0;
			int readReturn = 0;
			readReturn = read(conn, &type, 1);
			while(type != EOF){

			//determine instruciton type
				switch(type){
					case open_call:
						//sleep(5);
						//printf("Open Instruc\n");
						c_open(conn);
						break;
					case close_call:
						//printf("Close Instruc\n");
						c_close(conn);
						break;
					case read_call:
						//printf("Read Instruc\n");
						c_read(conn);
						break;
					case write_call:
						//printf("write Instruc\n");
						c_write(conn);
						break;
					case seek_call:
						c_lseek(conn);
						break;
					case pipe_call:
						c_pipe(conn);
						break;
					case dup2_call:
						c_dup2(conn);
						break;
				}
				//read next instuction 
				readReturn = read(conn, &type, 1);
				if(readReturn == -1){ perror("INSTURCTION READ ERROR\n"); }
				//printf("end instruction\n\n");
			}
			exit(1);
			//return 0;
		}
		//printf("waiting \n");
		//wait(&status);


		//sleep(1);
	}
	
	//printf("yeet\n");

	return 0; // placeholder
}


