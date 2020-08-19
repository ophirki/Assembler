#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "data_structs.h"
#define LINE_LEN 81
#define LABEL_LEN 31
#define RES_WORDS 32
#define AS_COM_LEN 16
#define DATA_LEN 5
#define STRING_LEN 7
#define EXTERN_LEN 7
#define ENTRY_LEN 6
#define MAX_NUM_IN_DATA_LIST 8192
#define SHORT_AS_COM_LEN 3
#define TYPE_2_ALLOCATION 2
extern sym_list *sym_t;
extern data_list *data_l;

char* whitespace_skip(char *s);
int is_label_in_table(char *label, sym_list *sym_list);
int is_valid_data_command(char *p);
int is_valid_string_command(char *p);
int is_valid_extern_command(char *p);
int is_valid_entry_command(char *p);
char* is_there_label(char *s);
int is_valid_0_operand_command(char *s);
int is_valid_1_operand_command(char *s);
int is_valid_2_operands_command(char *s);
int allocation_type(char *s);

/*Checking validation of a given label (s is start of label, may contain more than just the label*/
int is_valid_label(char *s){
	int i;
	char *p = whitespace_skip(s);
	char *end_label = strchr(p, ':');
	char label[LABEL_LEN] = {'\0'};
	char * reserved_words[RES_WORDS] = {"auto", "break", "case", "char", "const", "continue", "default", "do", "double", "else", "enum", "extern", "float", "for", "goto", "if", "int", "long", "register", "return", "short", "signed", "sizeof", "static", "struct", "switch", "typedef", "union", "unsigned", "void", "volatile", "while"};
	/*if label is too long*/
	if ((end_label - p) > LABEL_LEN)
		return 0;
	/*if label doesn't start with an alphabet letter*/
	if (!isalpha(*p))
		return 0;
	memmove(label, p, end_label-p);
	/*if label isn't a reserved word*/
	for(i=0; i< RES_WORDS; i++){
		if(!strcmp(label, reserved_words[i]))
			return 0;
	}
	return 1;	
}

/*The function checks whether the label is in the symbol table*/
int is_label_in_table(char *label, sym_list *symb_list){
	sym_list *lp;
	for(lp = symb_list; (*lp).sym; lp = (*lp).next){
		if(!strcmp(label, (*lp).sym))
			return 1;
	}
	return 0;
}

/*The function checks where the given line is an order line*/
int is_order_line(char *s){
	char *as_com[AS_COM_LEN] = {"mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"}, *p;
	int i, is_assembly_com = 0, opcode_of_command, result_is_valid;
	p = whitespace_skip(s);
	if (is_there_label(p)){
		p = is_there_label(p) + 1;
		p = whitespace_skip(p);
	}
	/*checks if the assembly command is valid*/
	for (i = 0; i < AS_COM_LEN; i++){
		if (!strncmp(p, as_com[i], strlen(as_com[i]))){
			is_assembly_com = 1;
			opcode_of_command = i;
			}
	}
	if (!is_assembly_com){
		fprintf(stderr, "invalid order line. not an assembly command.\n");
		return 0;
	}
	else{
		switch (opcode_of_command){
			case 0:
				result_is_valid = is_valid_2_operands_command(s);
				break;
			case 1:
				result_is_valid = is_valid_2_operands_command(s);
				break;
			case 2:
				result_is_valid = is_valid_2_operands_command(s);
				break;
			case 3:
				result_is_valid = is_valid_2_operands_command(s);
				break;
			case 4:
				result_is_valid = is_valid_1_operand_command(s);
				break;
			case 5:
				result_is_valid = is_valid_1_operand_command(s);	
				break;
			case 6:
				result_is_valid = is_valid_2_operands_command(s);
				break;
			case 7:
				result_is_valid = is_valid_1_operand_command(s);
				break;	
			case 8:
				result_is_valid = is_valid_1_operand_command(s);
				break;
			case 9:
				result_is_valid = is_valid_1_operand_command(s);
				break;
			case 10:
				result_is_valid = is_valid_1_operand_command(s);
				break;
			case 11:
				result_is_valid = is_valid_1_operand_command(s);
				break;
			case 12:
				result_is_valid = is_valid_1_operand_command(s);
				break;
			case 13:
				result_is_valid = is_valid_1_operand_command(s);
				break;
			case 14:
				result_is_valid = is_valid_0_operand_command(s);
				break;
			case 15:
				result_is_valid = is_valid_0_operand_command(s + 1);
				break;				
		}
		return result_is_valid;
	}
	
}

