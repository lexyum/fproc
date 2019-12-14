#ifndef TREE_OPS_H
#define TREE_OPS_H

#include <genetree.h>

int fill_tree (struct gene_tree_t *gene_tree);

int merge_tree (struct gene_tree_t *src_tree, struct gene_tree_t *dest_tree);

void print_tree (const struct gene_node_t *gene_node, FILE *stream);

void print_tree_full (const struct gene_node_t *root, FILE *stream);

void operate_tree (const struct gene_node_t *root,
		   void (*node_op)(char *, char *));

int search_tree (const struct gene_node_t *root, const char *string,
		 int (*search_fn)(const char *, const char *, const char *));

#endif /* TREE_OPS_H */
