#include "r_client.h"
#include "pgm.h"

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

/* main - entry point for client applications.
 *
 * You are expected to develop a client program which will provide an 
 * environment into which we can plug an application and execute it 
 * using the remote versions of the supported calls. The client program 
 * therefore should expect a <hostname> <portnumber> pair as its first 
 * two arguments and attempt to connect the server. Once it connects, it
 * should call the user program which has a function called entry 
 * analogous to the main program in an ordinary C program. The entry 
 * routine should have the argv and argc arguments and return an integer 
 * value, just like the ordinary main. The client program should strip 
 * off the first two arguments, create a new argv array and call the entry procedure. 
 * Finally, when entry exits, the return value should be returned from the 
 * main procedure of the client.
 */
int socketfd;

int main(int argc, char *argv[]){
	if(argc < 3){
		printf("Not Enough Arguments: ./pgm1 <hostname> <port> <intput> <output>");
		exit(1);
	}
	
	char* remhost; unsigned short remport;
	int sock;

	struct sockaddr_in remote;
	struct hostent *h;

	remhost = argv[1];
	remport = atoi(argv[2]);

	sock = socket(AF_INET, SOCK_STREAM, 0);

	bzero((char*) &remote, sizeof(remote));
	remote.sin_family = (short) AF_INET;
	h = gethostbyname(remhost);
	bcopy((char*)h->h_addr_list[0],  (char*) &remote.sin_addr, h->h_length);
	remote.sin_port = htons(remport);
	printf("attempting to connect, Host: %s  Port: %d \n", h->h_name, sock);
	
	connect(sock, (struct sockaddr*) &remote, sizeof(remote));
	printf("connected \n");
	socketfd = sock;
	entry(argc, argv);
	printf("done\n");
	char end = EOF;
	write(socketfd, &end, 1); //write end of file to close
	return 0; // placeholder
}

/* r_open
 * remote open
 */
int r_open(const char *pathname, int flags, int mode){
	int L;
	char* msg;
	const char* p;
	int in_msg;
	int in_err;
	int u_l;


	p = pathname;
	while(*p) p++;
	u_l = (p)-pathname;
	L = 1 +					// opcode
		2 + u_l +			//2-byte length field
		sizeof(flags) + 	//int byte for flag
		sizeof(mode);		//int bytes for mode

	msg = malloc(L);
	L=0;
	msg[L++] = open_call;
	msg[L++] = (u_l >> 8) & 0xff;
	msg[L++] = (u_l) & 0xff;
	
	//put pathname
	int i = 0;
	for(i = 0; i < u_l; i++){
		msg[L++] = pathname[i]; 
	}
	printf("flag: %x \n", flags);
	printf("mode: %x \n", mode);
	//put the flags
	msg[L++] = (flags >> 24) & 0xff;
	msg[L++] = (flags >> 16) & 0xff;
	msg[L++] = (flags >>  8) & 0xff;
	msg[L++] = (flags      ) & 0xff;
	//put the mode
	msg[L++] = (mode >> 24) & 0xff;
	msg[L++] = (mode >> 16) & 0xff;
	msg[L++] = (mode >> 8) & 0xff;
	msg[L++] = (mode     ) & 0xff;
	
	
	printf("0x");
	for(int i = 0; i < 19; i++){
		printf("%X ", msg[i]);
	}
	printf("\n");
	printf("L: %d \n", L);
	
	
	
	write(socketfd, msg, L);
	char result[8] = {" "};
	read(socketfd, result, 8);
	//printf("read done\n");
	in_msg = (result[0] << 24) | (result[1] << 16) | (result[2] << 8) | result[3];
	in_err = (result[4] << 24) | (result[5] << 16) | (result[6] << 8) | result[7];
	printf("%d\n", in_msg);
	//errno = in_err;
	free(msg); //free msg

	errno = in_err;
	return in_msg;  
}

/* r_close
 * remote close
 */
int r_close(int fd){
	int L;
	char* msg;
	int in_msg;
	int in_err;

	L = 1 +	//opcode	
		sizeof(fd); 	//fd

	msg = malloc(L);	
	L = 0;


	msg[L++] = close_call;					//write opcode
	msg[L++] = (fd >> 24) & 0xff;			//write fd
	msg[L++] = (fd >> 16) & 0xff;
	msg[L++] = (fd >>  8) & 0xff;
	msg[L++] = (fd      ) & 0xff;

	write(socketfd, msg, L);

	char result[8] = {" "};
	read(socketfd, result, 8);
	//printf("read done\n");
	in_msg = (result[0] << 24) | (result[1] << 16) | (result[2] << 8) | result[3];
	in_err = (result[4] << 24) | (result[5] << 16) | (result[6] << 8) | result[7];
	//printf("%d\n", in_msg);
	//errno = in_err;
	free(msg); //free msg

	errno = in_err;
	return in_msg;

}

/* r_read
 * remote read
 */
int r_read(int fd, void *buf, int count){
	int L;
	char* msg;
	char* result;
	int in_msg;
	int in_err;

	L = 1 + 			//opcode
		sizeof(fd) +	//fd
		4;				//count
	
	msg = malloc(L);
	L= 0; 
	msg[L++] = read_call;
	
	msg[L++] = (fd >> 24) & 0xff;			//write fd
	msg[L++] = (fd >> 16) & 0xff;
	msg[L++] = (fd >>  8) & 0xff;
	msg[L++] = (fd      ) & 0xff;
											// count
	msg[L++] = (count >> 24) & 0xff;
	msg[L++] = (count >> 16) & 0xff;
	msg[L++] = (count >>  8) & 0xff;
	msg[L++] = (count      ) & 0xff;
	//printf("About to write to server\n");
	write(socketfd, msg, L);
	//printf("past write to server\n");

	result = malloc(count+8);
	read(socketfd, result, count+8);
	in_msg = (result[0] << 24) | (result[1] << 16) | (result[2] << 8) | result[3];	//grab message and error
	in_err = (result[4] << 24) | (result[5] << 16) | (result[6] << 8) | result[7];


	//put data into buf
	char* temp = (char*) buf;
	int i = 0;
	for(i = 0; i < in_msg; i++){
		temp[i] = result[i+8]; 
	}

	//printf("r_read Server return: ");
	//write(0, buf, in_msg);
	
	
	free(msg);		//free leaks
	free(result); 
	errno = in_err;
	return in_msg;

}

