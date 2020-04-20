/* include/treeops.h
*
* direct operations on gene_tree structures
*/


#ifndef TREE_OPS_H
#define TREE_OPS_H

#include <genetree.h>

int fill_tree (struct gene_tree *gene_tree);

int merge_tree (struct gene_tree *src_tree, struct gene_tree *dest_tree);

void print_tree (const struct gene_node *gene_node, FILE *stream);

void print_tree_full (const struct gene_node *root, FILE *stream);

void operate_tree (const struct gene_node *root,
		   void (*node_op)(char *, char *));

int search_tree (const struct gene_node *root, const char *string,
		 int (*search_fn)(const char *, const char *, const char *));

#endif /* TREE_OPS_H */
