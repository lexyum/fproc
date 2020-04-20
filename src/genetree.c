
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <genetree.h>

static void free_gene_node (struct gene_node *gene_node);

static struct gene_node *init_gene_node (const char *defline, size_t defline_len, const char *sequence, size_t sequence_len);

/* Define an ordering for gene sequences g1 and g2. */
int genecmp (const struct gene_node *g1, const struct gene_node *g2)
{
	/* Crudest possible ordering - alphabetical comparison of deflines */
	return strcmp(g1->defline, g2->defline);
}	

/* Given filename, and length (excluding null character), initialise and return gene_tree structure.
   Return NULL pointer on failure. */
struct gene_tree *init_gene_tree (const char *filename, size_t file_len)
{
	struct gene_tree *tree = malloc(sizeof(*tree));

	if (tree  != NULL) {
		tree->filename = malloc((file_len + 1));
		tree->size = 0;
		tree->root = NULL;
	}

	/*  NOTE: Can strcpy actually fail and return NULL? */
	if ((tree->filename == NULL) || (strcpy(tree->filename, filename) == NULL)) {
		free(tree);
		tree = NULL;
	}

	return tree;
}

void free_gene_tree (struct gene_tree *tree)
{
	if (tree != NULL) {
		free_gene_node(tree->root);
		free(tree->filename);
		tree->filename = NULL;
	}
	free(tree);
	tree=NULL;
}

/* Add new node to gene_tree, if not already present.
   Return 0 on success, -1 on failure. */
int gene_tree_insert (struct gene_tree *tree,
		      const char *defline, size_t defline_len, const char *sequence, size_t sequence_len)
{
	if (defline == NULL || sequence == NULL) {
		return -1;
	}
	struct gene_node *new_node = init_gene_node(defline, defline_len, sequence, sequence_len);

	if (new_node == NULL) {
		return -1;
	}
	else if (tree->root == NULL) {
		tree->root = new_node;

		++(tree->size);
		return 0;
	}

	int nodecmp;

	struct gene_node *curr_node = tree->root;
	struct gene_node *prev_node = NULL;

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

	if (nodecmp < 0) {
		prev_node->left = new_node;
	}
	else {
		prev_node->right = new_node;
	}

	++(tree->size);
	return 0;
}

/*
 * STATIC FUNCTION DEFINITIONS
 */

static struct gene_node *init_gene_node (const char *defline, size_t defline_len, const char *sequence, size_t sequence_len)
{
	struct gene_node *node = malloc(sizeof(*node));
	if (node != NULL) {
		node->defline = malloc(defline_len + 1);
		node->sequence = malloc(sequence_len + 1);

		node->right = NULL;
		node->left = NULL;
	}


	/* NOTE: Can strcpy actually fail and return NULL? */
	if ((node->defline == NULL) || (strcpy(node->defline, defline) == NULL)) {
		free_gene_node(node);
	}
	if ((node->sequence == NULL) || (strcpy(node->sequence, sequence) == NULL)) {
		free_gene_node(node);
	}

	return node;
}

static void free_gene_node (struct gene_node *node)
{
	if (node != NULL) {
		free(node->defline);
		free(node->sequence);
		node->defline = NULL;
		node->sequence = NULL;
		
		/* free children recursively */
		free_gene_node(node->right);
		free_gene_node(node->left);
	}
	free(node);
	node = NULL;
}
