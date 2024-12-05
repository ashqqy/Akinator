#ifndef AKINATOR
#define AKINATOR

#include <stdio.h>

#include "Tree.h"

struct akinator_t
{
    tree_node_t* root_node;
    stack_t* stack1;
    stack_t* stack2;
    char* buffer;
    size_t buffer_shift;
    size_t buffer_size;
    char* database_buffer;
};

enum akinator_mode_t
{
    SEX_MODE            = 0,
    GUESSING_MODE       = 1,
    DEFINITION_MODE     = 2,
    COMPARATION_MODE    = 3,
    DATABASESHOW_MODE   = 4,
    DATABASEUPLOAD_MODE = 5,
    EXITNOSAVING_MODE   = 6,
    EXITSAVING_MODE     = 7
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
akinator_error_t GuessingAddNewCharacter (akinator_t* akinator, char* temp_buf, int* n_readen, tree_node_t* current_node);
akinator_error_t GuessingMode (akinator_t* akinator);
void AkinatorNodeSearch (akinator_t* akinator, char* temp_buf, tree_node_t** founded_node, const char* phrase);
akinator_error_t AkinatorNodePathFill (stack_t* stack, tree_node_t* node);
akinator_error_t AkinatorDefinitionSmall (stack_t* stack);
akinator_error_t DescriptionMode (akinator_t* akinator);
akinator_error_t ComparationMode (akinator_t* akinator);
akinator_error_t DatabaseShowMode (akinator_t* akinator);
akinator_error_t DatabaseWrite (tree_node_t* node, FILE* database_out);

#define DescriptionMode_(akinator)                  \
{                                                   \
    DescriptionMode (&akinator);                    \
    StackDestroy (akinator.stack1);                 \
    free (akinator.stack1); akinator.stack1 = NULL; \
}

#define ComparationMode_(akinator)                  \
{                                                   \
    ComparationMode (&akinator);                    \
    StackDestroy (akinator.stack1);                 \
    free (akinator.stack1); akinator.stack1 = NULL; \
    StackDestroy (akinator.stack2);                 \
    free (akinator.stack2); akinator.stack2 = NULL; \
}

#define DatabaseShowMode_(akinator)                 \
{                                                   \
    TreeDump (akinator.root_node);                  \
}               

#define DataBaseUploadMode_(database_in, akinator)                  \
{                                                                   \
    TreeDestroy (akinator.root_node);                               \
    akinator_error_t dberr = ReadDataBase (database_in, &akinator); \
                                                                    \
    if (dberr != AKINATOR_OK)                                       \
    {                                                               \
        if (akinator.stack1 != NULL)                                \
            while (akinator.stack1->size != 0)                      \
            {                                                       \
                tree_node_t* node = NULL;                           \
                StackPop (akinator.stack1, &node);                  \
                TreeDestroy (node);                                 \
            }                                                       \
    }                                                               \
                                                                    \
    StackDestroy (akinator.stack1);                                 \
    free (akinator.stack1); akinator.stack1 = NULL;                 \
}

#define ExitNoSavingMode() break; // TODO caps for all defines

#define ExitSavingMode_(root_node, database_out)    \
{                                                   \
    CustomWarning (root_node    != NULL, NULL_PTR); \
    CustomWarning (database_out != NULL, NULL_PTR); \
    DatabaseWrite (root_node, database_out);        \
    break;                                          \
}       


char* BufferResize (char* buffer, size_t* buffer_size, size_t new_buffer_size);

#endif // AKINATOR
