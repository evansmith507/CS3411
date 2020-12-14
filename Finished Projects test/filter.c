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
#include <poll.h>
// Jake Muller CS3411 
//----------------------------------------------------------------------
// NOTES on program as specification were generally unclear on things
//----------------------------------------------------------------------
// Whenever the system call output mode and there is data waiting to
// be output, the systems dumps "max_lines" and returns to command mode.  
// Although because the command called was output "<pid> o <more> #"
// is still displayed dispite going to output mode. 


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
pid_t storePid;
char mode;
int maxLine;
int childIn; //for child process where inputs are received
int childOut; //for child process where outputs are sent
int parentRead; //for parent where child outputs are read from
int parentWrite; //for parent where it writes to child
int largestFD; //largest fd for select
int childErrorOut; 
bool childHasDied;
int commandCalled;
bool skiploop;
bool switchToCommand;


//Handle childoutputs
//NEEDED IF CALLING IN OUTPUT COMMAND: OUTDATED
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
		
		if(line >= maxLine){
			return 1; //more lines to read
		}else{
			return 0; //done reading
		}
	}
	return 0;
}

//TODO:: MAKE IT COMMANDS WITH RANDOM SHIT AT THE END DONT WORK (ie: /i jk  or /o /i)
void HandleCommand(char* buff){
	char command = buff[1];
	char extra[10] = "";
	int i = 0;
	for(i = 0; i < 10; i++){
		if(buff[i+3] <= '9' || buff[i+3] >= '0'){
			extra[i] = buff[i+3];
		}else{
			break;
		}
	} 
	

	commandCalled = 1;
	int temp;
	switch (command){
	case 'i':
		mode = INPUTMODE;
		//printf("Change to input mode %d \n", mode);
		break;
	case 'o': 
		mode = OUTMODE;
		skiploop = true;		
		break;
	case 'c':
		mode = COMMANDMODE;
		//printf("Change to command mode %d \n", mode);
		break;
	case 'm':
		//check if there is even a number	
		temp = atoi(extra);
		//printf("temp: %d\n", temp);
		if(temp == 0){
			printf("Invialid or No arugment given: /m <Integer>\n");
		}else{
			maxLine = temp;
		}
		//printf("new Maxline: %d\n", maxLine);
		break;
	case 'k':
		temp = atoi(extra);
		if(temp == 0){
			printf("Invialid or No arugment given: /k <Integer>\n");
		}else{
			//printf("signal sent: %d, to: %d\n", temp, storePid);
			kill(storePid, temp);
		}
		
		//signal send
		break;
	case '/':
		write(parentWrite, "/", 1);
		break;
	default:
		commandCalled = 0;
		printf("Incorrect Command Given:\n");
	}
}

void HandleStdIn(){
	char buff[256] = {" "};
	int l = 0;
	
	l = read(0, buff, sizeof(buff));
	//printf("read from buf: %s childin Fd: %d \n", buff, childIn);
	//fflush(stdout);
	//printf("read length: %d\n",l);
	//write(parentWrite, buff, l);
	switch (mode){
	case COMMANDMODE:
		if(buff[0] == '/'){ 
			HandleCommand(buff);
		}else{
			//printf("Input blocked: Currently in Command mode\n");
		}
		break;
	case INPUTMODE:
		if(buff[0] == '/' && buff[1] != '/'){ 
			HandleCommand(buff);
		}else if(buff[1] == '/'){
			write(parentWrite, buff+1, l);
		}else{
			write(parentWrite, buff, l);
		}
		break;
	case OUTMODE:
		if(buff[0] == '/' && buff[1] != '/'){ 
			HandleCommand(buff);	
		}else if(buff[1] == '/'){
			write(parentWrite, buff+1, l);
		}else{
			write(parentWrite, buff, l);
		}

		break;
	default:
		break;
	}
}


