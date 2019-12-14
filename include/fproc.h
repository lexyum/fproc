#ifndef FPROC_H
#define FPROC_H

#include <genetree.h>

/* initialise file buffer and store contents of INFILE */
int fproc_read(const char *infile);

/* initialise file buffer N if not empty and store contents of INFILE.
   does nothing if N is already allocated */
int fproc_read_n(const char *infile, const size_t destN);

/* print deflines gene_tree corresponding to SRC_FILE to stdout */
void fproc_print(const size_t srcN);

/* print deflines and sequences corresponding to SRC_FILE to stdout */
void fproc_print_all(const size_t srcN);

/* print contents of file_array */
void fproc_list(void);

/* write gene_tree corresponding to SRC_FILE to OUTFILE */
int fproc_write(const size_t srcN, const char *outfile);

/* add gene_tree corresponding to SRC_TREE to gene tree corresponding to DEST_TREE and rebalance */
int fproc_merge(const size_t srcN, const size_t destN);

/* search GENE_TREE for a defline containing STRING */
int fproc_search_defline(const size_t srcN, const char *string);

/* search GENE_TREE for a sequence containing STRING */
int fproc_search_sequence(const size_t srcN, const char *string);

/* delete GENE_TREE */
int fproc_delete(const size_t srcN);

/* delete all trees */
int fproc_delete_all(void);


#endif /* FPROC_H */
