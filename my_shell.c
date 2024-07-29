#include  <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include "utils/vars.h"
#include "utils/tokenize.h"
#include "utils/commands.h"

int main(int argc, char* argv[]) {
	// Input buffer for user commands.
	char  line[MAX_INPUT_SIZE];  

	// Tokenized input buffer.        
	char  **tokens;              
	int i, status;

	// Signal handling for SIGINT (Ctrl+C)
    signal(SIGINT, signalHandler_SIGINT);
	welcomeScreen();
	printPromptFlag = 1;

	while(1) {
		/* BEGIN: TAKING INPUT */
		bzero(line, sizeof(line));
		if(printPromptFlag == 1) printPrompt();
		printPromptFlag = 1;

		// ^ = complement , ^\n: the scanset is all characters except \n.
		scanf("%[^\n]", line);
		getchar();

		// Terminate the input buffer with a newline character.
		line[strlen(line)] = '\n'; 

		// Tokenize the input buffer into an array of strings.
		// (tokens[0]="cd" , tokens[1]="Desktop")
		tokens = tokenize(line);

		// Check if the input buffer is empty.
		if (tokens[0]==NULL) {
			continue;
		}

		if(numPipes > 0) {
			// Execute the piped command.
			executePipedCommands(tokens);
		} else {
			// Check if the command is a foreground or background command.
			// Commands to be executes in background end with a '&'
			if(strcmp(tokens[noOfTokens-1],"&") != 0) {	

				// Execute the cd command	
				if (strcmp(tokens[0],"cd")==0) {
					if(!tokens[2]) {
						if(isValidDirectory(tokens[1])) {
							chdir(tokens[1]);
						}
						else {
							printf("Please enter valid Directory name.\n");
							continue;
						}
					}
					else {
						printf("Shell: Incorrect command\n");
						continue;
					}
				}
				// Handle the exit command.
				else if (strcmp(tokens[0],"exit") == 0) {
					exitFunction(tokens);
				}
				// Execute the command in the foreground.
				else {
					executeCommand(tokens);
				}
			}
			else {
				// Remove the '&' token from the input buffer.
				tokens[noOfTokens-1] = NULL;
				// Execute the command in the background.
				executeCommandInBackground(tokens);
			}
		}

		// Freeing the allocated memory	
		for(i=0;tokens[i]!=NULL;i++) {
			free(tokens[i]);
		}
		free(tokens);
	}
	return 0;
}
