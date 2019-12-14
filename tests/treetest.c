/* Unit tests for base gene_tree functions */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <genetree.h>
#include <treeops.h>
#include <dsw.h>

const char *testfile = "test1";
const size_t testsize = 9;

const char *deflines[] = {
	"defline1\n",
	"defline2\n",
	"defline3\n",
	"defline4\n",
	"defline5\n",
	"defline6\n",
	"defline7\n",
	"defline8\n",
	"defline9\n",
};

const char *sequences[] = {
	"sequence1\n",
	"sequence2\n",
	"sequence3\n",
	"sequence4\n",
	"sequence5\n",
	"sequence6\n",
	"sequence7\n",
	"sequence8\n",
	"sequence9\n",
};


int main()
{
	struct gene_tree_t *test_gene_tree = init_gene_tree (testfile, testsize);
       
	/* Check tree was correctly initialised */
	assert(strcmp(test_gene_tree->filename, testfile) == 0);
	assert(test_gene_tree->size == 0);
	assert(test_gene_tree->root == NULL);

	/* Fill the tree */
	for (unsigned int i = 0; i < testsize; i++) {
		if (gene_tree_insert(test_gene_tree, deflines[i], strlen(deflines[i]), sequences[i], strlen(sequences[i])) == -1) {
			fprintf(stderr, "error adding sequence %s to tree\n", deflines[i]);
		}
	}

	/* Print the tree */
	printf("Printing initial tree:\n");
	printf("----------------------\n");
	print_tree_full(test_gene_tree->root, stdout);

	/* Convert tree to vine */
	tree_to_vine(test_gene_tree);

	/* Print tree */
	printf("\n\nPrinting vine:\n");
	printf("--------------\n");
	print_tree_full(test_gene_tree->root, stdout);

	/* Convert to balanced tree */
	vine_to_tree(test_gene_tree);

	/* Print tree */
	printf("\n\nPrinting balanced tree:\n");
	printf("-----------------------\n");
	print_tree_full(test_gene_tree->root, stdout);

	/* Cleanup and exit */
	free_gene_tree(test_gene_tree);
}
