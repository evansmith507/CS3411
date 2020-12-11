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


#define INPUTMODE 'i'
#define OUTMODE 'o'
#define COMMANDMODE 'c'


//mode and maxline output
pid_t childPid;
char mode;
int maxLine;
//global fd's
int childIn; //for child process where inputs are received
int childOut; //for child process where outputs are sent
int parentRead; //for parent where child outputs are read from
int parentWrite; //for parent where it writes to child
int largestFD; //largest fd for select
int childErrorOut; //


//Handle childoutputs
//NEED TO IMPLEMENT MAX LINE
int HandleChildOut(){
	char buff[1];
	int l;
	if(mode == OUTMODE){
		
		int line = 0;
		while(line < maxLine){
			l = read(parentRead, buff, 1);
			//printf("l: %d, buf[0]: %X \n", l, buff[0]);
			//printf("read: %c\n", buff[0]);
			if(l == -1){ break; }
			if(buff[0] == '\n'){
				line++;
			}
			
			//printf("before Write\n");
			write(1, buff, l);
			//printf("after Write\n");
		}
		//return 1 is ther is a need to say more
		//printf("line %d \n", line);
		//printf("stuck here\n");
		if(line >= maxLine){
			return 1; //more lines to read
		}else{
			return 0; //done reading
		}
	}
	return 0;
}


//TODO:: NEED TO FINISH HANDLE COMMAND
//TODO:: MAKE IT COMMANDS WITH RANDOM SHIT AT THE END DONT WORK (ie: /i jk  or /o /i)
void HandleCommand(char* buff){
	char command = buff[1];
	
	switch (command){
	case 'i':
		mode = INPUTMODE;
		printf("Change to input mode %d \n", mode);
		break;
	case 'o': 
		mode = OUTMODE;
		//int more = HandleChildOut();
		//printf("here \n");
		//if(more == 1){
		//	printf("%d %c more #\n", childPid, mode);
		//}else{
		//	printf("%d %c #\n", childPid, mode);
		//}
		
		break;
	case 'c':
		mode = COMMANDMODE;
		printf("Change to command mode %d \n", mode);
		break;
	case 'm':
		maxLine = atoi(&buff[3]);

		break;
	case 'k':
		//signal send
		break;
	case '/':
		write(parentWrite, "/", 1);
		break;
	default:
		printf("/%c is an incorrect command \n", buff[1]);
	}
}

//INSERT REST OF MODES WHEN INPUT IS GIVEN
void HandleStdIn(){
	char buff[128] = {" "};
	int l = 0;
	
	l = read(0, buff, sizeof(buff));
	//printf("read from buf: %s childin Fd: %d \n", buff, childIn);
	//fflush(stdout);
	printf("read length: %d\n",l);
	//write(parentWrite, buff, l);
	switch (mode){
	case COMMANDMODE:
		if(buff[0] == '/'){ 
			HandleCommand(buff);
		}
		break;
	case INPUTMODE:
		if(buff[0] == '/' && buff[1] != '/'){ 
			HandleCommand(buff);
		}else{
			write(parentWrite, buff, l);
		}
		break;
	case OUTMODE:
		if(buff[0] == '/' && buff[1] != '/'){ 
			HandleCommand(buff);
		}else{
			write(parentWrite, buff, l);
		}

		break;
	default:
		break;
	}



}


int main(int argc, char *argv[]){
	//Hint: use select() to determine when reads may execute without blocking.
	if(argc < 2){
		printf("More Arguments Nedded, ./filter <program> <arguments> \n");
		return 0;
	}

	mode = COMMANDMODE;
	maxLine = 2;
	//setup pipes 
	int inputFd[2];
	int outputFd[2];
	int errorFd[2];

	if(pipe(inputFd)  == -1){ /*pipe error*/ } 
	if(pipe(outputFd)  == -1){ /*pipe error*/ } 
	if(pipe(errorFd)  == -1){ /*pipe error*/ } 

	childIn = inputFd[0];
	childOut = outputFd[1];
	parentRead = outputFd[0];
	//set this descriptor to nonblock to allow pipes to stop reading
	if(fcntl(parentRead, F_SETFL, O_NONBLOCK) == -1){
		printf("nonblock fail\n");
		return 0;
	}
	parentWrite = inputFd[1];
	childErrorOut = errorFd[1];
	largestFD = errorFd[1];

	//fork program
	pid_t pid = fork();
	if(pid == 0){
		//child process
		char* programName = argv[1];
		//change pipes over
		dup2(childIn, 0);
		dup2(childOut, 1);
		dup2(errorFd[0], 2);
		//get child PID
		childPid = getppid();
		
		execvp(programName, &argv[1]); // &argv[1] is fun 
		printf("failed to open\n");
		
	}else{
		//set up varaibles
		int r;
		//int readReturn = 0;
		fd_set rds;
		printf("%d\n",childPid);
		//fd_set wts;

		while(1){

			//prepare descriptors
			FD_ZERO(&rds);
			FD_SET(0, &rds);
			FD_SET(parentRead, &rds);
			FD_SET(childErrorOut, &rds);
			
			r = select(largestFD+1, &rds, 0, 0, NULL);
			//printf("Select Unblocked \n");
			fflush(stdout);
			if(FD_ISSET(childErrorOut, &rds)){
				printf("child error ready \n");
				//implement 
			}

			if(FD_ISSET(0,&rds)){
				printf("stdin is set \n");
				HandleStdIn();
			}
			
			if( FD_ISSET(parentRead, &rds) && mode == OUTMODE ){
				//int more = HandleChildOut();
				//printf("done\n");
				int lines = 0;
				int readReturn = 0;
				char buff[2];
				
				
				
				while(1){
					readReturn = 0;
					//printf("before read \n");
					readReturn = read(parentRead, buff, 1);					
					//printf("ReadReturn: %d\n", readReturn);
					if(readReturn == 0 || readReturn == -1){ break; }
					//printf("buff[0]: %x \n", buff[0]);
					if(buff[0] == '\n'){ lines++; }
					write(1, buff, readReturn);
					if(lines >= maxLine){
						break;
					}
					
				}
				printf("done\n");
				if(lines >= maxLine){
					mode = COMMANDMODE;
				}
				
				
			}


			//readReturn = read(0, buff, 8);
			//readReturn = write(parentWrite, buff, readReturn);
			//readReturn = read(parentRead, buff, readReturn);
			//write(0, buff, readReturn);
			

			
		}
		//waitpid(pid, 0,0);
	}

	

	return 0; //placeholder
}
