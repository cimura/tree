#include <stdlib.h>
#include <stdio.h>
#include <strings.h>

typedef enum s_type
{
	T_CMD,
	T_PIPE,
	T_SEMI
}	t_type;


typedef struct s_node
{
	t_type	type;
	char	**argv;
	struct s_node *left;
	struct s_node *right;
}	t_node;

int	count_dptr(char **argv)
{
	int	count = 0;
	while (argv[count])
		count++;
	return count;
}

char	**make_dptr(char **argv)
{
	char	**result = malloc(sizeof(char *) * count_dptr(argv));
	int	i = 0;
	while (argv[i] != NULL)
	{
		result[i] = argv[i];
		i++;
	}
	result[i] = NULL;
	return result;
}

t_node	*make_tree(char **argv)
{
	t_node	*new_node = malloc(sizeof(t_node));
	new_node->argv = make_dptr(argv);
	new_node->left = NULL;
	new_node->right = NULL;
	return new_node;
}

t_node	*insert_node(char **argv, t_node *node)
{
	if (node == NULL)
		return make_tree(argv);
	else if (strcmp(*argv, "|") == 0)
		node->right = insert_node(argv, node->right);
	else if (strcmp(*argv, ";") == 0)
		node->left = insert_node(argv, node->left);
	return node;
}

void	print_inorder(t_node *node)
{
	if (node == NULL)
		return;
	print_inorder(node->left);
	printf("%s\n", *node->argv);
	print_inorder(node->right);
}

int	main(int argc, char **argv, char **envp) {
	t_node	*root = NULL;

	int i = 1;
	while (i < argc) {
		root = insert_node(&argv[i], root);
		i += count_dptr(root->argv);
	}
	print_inorder(root);
}