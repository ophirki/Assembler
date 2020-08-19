#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "data_structs.h"
#define LINE_LEN 81
#define LABEL_LEN 31
#define DATA_LEN 5
#define STRING_LEN 7
#define EXTERN_LEN 7
#define ENTRY_LEN 6
#define MAX_NUM_IN_DATA_LIST 8192
#define RELOCATABLE_TYPE 'r'
#define EXTERNAL_TYPE 'e'
#define ENTRY_TYPE 'a'
#define EXTERNAL_LAB_VAL 0
#define WORD_LEN 14
#define AS_COM_LEN 16
#define OPCODE_LEN 4
#define ARE_LEN 2
#define DST_OP_LEN 2
#define SRC_OP_LEN 2
#define TYPE_2_ALLOCATION_LEN 4
#define SHORT_AS_COM_LEN 3
#define TYPE_0_ALLOCATION 0
#define TYPE_1_ALLOCATION 1
#define TYPE_2_ALLOCATION 2
#define TYPE_3_ALLOCATION 3
extern sym_list *sym_t;
extern int datacnt;
extern int inscnt;

int line_type(char *s);
int is_valid_line(char *s);
int is_order_line(char *s);
char* is_there_label(char *s);
char* whitespace_skip(char *s);
int is_valid_guide_line(char *s);
void second_read_guide(char *s, int line_number);
void second_read_order(char *s, int line_number);
int is_valid_label(char *s);
int is_an_integer(char *s);
void data_guide_line(char *p);
void string_guide_line(char *p);
void extern_guide_line(char *p, int line_num);
void entry_guide_line(char *p, int line_num);
int convert_dec_to_bin(int n);
int translate_2_operands_command(char *s, int opcode_of_command, char *fname);
int translate_1_operand_command(char *s, int opcode_of_command, char *fname);
int translate_0_operand_command(char *s, int opcode_of_command, char *fname);
int allocation_type(char *s);
int translate_line(char *s, int line_number, char *fname);
int translate_order_line(char *s, int line_number, char *fname);
char * skip_label(char *s);
char * int_to_str(int n);
char * bits_to_dots(char *s);
int translate_words_per_parameter(char *s, char *fname);
int translate_type_2_allocation(char *s, char *fname);
char * bits_to_word(char * type_2_allocation_bits, char * opcode_in_str, char * src_operand_in_str, char * dst_operand_in_str, char * ARE_in_str);


/*Gains a Line, saving required info before translating. (according to type)*/
void second_read_line(char *s, int line_number){
	enum command{invalid, note, empty, guide, order};
	switch(line_type(s))
	{
		case invalid:
			fprintf(stderr, "invalid line!\n");
			break;
		case note:
			break;
		case empty:
			break;
		case guide:
			second_read_guide(s, line_number);
			break;
		case order:
			second_read_order(s, line_number);
			break;
	}
}
/*the function translates the given line and puts it in the output file*/
void third_read_line(char *s, int line_number, char *output_file){
	translate_line(s, line_number, output_file);
}

/*Gains a guide type line and saving the required info before translating - HHHHHHHHHHHHHHH*/
void second_read_order(char *s, int line_number){
	char *p = s;
	char label[LABEL_LEN];
	/*Checking if there is a label*/
	if(is_there_label(s)){
		p = whitespace_skip(s);
		if (!is_valid_label(p)){
			fprintf(stderr, "not a valid label. line ignored.\n");
		}
		/*adding label to symbol table*/
		else{
			strncpy(label, p, strchr(p, ':')-p);
			label[strchr(p, ':')-p] = '\0';
			add_to_sym_tab(label, line_number, RELOCATABLE_TYPE);
		}		
	}
}

