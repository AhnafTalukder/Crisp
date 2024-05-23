#include <stdio.h>
#include <stdlib.h>


#ifdef _WIN32
#include <string.h>

/* Fake readline function */
static char buffer[2048];

char * readline(char* prompt){
	fputs(prompt, stdout);
	fgets(buffer, 2048, stdin);
	char* copy = (char *) calloc(strlen(buffer)+1, sizeof(char));
	strncpy(copy, buffer, strlen(buffer));
	copy[strlen(copy) - 1] = '\0';
	return copy;

}

/*Fake add_history function */
void add_history(char * unused){}

#else
#include <editline/readline.h>
#endif

int main(int argc, char** argv){

	puts("Crisp Version 0.01");
	puts("Press Ctrl+c to Exit\n");


	while(1){

		char * input = readline("Crisp> ");
		add_history(input);

		printf("No you're a %s\n", input);

		free(input);
	}
	return 0;



}
