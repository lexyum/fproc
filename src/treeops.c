
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <genetree.h>
#include <dsw.h>

/* Populate initialised gene_tree.
   Return 0 on success, -1 on failure*/
int fill_tree (struct gene_tree_t *gene_tree)
{
	FILE *ifptr;

	if ((ifptr = fopen(gene_tree->filename, "r")) == NULL) {
		fprintf(stderr, "unable to open file %s\n", gene_tree->filename);
		return -1;
	}

	char *defline_buf = NULL;
	char *sequence_buf = NULL;

	/* lengths NOT including trailing null character */
	size_t defline_len = 0;
	size_t sequence_len = 0;

	/* Step down through file, inserting a node into the tree when a header and matching sequence
           are found. */

	while (getline(&sequence_buf, &sequence_len, ifptr) != -1) {

		if (*sequence_buf == '>') {

			/* 
			 * sequence_len excludes trailing null character, but strcpy expects a
			 * destination large enough to include it.
			 *
			 * Since strcpy() does not check argument lengths it is our responsibility
			 * to allocate enough space before copying! 
			 */
			
			defline_len = sequence_len - 1;

			/* need a temporary buffer to properly handle errors, since realloc
			   will return NULL but leave initial buffer unchanged */
			char *tmp_buf = realloc(defline_buf, defline_len + 1);
			if (tmp_buf == NULL) {
				free(defline_buf);
				defline_buf = NULL;

				free(sequence_buf);
				sequence_buf = NULL;
				
				fclose(ifptr);
				return -1;
			}
			else {
				defline_buf = tmp_buf;
				strcpy(defline_buf, sequence_buf + 1);
			}
		}
		else {
			if(gene_tree_insert(gene_tree, defline_buf, defline_len, sequence_buf, sequence_len) == -1) {
				free(defline_buf);
				defline_buf = NULL;
				
				free(sequence_buf);
				sequence_buf = NULL;
				
				fclose(ifptr);  /* NO CHECK ON RETURN VALUE -- what would I do? */
				return -1;
			}
		}
	}

	free(sequence_buf);
	sequence_buf = NULL;
	
	free(defline_buf);
	defline_buf = NULL;
	
	fclose(ifptr); /* NO CHECK ON RETURN VALUE */

	return 0;
}

int merge_tree ( struct gene_tree_t *src_tree, struct gene_tree_t *dest_tree)
{
	if (dest_tree->root == NULL) {
		free_gene_tree(dest_tree);
		dest_tree = src_tree;
		return 0;
	}
	else if (src_tree->root == NULL) {
		free_gene_tree(src_tree);
		return 0;
	}

	/* right rotate src_tree to a singly-linked list */
	tree_to_vine(src_tree);
	
	do { /* while (src_tree->root != NULL) */
		int nodecmp;

		struct gene_node_t *curr_node = dest_tree->root;
		struct gene_node_t *prev_node = NULL;

		do { /* while (curr_node != NULL) */

			nodecmp = genecmp(src_tree->root, curr_node);
			if (nodecmp < 0) {
				prev_node = curr_node;
				curr_node = curr_node->left;
			}
			else if (nodecmp > 0) {
				prev_node = curr_node;
				curr_node = curr_node->right;
			}
			else {
				break;
			}
		} while (curr_node != NULL);

		if (nodecmp == 0) {
			/* src_node is already in dest_tree - pop off src_tree and free */
			struct gene_node_t *old_head = src_tree->root;
			
			src_tree->root = src_tree->root->right;
			--(src_tree->size);

			free(old_head->defline);
			free(old_head->sequence);
			old_head->defline = NULL;
			old_head->sequence = NULL;

			free(old_head);
		}
		/* src_node not already in dest_tree - pop off src_tree and add to dest_tree */
		else if (nodecmp < 0) {
			prev_node->left = src_tree->root;
			++(dest_tree->size);

			src_tree->root = src_tree->root->right;
			--(src_tree->size);
			
			prev_node->left->left = NULL;
			prev_node->left->right = NULL;
		}
		else if (nodecmp > 0) {
			prev_node->right = src_tree->root;
			++(dest_tree->size);
			
			src_tree->root = src_tree->root->right;
			--(src_tree->size);
			
			prev_node->right->left = NULL;
			prev_node->right->right = NULL;
		}
		
	} while (src_tree->root != NULL);

	/* sanity check - size of empty tree should be 0 */
	if (src_tree->size != 0) {
		fprintf(stderr, "merge error: possible memory leak\n");
		return -1;
	}


	/* free empty src_tree and set to NULL so the file_array element can be reallocated */
	free_gene_tree(src_tree);
	src_tree = NULL;

	balance_tree(dest_tree);
	return 0;
}
/* recursively print labels on binary tree */
void print_tree (const struct gene_node_t *root, FILE *stream)
{
	if (root != NULL) {
		fprintf(stream, ">%s\n", root->defline);
		print_tree(root->left, stream);
		print_tree(root->right, stream);
	}
}

/* recursively print full contents of binary tree */
void print_tree_full (const struct gene_node_t *root, FILE *stream)
{
	if (root != NULL) {
		fprintf(stream, ">%s%s", root->defline, root->sequence);
		print_tree_full(root->left, stream);
		print_tree_full(root->right, stream);
	}
}

/* operate recursively on binary tree nodes.
   NOTE: if contents are changed, tree may need rebalancing. */

void operate_tree (const struct gene_node_t *root,
		   void (*node_op)(char *, char *))
{

	if (root != NULL) {
	        (*node_op)(root->defline, root->sequence);
		operate_tree(root->left, node_op);
		operate_tree(root->right, node_op);
	}
}

int search_tree (const struct gene_node_t *root, const char *string,
		 int (*search_fn)(const char *, const char *, const char *))
{
	int count = 0;
	if (root != NULL) {
		if ((*search_fn)(root->defline, root->sequence, string) == 1) {
			++count;
			fprintf(stdout, "Match found: %s\n", root->defline);
		}
		count += search_tree(root->left, string, search_fn);
		count += search_tree(root->right, string, search_fn);
	}
	return count;
}
		
		
