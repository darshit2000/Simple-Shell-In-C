all:
	gcc -o shell my_shell.c 
	
clean:
	rm -f shell 

.PHONY: all clean