/*the function gains a string and returns if it is an integer*/
int is_an_integer(char *s){
	int is_int = 1;
	if ((*s == '-') || (*s == '+') || (isdigit(*s)))
		s++;
	while((*s != '\n') && (*s != '\0') && s && is_int){
		is_int = isdigit(*s);
		s++;
	}	
	return is_int;
}

/*the function returns a non-zero if the guide line is valid*/
int is_valid_guide_line(char *s){
	char *p = s;
	char * guide_words[] = {".data", ".string", ".extern", ".entry"};
	p = whitespace_skip(s);
	if (is_there_label(p)){
		if (!is_valid_label(p)){
			fprintf(stderr, "invalid guide command. not a valid label.\n");
			return 0;
		}
		p = is_there_label(p) + 1;
		p = whitespace_skip(p);
	}
	if (!strncmp(p, guide_words[0], DATA_LEN))
		return is_valid_data_command(p);
	else if (!strncmp(p, guide_words[1], STRING_LEN))
		return is_valid_string_command(p);
	else if (!strncmp(p, guide_words[2], EXTERN_LEN))
		return is_valid_extern_command(p);
	else if (!strncmp(p, guide_words[3], ENTRY_LEN))
		return is_valid_entry_command(p);
	else{
		fprintf(stderr, "not a valid guide line. no valid guide word.\n");
		return 0;
	}
		
}

/*the function returns a non-zero if the data command is valid*/
int is_valid_data_command(char *s){
	/*numbers_minus_commas is the substraction of the number of commas seen from the number of numbers seen in the command*/
	int numbers_minus_commas = 0, num_from_data;
	char *token, *p = s, s_cpy[LINE_LEN];
	p += DATA_LEN;
	p = whitespace_skip(p);
	/*Commas check*/
	while(*p){
		if(*p == ','){
			numbers_minus_commas--;
			p++;
		}
		else if(isdigit(*p)){
			numbers_minus_commas++;
			while(isdigit(*p))
				p++;
		}
		else
			p++;
		if(numbers_minus_commas > 1){
			fprintf(stderr, "invalid data command. missing commas in data command.\n");
			return 0;
		}
		if(numbers_minus_commas < 0){
			fprintf(stderr, "invalid data command. too many commas in data command.\n");
			return 0;
		}
	}
	if (numbers_minus_commas != 1){
		fprintf(stderr, "invalid data command. the substraction of the number of commas seen from the number of numbers seen in the command does not equal 1\n");
		return 0;
	}
	p = s + DATA_LEN;
	strcpy(s_cpy, p);
	token = strtok(s_cpy, " ,");
	while(token != NULL){
		/*is token is integer*/
		if (!is_an_integer(token)){
			fprintf(stderr, "invalid data command. the string: %s is not a valid number\n", token);
			return 0;
		}
		num_from_data = atoi(token);
		/*is the number in range*/
		if ((num_from_data >= MAX_NUM_IN_DATA_LIST) || (num_from_data <= -MAX_NUM_IN_DATA_LIST)){
			fprintf(stderr, "invalid data command. number: %d is out of range\n", num_from_data);
			return 0;
		}
		token = strtok(NULL, " ,");
	}
	return 1;
}

/*the function returns a non-zero if the string command is valid*/
int is_valid_string_command(char *p){
	p += STRING_LEN;
	p = whitespace_skip(p);
	if (*p != '"'){
		fprintf(stderr, "invalid string command. the char: %c, is invalid parameter for string guide line to start with\n", *p);
		return 0;
	}
	else{
		while(*p != '"'){
			if(*p == '\n'){
				fprintf(stderr, "invalid string command.the string in the string guide line doesn't end with \"\n");
				return 0;
			}
			p++;
		}
	}
	return 1;
}

