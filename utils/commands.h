#include "signalHandler.h"

int isValidDirectory(char *dirName);
void executeCommand(char **tokens);
void executeCommandInBackground(char **tokens);
void executePipedCommands(char **tokens);
void exitFunction(char **tokens);
void welcomeScreen();
void printPrompt();

// Checks if a given directory path is valid.
int isValidDirectory(char *dirName) {
	struct stat stats;
    stat(dirName, &stats);
    if (S_ISDIR(stats.st_mode))	{
		return 1;
	}  
    return 0;
}

// Executes a command in the foreground.
void executeCommand(char **tokens)
{
	int status=0;
	pid_t childPid = fork();
    //fgPid = childPid;

	//child process
	if (childPid == 0) {
		//to check for child process in ps -eF | grep './my_shell'		
		execvp(tokens[0],tokens);
		printf("Shell : Incorrect command\n");
		exit(1);
	}
	else {
		fgPid = childPid;
        waitpid(childPid, &status, WUNTRACED);
		fgPid = -1;
	}
}

// Executes a command in the background.
void executeCommandInBackground(char **tokens) {
	int status;
	pid_t bgChildPid = fork();
	setpgid(0,0);

	if (bgChildPid == 0) {		
		execvp(tokens[0],tokens);
		printf("Shell : Incorrect command\n");
		exit(1);
	} 
    else {
		bgPid = bgChildPid;
        signal(SIGCHLD,signalHandler_SIGCHLD);
        processes[noOfProcesses++] = bgChildPid;
	}
}

// Executes a series of piped commands.
void executePipedCommands(char **tokens) {
    int i = 0;
    int numCommands = numPipes + 1;
    int pipefds[2 * numPipes];

    // Create pipes
    for (i = 0; i < numPipes; i++) {
        if (pipe(pipefds + i*2) < 0) {
            perror("Pipe failed");
            exit(EXIT_FAILURE);
        }
    }

    int j = 0;
    int commandIndex = 0;
    pid_t pid;
    int pipeIndex = 0;

    while (tokens[j] != NULL) {
        if (strcmp(tokens[j], "|") == 0 || tokens[j+1] == NULL) {
            char **commandTokens;
            if (strcmp(tokens[j], "|") == 0) {
                tokens[j] = NULL;
                commandTokens = &tokens[commandIndex];
                commandIndex = j + 1;
            } else {
                commandTokens = &tokens[commandIndex];
            }

            pid = fork();
            if (pid == 0) {
                // Child process
                if (pipeIndex > 0) {
                    // Not the first command; set input from the previous pipe
                    if (dup2(pipefds[(pipeIndex - 1) * 2], STDIN_FILENO) < 0) {
                        perror("dup2 in");
                        exit(EXIT_FAILURE);
                    }
                    // Close the read end of the previous pipe
                    close(pipefds[(pipeIndex - 1) * 2]);
                    // Close the write end of the previous pipe
                    close(pipefds[(pipeIndex - 1) * 2 + 1]);
                }

                if (pipeIndex < numPipes) {
                    // Not the last command; set output to the next pipe
                    if (dup2(pipefds[pipeIndex * 2 + 1], STDOUT_FILENO) < 0) {
                        perror("dup2 out");
                        exit(EXIT_FAILURE);
                    }
                    // Close the read end of the next pipe
                    close(pipefds[pipeIndex * 2]);
                    // Close the write end of the next pipe
                    close(pipefds[pipeIndex * 2 + 1]);
                }

                // Close all other pipe fds in the child
                for (i = 0; i < 2 * numPipes; i++) {
                    if (i != (pipeIndex - 1) * 2 && i != (pipeIndex - 1) * 2 + 1 &&
                        i != pipeIndex * 2 && i != pipeIndex * 2 + 1) {
                        close(pipefds[i]);
                    }
                }

                if (execvp(commandTokens[0], commandTokens) < 0) {
                    perror("Command execution failed");
                    exit(EXIT_FAILURE);
                }
            } else if (pid < 0) {
                perror("Fork failed");
                exit(EXIT_FAILURE);
            }

            pipeIndex++; // Increment the pipe index
        }
        j++;
    }

    // Close all pipe fds in the parent
    for (i = 0; i < 2 * numPipes; i++) {
        close(pipefds[i]);
    }

    // Wait for all child processes to finish
    for (i = 0; i < numCommands; i++) {
        wait(NULL);
    }
}

// Exits the shell, killing any background processes.
void exitFunction(char **tokens) {
	for (int i = 0; i < noOfProcesses; i++)	{
		if (processes[i] > 0) {
			kill(processes[i], SIGKILL);
			waitpid(processes[i], NULL, 0);
			processes[i] = -10;
		}	
	}

	for(int i=0;tokens[i]!=NULL;i++) {
		free(tokens[i]);
	}
	free(tokens);
	exit(0);
}

// Prints the welcome screen for the shell.
void welcomeScreen() {
        printf("\n\t============================================\n");
        printf("\t               Simple Shell\n");
        printf("\t--------------------------------------------\n");
        printf("\t             Created by: Darshit Gandhi:\n");
        printf("\t============================================\n");
        printf("\n");
}

// Prints the prompt for the shell.
void printPrompt() {
	char hostn[1204] = "";
	char* currentDirectory = (char*) calloc(1024, sizeof(char));
	gethostname(hostn, sizeof(hostn));
	printf("%s@%s: %s> ", getenv("LOGNAME"), hostn, getcwd(currentDirectory, 1024));
}