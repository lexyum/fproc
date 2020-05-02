/* fproc.c - wrappers for functions in treeops.c 
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

/* file array, initialised to array of NULLS by compiler */
#define FILE_MAX 10
static struct gene_tree *file_list[FILE_MAX];

/* search functions, passed by reference */
static int defsearch(const char *defline, const char *sequence, const char *string);
static int seqsearch(const char *defline, const char *sequence, const char *string);

/* read from infile, construct tree, and store in FILE_LIST[n] */
int fproc_read_n(const char *infile, const size_t destN)
{
	if (destN > FILE_MAX) {
		fprintf(stderr, "error: buffer number %lu is out of bounds", destN + 1);
		return -1;
	}
	else if (file_list[destN] != NULL) {
		fprintf(stdout, "read failed: file buffer %lu not empty\n", destN + 1);
		return 0;
	}
	else if ((file_list[destN] = init_gene_tree(infile, strlen(infile))) == NULL) {
		fprintf(stderr, "failed to initialise tree for file %s\n", infile);
		return -1;
	}
	else if (fill_tree((struct gene_tree *)file_list[destN]) == -1) {
		free_gene_tree((struct gene_tree *)file_list[destN]);
		file_list[destN] = NULL;
		return -1;
	}
	else {
		fprintf(stdout,"file %s successfully stored in buffer %lu\n", infile, destN + 1);
		return 0;
	}
}

/* read from infile, construct tree, and store in next free node */
int fproc_read(const char *infile)
{
	for (long unsigned int i = 0; i < FILE_MAX; i++) {
		if (file_list[i] != NULL)
			continue;
		else if ((file_list[i] = init_gene_tree(infile, strlen(infile))) == NULL) {
			fprintf(stderr, "failed to initialise tree for file %s\n", infile);
			return -1;
		}
		else if (fill_tree((struct gene_tree *)file_list[i]) == -1) {
			free_gene_tree((struct gene_tree *)file_list[i]);
			file_list[i] = NULL;
			return -1;
		}
		else {
			fprintf(stdout, "file %s successfully stored in buffer %lu\n", infile, i + 1);
			return 0;
		}
	}
	fprintf(stderr, "could not import file %s: FILE_MAX %d reached\n", infile, FILE_MAX);
	return -1;
}

/* print deflines from tree in FILE_LIST[n] */
void fproc_print (const size_t srcN)
{
	if (srcN >= FILE_MAX)
		fprintf(stderr, "error: buffer number %lu is out of bounds\n", srcN + 1);
	else if (file_list[srcN] == NULL)
		fprintf(stdout, "could not print contents of buffer %lu: buffer is empty\n", srcN + 1);
	else {
		struct gene_tree *tmp = (struct gene_tree *)file_list[srcN];
		print_tree(tmp->root, stdout);
	}
}

/* print deflines and sequences from tree in FILE_LIST[n] */
void fproc_print_all(const size_t srcN)
{
	if (srcN >= FILE_MAX)
		fprintf(stderr, "error: buffer number %lu is out of bounds\n", srcN + 1);
	else if (file_list[srcN] == NULL)
		fprintf(stdout, "could not print contents of buffer %lu: buffer is empty\n", srcN + 1);
	else {
		struct gene_tree *tmp = (struct gene_tree *)file_list[srcN];
		print_tree_full(tmp->root, stdout);
	}
}

/* list contents of FILE_LIST */
void fproc_list(void)
{
	for (long unsigned int i = 0; i < FILE_MAX; i++) {
		if (file_list[i] != NULL) {
			struct gene_tree *tmp = (struct gene_tree *)file_list[i];
			fprintf(stdout, "%2lu: %-40s (%lu sequences)\n", i + 1, tmp->filename, tmp->size);
		}
		else
			fprintf(stdout, "%2lu: [FREE]\n", i + 1);
	}
}

/* write contents of FILE_ARRAY[n] to outfile */
int fproc_write(const size_t srcN, const char *outfile)
{
	if (srcN >= FILE_MAX) {
		fprintf(stderr, "error: buffer number %lu is out of bounds\n", srcN + 1);
		return -1;
	}
	
	FILE *ofptr = fopen(outfile, "w");
	
	if (ofptr == NULL) {
		fprintf(stderr, "error: unable to open file %s for writing\n", outfile);
		return -1;
	}

	if (file_list[srcN] == NULL)
		fprintf(stdout, "could not write contents of buffer %lu: buffer is empty\n", srcN + 1);
	else {
		struct gene_tree *tmp = (struct gene_tree *)file_list[srcN];
		print_tree_full(tmp->root, ofptr);
	}
	fclose(ofptr);
	return 0;
}

