//#define DEBUG
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <genetree.h>

static void free_gene_node (struct gene_node_t *gene_node);

static struct gene_node_t *init_gene_node (const char *defline, size_t defline_len, const char *sequence, size_t sequence_len);

/* Define an ordering for gene sequences g1 and g2. */
int genecmp (const struct gene_node_t *g1, const struct gene_node_t *g2)
{
	/* Crudest possible ordering - alphabetical comparison of deflines */
	return strcmp(g1->defline, g2->defline);

#ifdef DEBUG
	if ((g1 == NULL) || (g2 == NULL)) {
		fputs("invalid comparison of null gene_node, exiting.\n", stderr);
		exit(EXIT_FAILURE);
	}
#endif /* DEBUG */
}	

/* Given filename, and length (excluding null character), initialise and return gene_tree structure.
   Return NULL pointer on failure. */
struct gene_tree_t *init_gene_tree (const char *filename, size_t file_len)
{
	struct gene_tree_t *gene_tree = malloc(sizeof(struct gene_tree_t));

	if (gene_tree  != NULL) {
		gene_tree->filename = malloc((file_len + 1)*sizeof(char));
		gene_tree->size = 0;
		gene_tree->root = NULL;
	}

         /* Short-circuit evaluation - strcpy() only called if malloc failed.
	    NOTE: Can strcpy actually fail and return NULL? Shouldn't hurt to check,
	           but nice to avoid redundant code. */
	if ((gene_tree->filename == NULL) || (strcpy(gene_tree->filename, filename) == NULL)) {
		free(gene_tree);
		gene_tree = NULL;
	}

	return gene_tree;
}

void free_gene_tree (struct gene_tree_t *gene_tree)
{
	if (gene_tree != NULL) {
		free_gene_node(gene_tree->root);
		free(gene_tree->filename);
		gene_tree->filename = NULL;
	}
	free(gene_tree);
	gene_tree=NULL;
}

/* Add new node to gene_tree, if not already present.
   Return 0 on success, -1 on failure. */
int gene_tree_insert (struct gene_tree_t *gene_tree,
		      const char *defline, size_t defline_len, const char *sequence, size_t sequence_len)
{
	if (defline == NULL || sequence == NULL) {
		return -1;
	}
	struct gene_node_t *new_node = init_gene_node(defline, defline_len, sequence, sequence_len);

	if (new_node == NULL) {
		return -1;
	}
	else if (gene_tree->root == NULL) {
		gene_tree->root = new_node;

		++(gene_tree->size);
		return 0;
	}

	int nodecmp;

	struct gene_node_t *curr_node = gene_tree->root;
	struct gene_node_t *prev_node = NULL;

	do { /* while (curr_node != NULL) */
		
		nodecmp = genecmp(new_node, curr_node);
		if (nodecmp < 0) {
			prev_node = curr_node;
			curr_node = curr_node->left;
		}
		else if (nodecmp > 0) {
			prev_node = curr_node;
			curr_node = curr_node->right;
		}
		else {
			/* already in tree */
			free_gene_node(new_node);
			return 0;
		}
	} while (curr_node != NULL);

	if (nodecmp < 1) {
		prev_node->left = new_node;
	}
	else {
		prev_node->right = new_node;
	}

	++(gene_tree->size);
	return 0;
}

/*
 * STATIC FUNCTION DEFINITIONS
 */

static struct gene_node_t *init_gene_node (const char *defline, size_t defline_len, const char *sequence, size_t sequence_len)
{
	struct gene_node_t *gene_node = malloc(sizeof(struct gene_node_t));
	if (gene_node != NULL) {
		gene_node->defline = malloc((defline_len + 1)*sizeof(char));
		gene_node->sequence = malloc((sequence_len + 1)*sizeof(char));

		gene_node->right = NULL;
		gene_node->left = NULL;
	}

	/* Short-circuit evaluation - strcpy() only called if malloc failed.
	   NOTE: Can strcpy actually fail and return NULL? Shouldn't hurt to check,
	         but nice to avoid redundant code. */
	
	if ((gene_node->defline == NULL) || (strcpy(gene_node->defline, defline) == NULL)) {
		free_gene_node(gene_node);
	}
	if ((gene_node->sequence == NULL) || (strcpy(gene_node->sequence, sequence) == NULL)) {
		free_gene_node(gene_node);
	}

	return gene_node;
}

static void free_gene_node (struct gene_node_t *gene_node)
{
	if (gene_node != NULL) {
		free(gene_node->defline);
		free(gene_node->sequence);
		gene_node->defline = NULL;
		gene_node->sequence = NULL;
		
		/* free children recursively */
		free_gene_node(gene_node->right);
		free_gene_node(gene_node->left);
	}
	free(gene_node);
	gene_node = NULL;
}
