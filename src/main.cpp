#include <stdio.h>
#include <stdlib.h>
#include "Akinator.h"
#include "Common.h"

//------------------------------------------------------

int main ()
{
    FILE* database = fopen ("./Database/Database.txt", "a+");

    tree_t tree = {};
    TreeInit (&tree);

    // если есть база данных, то читаем оттуда
    if (database != NULL && FileSizeFinder (database) > 10)
    {
    ;
    }
    // иначе создаём первый лист
    else    
    {
        CreateAndLinkNode (&tree, "Ваш воображаемый друг", &tree.root_node);
    }

    while (1)
    {
        MODE mode = StartMenuMode ();

        if      (mode == GUESSING_MODE)
            GuessingMode (&tree);
        else if (mode == DEFINITION_MODE)
            DefinitionMode ();
        else if (mode == COMPARATION_MODE)
            ComparationMode ();
        else if (mode == DATABASESHOW_MODE)
            DatabaseShowMode ();
        else if (mode == EXITNOSAVING_MODE)
            ExitNoSavingMode ();
        else if (mode == EXITSAVING_MODE) 
            ExitSavingMode ();
    }

    TreeDump (&tree);

    return 0;
}

//------------------------------------------------------
