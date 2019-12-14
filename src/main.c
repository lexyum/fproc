//#define DEBUG
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef __STRICT_ANSI__
#include <unistd.h>
#else
#include <getopt.h>
#endif /* __STRICT_ANSI__ */

#include <fproc.h>

#define BUF_MAX 80

size_t file_max = 10;
void **file_array;

void print_welcome (void)
{
	fputs("********************************************************************************\n" \
	      "*                                                                              *\n" \
	      "*                                                                              *\n" \
	      "*                            fproc                                             *\n" \
	      "*                                                                              *\n" \
	      "*                                                                              *\n" \
	      "*                                                                              *\n" \
	      "********************************************************************************\n", stdout);
	fputs("fproc v1.0\n", stdout);
	fputs("Type `help' or `credits' for more information.\n", stdout);
}

void print_help (void)
{
	fputs("fproc - a terminal-based program for FASTA file manipulation.\n\n", stdout);
	fputs("List of commands:\n\n" \
	      "\tread FILE               read in and store contents of FILE\n"\
	      "\tread-to FILE N          read in and store contents of FILE in buffer N, if free\n"
	      "\tprint N                 print description lines from file N\n"\
	      "\tprint-full N            print description lines and sequences from file N\n"\
	      "\tlist                    print contents of file buffer\n"\
	      "\twrite N FILE            write contents of file N to output file FILE\n"	\
	      "\tmerge N1 N2             merge contents of file N1 into file N2\n"\
	      "\tsearch-label N STRING   search file N for description lines containing STRING\n"\
	      "\tsearch-seq N STRING     search file N for sequences containing STRING\n"\
	      "\tdelete N                delete file N from file buffer\n"\
	      "\tdelete-all              delete all files from file buffer\n\n"\
	      "\thelp                    display this help message\n"\
	      "\tcredits                 display credits\n\n", stdout);
	      
	fputs("Use `quit' or `Ctrl-D' to exit.\n\n", stdout);
}

void print_credits (void)
{
	fputs("Written by Alexander Moore, Dec 2019\n", stdout);
}

