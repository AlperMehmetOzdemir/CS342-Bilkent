#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct node
{
    char *key;
    struct node *left, *right;
};

struct node *search(char *key, struct node *leaf)
{
    if( leaf != NULL )
    {
        if(key == leaf->key)
        {
            return leaf;
        }
        else if(key < leaf->key)
        {
            return search(key, leaf->left);
        }
        else
        {
            return search(key, leaf->right);
        }
    }
    else return 0;
}

insert(char *key, struct node **leaf)
{
    if( *leaf == NULL )
    {
        *leaf = (struct node*) malloc( sizeof( struct node ) );
        (*leaf)->key = key;
        /* initialize the children to null */
        (*leaf)->left = NULL;
        (*leaf)->right = NULL;
    }
    else if(strcmp(key, (*leaf)->key) < 0)
    {
        insert( key, &(*leaf)->left );
    }
    else if(strcmp(key, (*leaf)->key) > 0)
    {
        insert( key, &(*leaf)->right );
    }
}

void destroy_tree(struct node *leaf)
{
    if( leaf != NULL )
    {
        destroy_tree(leaf->left);
        destroy_tree(leaf->right);
        free( leaf );
    }
}

void inorder(struct node *tree)
{
    if (tree)
    {
        inorder(tree->left);
        printf(tree->key);
        inorder(tree->right);
    }
}

int main() {

    struct node *root;

    char *text = "asd";

    insert(text, &root);

    inorder(root);

    return 0;
}