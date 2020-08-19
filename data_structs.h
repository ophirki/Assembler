#ifndef data_structs_h
#define data_structs_h
#define MAX_LENGTH_LABEL 31

typedef struct symbol_table{
	char sym[MAX_LENGTH_LABEL];
	int value; 
	char type;
	struct symbol_table * next;
} sym_list;

typedef struct data_array{
	int value;
	struct data_array * next;
} data_list;

typedef struct entries_array{
	char sym[MAX_LENGTH_LABEL];
	int value;
	struct entries_array * next;
} entries_list;

void add_to_sym_tab(char * sym, int value, char type);
void add_to_data_list(int value);
void add_to_entries_list(char *sym, int line_num);
void print_entries_to_file(char * fname);
void print_externals_to_file(char * fname);
void print_data_to_file(char * fname);
void structs_init();
#endif
