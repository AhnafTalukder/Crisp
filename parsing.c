#include <stdio.h>
#include <stdlib.h>
#include "mpc.h"

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
	

	/* Create Some Parsers */
	mpc_parser_t* Number = mpc_new("number");
	mpc_parser_t* Operator = mpc_new("operator");
	mpc_parser_t* Expr = mpc_new("expr");
	mpc_parser_t* Lispy = mpc_new("lispy");

	mpca_lang(MPCA_LANG_DEFAULT,
  "                                                     \
    number   : /-?[0-9]+/ ;                             \
    operator : '+' | '-' | '*' | '/' ;                  \
    expr     : <number> | '(' <operator> <expr>+ ')' ;  \
    lispy    : /^/ <operator> <expr>+ /$/ ;             \
  ",
  Number, Operator, Expr, Lispy);

	


	while(1){

		char * input = readline("Crisp> ");
		add_history(input);

		mpc_result_t r;
		if(mpc_parse("<stdin>", input, Lispy, &r)){
			/* On Success Print the AST */
			mpc_ast_print(r.output);
			mpc_ast_delete(r.output);
		} else{
			/* Otherwise Print the Error */
			mpc_err_print(r.error);
			mpc_err_delete(r.error);

		}

		free(input);
	}

	mpc_cleanup(4, Number, Operator, Expr, Lispy);
	return 0;



}
