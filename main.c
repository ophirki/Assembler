#include <stdio.h>
#include <stdlib.h>
#include "data_structs.h"

struct sym_list * sym_t = NULL;
struct data_list * data_l = NULL;
struct entries_list * ent_l = NULL;
struct data_list * dc = NULL;
int datacnt = 0;
int inscnt = 0;

void assembler(char *src_file);
int line_type(char *s);

int main(int argc, char *argv[]){
	int i;
	structs_init();
	/*Checking the number of arguments given*/
	if (argc < 2){
		fprintf(stderr, "You entered wrong number of arguments. You should enter atleat 2 arguments.\n");
		free(sym_t);
		return 0;
	}
	/*treating every source file*/
	for (i=1; i<argc; i++){
		assembler(argv[i]);
	}
	free(sym_t);
	return 0;
}
