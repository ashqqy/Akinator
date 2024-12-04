#ifndef COMMON
#define COMMON

#include <stdio.h>

typedef const char* tree_elem_t;

struct tree_node_t
{
    tree_node_t* parent;
    tree_elem_t  data;
    tree_node_t* left;
    tree_node_t* right;
};

typedef tree_node_t* stack_elem_t;

struct stack_t
    {
    ssize_t size;
    ssize_t capacity;
    stack_elem_t* data;
    };

//----------------------------------------------------------------

size_t FileSizeFinder (FILE* file_input);
void CleanBuffer ();

//----------------------------------------------------------------

#endif // COMMON