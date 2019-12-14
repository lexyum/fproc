/* dsw.c - implementation of Day-Stout-Warren algorithm for balancing
   a binary search tree. */

#include <stdlib.h>

#include <genetree.h>

static void rotate_right (struct gene_node_t *pivot);
static void rotate_left (struct gene_node_t *pivot);

size_t count_ground_leaves (struct gene_tree_t *gene_tree)
{
	
	size_t tmp = (gene_tree->size) + 1;
	int count = 0;
	
	while ((tmp >>= 1) != 0) {
		++count;
	}

	return (gene_tree->size + 1 - (1 << count));
}

void tree_to_vine (struct gene_tree_t *gene_tree)
{
	struct gene_node_t *curr_node = gene_tree->root;

	while (curr_node != NULL) {
		while (curr_node->left != NULL) {
			rotate_right(curr_node);
		}
		curr_node = curr_node->right;
	}
}

 void vine_to_tree (struct gene_tree_t *gene_tree)
{
	size_t ground_leaves = count_ground_leaves(gene_tree);

	struct gene_node_t *curr_node = gene_tree->root;

	/* rotate left on odd elements until we have lowest level. */
	for (size_t i = 0; i < ground_leaves; i++) {
		rotate_left(curr_node);
		curr_node = curr_node->right; /* skip even elements */
	}

	size_t vine_len = (gene_tree->size) - ground_leaves - 1;

	for (size_t i = vine_len/2; i > 0; i = vine_len/2) {
		curr_node = gene_tree->root;
		for (size_t j = 0; j < i; j++) {
			rotate_left(curr_node);
			curr_node = curr_node->right;
		}
		vine_len = vine_len - i - 1;
	}
}

void balance_tree (struct gene_tree_t *gene_tree)
{
	if (gene_tree->root == NULL) {
		return;
	}

	tree_to_vine(gene_tree);
	vine_to_tree(gene_tree);
}

/********************
 * STATIC FUNCTIONS *
 ********************/

/*
 * TREE ROTATIONS:
 *
 * Left rotation on A               Right rotation on B
 *
 *    A            B                    B           A
 *   / \          / \                  / \         / \
 *  X   B   =>   A   Z                A   Z   =>  X   B
 *     / \      / \                  / \             / \
 *    Y   Z    X   Y                X   Y           Y   Z
 *
 * (X, Y, Z all subtrees)
 *
 * We can implement this in three steps:
 *
 * 1) Swap children of the pivot node
 *
 * 2) Swap A and B
 *
 * 3) Permute X, Y, Z
 *
 * Lots of shuffling of pointers, but no data is affected
 *
 */

static void rotate_right (struct gene_node_t *pivot)
{
	/* step 1 */
	struct gene_node_t *node_tmp = pivot->left;
	pivot->left = pivot->right;
	pivot->right = node_tmp;

	/* step 2 */
	char *pivot_defline = pivot->defline;
	char *pivot_sequence = pivot->sequence;

	pivot->defline = pivot->right->defline;
	pivot->sequence = pivot->right->sequence;
	pivot->right->defline = pivot_defline;
	pivot->right->sequence = pivot_sequence;

	/* step 3 */
	node_tmp = pivot->left;
	pivot->left = pivot->right->left;
	pivot->right->left = pivot->right->right;
	pivot->right->right = node_tmp;
}

static void rotate_left (struct gene_node_t *pivot)
{
		/* step 1 */
	struct gene_node_t *node_tmp = pivot->right;
	pivot->right = pivot->left;
	pivot->left = node_tmp;

	/* step 2 */
	char *pivot_defline = pivot->defline;
	char *pivot_sequence = pivot->sequence;

	pivot->defline = pivot->left->defline;
	pivot->sequence = pivot->left->sequence;
	pivot->left->defline = pivot_defline;
	pivot->left->sequence = pivot_sequence;

	/* step 3 */
	node_tmp = pivot->right;
	pivot->right = pivot->left->right;
	pivot->left->right = pivot->left->left;
	pivot->left->left = node_tmp;
}
