#ifndef AKINATOR
#define AKINATOR

#include <stdio.h>

typedef const char* tree_elem_t;

struct tree_node_t
{
    tree_elem_t data;
    tree_node_t* left;
    tree_node_t* right;
};

struct tree_t
{
    tree_node_t* root_node;
    int n_nodes;
};

enum tree_error_t
{
    CHIKI_PUKI = 1,

    ALLOCATION_ERROR = 2
};

enum MODE
{
    SEX_MODE          = 0,
    GUESSING_MODE     = 1,
    DEFINITION_MODE   = 2,
    COMPARATION_MODE  = 3,
    DATABASESHOW_MODE = 4,
    EXITNOSAVING_MODE = 5,
    EXITSAVING_MODE   = 6
};

tree_error_t TreeInit (tree_t* tree);

tree_error_t CreateAndLinkNode (tree_t* tree, tree_elem_t data, tree_node_t** node_to_link_to);
tree_node_t* CreateNode (tree_t* tree, tree_elem_t data);
tree_error_t LinkNode (tree_node_t* node, tree_node_t** node_to_link_to);
tree_error_t EditNodeData (tree_node_t* node, tree_elem_t new_data);

MODE StartMenuMode ();
void GuessingMode (tree_t* tree);
void DefinitionMode ();
void ComparationMode ();
void DatabaseShowMode ();
void ExitNoSavingMode ();
void ExitSavingMode ();

void TreeDump (tree_t* tree);
void TreeNodeDescrDump (FILE* dump_file, tree_node_t* node);
void TreeNodeLinkDump (FILE* dump_file, tree_node_t* node);

#endif // AKINATOR
