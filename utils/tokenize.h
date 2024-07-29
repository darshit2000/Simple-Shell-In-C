#include <stdio.h>

char **tokenize(char *line);

// Splits the string by space and returns the array of tokens 
char **tokenize(char *line) {
	noOfTokens = 0; 
	numPipes = 0;
	// **tokens = string of tokens   |   tokenNo 
	char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));

	// *token = single token  |  tokenIndex
	char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));

	int i, tokenIndex = 0, tokenNo = 0, inQuote = 0;
	
	for(i =0; i < strlen(line); i++) {
		char readChar = line[i];

		if (readChar == '"' || readChar == '\'') {
			inQuote = !inQuote;
		} 
		else if (readChar == ' ' || readChar == '\n' || readChar == '\t') {
			token[tokenIndex] = '\0';
			// Only enter this loop if there is atleast 1 char
			if (tokenIndex != 0) {
					tokens[tokenNo] = (char*)malloc(MAX_TOKEN_SIZE*sizeof(char));
					strcpy(tokens[tokenNo++], token);
					tokenIndex = 0; 
					noOfTokens++;
			}
		} 
		else {
			if(readChar == '|') {
				numPipes++;
			}
			token[tokenIndex++] = readChar;
		}
	}
	
	free(token);
	tokens[tokenNo] = NULL ;
	return tokens;
}