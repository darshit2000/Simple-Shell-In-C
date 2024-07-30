Simple Shell in C
==============

Simple and thoroughly commented shell written in C. Created by Darshit Gandhi as an University exercise.

Features
--------

* Shell supports all standard shell commands like `exit`, `pwd`, `clear`, `cd`, `echo`, `top`, `ps`, `grep`, `wc`, `cat`, etc.
* Commands can be executed in the background by appending an `&` at the end of the command. Shell supports simultaneous execution of background commands.
* Piping implemented (`<cmd1> | <cmd2>`) via `pipe` and `dup2` syscalls. Multiple piping is allowed.
* Shell handles the CTRL+C shortcut to kill long-running commands (shell is not exited).
* Program invocation is done by forking and child processes.


Source Code Structure
--------
* my_shell.c: Main source file containing the shell implementation.
* utils/commands.h: Header file for command execution functions.
* utils/signalHandler.h: Header file for handling interupts.
* utils/tokenizer.h: Header file for tokenization the given command.
* utils/vars.h: Header file containing the global variables.


Compilation and Usage
--------
Recommended operating system for running this program is Ubuntu.

1. To compile and run the program, execute the following in the directory that contains the my_shell.c file:
```
make  
```
```
./shell
```

2. Execute some command in foreground:
```
> ls -al
```

3. Execute some command in background:
```
> sleep 50 &
```

4. Execute some commands with pipe:
```
> cat my_shell.c | grep "main" | wc -l
```

5. Terminate a long-running process running in foreground with Ctrl + C

6. Exit the custom shell and kill all background processes:
```
> exit
```

7. Clean Object files:
```
make clean
```



