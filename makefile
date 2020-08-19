assembler:	main.c readf.c translate.c validation.c data_structs.c help_funcs.c
	gcc -Wall -ansi -pedantic -g main.c readf.c translate.c validation.c data_structs.c help_funcs.c -o assembler
