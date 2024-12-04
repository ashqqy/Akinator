#include <stdio.h>
#include <stdlib.h>

#include "Akinator.h"
#include "CustomWarning.h"
#include "Tree.h"
#include "Common.h"
#include "Stack.h"

//------------------------------------------------------

int main () //int argc, const char* argv
{
    FILE* database = fopen ("./Database/Database.txt", "a+");
    akinator_t akinator = {};
    AkinatorInit (&akinator);

    // если есть база данных, то читаем оттуда
    akinator_error_t dberr = ReadDataBase (database, &akinator);

    // если базы данных нет или при чтении была ошибка, то создаём первый лист
    if (dberr != AKINATOR_OK)
    {
        // освобождаем узлы, которые могли быть созданы при чтении базы данных
        while (akinator.stack->size != 0)
        {
            tree_node_t* node = NULL;
            StackPop (akinator.stack, &node);
            TreeDestroy (node);
        }

        NodeLink (NodeCreate ("Ваш воображаемый друг"), NULL, &akinator.root_node);  
    }

    StackDestroy (akinator.stack);
    free (akinator.stack); akinator.stack = NULL;     

    while (1)
    {
        akinator_mode_t mode = StartMenuMode ();

        if      (mode == GUESSING_MODE)
            GuessingMode (&akinator);
        else if (mode == DEFINITION_MODE)
            DefinitionMode ();
        else if (mode == COMPARATION_MODE)
            ComparationMode ();
        else if (mode == DATABASESHOW_MODE)
            DatabaseShowMode ();
        else if (mode == EXITNOSAVING_MODE)
            break;
        else if (mode == EXITSAVING_MODE) 
            ExitSavingMode (akinator.root_node);
    }
    
    TreeDump (akinator.root_node);
    TreeDestroy (akinator.root_node);
    free (akinator.database_buffer); akinator.database_buffer = NULL;
    free (akinator.buffer); akinator.buffer = NULL;

    return 0;
}

//------------------------------------------------------