int main (void)
{
	print_welcome();
		
	/* startup: initialise file_array */
	if ((file_array =  calloc(file_max, sizeof(void *))) == NULL) {
		printf("failed to initialise file array, exiting.\n");
		exit(EXIT_FAILURE);
	}

	char termbuf[BUF_MAX];
	char *token;

	while (1) {
		fputs("(fproc) > ", stdout);

		if (fgets(termbuf, BUF_MAX, stdin) == NULL) { 		/* read input, exiting on Ctrl-D */
			fputc('\n', stdout);
			exit(EXIT_SUCCESS);
		}
		else if ((token = strtok(termbuf, " \t\n")) == NULL) {  /* get first command, prompting again if empty */
			continue;
		}

		/* LIST OF COMMANDS
		 *
		 * When a match is found a corresponding function from <fproc.h> is called
		 * to implement the command. These are wrappers for functions acting on the
		 * underlying storage structures directly, and shield the front-end 
		 * (main(), basically) from the actual implementation of the commands.
		 *
		 */
		
		if (!strcmp(token, "quit") ||
			 !strcmp(token, "exit") ||
			 !strcmp(token, "q")) {
			exit(EXIT_SUCCESS);
		}
		/* parse input */
		else if (!strcmp(token, "read")) {
			char *infile = strtok(NULL, " \t\n");

			if (infile == NULL) {
				fputs("input file required\n", stdout);
			}
			else {
				fproc_read(infile);
			}
			continue;
		}
		else if (!strcmp(token, "read-to")) {
			char *infile = strtok(NULL, " \t\n");
			char *destbuf;
			
			unsigned long int destN;

			if (infile == NULL) {
				fputs("input file required\n", stdout);
			}
			else if ((destbuf = strtok(NULL, " \t\n")) == NULL) {
				fprintf(stdout, "destination buffer required\n");
			}
			else if ((destN = strtoul(destbuf, NULL, 10)) == 0) {
				fprintf(stdout, "%s is not a valid number\n", destbuf);
			}
			else {
				fproc_read_n(infile, destN - 1);
			}
			continue;
		}
		
		else if (!strcmp(token, "print")) {
			char *infile = strtok(NULL, " \t\n");
			unsigned long int srcN;

			if (infile == NULL) {
				fputs("filename required\n", stdout);
			}
			else if ((srcN = strtoul(infile, NULL, 10)) == 0) {
				fprintf(stdout, "%s is not a valid buffer number\n", infile);
			}
			else {
				fproc_print(srcN - 1);
			}
			continue;
		}
		
		else if (!strcmp(token, "print-all")) {
			char *infile = strtok(NULL, " \t\n");
			unsigned long int srcN;

			if (infile == NULL) {
				fputs("source file buffer required\n", stdout);
				fputs("usage: `print-all n'\n", stdout);
			}
			else if ((srcN = strtoul(infile, NULL, 10)) == 0) {
				fprintf(stdout, "%s is not a valid buffer number.\n", infile);
			}
			else {
				fproc_print_all(srcN - 1);
			}
			continue;
		}
		
		else if (!strcmp(token, "list")) {
			fproc_list();
		}
		
		else if (!strcmp(token, "write")) {
			char *infile = strtok(NULL, " \t\n");
			char *outfile = strtok(NULL, " \t\n");
			unsigned long int srcN;

			if (infile == NULL) {
				fputs("source buffer number required\n", stdout);
				fputs("usage: `write n file'\n", stdout);
			}
			else if (outfile == NULL) {
				fputs("output filename required\n", stdout);
				fputs("usage: `write n file'\n", stdout);
			}
			else if ((srcN = strtoul(infile, NULL, 10)) == 0) {
				fprintf(stdout, "%s is not a valid buffer number\n", infile);
			}
			else {
				fproc_write(srcN - 1, outfile);
			}
			continue;
		}
		else if (!strcmp(token, "merge")) {
			char *file1 = strtok(NULL, " \t\n");
			char *file2 = strtok(NULL, " \t\n");
			unsigned long int srcN;
			unsigned long int destN;

			if (file1 == NULL || file2 == NULL) {
				fputs("two arguments required\n", stdout);
			}
			else if ((srcN = strtoul(file1, NULL, 10)) == 0) {
				fprintf(stdout, "%s is not a valid buffer number\n", file1);
			}
			else if ((destN = strtoul(file2, NULL, 10)) == 0) {
				fprintf(stdout, "%s is not a valid buffer number\n", file2);
			}
			else {
				fproc_merge(srcN - 1, destN - 1);
			}
			continue;
		}
		else if (!strcmp(token, "search-label")) {
			char *srcfile = strtok(NULL, " \t\n");
			char *string;
			unsigned long int srcN;

			if (srcfile == NULL) {
				fputs("source buffer number required\n", stdout);
				fputs("usage: search-label n string\n", stdout);
			}
			else if ((srcN = strtoul(srcfile, NULL, 10)) == 0) {
				fprintf(stdout, "%s is not a valid buffer number\n", srcfile);
			}
			else if ((string = strtok(NULL, " \t\n")) == NULL) {
				fputs("search string required\n", stdout);
				fputs("usage: search-label n label\n", stdout);
			}
			else {
				fproc_search_defline(srcN - 1, string);
			}
		}
		
		else if (!strcmp(token, "search-seq")) {
			char *srcfile = strtok(NULL, " \t\n");
			char *string;
			unsigned long int srcN;

			if (srcfile == NULL) {
				fputs("source buffer number required\n", stdout);
				fputs("usage: search-seq n string\n", stdout);
			}
			else if ((srcN = strtoul(srcfile, NULL, 10)) == 0) {
				fprintf(stdout, "%s is not a valid buffer number\n", srcfile);
			}
			else if ((string = strtok(NULL, " \t\n")) == NULL) {
				fputs("search string required\n", stdout);
				fputs("usage: search-seq n string\n", stdout);
			}
			else {
				fproc_search_sequence(srcN - 1, string);
			}
		}

		else if (!strcmp(token, "delete")) {
			char *filename = strtok(NULL, " \t\n");
			unsigned long int srcN;
			if (filename == NULL) {
				fputs("target filename required\n", stdout);
			}
			else if ((srcN = strtoul(filename, NULL, 10)) == 0) {
				fprintf(stdout, "%s is not a valid buffer number\n", filename);
			}
			else {
				fproc_delete(srcN - 1);
			}
			continue;
		}
		else if (!strcmp(token, "delete-all")) {
			fproc_delete_all();
		}

		/* GENERIC INFORMATION */
		else if (!strcmp(token, "help")) {
			print_help();
		}
		else if (!strcmp(token, "credits")) {
			print_credits();
		}
		else {
			fputs("command not recognised.\n", stdout);
			print_help();
		}
	}

	for (unsigned int i = 0; i < file_max; i++) {
		free(file_array[i]);
		file_array[i] = NULL;
	}
	free(file_array);

	exit(EXIT_SUCCESS);
}