int main(int argc, char *argv[]){
	//check arguments
	if(argc < 2){
		printf("More Arguments Nedded, ./filter <program> <arguments> \n");
		return 0;
	}
	int status;
	bool outputcalled = false;
	commandCalled = 0;
	childHasDied = false;
	mode = COMMANDMODE;
	maxLine = 20;
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
	parentWrite = inputFd[1];
	childErrorOut = errorFd[1];
	largestFD = errorFd[1];

	//fork program
	pid_t pid = fork();
	storePid = pid;
	if(pid == 0){
		//child process
		char* programName = argv[1];
		//change pipes over
		dup2(childIn, STDIN_FILENO);
		dup2(childOut, STDOUT_FILENO);
		dup2(errorFd[1], STDERR_FILENO);
		//get child PID
		//childPid = getppid();

		//try pathname first (for self made)
		execv(programName, &argv[1]); // &argv[1] is fun 
		//then file (for standard systems like cat)
		execvp(programName, &argv[1]);
		printf("failed to open\n");
		exit(-1);
		
	}else{
		//set up varaibles
		int r;
		//int readReturn = 0;
		fd_set rds;
		//printf("%d\n",pid);
		//fd_set wts;
		int more;
		skiploop = false;
		switchToCommand = false;
		while(1){
			commandCalled = 0;
			outputcalled = false;
			if(skiploop){
				commandCalled = 1;
				outputcalled = true;
			}
			more = 0;
			switchToCommand = false;
			skiploop = false;
			
			//prepare descriptors
			FD_ZERO(&rds);
			FD_SET(0, &rds);
			if(mode == OUTMODE){
				FD_SET(parentRead, &rds);
			}
			
			FD_SET(errorFd[0], &rds);

			r = select(largestFD+1, &rds, 0, 0, NULL);
			if(r == -1){
				perror("select Error");
			}
			//printf("Select Unblocked \n");
			fflush(stdout);
			


			if(FD_ISSET(errorFd[0], &rds)){
				//printf("child error ready \n");
				//char buff[128];
				//int errorReturn = read(errorFd[0], buff, sizeof(buff));
				//write(STDERR_FILENO, buff, errorReturn);
				//perror(buff);
				char buff;
				int errorRead = 0;
				while(1){
					if (poll(&(struct pollfd){ .fd = errorFd[0], .events = POLLIN }, 1, 0)==1) {
    					errorRead = read(errorFd[0], &buff, 1);	
					}else{
						break;
					}
					write(STDERR_FILENO, &buff, errorRead);
				}

			}

			if(FD_ISSET(0,&rds)){
				//printf("stdin is set \n");
				HandleStdIn();
			}

			//if output mode called and there is nothing in buffer cancel loop skip 
			if(skiploop == true){
				if (poll(&(struct pollfd){ .fd = parentRead, .events = POLLIN }, 1, 0)==0){
					skiploop = false;
				}
			}
			if( FD_ISSET(parentRead, &rds) && mode == OUTMODE && outputcalled == true ){
				//int more = HandleChildOut();
				//printf("done\n");
				int lines = 0;
				int readReturn = 0;
				char buff[2];
				
				while(1){
					readReturn = 0;
					//printf("before read \n");
					if (poll(&(struct pollfd){ .fd = parentRead, .events = POLLIN }, 1, 0)==1) {
						//printf("can you read at all dude\n");
    					readReturn = read(parentRead, buff, 1);	
					}else{
						break;
					}	
									
					//printf("ReadReturn: %d\n", readReturn);
					if(readReturn == 0 || readReturn == -1){ break; }
					//printf("buff[0]: %x \n", buff[0]);
					if(buff[0] == '\n'){ lines++; }
					write(1, buff, readReturn);
					if(lines >= maxLine && poll(&(struct pollfd){ .fd = parentRead, .events = POLLIN }, 1, 0)==1){ //check if there is anything left in thing
						more = 1;
						break;
					}
					
				}
				
				//if(lines >= maxLine /*&& more == 1*/ ){
					switchToCommand = true;
					//mode = COMMANDMODE;
				//}
			}
			if (waitpid(storePid, &status, WNOHANG) == -1 && childHasDied == false) {
				char foo[59];
				sprintf(foo, "The child %d has terminated with code %d\n", storePid, status);
   				//printf("The child %d has terminated with code %d\n", storePid, status);
				int i = 0;
				for(i = 0; i < 59; i++){
					if(foo[i] == 0){ break; }
					write(STDERR_FILENO, &foo[i], 1);
				}
    			childHasDied = true;
			}

			if(commandCalled && skiploop == false){
				printf("%d %c", pid, mode);
				if(more){
					printf(" more");
				}
				printf(" #\n");
				if(switchToCommand){
					mode = COMMANDMODE;
				}
			}
			
		}
		//waitpid(pid, 0,0);
	}

	

	return 0; //placeholder
}
