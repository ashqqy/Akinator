#include <stdio.h>
#include <stdlib.h>

#include "Akinator.h"
#include "CustomWarning.h"
#include "Tree.h"
#include "Common.h"
#include "Stack.h"

//------------------------------------------------------

int main () // TODO int argc, const char* argv
{
    FILE* database_in  = fopen ("./Database/Database_in.txt", "rb");
    FILE* database_out = fopen ("./Database/Database_out.txt", "w");
    akinator_t akinator = {};
    AkinatorInit (&akinator);

    NodeLink (NodeCreate ("чикибамбони"), NULL, &akinator.root_node);    

    while (1)
    {
        akinator_mode_t mode = StartMenuMode ();

        if      (mode == GUESSING_MODE)
            GuessingMode (&akinator);
        else if (mode == DEFINITION_MODE)
            DescriptionMode_(akinator)
        else if (mode == COMPARATION_MODE)
            ComparationMode_(akinator)
        else if (mode == DATABASESHOW_MODE)
            DatabaseShowMode_(akinator)
        else if (mode == DATABASEUPLOAD_MODE)
            DataBaseUploadMode_(database_in, akinator)
        else if (mode == EXITNOSAVING_MODE)
            ExitNoSavingMode()
        else if (mode == EXITSAVING_MODE) 
            ExitSavingMode_(akinator.root_node, database_out);
    }
    
    TreeDestroy (akinator.root_node);
    free (akinator.database_buffer); akinator.database_buffer = NULL;
    free (akinator.buffer); akinator.buffer = NULL;

    return 0;
}

//------------------------------------------------------
