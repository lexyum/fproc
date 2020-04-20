#ifndef GENE_TREE_H
#define GENE_TREE_H

/* 
 * struct gene_node : leaf of binary tree
 */

struct gene_node {
	char *defline;
	char *sequence;

	struct gene_node *right;
	struct gene_node *left;
};

int genecmp (const struct gene_node *g1, const struct gene_node *g2);

/*
 * struct gene_tree : 
 *
 * Each FASTA file read in is assigned a binary tree,
 * in which all the sequences in the file are stored.
 *
 * All manipulations should be performed by functions
 * acting on gene_trees.
 *
 */

struct gene_tree {
	char *filename; /* name of parent file */

	size_t size; 
	struct gene_node *root;
};

struct gene_tree *init_gene_tree (const char *filename, size_t file_len);

void free_gene_tree (struct gene_tree *gene_tree);

int gene_tree_insert (struct gene_tree *gene_tree,
		      const char *defline, size_t defline_len, const char *sequence, size_t sequence_len);

#endif /* GENE_TREE_H */
