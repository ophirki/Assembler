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
#define SRC_REG_STRT_BIT 8
#define SRC_REG_END_BIT 13
#define DST_REG_STRT_BIT 2
#define DST_REG_END_BIT 7
#define STRT_IMMED_NUM 2
#define END_IMMED_NUM 13
#define INF_STRT 100

extern sym_list *sym_t;
extern int inscnt;

int is_valid_line(char *s);
char * is_there_label(char *s);
char * int_to_str(int n);
int convert_dec_to_bin(int n);
int allocation_type(char *s);
char * bits_to_dots(char *s);
char * skip_label(char *s);
int translate_words_per_parameter(char *s, char *fname);
int translate_type_2_allocation(char *s, char *fname);
char * generate_a_word(char *word, int strt_bit, int end_bit, char *s);
char * whitespace_skip(char *s);
int is_order_line(char *s);
int is_valid_guide_line(char *s);
int is_label_in_table(char *label, sym_list *symb_list);
int label_location(char *label);
void put_counters(int instr_counter, int data_counter, char *fname);

/*The function returns the Type of the given line from an .as file.*/
int line_type(char *s){
	enum command{invalid, note, empty, guide, order};
	char *p, s_cpy[LINE_LEN];
	strncpy(s_cpy, s, LINE_LEN);
	/*Checking whether it's a note line or an empty line*/
	p = whitespace_skip(s_cpy);
	if(*p == ';')
		return note;
	if((*p == '\n') || !(*p))
		return empty;
	/*moving pointer if there is a label*/
	if (is_there_label(s_cpy)){
		char *temp_p = is_there_label(s_cpy);
		p = whitespace_skip(++temp_p);
	}
	/*Checking whether it's a guide line*/
	if(*p == '.')
		return guide;
	/*Checking whether it's an order line*/
	if (is_order_line(p))
		return order;	
	else
		return invalid;
}
/*skips the given pointer to the next non-whitespace char*/
char * whitespace_skip(char *s){
	char *p = s;
	while((*p == ' ') || (*p == '\t'))
		p++;
	return p;
}
/*Checking validity of the line before starting to proccess it NTM*/
int is_valid_line(char *s){
	enum command{invalid, note, empty, guide, order};
	int result_is_valid;
	switch(line_type(s))
	{
		case invalid:
			fprintf(stderr, "invalid line!\n");
			result_is_valid = 0;
			break;
		case note:
			result_is_valid = 1;
			break;
		case empty:
			result_is_valid = 1;
			break;
		case guide:
			result_is_valid = is_valid_guide_line(s);
			break;
		case order:
			result_is_valid = is_order_line(s);
			break;
	}
	return result_is_valid;
}

/*Checking if there is a label*/
char * is_there_label(char *s){
	char *p = strchr(s, ':');
	return p;
}

/*the function converts an integer number to a binary number represented in str*/
char * int_to_str(int n){
	int binaryNumber = convert_dec_to_bin(n), i = WORD_LEN - 1;
	char *binaryStr = malloc(WORD_LEN+1);
	if (binaryStr == NULL)
			fprintf(stderr, "Error! memory not allocated.");
	while (binaryNumber != 0){
		*(binaryStr + i) = (binaryNumber % 10) + '0';
		i--;
		binaryNumber = binaryNumber / 10;
	}
	while (i > -1){
		*(binaryStr + i) = '0';
		i--;
	}
	*(binaryStr + WORD_LEN) = '\0';
	return binaryStr;
}

/*converts a decimal into binary representaion*/
int convert_dec_to_bin(int n){
	int binaryNumber = 0, remainder, i = 1;
	/*2's complement if n is negative*/
	if (n < 0)
		n = MAX_NUM_IN_DATA_LIST + 1 + n;
	while (n!=0){
		remainder = n%2;
		n /= 2;
		binaryNumber += remainder*i;
		i *= 10;
	}
	return binaryNumber;
}

/*the function returns the allocation type number of the operand, returns -1 if invalid*/
int allocation_type(char *s){
	char *p = s; 
	static char temp_label[LINE_LEN];
	if((*p) == '#'){
		p++;
		if(((*p) == '-') || ((*p) == '+'))
			p++;
		for (;*p; p++){
			if (!isdigit(*p)){
				fprintf(stderr, "invalid order command. Can't use a non number after #\n");
				return -1;
				}
		}
		return TYPE_0_ALLOCATION;
	}
	else if(is_label_in_table(s, sym_t)){
		return TYPE_1_ALLOCATION;
	}
	else if(strchr(s, '(') && strchr(s, ')')){
		strncpy(temp_label, s, strchr(s, '(') - s);
		temp_label[strchr(s, '(') - s] = '\0';
		if (is_label_in_table(temp_label, sym_t))
			return TYPE_2_ALLOCATION;
		else{
			fprintf(stderr, "invalid order command. label not in table\n");
			return -1;
		}
	}
	
	else if ((*p) == 'r'){
		p++;
		if (((int)((*p) - '0') < 8) && ((int)((*p) - '0') > -1))
			return TYPE_3_ALLOCATION;
		else{
			fprintf(stderr, "invalid order command. no registry named r%c\n", *p);
			return -1;
		}
	}
	else{
		fprintf(stderr, "invalid allocation, operand unidentified.\n");
		return -1;
	}	
}