/* combine contents of FILE_LIST[srcN], FILE_LIST[destN], and rebalance resulting tree */
int fproc_merge(const size_t srcN, const size_t destN)
{

	if (srcN >= FILE_MAX) {
		fprintf(stderr, "error: source buffer number %lu is out of bounds\n", srcN + 1);
		return -1;
	}
	else if (destN >= FILE_MAX) {
		fprintf(stderr, "error: destination buffer number %lu is out of bounds\n", destN + 1);
		return -1;
	}
	else if (srcN == destN)
		return 0;
	else if (file_list[srcN] == NULL)
		fprintf(stdout, "buffer %lu is empty: nothing to do\n", srcN + 1);
	else if (file_list[destN] == NULL)
		file_list[destN] = file_list[srcN];
	else
		merge_tree((struct gene_tree *)file_list[srcN], (struct gene_tree *)file_list[destN]);

	file_list[srcN] = NULL;
	return 0;
}

/* search deflines in FILE_LIST[srcN] for string */
int fproc_search_defline(const size_t srcN, const char *string)
{
	if (srcN >= FILE_MAX) {
		fprintf(stderr, "error: source buffer number %lu is out of bounds\n", srcN + 1);
		return -1;
	}
	else if (file_list[srcN] == NULL) {
		fprintf(stdout, "buffer %lu is empty: nothing to do\n", srcN + 1);
		return 0;
	}
	else {
		struct gene_tree *tmp = (struct gene_tree *)file_list[srcN];
		return search_tree(tmp->root, string, &defsearch);
	}

}

/* search sequences in FILE_LIST[srcN] for string */
int fproc_search_sequence(const size_t srcN, const char *string)
{
	if (srcN >= FILE_MAX) {
		fprintf(stderr, "error: source buffer number %lu is out of bounds\n", srcN + 1);
		return -1;
	}
	else if (file_list[srcN] == NULL) {
		fprintf(stdout, "buffer %lu is empty: nothing to do\n", srcN + 1);
		return 0;
	}
	else {
		struct gene_tree *tmp = (struct gene_tree *)file_list[srcN];
		return search_tree(tmp->root, string, &seqsearch);
	}

}

/* delete contents of FILE_LIST[srcN] */
int fproc_delete(const size_t srcN)
{
	if (srcN >= FILE_MAX) {
		fprintf(stderr, "error: buffer number %lu is out of bounds\n", srcN + 1);
		return -1;
	}

	if (file_list[srcN] == NULL)
		fprintf(stdout, "buffer %lu is empty: nothing to do\n", srcN + 1);
	else {
		free_gene_tree((struct gene_tree *)file_list[srcN]);
		file_list[srcN] = NULL;
	}
	return 0;
}

/* delete all stored files */
void fproc_delete_all(void)
{
	for (long unsigned int i = 0; i < FILE_MAX; i++) {
		if (file_list[i] != NULL) {
			free_gene_tree((struct gene_tree *)file_list[i]);
			file_list[i] = NULL;
		}
	}
}

/* Static function declarations */

static int defsearch(const char *defline, const char *sequence, const char *string)
{
	char *match;
	if ((match = strstr(defline, string)) != NULL) {
		fputs("Match found:\n", stdout);
		
		for (const char *tmp = defline; tmp != match; ++tmp) {
			fputc(*tmp, stdout);
		}
		fprintf(stdout, "\033[0;31m%s\033[0m%s\n", string, match + strlen(string));
		return 1;
	}
	else 
		return 0;
}
static int seqsearch(const char *defline, const char *sequence, const char *string)
{
	char *match;
	if ((match = strstr(sequence, string)) != NULL) {
		fputs("Match found:\n", stdout);

		for (const char *tmp = sequence; tmp != match; ++tmp) {
			fputc(*tmp, stdout);
		}
		fprintf(stdout, "\033[0;31m%s\033[0m%s\n", string, match + strlen(string));
		return 1;
	}
	else
		return 0;
}
