/* fproc.c - wrappers for gene_tree functions in treeops.c 
 *
 * Currently, all functions return 0 on success and -1 on failure.
 * TO DO: implement proper error handler
 *    
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <genetree.h>
#include <treeops.h>
#include <dsw.h>

/*
 * FILE_ARRAY[] is a global array of VOID pointers with length FILE_MAX, declared in `main.c'
 * Q: Why is FILE_ARRAY[] a global array of VOID pointers?
 * A: Modularity - main() knows nothing about the underlying data structures!
 *   
 * Q: Is this actually the best way to implement this?
 * A: TBD... 
 */

extern size_t file_max;
extern void **file_array;

/* search functions, passed by reference */

static int defsearch(const char *defline, const char *sequence, const char *string);
static int seqsearch(const char *defline, const char *sequence, const char *string);


int fproc_read_n(const char *infile, const size_t destN)
{
	if (destN > file_max) {
		fprintf(stderr, "error: buffer number %lu is out of bounds", destN + 1);
		return -1;
	}
	else if (file_array[destN] != NULL) {
		fprintf(stdout, "read failed: file buffer %lu not empty\n", destN + 1);
		return 0;
	}
	else if ((file_array[destN] = init_gene_tree(infile, strlen(infile))) == NULL) {
		fprintf(stderr, "failed to initialise tree for file %s\n", infile);
		return -1;
	}
	else if (fill_tree((struct gene_tree_t *)file_array[destN]) == -1) {
		fprintf(stderr, "failed to fill tree for file %s\n", infile);
		free_gene_tree((struct gene_tree_t *)file_array[destN]);
		file_array[destN] = NULL;
		return -1;
	}
	else {
		fprintf(stdout,"file %s successfully stored in buffer %lu\n", infile, destN + 1);
		return 0;
	}
}
		
int fproc_read(const char *infile)
{
	for (long unsigned int i = 0; i < file_max; i++) {
		if (file_array[i] != NULL) {
			continue;
		}
		else if ((file_array[i] = init_gene_tree(infile, strlen(infile))) == NULL) {
			fprintf(stderr, "failed to initialise tree for file %s\n", infile);
			return -1;
		}
		else if (fill_tree((struct gene_tree_t *)file_array[i]) == -1) {
			fprintf(stderr, "failed to fill tree for file %s\n", infile);
			free_gene_tree((struct gene_tree_t *)file_array[i]);
			file_array[i] = NULL;
			return -1;
		}
		else {
			fprintf(stdout, "file %s successfully stored in buffer %lu\n", infile, i + 1);
			return 0;
		}
	}
	fprintf(stderr, "could not import file %s: file_max %lu reached", infile, file_max);
	return -1;
}

void fproc_print (const size_t srcN)
{
	if (srcN >= file_max) {
		fprintf(stderr, "error: buffer number %lu is out of bounds\n", srcN + 1);
	}
	else if (file_array[srcN] == NULL) {
		fprintf(stdout, "could not print contents of buffer %lu: buffer is empty\n", srcN + 1);
	}
	else {
		struct gene_tree_t *tmp = (struct gene_tree_t *)file_array[srcN];
		print_tree(tmp->root, stdout);
	}
}

void fproc_print_all(const size_t srcN)
{
	if (srcN >= file_max) {
		fprintf(stderr, "error: buffer number %lu is out of bounds\n", srcN + 1);
	}
	else if (file_array[srcN] == NULL) {
		fprintf(stdout, "could not print contents of buffer %lu: buffer is empty\n", srcN + 1);
	}
	else {
		struct gene_tree_t *tmp = (struct gene_tree_t *)file_array[srcN];
		print_tree_full(tmp->root, stdout);
	}
}

void fproc_list(void)
{
	for (long unsigned int i = 0; i < file_max; i++) {
		if (file_array[i] != NULL) {
			struct gene_tree_t *tmp = (struct gene_tree_t *)file_array[i];
			fprintf(stdout, "%2lu: %-40s (%lu sequences)\n", i + 1, tmp->filename, tmp->size);
		}
		else {
			fprintf(stdout, "%2lu: [FREE]\n", i + 1);
		}
	}
}

