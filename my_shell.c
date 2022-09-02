#include  <stdio.h>
#include  <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64

int noOfTokens = 0; 
//pid_t childPid = 0;
//pid_t bgChildPid = 0;
pid_t fgPid;
pid_t bgPid;
//pid_t grandChildPid = 0;
int processes[1000];
int noOfProcesses = 0;

/* Splits the string by space and returns the array of tokens
*
*/
char **tokenize(char *line)
{
	noOfTokens = 0;
	// **tokens = string of tokens   |   tokenNo 
	char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));

	// *token = single token  |  tokenIndex
	char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));

	int i, tokenIndex = 0, tokenNo = 0;

	for(i =0; i < strlen(line); i++)
	{
			char readChar = line[i];

			if (readChar == ' ' || readChar == '\n' || readChar == '\t')
			{
				token[tokenIndex] = '\0';
				// Only enter this loop if there is atleast 1 char
				if (tokenIndex != 0)
				{
						tokens[tokenNo] = (char*)malloc(MAX_TOKEN_SIZE*sizeof(char));
						strcpy(tokens[tokenNo++], token);
						tokenIndex = 0; 
						noOfTokens++;
				}
			} 
			else 
			{
				token[tokenIndex++] = readChar;
			}
	}
	
	free(token);
	tokens[tokenNo] = NULL ;
	return tokens;
}

void exitFunction(char **tokens)
{
	for (int i = 0; i < noOfProcesses; i++)
	{
		//printf("\n Process %d %d", i, processes[i]);
		kill(processes[i], SIGKILL);
	}

	for(int i=0;tokens[i]!=NULL;i++)
	{
		free(tokens[i]);
	}
	free(tokens);

	exit(0);
	
}

//Function to execute command
void executeCommand(char **tokens)
{
	int status=0;
	pid_t childPid = fork();
    fgPid = childPid;
	//child process
	if (childPid == 0)
	{
		//to check for child process in ps -eF | grep './my_shell'
		sleep(20);
		
		/*
		To check when the backgroud process is returning faster than foreground process, 
		then check by doing sleep(10) in foreground and sleep(5) in background method

		sleep(10);
		*/
		execvp(tokens[0],tokens);
		printf("Shell : Incorrect command\n");
		exit(1);
	}
	else
	{
        waitpid(childPid, &status, WUNTRACED);
	}
}

void childSignalHandler(int signum) {
	int status;
	pid_t return_pid = waitpid(bgPid, &status, WNOHANG);

	if (return_pid == bgPid) 
	{
		printf("Shell: Background process finished\n");
	}

}

void executeCommandInBackground(char **tokens)
{
	int status;
	pid_t bgChildPid = fork();
    bgPid = bgChildPid;
	setpgid(0,0);
	//printf("Background PGID : %d \n",getpgid(getpid()));

	if (bgChildPid == 0)
	{		
		sleep(20);
		execvp(tokens[0],tokens);
		printf("Shell : Incorrect command\n");
		exit(1);
	}
	else
	{
        signal(SIGCHLD,childSignalHandler);
        processes[noOfProcesses++] = bgChildPid;	
	}
}

void ctrl_C_Handler(int sig)
{
    int status;
	if(kill(fgPid, 9)<0)
    {
        perror("Ctrl+C ");
    }
	printf(" Foreground process closed.\n");
}

int main(int argc, char* argv[]) {
	char  line[MAX_INPUT_SIZE];            
	char  **tokens;              
	int i,status;

    signal(SIGINT, ctrl_C_Handler);
	while(1) 
	{		
		/* BEGIN: TAKING INPUT */
		bzero(line, sizeof(line));
		printf("$ ");
		// ^ = complement , ^\n: the scanset is all characters except \n.
		scanf("%[^\n]", line);
		getchar();

		line[strlen(line)] = '\n'; //terminate with new line
		// array of string (tokens[0]="cd" , tokens[1]="Desktop")
		tokens = tokenize(line);
   
       //do whatever you want with the commands, here we just print them

		if (tokens[0]==NULL)
		{
			continue;
		}
		
		/*
		for(i=0;tokens[i]!=NULL;i++)
		{
			printf("found token %s (remove this debug output later)\n", tokens[i]);
		}
        */
	   //int size = sizeof(tokens)/sizeof(tokens[0]);
	   if(strcmp(tokens[noOfTokens-1],"&") != 0)
	   {		
			if (strcmp(tokens[0],"cd")==0)
			{
				chdir(tokens[1]);
			}
			else if (strcmp(tokens[0],"exit") == 0)
			{
				exitFunction(tokens);
			}
			else
			{
				executeCommand(tokens);
			}
	   }
	   else
	   {
			tokens[noOfTokens-1] = NULL;
			executeCommandInBackground(tokens);
	   }

		// Freeing the allocated memory	
		for(i=0;tokens[i]!=NULL;i++)
		{
			free(tokens[i]);
		}
		free(tokens);

	}
	return 0;
}
