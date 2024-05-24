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

int number_of_nodes(mpc_ast_t* t);
long eval (mpc_ast_t* t);
long eval_op(long x, char * op, long y);


int number_of_nodes(mpc_ast_t* t){
	if(t-> children_num == 0) {return 1; }
	if(t-> children_num >= 1){
		
		int total = 1;
		for(int i = 0; i < t->children_num; i++){
			total=total + number_of_nodes(t->children[i]);

		}
	return total;
	}
	return 0;

}

long eval(mpc_ast_t* t){
	
	/* If tagged as a number return it directly. */
	if(strstr(t->tag, "number")){
		return atoi(t->contents);
	}

	/*The operator is always second child*/
	char* op = t->children[1]->contents;

	/*We store the third child in x*/
	long x = eval(t->children[2]);


	int i = 3;
	while(strstr(t->children[i]->tag, "expr")){
		x = eval_op(x, op, eval(t->children[i]));
		i++;
	}

	return x;
}	

long eval_op(long x, char* op, long y){
	if(strcmp(op, "+")==0) {return x+y;}
	if(strcmp(op, "-")==0) {return x-y;}
	if(strcmp(op, "*")==0) {return x*y;}
	if(strcmp(op, "/")==0) {return x / y;}
	return 0;

}

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
    	operator : '+' | '-' | '*' | '/' | '%' ;                  \
    	expr     : <number> | '(' <operator> <expr>+ ')' ;  \
    	lispy    : /^/ <operator> <expr>+ /$/ ;             \
  	",
	Number, Operator, Expr, Lispy);

	/* Practice parsers */

	//(a+b)* Closure of set [a, b]
	mpc_parser_t* Str = mpc_new("str");

	mpca_lang(MPCA_LANG_DEFAULT, 
	"                                 \
		str : /^/ /[ab]*/ /$/;    \
	", Str);	

	mpc_parser_t* A = mpc_new("a");
	mpc_parser_t* B = mpc_new("b");
	mpc_parser_t* cab = mpc_new("cab");
	mpc_parser_t* pattern = mpc_new("pattern");

	mpca_lang(MPCA_LANG_DEFAULT, 
	"                                   \
		a: 'a';  		    \
		b: 'b';      		    \
		cab: <a> <b>; 	             \
		pattern: /^/ <cab>* <a>? <b>?/$/; \
	", A, B, cab, pattern);

	while(1){

		char * input = readline("Crisp> ");
		add_history(input);

		mpc_result_t r;
		if(mpc_parse("<stdin>", input, Lispy, &r)){
			/* Load AST from output */
			mpc_ast_t* a = r.output;
			printf("Tag: %s\n", a->tag);
			printf("Contents: %s\n", a->contents);
			printf("Number of children: %i\n", a->children_num);

			/* Get First Child */
			mpc_ast_t* c0 = a->children[0];
			printf("First Child Tag: %s\n", c0->tag);
			printf("First Child Contents: %s\n", c0->contents);
			printf("First Child Number of children: %i\n", c0->children_num);

			long result = eval(r.output);
			printf("%li\n", result);
			mpc_ast_delete(r.output);

		} else{
			/* Otherwise Print the Error */
			mpc_err_print(r.error);
			mpc_err_delete(r.error);

		}

		free(input);
	}

	mpc_cleanup(4, Number, Operator, Expr, Lispy);
	mpc_cleanup(1, Str);
	mpc_cleanup(4, A, B, cab, pattern);
	return 0;



}
