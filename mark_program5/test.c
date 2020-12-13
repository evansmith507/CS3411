#include <sys/socket.h>
#include <signal.h>
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
 #include <sys/select.h>
 #include <stdbool.h>

int main(int argc, char *argv[]){
    fd_set rds;
    FD_ZERO(&rds);
	FD_SET(0, &rds);
    int r = select(3, &rds, 0, 0, NULL);

    
    char thing;
    if(FD_ISSET(0, &rds))
        read(0, &thing, 1);
    if(thing == '5'){
       //write(STDERR_FILENO, "leaving\n", 8);
       perror("this is error");
    }
    printf("leaving\n");
    //write(STDOUT_FILENO, "leaving\n", 8);
    
    //perror("no you are");
    
    return 0;
}