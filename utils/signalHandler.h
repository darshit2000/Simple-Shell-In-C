void signalHandler_SIGCHLD(int signum);
void signalHandler_SIGINT(int sig);

/*
 Signal handler for SIGCHLD signal.
 
  This function is called when a child process terminates. It checks the status of all background processes and 
  prints a message if a process has finished. If the finished process is the current background process, 
  its PID is reset to -10.
  
  Example:
      // Assume we have a background process with PID 1234
      // When the process finishes, the signal handler will be called
      // Output: Shell: Background process finished (PID : 1234)
*/
void signalHandler_SIGCHLD(int signum) {
	int status;
    for (int i = 0; i < noOfProcesses; i++) {
        if(processes[i] > 0) {
            pid_t return_pid = waitpid(processes[i], &status, WNOHANG);
            if (return_pid == processes[i]) {
                printf("Shell: Background process finished (PID : %d)\n",processes[i]);
                if (processes[i] == bgPid) {
                    processes[i] = -10;
                }
            }
        }
    }
}

/*
  Signal handler for SIGINT signal.
  
  This function is called when the user presses Ctrl+C. It checks if there is a foreground process running 
  and kills it if it exists. If no foreground process is running, it prints a message indicating that.
  
  Example:
      // Assume we have a foreground process with PID 5678
      // When the user presses Ctrl+C, the signal handler will be called
      // Output: Foreground process closed.
      //         PID : 5678 is now reaped.
*/
void signalHandler_SIGINT(int sig) {
    int status;
	if(fgPid > 0) {
		if(kill(fgPid, 9)<0) {
			perror("Ctrl+C ");
		}
		printf(" Foreground process closed.\n");
		printf("PID : %d is now reaped.\n", fgPid);
		printPromptFlag = 1;
	}
	else {
		printf(" No Foreground process is executing.\n");
		printPromptFlag = 1;
	}
}