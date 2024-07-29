#include  <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64

int noOfTokens = 0; 
pid_t fgPid = -1;
pid_t bgPid;
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

int isValidDirectory(char *dirName)
{
	struct stat stats;
    stat(dirName, &stats);
    if (S_ISDIR(stats.st_mode))
	{
		return 1;
	}  
    return 0;
}

void exitFunction(char **tokens)
{
	for (int i = 0; i < noOfProcesses; i++)
	{
		if (processes[i] > 0)
		{
			//printf("\n Process %d %d", i, processes[i]);
			kill(processes[i], SIGKILL);
			waitpid(processes[i], NULL, 0);
			//printf("PID : %d is now reaped.\n", processes[i]);
			processes[i] = -10;
		}	
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
    //fgPid = childPid;

	//child process
	if (childPid == 0)
	{
		//to check for child process in ps -eF | grep './my_shell'
		//sleep(10);
		
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
		fgPid = childPid;
        waitpid(childPid, &status, WUNTRACED);
	}
}

void childSignalHandler(int signum) {
	int status;
	// pid_t return_pid = waitpid(bgPid, &status, WNOHANG);

	// if (return_pid == bgPid) 
	// {
	// 	printf("Shell: Background process finished\n");
		for (int i = 0; i < noOfProcesses; i++)
		{
			if(processes[i] > 0)
			{
				pid_t return_pid = waitpid(processes[i], &status, WNOHANG);
				if (return_pid == processes[i]) 
				{
					printf("Shell: Background process finished (PID : %d)\n",processes[i]);
					if (processes[i] == bgPid)
					{
						processes[i] = -10;
					}
				}
			}
		}
		
	// }

}

void executeCommandInBackground(char **tokens)
{
	int status;
	pid_t bgChildPid = fork();
    
	setpgid(0,0);
	//printf("Background PGID : %d \n",getpgid(getpid()));

	if (bgChildPid == 0)
	{		
		//sleep(5);
		execvp(tokens[0],tokens);
		printf("Shell : Incorrect command\n");
		exit(1);
	}
	else
	{
		bgPid = bgChildPid;
        signal(SIGCHLD,childSignalHandler);
        processes[noOfProcesses++] = bgChildPid;	
	}
}

void ctrl_C_Handler(int sig)
{
    int status;
	if(fgPid > 0)
	{
		if(kill(fgPid, 9)<0)
		{
			perror("Ctrl+C ");
		}
		printf(" Foreground process closed.\n");
		printf("PID : %d is now reaped.\n", fgPid);
	}
	else
	{
		printf(" No Foreground process is executing.\n");
	}
}

int main(int argc, char* argv[]) {
	char  line[MAX_INPUT_SIZE];            
	char  **tokens;              
	int i,status;

    signal(SIGINT, ctrl_C_Handler);

	while(1) 
	{		

		//TO GET THE "BACKGROUND PROCESS COMPLETED" AFTER PRESSING ENTER

		// pid_t w = waitpid(bgPid, &status, WNOHANG);
		// if(w == bgPid)
		// {
		// 	printf("Shell: Background process finished\n");
		// 	for (int i = 0; i < noOfProcesses; i++)
		// 	{
		// 		if (processes[i] == bgPid)
		// 		{
		// 			processes[i] = -10;
		// 		}			
		// 	}
		// }

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

	   if(strcmp(tokens[noOfTokens-1],"&") != 0)
	   {		
			if (strcmp(tokens[0],"cd")==0)
			{
				// To check the syntax for cd command
				if(!tokens[2])
				{
					if(isValidDirectory(tokens[1]))
					{
						chdir(tokens[1]);
					}
					else
					{
						printf("Please enter valid Directory name.\n");
						continue;
					}
				}
				else
				{
					printf("Shell: Incorrect command\n");
					continue;
				}
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
