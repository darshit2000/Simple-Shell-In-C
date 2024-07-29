#include <stdio.h>

// Maximum size of the input buffer. 
#define MAX_INPUT_SIZE 1024

// Maximum size of a token. 
#define MAX_TOKEN_SIZE 64

// Maximum number of tokens. 
#define MAX_NUM_TOKENS 64

// Keeps track of the number of tokens in the current input.
int noOfTokens = 0; 

// Stores the process ID of the foreground process.
pid_t fgPid = -1;

// Stores the process ID of the background process.
pid_t bgPid;

// Stores the process IDs of all processes.
int processes[1000];

// Keeps track of the number of processes.
int noOfProcesses = 0;

// Used to determine whether to print the prompt or not.
int printPromptFlag = 0;

// Keeps track of the number of pipes.
int numPipes = 0;