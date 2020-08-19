#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "data_structs.h"
#define LINE_LEN 81
#define INF_STRT 100
extern int datacnt;
extern int inscnt;

int line_type(char *s);
int is_valid_line(char *s);
void second_read_line(char *s, int line_number);
void third_read_line(char *s, int line_number, char *output_file);
char * whitespace_skip(char *s);
void print_entries_to_file(char * fname);
void print_externals_to_file(char * fname);
void print_data_to_file(char * fname);
void put_counters(int instr_counter, int data_counter, char *fname);

/*the function gains the name of the source file and converts it to machine code*/
void assembler(char *src_file){
	char new_line[LINE_LEN], *fname = malloc(sizeof(src_file)), *output_fname = malloc(sizeof(src_file)), *ent_fname = malloc(sizeof(src_file)), *ext_fname = malloc(sizeof(src_file));
	int line_number = 0, is_valid_file = 1, i;
	FILE *fd, *fd2;
	strcpy(fname, src_file);
	strcpy(output_fname, src_file);
	strcpy(ent_fname, src_file);
	strcpy(ext_fname, src_file);
	output_fname = strcat(output_fname, ".ob");
	fname = strcat(fname, ".as");
	ent_fname = strcat(ent_fname, ".ent");
	ext_fname = strcat(ext_fname, ".ext");
	fd = fopen(fname, "r");
	/*reading from the file line by line*/
	if (!fd){
		fprintf(stderr, "Couldn't open the file.\n");
	}
	else {
		/*First read of the input: checking if valid.*/
		fseek(fd, 0, SEEK_SET);
		while(fgets(new_line, LINE_LEN, fd)){
			if (!is_valid_line(new_line))
				is_valid_file = 0;
		}
		if (is_valid_file){
			/*Second read of the input: collecting labels*/
			fseek(fd, 0, SEEK_SET);
			while(fgets(new_line, LINE_LEN, fd)){
				line_number++;
				second_read_line(new_line, line_number);
			}
			/*Third read of the input: translating to machine code*/
			fd2 = fopen(output_fname, "a");
			for (i=1; i < INF_STRT; i++)
				fprintf(fd2, "\n");
			fclose(fd2);
			fseek(fd, 0, SEEK_SET);
			while(fgets(new_line, LINE_LEN, fd)){
				line_number++;
				printf("%s\n", new_line);
				third_read_line(new_line, line_number, output_fname);
			}
			print_data_to_file(output_fname);
			put_counters(inscnt, datacnt, output_fname);
			print_entries_to_file(ent_fname);
			print_externals_to_file(ext_fname);
		}
	}
	fclose(fd); /*closing the file*/
	free(fname);
}