/*Gains a guide type line and saving the required info before translating*/
void second_read_guide(char *s, int line_number){
	int label_to_add = 0;
	char *p = s;
	char label[LABEL_LEN];
	char * guide_words[] = {".data", ".string", ".extern", ".entry"};
	/*Checking if there is a label*/
	if(is_there_label(s)){
		p = whitespace_skip(s);
		if (!is_valid_label(p)){
			fprintf(stderr, "not a valid label. line ignored.\n");
		}
		/*adding label to symbol table*/
		else{
			strncpy(label, p, strchr(p, ':')-p);
			label[strchr(p, ':')-p] = '\0';
			/*a flag to remember to add the label (can't add now because don't know the type yet (A,R,O)*/
			label_to_add = 1;
			p = skip_label(p);
		}
			
	}
	p = whitespace_skip(p);
	/*checking for guide word and calling the relavant function to continue. adding a label if it was found earlier.*/
	if (!strncmp(p, guide_words[0], DATA_LEN)){
		if (label_to_add)
			add_to_sym_tab(label, line_number, RELOCATABLE_TYPE);	
		data_guide_line(p);
	}
	else if (!strncmp(p, guide_words[1], STRING_LEN)){
		if (label_to_add)
			add_to_sym_tab(label, line_number, RELOCATABLE_TYPE);
		string_guide_line(p);
	}
	else if (!strncmp(p, guide_words[2], EXTERN_LEN))
		extern_guide_line(p, line_number);
	else if (!strncmp(p, guide_words[3], ENTRY_LEN))
		entry_guide_line(p, line_number);
	else
		fprintf(stderr, "%s is invalid guide word.\n", p);		
}


/*adding the numbers in data guide line to the data list*/
void data_guide_line(char *p){
	char *token;
	p += DATA_LEN;
	token = strtok(p, " ,");
	while (token != NULL){
		add_to_data_list(atoi(token));
		token = strtok(NULL, " ,");		
	}
}
/*adding the strings in string guide line to the data list*/
void string_guide_line(char *p){
	int ascii_of_char;
	p += STRING_LEN;
	p = whitespace_skip(p);
	while(*p != '"'){
		ascii_of_char = (int)(*p);
		add_to_data_list(ascii_of_char);
		p++;
	}
	add_to_data_list('\0');
}

/*declaring a label will be used from an external file*/
void extern_guide_line(char *p, int line_num){
	char label[LABEL_LEN];
	p += EXTERN_LEN;
	p = whitespace_skip(p);
	strncpy(label, p, strchr(p, '\n')-p);
	*(label + (strchr(p, '\n')-p)) = '\0';
	add_to_sym_tab(label, line_num, EXTERNAL_TYPE);
}

/*declaring a lable can be used from this file in others*/
void entry_guide_line(char *p, int line_num){
	char label[LABEL_LEN];
	p += ENTRY_LEN;
	p = whitespace_skip(p);
	strncpy(label, p, strchr(p, '\n')-p);
	*(label + (strchr(p, '\n')-p)) = '\0';
	add_to_entries_list(label, line_num);
}