int fproc_write(const size_t srcN, const char *outfile)
{
	if (srcN >= file_max) {
		fprintf(stderr, "error: buffer number %lu is out of bounds\n", srcN + 1);
		return -1;
	}
	
	FILE *ofptr = fopen(outfile, "w");
	
	if (ofptr == NULL) {
		fprintf(stderr, "error: unable to open file %s for writing\n", outfile);
		return -1;
	}

	if (file_array[srcN] == NULL) {
		fprintf(stdout, "could not write contents of buffer %lu: buffer is empty\n", srcN + 1);
	}
	else {
		struct gene_tree_t *tmp = (struct gene_tree_t *)file_array[srcN];
		print_tree_full(tmp->root, ofptr);
	}
	fclose(ofptr);
	return 0;
}

int fproc_merge(const size_t srcN, const size_t destN)
{

	if (srcN >= file_max) {
		fprintf(stderr, "error: source buffer number %lu is out of bounds\n", srcN + 1);
		return -1;
	}
	else if (destN >= file_max) {
		fprintf(stderr, "error: destination buffer number %lu is out of bounds\n", destN + 1);
		return -1;
	}
	else if (srcN == destN) {
		return 0;
	}
	else if (file_array[srcN] == NULL) {
		fprintf(stdout, "buffer %lu is empty: nothing to do\n", srcN + 1);
	}
	else if (file_array[destN] == NULL) {
		file_array[destN] = file_array[srcN];
	}
	else {
		merge_tree((struct gene_tree_t *)file_array[srcN], (struct gene_tree_t *)file_array[destN]);
	}
	
	file_array[srcN] = NULL;
	return 0;
}

int fproc_search_defline(const size_t srcN, const char *string)
{
	if (srcN >= file_max) {
		fprintf(stderr, "error: source buffer number %lu is out of bounds\n", srcN + 1);
		return -1;
	}
	else if (file_array[srcN] == NULL) {
		fprintf(stdout, "buffer %lu is empty: nothing to do\n", srcN + 1);
		return 0;
	}
	else {
		struct gene_tree_t *tmp = (struct gene_tree_t *)file_array[srcN];
		return search_tree(tmp->root, string, &defsearch);
	}

}

int fproc_search_sequence(const size_t srcN, const char *string)
{
	if (srcN >= file_max) {
		fprintf(stderr, "error: source buffer number %lu is out of bounds\n", srcN + 1);
		return -1;
	}
	else if (file_array[srcN] == NULL) {
		fprintf(stdout, "buffer %lu is empty: nothing to do\n", srcN + 1);
		return 0;
	}
	else {
		struct gene_tree_t *tmp = (struct gene_tree_t *)file_array[srcN];
		return search_tree(tmp->root, string, &seqsearch);
	}

}

int fproc_delete(const size_t srcN)
{
	if (srcN >= file_max) {
		fprintf(stderr, "error: buffer number %lu is out of bounds\n", srcN + 1);
		return -1;
	}

	if (file_array[srcN] == NULL) {
		fprintf(stdout, "buffer %lu is empty: nothing to do\n", srcN + 1);
	}
	else {
		free_gene_tree((struct gene_tree_t *)file_array[srcN]);
		file_array[srcN] = NULL;
	}
	return 0;
}

void fproc_delete_all(void)
{
	for (long unsigned int i = 0; i < file_max; i++) {
		if (file_array[i] != NULL) {
			free_gene_tree((struct gene_tree_t *)file_array[i]);
			file_array[i] = NULL;
		}
	}
}

/* Static function declarations */

static int defsearch(const char *defline, const char *sequence, const char *string)
{
	char *match;
	if ((match = strstr(defline, string)) != NULL) {
		fputs("Match found:\n", stdout);
		const char *tmp = defline;
		while (tmp != match) {
			fputc(*tmp, stdout);
		}
		tmp += strlen(string);
		fprintf(stdout, "\033[0;31m%s\033[0m%s\n", string, tmp);
		return 1;
	}
	else {
		return 0;
	}
}
static int seqsearch(const char *defline, const char *sequence, const char *string)
{
	char *match;
	if ((match = strstr(sequence, string)) != NULL) {
		const char *tmp = sequence;
		while (tmp != match) {
			fputc(*tmp, stdout);
		}
		fprintf(stdout, "\033[0;31m%s\033[0m", string);
		tmp += strlen(string);
		fputs(tmp, stdout);
		return 1;
	}
	else {
		return 0;
	}
}
