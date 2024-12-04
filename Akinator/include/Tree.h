#ifndef TREE
#define TREE

#include "Common.h"

tree_node_t* NodeCreate (tree_elem_t data);
tree_node_t* NodeLink (tree_node_t* node, tree_node_t* parent_node, tree_node_t** node_to_link_to);
tree_node_t* NodeEditData (tree_node_t* node, tree_elem_t new_data);
void TreeDestroy (tree_node_t* node);

void TreeDump (tree_node_t* root_node);
void TreeNodeDescrDump (FILE* dump_file, tree_node_t* node);
void TreeNodeLinkDump (FILE* dump_file, tree_node_t* node);

#endif //TREE