/*the function translates 1,0 to '/','.'*/
char * bits_to_dots(char *s){
	char *p;
	for (p = s; *p; p++){
		if ((*p) == '1')
			(*p) = '/';
		else if ((*p) == '0')
			(*p) = '.';
	}
	return s;
}
/*the function returns a pointer to the first argument after a label in a line*/
char * skip_label(char *s){
	char *p = s;
	if(is_there_label(s)){
		p = whitespace_skip((is_there_label(s)+1));
	}
	else{
		p = whitespace_skip(p);
	}
	return p;
}

/*the function translate each parameter to a machine code word */
int translate_words_per_parameter(char *s, char *fname){
	char *token, *parm1, *parm2, s_cpy[LINE_LEN], word1[WORD_LEN+1] = "00000000000000\0", word2[WORD_LEN+1] = "00000000000000\0";
	FILE *fd;
	fd = fopen(fname, "a");
	strcpy(s_cpy, s);
	token = strtok(s_cpy, " ,");
	parm1 = token;
	token = strtok(NULL, " ,");
	if (*parm1 == 'r')
		strncpy(word1, generate_a_word(word1, SRC_REG_STRT_BIT, SRC_REG_END_BIT, int_to_str((*(parm1 + 1)) - '0')), WORD_LEN + 1);
	/*if there are 2 parmeters*/
	if(token){
		parm2 = token;
		/*if the parms are both registers*/
		if ((*parm1 != 'r') && (*parm2 == 'r'))
			strncpy(word2, generate_a_word(word2, SRC_REG_STRT_BIT, SRC_REG_END_BIT, int_to_str((*(parm2+1)) - '0')), WORD_LEN + 1);
		else if ((*parm1 == 'r') && (*parm2 == 'r'))
			strncpy(word1, generate_a_word(word1, DST_REG_STRT_BIT, DST_REG_END_BIT, int_to_str((*(parm2+1)) - '0')), WORD_LEN);
		else if (*parm2 == '#')
			strncpy(word2, generate_a_word(word2,STRT_IMMED_NUM, END_IMMED_NUM, int_to_str((*(parm2+1)) - '0')), WORD_LEN);
		else if(is_label_in_table(parm2, sym_t))
			strncpy(word2, bits_to_dots(int_to_str(label_location(parm2))), WORD_LEN);
		else {
			fprintf(stderr, "label is not in table (parm2)\n");
			return 0;
		}
	}
	if ((*parm1 != 'r') && (*parm1 == '#'))
		strncpy(word1, generate_a_word(word1,STRT_IMMED_NUM, END_IMMED_NUM, int_to_str((*(parm1 + 1)) - '0')), WORD_LEN + 1);
	else if((*parm1 != 'r') && (is_label_in_table(parm1, sym_t)))
		strncpy(word1, int_to_str(label_location(parm1)), WORD_LEN);
	else if((*parm1 != 'r') && (!is_label_in_table(parm1, sym_t))){
		fprintf(stderr, "label is not in table (parm1)\n");
		return 0;
	}
	if (strcmp(word1, "00000000000000")){
		fprintf(fd, "%s\n", bits_to_dots(word1));
		inscnt++;
		}
	if (strcmp(word2, "00000000000000")){
		fprintf(fd, "%s\n", bits_to_dots(word2));
		inscnt++;
		}
	fclose(fd);
	return 1;
}

