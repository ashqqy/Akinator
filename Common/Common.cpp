#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "Common.h"

//-------------------------------------------------------

size_t FileSizeFinder (FILE* file_input)
    {
    fseek (file_input, 0L, SEEK_END); //лучше fstat
    size_t size_file_input = (size_t) ftell(file_input);
    fseek(file_input, 0L, SEEK_SET);

    return size_file_input;
    }

//--------------------------------------------------------------

void CleanBuffer ()
    {
    int last_symb = 0;
    while ((last_symb = getchar ()) != '\n' && last_symb != EOF)
        {
        }
    }
    
//--------------------------------------------------------------
