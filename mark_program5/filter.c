#define DEBUG

// Add any includes you require.
#include <poll.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <unistd.h>

typedef enum { COMMAND = 99, INPUT = 105, INPUT_OUTPUT = 111 } FilterMode;
int childPID;

int stdinPipe[2];
int stdoutPipe[2];
int stderrPipe[2];

char buf[2];

/*  This function gets called when the parent receives a SIGINT signal in order
 *  to make sure the child is killed as well.
 */
void killChild(int code) {
  kill(childPID, SIGKILL);
  printf("\n");
  exit(0);
}

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
 * 	/m <integer> : Set the max-text, maximum number of lines to be
 * displayed. /k <integer> : Send the child process indicated signal.
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
 * i (input only), c (command), o(input/output)), optional <more> is the text
 * "more" if there is data available to be displayed, and lastly the pound
 * character.
 */
int main(int argc, char *argv[]) {
  // Hint: use select() to determine when reads may execute without blocking.

  // Make sure there are enough arguments
  if (argc < 2) {
    printf("Usage: filter <program name> <arguments>\n");
    return 0;
  }

  // Create the pipes for stdin, stdout, stderr
  pipe(stdinPipe);
  pipe(stdoutPipe);
  pipe(stderrPipe);

  // This isn't used, but it has to be here :)
  int status;

  // Fork a child, storing the child's PID
  childPID = fork();
  if (childPID == 0) {
    // Redirect stdin, stdout, and stderr to pipes
    dup2(stdinPipe[0], STDIN_FILENO);
    dup2(stdoutPipe[1], STDOUT_FILENO);
    dup2(stderrPipe[1], STDERR_FILENO);

    // Shift argv one position to the left
    int i = 0;
    for (i = 0; i < argc - 1; i++) {
      argv[i] = argv[i + 1];
    }

    // Set the last element to NULL for execvp's usage
    argv[argc - 1] = NULL;
    execv(argv[0], argv);
    execvp(argv[0], argv);

    // Something has gone horribly wrong at this point
    perror("Error executing child");
    exit(-1);

  } else {
    // Assign killChild() to handle SIGINT
    signal(SIGINT, killChild);

    // Store the pipe fds for ease of use
    int stdinChild = stdinPipe[1];
    int stdoutChild = stdoutPipe[0];
    int stderrChild = stderrPipe[0];

    // Close the ends of the pipes that we aren't using
    close(stdinPipe[0]);
    close(stdoutPipe[1]);
    close(stderrPipe[1]);

    fd_set rfds;

    // Various variables we need
    FilterMode mode = COMMAND;
    bool more = false;
    bool hasPrintedInputPrompt = false;
    bool skipPrompt = false;
    bool childHasDied = false;
    bool ioModeShouldPrint;
    int maxText = 20;

    // Start the main loooooop yaayyyyy!!!!
    while (1) {
      FD_ZERO(&rfds);
      FD_SET(0, &rfds);
      if (mode == INPUT_OUTPUT && ioModeShouldPrint) {
        FD_SET(stdoutChild, &rfds);
      }
      FD_SET(stderrChild, &rfds);

      fflush(stdout);
      
      // Wait for select to do its thing
      int r = select(6, &rfds, 0, 0, NULL);

      // Check if stdin has something to read
      if (FD_ISSET(0, &rfds)) {
        // Make a 128 character write buffer
        char buf[128];
        
        // Make an 11 character digit buffer
        char intBuf[11];

        int l;
        l = read(0, buf, sizeof(buf));
        switch (mode) {
          // If we're in COMMAND mode, the only thing that has to be done
          // differently is only allowing commands. It only checks for a
          // command and then it falls through to INPUT and INPUT_OUTPUT mode
          // cases, which do exactly the same thing. 
          
          case COMMAND:
            // Check for command
            if (buf[0] != '/') {
              printf("Error: Expected a command!\n");
              break;
            }
          case INPUT:
          case INPUT_OUTPUT:
            // Check for single slash
            if (buf[0] == '/' && buf[1] != '/') {
              switch (buf[1]) {
                case 'i':
                  mode = INPUT;
                  hasPrintedInputPrompt = false;
                  break;
                case 'o':
                  mode = INPUT_OUTPUT;
                  ioModeShouldPrint = true;
                  skipPrompt = true;
                  break;
                case 'c':
                  mode = COMMAND;
                  break;
                case 'm': {
                  // Iterate through the buffer adding digits to intBuf, making
                  // a string essentially.
                  int i = 3;
                  for (i = 3; i < 13; i++) {
                    char digit;
                    digit = buf[i];
                    if (digit < 48 || digit > 57) break;
                    intBuf[i - 3] = digit;
                  }
                  intBuf[i - 3] = '\0';

                  // Convert the string to an int
                  maxText = atoi(intBuf);
                  break;
                }
                case 'k': {
                  int i = 3;
                  for (i = 3; i < 13; i++) {
                    char digit;
                    digit = buf[i];
                    if (digit < 48 || digit > 57) break;
                    intBuf[i - 3] = digit;
                  }
                  intBuf[i - 3] = '\0';
                  int sigNum = atoi(intBuf);
                  kill(childPID, sigNum);
                  break;
                }
                default:
                  // Invalid command
                  printf("Error: Invalid command!\n");
                  break;
              }
            } else {
              if (buf[1] == '/') {
                int i = 0;
                for (i = 0; i < l; i++) {
                  buf[i] = buf[i + 1];
                }
                l--;
              }
              write(stdinChild, buf, l);
            }
            break;
        }
      }

      // Check if stdout has something to read
      if (FD_ISSET(stdoutChild, &rfds)) {
        int lines = 0;

        char ch;
        int length;

        while (lines < maxText &&
               (poll(&(struct pollfd){.fd = stdoutChild, .events = POLLIN}, 1,
                     0) == 1 &&
                (length = read(stdoutChild, &ch, 1))) == 1) {
          write(1, &ch, 1);
          if (ch == '\n') lines++;
        }
        if (ch != '\n') {
          printf("\n");
        }
        more = (length == 1) &&
               (poll(&(struct pollfd){.fd = stdoutChild, .events = POLLIN}, 1,
                     0) == 1);
        ioModeShouldPrint = false;
        // mode = COMMAND;
      } else {
        more = false;
      }

      // Check if stderr has something to read
      if (FD_ISSET(stderrChild, &rfds)) {
        char buf[256];
        int length = read(stderrChild, buf, sizeof(buf));
        write(1, buf, length);
      } 

      // Use waitpid to check if the child has died and see if we've already
      // printed the obituary.
      if (waitpid(childPID, &status, WNOHANG) == -1 && childHasDied == false) {
        printf("The child %d has terminated with code %d\n", childPID, status);
        childHasDied = true;
      }

      // If mode is INPUT and it hasn't printed the prompt yet or if the mode
      // is just not INPUT
      // AND
      // Skipping isn't allowed
      if (((mode == INPUT && !hasPrintedInputPrompt) || (mode != INPUT)) &&
          skipPrompt == false) {
        printf("%d %c %s#\n", childPID, mode, more ? "more " : "");
        // Save that we've printed the input prompt so INPUT mode won't spam it
        if (mode == INPUT) hasPrintedInputPrompt = true;
      }
      skipPrompt = false;
    }
  }

  return 0;  // placeholder
}