/*the function recieves a type 2 allocation parmeters and  translate them to words in machine code */
int translate_type_2_allocation(char *s, char *fname){
	char * token, s_cpy[LINE_LEN], label[LABEL_LEN], *parm2, *parm3, word1[WORD_LEN+1] = "00000000000000\0", word2[WORD_LEN+1] = "00000000000000\0", word3[WORD_LEN+1] = "00000000000000\0";
	FILE *fd;
	fd = fopen(fname, "a");
	strncpy(s_cpy, s, LINE_LEN);
	token = strtok(s_cpy, "( ,)");
	strncpy(label, token, LABEL_LEN);
	if(!is_label_in_table(label, sym_t)){
		fprintf(stderr, "label is not in table\n");
		return 0;
	}
	else{
		strncpy(word1, int_to_str(label_location(label)), WORD_LEN);
		token = strtok(NULL, "( ,)");
		parm2 = token;
		if (*parm2 == '#')
			strncpy(word2, generate_a_word(word2,STRT_IMMED_NUM, END_IMMED_NUM, int_to_str((*(parm2 + 1)) - '0')), WORD_LEN + 1);
		else if (*parm2 == 'r'){
			strncpy(word2, generate_a_word(word2, SRC_REG_STRT_BIT, SRC_REG_END_BIT, int_to_str((*(parm2 + 1)) - '0')), WORD_LEN + 1);
		}
		else if (is_label_in_table(parm2, sym_t)){
			strncpy(word2, int_to_str(label_location(parm2)), WORD_LEN);
		}		
		token = strtok(NULL, "( ,)");
		parm3 = token;
		if (*parm3 == '#')
			strcpy(word3, generate_a_word(word3,STRT_IMMED_NUM, END_IMMED_NUM, int_to_str((*(parm3 + 1)) - '0')));
		else if ((*parm3 == 'r') && (*parm2 == 'r'))
			strncpy(word2, generate_a_word(word2, DST_REG_STRT_BIT, DST_REG_END_BIT, int_to_str((*(parm3 + 1)) - '0')), WORD_LEN + 1);
		else if ((*parm3 == 'r') && (*parm2 != 'r'))
			strncpy(word3, generate_a_word(word3, DST_REG_STRT_BIT, DST_REG_END_BIT, int_to_str((*(parm3 + 1)) - '0')), WORD_LEN + 1);
		else if (is_label_in_table(parm3, sym_t)){
			strncpy(word3, int_to_str(label_location(parm3)), WORD_LEN);
		}
		fprintf(fd, "%s\n", bits_to_dots(word1));
		inscnt++;
		if (strcmp(word2, "00000000000000")){
			fprintf(fd, "%s\n", bits_to_dots(word2));
			inscnt++;
			}
		if (strcmp(word3, "00000000000000")){
			fprintf(fd, "%s\n", bits_to_dots(word3));
			inscnt++;
			}
	}
	fclose(fd);
	return 1;
}

/*the function places the counters in the right place in the output file (i know the second read should do it but i have no time to fix it, ani mitnatsel!)*/
void put_counters(int instr_counter, int data_counter, char *fname){
	FILE *fd, *fd_tmp;
	char *tmpf = "tmpfile.tmp", new_line[LINE_LEN];
	int i = 1;
	fd = fopen(fname, "r");
	fd_tmp = fopen(tmpf, "a");
	fseek(fd, 0, SEEK_SET);
	while(fgets(new_line, LINE_LEN, fd))
		fprintf(fd_tmp, "%s", new_line);
	fclose(fd);
	fclose(fd_tmp);
	fd_tmp = fopen(tmpf, "r");
	fd = fopen(fname, "w");
	while(fgets(new_line, LINE_LEN, fd_tmp)){
		if (i == INF_STRT - 1)
			fprintf(fd, "%d %d\n", instr_counter, data_counter);
		else{
			fprintf(fd, "%s", new_line);
		}
		i++;
	}
	fclose(fd_tmp);
	remove(tmpf);
	fclose(fd);
}

/*the function puts the given string (s) between the start and end bits in the given word*/
char * generate_a_word(char *word, int strt_bit, int end_bit, char *s){
	char * new_word = malloc(WORD_LEN+1);
	int i, j;
	if (new_word == NULL)
		fprintf(stderr, "Error! memory not allocated.");
	strncpy(new_word, word, WORD_LEN);
	for (j = strlen(s) - 1, i = WORD_LEN - strt_bit - 1; i > WORD_LEN - end_bit - 1; i--, j--)
		*(new_word+i) = *(s + j);
	*(new_word + WORD_LEN) = '\0';
	return new_word;
}
/*the function gains bits and concatenates them to a word in the language*/
char * bits_to_word(char * type_2_allocation_bits, char * opcode_in_str, char * src_operand_in_str, char * dst_operand_in_str, char * ARE_in_str){
	char * new_word = malloc(WORD_LEN+1), *p;
	p = new_word;
	if (new_word == NULL)
		fprintf(stderr, "Error! memory not allocated.");
	/*copying the bits into the new word in the right order*/
	strncpy(p, type_2_allocation_bits, TYPE_2_ALLOCATION_LEN);
	p += TYPE_2_ALLOCATION_LEN;
	strncpy(p, opcode_in_str, OPCODE_LEN);
	p += OPCODE_LEN;
	strncpy(p, src_operand_in_str, SRC_OP_LEN);
	p += SRC_OP_LEN;
	strncpy(p, dst_operand_in_str, DST_OP_LEN);
	p += DST_OP_LEN;
	strncpy(p, ARE_in_str, ARE_LEN);
	p += ARE_LEN;
	*(new_word + (WORD_LEN)) = '\n';
	return new_word;
}