/*the function returns a non-zero if the extern command is valid*/
int is_valid_extern_command(char *p){
	char p_cpy[LABEL_LEN+1], *p2;
	strncpy(p_cpy, p, LABEL_LEN);
	p2 = p_cpy;
	p2 += EXTERN_LEN;
	p2 = whitespace_skip(p2);
	while((*p2 != '\n')&&(*p2 != '\0')){
		p2++;
	}
	*p2 = ':';
	*(p2+1) = '\0';
	p2 = p_cpy;
	p2 += ENTRY_LEN;
	p2 = whitespace_skip(p2);
	if(is_valid_label(p2))
		return 1;
	else{
		fprintf(stderr, "invalid extern command. invalid label given.\n");
		return 0;
	}
		
}
/*the function returns a non-zero if the entry command is valid*/
int is_valid_entry_command(char *p){
	char p_cpy[LABEL_LEN+1], *p2;
	strncpy(p_cpy, p, LABEL_LEN);
	p2 = p_cpy;
	p2 += ENTRY_LEN;
	p2 = whitespace_skip(p2);
	while((*p2 != '\n')&&(*p2 != '\0')){
		p2++;
	}
	*p2 = ':';
	*(p2+1) = '\0';
	p2 = p_cpy;
	p2 += ENTRY_LEN;
	p2 = whitespace_skip(p2);
	if(is_valid_label(p2))
		return 1;
	else{
		fprintf(stderr, "invalid entry command. invalid label given.\n");
		return 0;
	}
}

/*the function returns a non-zero if there are 2 valid operands in this command*/
int is_valid_2_operands_command(char *s){
	char *p = s + SHORT_AS_COM_LEN, *token, s_cpy[LINE_LEN];
	int operands_minus_commas = 0, number_of_operands = 0;
	p = whitespace_skip(p);
	/*Commas check*/
	while((*p != '\n')&&(*p)){
		if(*p == ','){
			operands_minus_commas--;
			p++;
		}
		else if(((*p) != ' ')&&((*p) != '\t')){
			operands_minus_commas++;
			p++;
			while(((*p) != ' ')&&((*p) != '\t') &&((*p) != ',')&&((*p) != '\n')&&(*p))
				p++;
		}
		else
			p++;
		if(operands_minus_commas > 1){
			fprintf(stderr, "invalid order command. missing commas in order command.\n");
			return 0;
		}
		if(operands_minus_commas < 0){
			fprintf(stderr, "invalid order command. too many commas in order command.\n");
			return 0;
		}
	}
	if (operands_minus_commas != 1){
		fprintf(stderr, "invalid order command. the substraction of the number of commas seen from the number of operands seen in the command does not equal 1\n");
		return 0;
	}
	/*check the number of operands*/
	p = s + SHORT_AS_COM_LEN;
	p = whitespace_skip(p);
	strcpy(s_cpy, p);
	token = strtok(s_cpy, " ,");
	while(token != NULL){
		number_of_operands++;
		token = strtok(NULL, " ,");
		}
	if(number_of_operands != 2){
		fprintf(stderr, "invalid order command. should be 2 operands for this command.\n");
		return 0;
	}
	return 1;
}

/*the function returns a non-zero if there is 1 valid operand in this command*/
int is_valid_1_operand_command(char *s){
	char *p = s, *token;
	int number_of_operands = 0;
	p = whitespace_skip(p);
	if (is_there_label(p)){
		p = is_there_label(p) + 1;
		p = whitespace_skip(p);
	}
	p = p +  SHORT_AS_COM_LEN;
	p = whitespace_skip(p);
	if(strchr(p, '(') && strchr(p, ')')){
		token = strtok(p, " (,)");
		while(token != NULL){
			number_of_operands++;
			token = strtok(NULL, " (,)");
			}
		/*to reduce the inner parmeters (and the \n) in type 2 allocation*/
		number_of_operands -= 3;
		if(number_of_operands != 1){
			fprintf(stderr, "invalid order command. should be 1 operand for this command.\n");
			return 0;
		}
	}
	else{
		/*check the number of operands*/
		token = strtok(p, " ,");
		while(token != NULL){
			number_of_operands++;
			token = strtok(NULL, " ,");
			}
		if(number_of_operands != 1){
			fprintf(stderr, "invalid order command. should be 1 operand for this command.\n");
			return 0;
		}
	}
	return 1;
}

/*the function returns a non-zero if there is no operand in this command*/
int is_valid_0_operand_command(char *s){
	char *p = s + SHORT_AS_COM_LEN;
	p = whitespace_skip(p);
	if (is_there_label(p)){
		p = is_there_label(p) + 1;
		p = whitespace_skip(p);
	}
	/*check the number of operands*/
	if((*p)&&((*p) != '\n')){
		fprintf(stderr, "invalid order command. should be no operand for this command.\n");
		return 0;
	}
	return 1;
}












