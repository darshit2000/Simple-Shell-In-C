#include  <stdio.h>
#include  <sys/types.h>
#include<sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64

/* Splits the string by space and returns the array of tokens
*
*/
char **tokenize(char *line)
{
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

//Function to execute command
void executeCommand(char **tokens)
{
	pid_t pid = fork();
	
	//child process
	if (pid == 0)
	{
		//to check for child process in ps -eF
		//sleep(5);
		
		execvp(tokens[0],tokens);
		printf("Shell : Incorrect command\n");
		exit(1);
	}
	else
	{
		// checked with ps -eF, when the child process completes execution, it is reaped
		pid_t w = wait(NULL);
	}
}

void executeCommandInBackground(char **tokens)
{
	pid_t childPid = fork();
	
	//child process
	if (childPid == 0)
	{
		//to check for child process in ps -eF
		//sleep(5);
		pid_t grandChildPid = fork();

		if (grandChildPid == 0)
		{
			execvp(tokens[0],tokens);
			printf("Shell : Incorrect command\n");
			exit(1);
		}
		else
		{
			exit(0);
		}
		
	}
	else
	{
		// checked with ps -eF, when the child process completes execution, it is reaped
		//pid_t w = wait(NULL);
		pid_t w = wait(NULL);
	}
}

int main(int argc, char* argv[]) {
	char  line[MAX_INPUT_SIZE];            
	char  **tokens;              
	int i;


	while(1) 
	{			
		/* BEGIN: TAKING INPUT */
		bzero(line, sizeof(line));
		printf("$ ");
		// ^ = complement , ^\n: the scanset is all characters except \n.
		scanf("%[^\n]", line);
		getchar();

		//printf("Command entered: %s (remove this debug output later)\n", line);
		/* END: TAKING INPUT */

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
	   int size = sizeof(tokens)/sizeof(tokens[0]);
	   if(strcmp(tokens[size-1],"&") != 0)
	   {		
			if(strcmp(tokens[0],"cd")==0)
			{
				chdir(tokens[1]);
			}
			else
			{
				executeCommand(tokens);
			}
	   }
	   else
	   {
			tokens[size-1] = '\0';
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
