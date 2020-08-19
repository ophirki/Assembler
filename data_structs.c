#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "data_structs.h"
#define MAX_LENGTH_LABEL 31
#define MAX_NUM_IN_DATA_LIST 8192
#define EXTERNAL_TYPE 'e'
#define INF_STRT 100

extern sym_list *sym_t;
extern data_list *data_l;
extern data_list *dc;
extern entries_list *ent_l;
extern int datacnt;

char * int_to_str(int n);
char * bits_to_dots(char *s);

/*the function adds the given symbol and value to the symbol table*/
void add_to_sym_tab(char * sym, int value, char type){
	sym_list *lp = sym_t, *lp_next = sym_t->next;
	if (!((*lp).sym[0])){
		memmove((*lp).sym, sym, MAX_LENGTH_LABEL);
		(*lp).value = value;
		(*lp).type = type;
		(*lp).next = NULL;
	}
	else{
		while((*lp_next).sym){
			lp = (*lp).next;
			lp_next = (*lp_next).next;
		}
		lp_next = malloc(sizeof(sym_list));
		if (lp_next == NULL)
			fprintf(stderr, "Error! memory not allocated.");
		memmove((*lp_next).sym, sym, MAX_LENGTH_LABEL);
		(*lp_next).value = value;
		(*lp_next).type = type;
		(*lp_next).next = NULL;
		(*lp).next = lp_next;
	}
}

/*adds the value to the data array*/
void add_to_data_list(int value){
	data_list *dptr = data_l, *dptr_next = data_l->next;
	if ((*dptr).value == MAX_NUM_IN_DATA_LIST){
		(*dptr).value = value;
		(*dptr).next = NULL;
		dc = dptr;
	}
	else{
		while(dptr_next){
			dptr = (*dptr).next;
			dptr_next = (*dptr_next).next;
		}
		dptr_next = malloc(sizeof(data_list));
		if (dptr_next == NULL)
			fprintf(stderr, "Error! memory not allocated.");
		(*dptr_next).value = value;
		(*dptr_next).next = NULL;
		(*dptr).next = dptr_next;
		dc = dptr_next;
	}
	datacnt++;
}

/*adds the symbol to the entry list*/
void add_to_entries_list(char *sym, int line_num){
	entries_list *elptr = ent_l, *elptr_next = ent_l->next;
	if (!((*elptr).sym[0])){
		memmove((*elptr).sym, sym, MAX_LENGTH_LABEL);
		(*elptr).value = line_num;
		(*elptr).next = NULL;
	}
	else{
		while((*elptr_next).sym){
			elptr = (*elptr).next;
			elptr_next = (*elptr_next).next;
		}
		elptr_next = malloc(sizeof(entries_list));
		if (elptr_next == NULL)
			fprintf(stderr, "Error! memory not allocated.");
		memmove((*elptr_next).sym, sym, MAX_LENGTH_LABEL);
		(*elptr).value = line_num;
		(*elptr_next).next = NULL;
		(*elptr).next = elptr_next;
	}
}

/*prints all the entries and their values to the given file*/
void print_entries_to_file(char * fname){
	entries_list *elptr = ent_l;
	FILE *fd;
	fd = fopen(fname, "a");
	while((*elptr).sym){
		fprintf(fd, "%s %d\n", (*elptr).sym, (*elptr).value + INF_STRT);
		elptr = (*elptr).next;
	}
	fclose(fd);
}

/*prints all the entries and their values to the given file*/
void print_data_to_file(char * fname){
	data_list *lp = dc;
	FILE *fd;
	fd = fopen(fname, "a");
	while((lp)&&((*lp).value)){
		fprintf(fd, "%s\n", bits_to_dots(int_to_str((*lp).value)));
		lp = (*lp).next;
	}
	fclose(fd);
}

/*prints all the externals and their lines to the given file*/
void print_externals_to_file(char * fname){
	sym_list *lp = sym_t;
	FILE *fd;
	fd = fopen(fname, "a");
	while((*lp).sym){
		if((*lp).type == EXTERNAL_TYPE)
			fprintf(fd, "%s %d\n", (*lp).sym, (*lp).value + INF_STRT);
		lp = (*lp).next;
	}
	fclose(fd);
}

/*the function returns the line number of the lable*/
int label_location(char *label){
	sym_list *lp = sym_t;
	int location = -1;
	char label_cpy[MAX_LENGTH_LABEL], *p;
	strncpy(label_cpy, label, MAX_LENGTH_LABEL);
	p = label_cpy;
	for (;*p;p++){
		if((*p) == '\n')
			*p = '\0';
	}
	while(((*lp).sym)){
		if (!strncmp(label_cpy, (*lp).sym, strlen((*lp).sym)))
			location = (*lp).value;
		lp = (*lp).next;
	}
	if (location == -1){
		fprintf(stderr, "Couldn't return location of lable, lable not found.\n");
		return -1;
	}
	else
		return location;
}

/*the function initialize the symbol table, data list and instruction list*/
void structs_init(){
	/*symbol table initialize*/
	sym_t = malloc(sizeof(sym_list));
	if (sym_t == NULL)
		fprintf(stderr, "Error! memory not allocated.");
	memset((*sym_t).sym, '\0', MAX_LENGTH_LABEL);
	/*data list initialize*/
	data_l = malloc(sizeof(data_list));
	if (data_l == NULL)
		fprintf(stderr, "Error! memory not allocated.");
	(*data_l).value = MAX_NUM_IN_DATA_LIST;
	(*data_l).next = NULL;
	/*dc initialize*/
	dc = data_l;
	/**/
	ent_l = malloc(sizeof(entries_list));
	if (ent_l == NULL)
		fprintf(stderr, "Error! memory not allocated.");
	memset((*ent_l).sym, '\0', MAX_LENGTH_LABEL);
}










