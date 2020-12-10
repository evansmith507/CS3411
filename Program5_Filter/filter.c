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
 #include <sys/select.h>
//Add any includes you require.

/* main - implementation of filter
 * In this project, you will be developing a filter program that forks
 * a child and then executes a program passed as an argument and intercepts 
 * all output and input from that program. The syntax for the execution of 
 * the filter program is given by:
 * 
 * 	filter <program name> <arguments to program>
 *
 * The following commands must be implemented:
 * 	//           : Pass the character '/' as input to the program.
 * 	/i           : Go to input only mode.
 * 	/o           : Go to input/output mode.
 * 	/c           : Go to command mode.
 * 	/m <integer> : Set the max-text, maximum number of lines to be displayed.
 * 	/k <integer> : Send the child process indicated signal.
 *
 * See the spec for details.
 * 
 * After receiving each command, the program should output a prompt indicating 
 * the current mode and if there is more data to be displayed. 
 * The prompt syntax is :
 *
 * 	<pid> m <more> #
 *
 * where <pid> is the process id of the child process, m is the current mode (
 * i (input only), c (command), o(input/output)), optional <more> is the text "more" 
 * if there is data available to be displayed, and lastly the pound character.
 */
int main(int argc, char *argv[]){
	//Hint: use select() to determine when reads may execute without blocking.
	if(argc < 2){
		printf("More Arguments Nedded, ./filter <program> <arguments> \n");
		return 0;
	}

	
	
	//setup pipes 
	int inputFd[2];
	int outputFd[2];
	int errorFd[2];

	if(pipe(inputFd)  == -1){ /*pipe error*/ } 
	if(pipe(outputFd)  == -1){ /*pipe error*/ } 
	if(pipe(errorFd)  == -1){ /*pipe error*/ } 

	int childIn = inputFd[0];
	int childOut = outputFd[1];
	int parentRead = outputFd[0];
	int parentWrite = inputFd[1];
	
	fd_set set;
	FD_ZERO(&set);
	FD_SET(childIn, &set);


	pid_t pid = fork();
	if(pid == 0){
		//child process
		char* programName = argv[1];
		//printf("%s \n", programName);
		dup2(childIn, 0);
		dup2(childOut, 1);
		dup2(errorFd[0], 2);
		
		execvp(programName, &argv[1]); // &argv[1] is fun 
		printf("failed to open\n");
		
	}else{
		//block all input and output (put into command mode)
		
		int lineNum = 20;
		char buff[8];
		int readReturn = 0;
		struct timeval reset = {0,0};
		reset.tv_sec = 0;
		reset.tv_usec = 1;
		while(1){
			//check if command

			//if command which one then set
			


			readReturn = read(0, buff, 8);
			readReturn = write(parentWrite, buff, readReturn);
			readReturn = read(parentRead, buff, readReturn);
			write(0, buff, readReturn);
			select(childIn+1,NULL, &set, NULL, NULL);

			
		}
		waitpid(pid, 0,0);
	}

	//parrent process

	
	
	//for the process
	
	//pid_t pid = fork();
	//if(pid == 0){ //child handles input program
		//set up pipes 
	//	int input[2];
	//	int output[2];
	//	int error[2];
	//}
	//parent handles filter
	

	return 0; //placeholder
}
