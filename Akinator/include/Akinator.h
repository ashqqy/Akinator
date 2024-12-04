#ifndef AKINATOR
#define AKINATOR

#include <stdio.h>

#include "Tree.h"

struct akinator_t
{
    tree_node_t* root_node;
    stack_t* stack;
    char* buffer;
    size_t buffer_shift;
    size_t buffer_size;
    char* database_buffer;
};

enum akinator_mode_t
{
    SEX_MODE          = 0,
    GUESSING_MODE     = 1,
    DEFINITION_MODE   = 2,
    COMPARATION_MODE  = 3,
    DATABASESHOW_MODE = 4,
    EXITNOSAVING_MODE = 5,
    EXITSAVING_MODE   = 6
};

enum akinator_error_t
{
    AKINATOR_OK             = 1,
    ALLOCATION_ERROR        = 2,
    EMPTY_DATABASE          = 3,
    INVALID_DATABASE_FORMAT = 4,
    BUFFER_OVERFLOW         = 5,
    STACK_ERROR             = 6,
    NULL_PTR                = 7
};

const size_t BUFFER_SIZE = 1024;

akinator_error_t AkinatorInit (akinator_t* akinator);

akinator_error_t ReadDataBase (FILE* database, akinator_t* akinator);

akinator_mode_t StartMenuMode ();
akinator_error_t GuessingMode (akinator_t* akinator);
void DefinitionMode ();
void ComparationMode ();
void DatabaseShowMode ();
akinator_error_t DatabaseWrite (tree_node_t* node);

#define ExitSavingMode(root_node) do             \
{                                                \
    CustomWarning (root_node != NULL, NULL_PTR); \
    DatabaseWrite (root_node);                   \
    break;                                       \
}                                                \
while (0)                                        

char* BufferResize (char* buffer, size_t* buffer_size, size_t new_buffer_size);

#endif // AKINATOR
