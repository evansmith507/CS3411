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

/* entry
 * This is essentially the "main" for any user program linked with
 * r_client. Main in r_client will establish the connection with the
 * server then call entry. From entry, we can implement the desired 
 * user program which may call any of the r_ RPC functions.
 *
 * pgm1 remotely opens an output file, locally opens an input file, 
 * copies the input file to the output file and closes both files.
 */
int entry(int argc, char* argv[]){
	//you will call the r_ functions here.
	if(argc < 5){
		printf("Not Enough Arguments: ./pgm1 <hostname> <port> <intput> <output>");
		return 0;
	}
	int local = open(argv[3],  O_RDWR, 0);
	if(local == -1){ 
		printf("OPEN FAIL\n");
	}

	int remote = r_open(argv[4], O_RDWR | O_APPEND, S_IRWXU | S_IRWXG | S_IRWXO);
	if(remote == -1){ 
		printf("OPEN FAIL\n");
	}
	char buf = 0;
	int readReturn = 0;
	while(1){
		readReturn = read(local, &buf, 1);
		if(readReturn == -1){ printf("READ ERROR \n"); break; }
		if(readReturn == 0){ break;	}
		r_write(remote, &buf, 1);
	}

	close(local);
	int closeReturn = r_close(remote);
	if(closeReturn == -1){
		printf("closeError\n");
	}

	
	return 0; //placeholder
}
