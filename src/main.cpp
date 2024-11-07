#include <stdio.h>

//------------------------------------------------------

int main ()
{
    FILE* dump_file = fopen ("./dump/dump.dot", "w");

    fclose (dump_file);
    return 0;
}

//------------------------------------------------------