/* r_write
 * remote write
 */
int r_write(int fd, const void *buf, int count){
	int L;
	char* msg;
	int in_msg;
	int in_err;

	L = 1 +
		sizeof(fd) + 
		sizeof(count) + 
		count;

	msg = malloc(L);
	L = 0;
	msg[L++] = write_call;
	msg[L++] = (fd >> 24) & 0xff;			//write fd
	msg[L++] = (fd >> 16) & 0xff;
	msg[L++] = (fd >>  8) & 0xff;
	msg[L++] = (fd      ) & 0xff;

	msg[L++] = (count >> 24) & 0xff;
	msg[L++] = (count >> 16) & 0xff;
	msg[L++] = (count >>  8) & 0xff;
	msg[L++] = (count      ) & 0xff;

	int i = 0;
	const char* buffer = buf;
	for(i = 0; i < count; i++){
		msg[L++] = buffer[i];
	}

	write(socketfd, msg, L);
	//printf("waiting to read\n");

	char result[8] = {" "};
	read(socketfd, result, 8);
	//printf("read done\n");
	in_msg = (result[0] << 24) | (result[1] << 16) | (result[2] << 8) | result[3];
	in_err = (result[4] << 24) | (result[5] << 16) | (result[6] << 8) | result[7];
	//printf("%d\n", in_msg);

	free(msg);
	errno = in_err;
	return in_msg; // placeholder
}

/* r_lseek
 * remote seek
 */
int r_lseek(int fd, int offset, int whence){
	int L;
	char* msg;
	int in_msg;
	int in_err;

	L = 1 +
		sizeof(fd) + 
		sizeof(offset) + 
		sizeof(whence);
	
	msg = malloc(L);
	L = 0;
	msg[L++] = seek_call;
	msg[L++] = (fd >> 24) & 0xff;			//write fd
	msg[L++] = (fd >> 16) & 0xff;
	msg[L++] = (fd >>  8) & 0xff;
	msg[L++] = (fd      ) & 0xff;

	msg[L++] = (offset >> 24) & 0xff;			//write offset
	msg[L++] = (offset >> 16) & 0xff;
	msg[L++] = (offset >>  8) & 0xff;
	msg[L++] = (offset      ) & 0xff;

	msg[L++] = (whence >> 24) & 0xff;			//write whence
	msg[L++] = (whence >> 16) & 0xff;
	msg[L++] = (whence >>  8) & 0xff;
	msg[L++] = (whence      ) & 0xff;

	write(socketfd, msg, L);
	
	char result[8] = " ";
	read(socketfd, result, 8);
	//printf("read done\n");
	in_msg = (result[0] << 24) | (result[1] << 16) | (result[2] << 8) | result[3];
	in_err = (result[4] << 24) | (result[5] << 16) | (result[6] << 8) | result[7];

	free(msg);
	errno = in_err;
	return in_msg;

}

/* r_pipe
 * remote pipe
 */
int r_pipe(int pipefd[2]){
	int in_msg;
	int in_err;
	int pipefd1;
	int pipefd2;
	char result[16] = " ";


	char opcode = pipe_call;
	write(socketfd, &opcode, 1);


	read(socketfd, result, 16);
	//printf("read done\n");
	in_msg = (result[0] << 24) | (result[1] << 16) | (result[2] << 8) | result[3];
	in_err = (result[4] << 24) | (result[5] << 16) | (result[6] << 8) | result[7];
	pipefd1 = (result[8] << 24) | (result[9] << 16) | (result[10] << 8) | result[11];
	pipefd2 = (result[12] << 24) | (result[13] << 16) | (result[14] << 8) | result[15];
	//set pipefd
	pipefd[0] = pipefd1;
	pipefd[1] = pipefd2;
	printf("pipefd1: %d, pipefd2: %d \n", pipefd1, pipefd2);
	errno = in_err;
	return in_msg; // placeholder
}

/* r_dup2
 * remote dup2
 */
int r_dup2(int oldfd, int newfd){
	int L = 0;
	int in_msg = 0;
	int in_err = 0;
	char result[8] = " ";

	L = 1 +
		sizeof(oldfd) + 
		sizeof(newfd);

	char* msg = malloc(L);

	msg[L++] = dup2_call;
	msg[L++] = (oldfd >> 24) & 0xff;			//write fd
	msg[L++] = (oldfd >> 16) & 0xff;
	msg[L++] = (oldfd >>  8) & 0xff;
	msg[L++] = (oldfd      ) & 0xff;

	msg[L++] = (newfd >> 24) & 0xff;			//write fd
	msg[L++] = (newfd >> 16) & 0xff;
	msg[L++] = (newfd >>  8) & 0xff;
	msg[L++] = (newfd      ) & 0xff;

	write(socketfd, msg, L);

	read(socketfd, result, 8);
	in_msg = (result[0] << 24) | (result[1] << 16) | (result[2] << 8) | result[3];
	in_err = (result[4] << 24) | (result[5] << 16) | (result[6] << 8) | result[7];

	errno = in_err;
	return in_msg; // placeholder
}