/*the function translate the line (if order line) to machine code words*/
int translate_line(char *s, int line_number, char *fname){
	enum command{invalid, note, empty, guide, order};
	int result_is_translated = 1;
	switch(line_type(s))
	{
		case invalid:
			fprintf(stderr, "invalid line!\n");
			break;
		case note:
			break;
		case empty:
			break;
		case guide:
			break;
		case order:
			result_is_translated = translate_order_line(s, line_number, fname);
			break;
	}
	return result_is_translated;
}
/*the function translates the ordr line according to the number of parmeters*/
int translate_order_line(char *s, int line_number, char *fname){
	char *p = s, *as_com[AS_COM_LEN] = {"mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"};
	int i, result_is_valid, opcode_of_command;
	p = skip_label(p);
	/*checks which assembly command is in the line*/
	for (i = 0; i < AS_COM_LEN; i++){
		if (!strncmp(p, as_com[i], strlen(as_com[i])))
			opcode_of_command = i;
	}
	switch (opcode_of_command){
		case 0:
			result_is_valid = translate_2_operands_command(s, opcode_of_command, fname);
			break;
		case 1:
			result_is_valid = translate_2_operands_command(s, opcode_of_command, fname);
			break;
		case 2:
			result_is_valid = translate_2_operands_command(s, opcode_of_command, fname);
			break;
		case 3:
			result_is_valid = translate_2_operands_command(s, opcode_of_command, fname);
			break;
		case 4:
			result_is_valid = translate_1_operand_command(s, opcode_of_command, fname);
			break;
		case 5:
			result_is_valid = translate_1_operand_command(s, opcode_of_command, fname);	
			break;
		case 6:
			result_is_valid = translate_2_operands_command(s, opcode_of_command, fname);
			break;
		case 7:
			result_is_valid = translate_1_operand_command(s, opcode_of_command, fname);
			break;	
		case 8:
			result_is_valid = translate_1_operand_command(s, opcode_of_command, fname);
			break;
		case 9:
			result_is_valid = translate_1_operand_command(s, opcode_of_command, fname);
			break;
		case 10:
			result_is_valid = translate_1_operand_command(s, opcode_of_command, fname);
			break;
		case 11:
			result_is_valid = translate_1_operand_command(s, opcode_of_command, fname);
			break;
		case 12:
			result_is_valid = translate_1_operand_command(s, opcode_of_command, fname);
			break;
		case 13:
			result_is_valid = translate_1_operand_command(s, opcode_of_command, fname);
			break;
		case 14:
			result_is_valid = translate_0_operand_command(s, opcode_of_command, fname);
			break;
		case 15:
			result_is_valid = translate_0_operand_command(s + 1, opcode_of_command, fname);
			break;				
	}
	return result_is_valid;	
}

/*the function translates a 2 operand commands to machine code*/
int translate_2_operands_command(char *s, int opcode_of_command, char *fname){/*HHHHHHHHHHHHHHHH*/
	char *token, *p, s_cpy[LINE_LEN];
	/*all the bits in the word (named by their meaning)*/
	char opcode_in_str[OPCODE_LEN], ARE_in_str[ARE_LEN], dst_operand_in_str[DST_OP_LEN], src_operand_in_str[SRC_OP_LEN], type_2_allocation_bits[TYPE_2_ALLOCATION_LEN];
	FILE *fd;
	fd = fopen(fname, "a");
	strcpy(s_cpy, s);
	p = skip_label(s_cpy);
	p = p + SHORT_AS_COM_LEN;
	p = whitespace_skip(p);
	/*bits 6-9*/
	strcpy(opcode_in_str, int_to_str(opcode_of_command) + (WORD_LEN - OPCODE_LEN));
	/*bits 4-5*/
	token = strtok(p, " ,");
	if (allocation_type(token) == -1){
		return -1;
	}
	else if(allocation_type(token) == TYPE_2_ALLOCATION){
		fprintf(stderr, "invalid order command. can't be type 2 allocation for command %d", opcode_of_command);
		return -1;
	}
	else
		strncpy(src_operand_in_str, int_to_str(allocation_type(token)) + (WORD_LEN - SRC_OP_LEN), SRC_OP_LEN);
	/*bits 2-3*/
	token = strtok(NULL, " ,");
	if (allocation_type(token) == -1){
		return -1;
	}
	else if(allocation_type(token) == TYPE_2_ALLOCATION){
		fprintf(stderr, "invalid order command. can't be type 2 allocation for command %d", opcode_of_command);
		return -1;
	}		
	else
		strncpy(dst_operand_in_str, int_to_str(allocation_type(token)) + (WORD_LEN - DST_OP_LEN), DST_OP_LEN);
	/*bits 10-13*/
	strncpy(type_2_allocation_bits, "0000", TYPE_2_ALLOCATION_LEN);
	/*bits 0-1*/
	strncpy(ARE_in_str, "00", ARE_LEN);
	fprintf(fd, "%s", bits_to_dots(bits_to_word(type_2_allocation_bits, opcode_in_str, src_operand_in_str, dst_operand_in_str, ARE_in_str)));
	inscnt++;
	fclose(fd);
	/*appoint p to the parmeters*/
	strcpy(s_cpy, s);
	p = skip_label(s_cpy);
	p = p + SHORT_AS_COM_LEN;
	p = whitespace_skip(p);
	translate_words_per_parameter(p, fname);
	return 1;
}

