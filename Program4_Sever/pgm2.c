#include "r_client.h"
//#include "pgm.h"



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
/* Add any includes needed*/

/* entry
 * This is essentially the "main" for any user program linked with
 * r_client. Main in r_client will establish the connection with the
 * server then call entry. From entry, we can implement the desired 
 * user program which may call any of the r_ RPC functions.
 *
 * pgm2 should open a local file as output and a remote file as input. 
 * It should seek the remote file to position 10 and copy the rest 
 * to the local file.
 */
int entry(int argc, char* argv[]){
	
	if(argc < 5){
		printf("Not Enough Arguments: ./pgm1 <hostname> <port> <intput> <output>");
		return 0;
	}
		
	/*	
	//test pipe
	int pipefd[2];
	int pipeReturn = r_pipe(pipefd);
	if(pipeReturn == -1){
		printf(" pipe fail \n");
	}
	printf("pipefd[0]: %d, pipefd[1]: %d\n", pipefd[0], pipefd[1]);
	r_write(pipefd[1], "HAHAPIPE", 8);
	char buffer[20] = " ";
	r_read(pipefd[0], buffer, 4);

	printf("Pipe return: %s \n", buffer);
	*/

	//Test dup2
	/*
	int remote = r_open(argv[3], O_RDWR, 0);
	int test = r_dup2(remote, 1);

	printf("TEST: %d \n", test);
	*/


	//submission 
	
	int local = open(argv[4], O_RDWR | O_CREAT | O_APPEND, S_IRWXU | S_IRWXG | S_IRWXO);
	
	int remote = r_open(argv[3], O_RDWR, 0);
	if(remote == -1){ 
		//printf("OPEN FAIL\n");
		return 0;
	}
	r_lseek(remote, 10, SEEK_CUR);

	char buf = 0;
	int readReturn = 0;
	while(1){
		readReturn = r_read(remote, &buf, 1);
		if(readReturn == -1){ 
			//printf("READ ERROR \n");
			break;
		}
		if(readReturn == 0){ break;	}
		write(local, &buf, 1);
	}
	
	return 0; //placeholder
}