/*the function translates a 2 operand commands to machine code*/
int translate_1_operand_command(char *s, int opcode_of_command, char *fname){
	char *parm2, *parm3, *p, s_cpy[LINE_LEN];
	/*all the bits in the word (named by their meaning)*/
	char opcode_in_str[OPCODE_LEN], ARE_in_str[ARE_LEN],src_operand_in_str[SRC_OP_LEN],  dst_operand_in_str[DST_OP_LEN], type_2_allocation_bits[TYPE_2_ALLOCATION_LEN];
	FILE *fd;
	fd = fopen(fname, "a");
	strcpy(s_cpy, s);
	p = skip_label(s_cpy);
	p = p + SHORT_AS_COM_LEN;
	p = whitespace_skip(p);
	/*bits 6-9*/
	strncpy(opcode_in_str, int_to_str(opcode_of_command) + (WORD_LEN - OPCODE_LEN), OPCODE_LEN);
	/*bits 0-1*/
	strncpy(ARE_in_str, "00", ARE_LEN);
	/*bits 2-3*/
	strncpy(dst_operand_in_str, int_to_str(allocation_type(p))+ (WORD_LEN - DST_OP_LEN), DST_OP_LEN);
	/*bits 4-5*/
	strncpy(src_operand_in_str, "00", SRC_OP_LEN);
	if (allocation_type(p) == -1){
		fclose(fd);
		return -1;
	}
	else if(allocation_type(p) == TYPE_2_ALLOCATION){
		strtok(p, "( ,)");
		parm2 = strtok(NULL, "( ,)");
		parm3 = strtok(NULL, "( ,)");
		/*bits 10-13*/
		strncpy(type_2_allocation_bits, strcat(int_to_str(allocation_type(parm2)) + (WORD_LEN - SRC_OP_LEN), int_to_str(allocation_type(parm3)) + (WORD_LEN - DST_OP_LEN)), TYPE_2_ALLOCATION_LEN);
		fprintf(fd, "%s", bits_to_dots(bits_to_word(type_2_allocation_bits, opcode_in_str, src_operand_in_str, dst_operand_in_str, ARE_in_str)));
		inscnt++;
		fclose(fd);
		strcpy(s_cpy, s);
		p = skip_label(s_cpy);
		p = p + SHORT_AS_COM_LEN;
		p = whitespace_skip(p);
		translate_type_2_allocation(p, fname);
	}
	else {
		/*bits 10-13*/
		strcpy(type_2_allocation_bits, "0000");
		fprintf(fd, "%s", bits_to_dots(bits_to_word(type_2_allocation_bits, opcode_in_str, src_operand_in_str, dst_operand_in_str, ARE_in_str)));
		inscnt++;
		fclose(fd);
		translate_words_per_parameter(p, fname);
	}
	return 1;
}

/*the function translates a 2 operand commands to machine code*/
int translate_0_operand_command(char *s, int opcode_of_command, char *fname){
	/*all the bits in the word (named by their meaning)*/
	char opcode_in_str[OPCODE_LEN];
	FILE *fd;
	fd = fopen(fname, "a");
	strncpy(opcode_in_str, int_to_str(opcode_of_command) + (WORD_LEN - OPCODE_LEN), OPCODE_LEN);
	fprintf(fd, "%s", bits_to_dots(bits_to_word("0000", opcode_in_str, "00", "00", "00")));
	inscnt++;
	fclose(fd);
	return 1;
}
















