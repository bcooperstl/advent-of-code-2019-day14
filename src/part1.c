#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "element.h"

int main (int argc, char * argv[])
{
    FILE * infile=NULL;
    char buffer[1024];
    elements world;
    memset(&world, 0, sizeof(elements));
    
    element * ore = add_element(&world, ORE);
    set_output_amount(ore, 1);
    
    memset(&buffer, '\0', sizeof(buffer));
    
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s filename\n", argv[0]);
        exit(1);
    }
    
    fprintf(stderr, "Opening file %s\n", argv[1]);
    
    infile=fopen(argv[1], "r");
    if (infile == NULL)
    {
        fprintf(stderr, "Error opening file %s\n", argv[1]);
        exit(1);
    }
    
    while (fgets(buffer, sizeof(buffer), infile))
    {
        char * eol = strchr(buffer, '\r');
        if (eol)
        {
            *eol='\0';
        }
        eol = strchr(buffer, '\n');
        if (eol)
        {
            *eol='\0';
        }
        process_line(&world, buffer);
    }
    
    fclose(infile);
    //dump_names(&world);
    fix_up_component_pointers(&world);
    element * fuel = find_element(&world, FUEL);
    fuel->needed=1;
    work_it(&world);
    printf("There were %d units of %s used\n", ore->produced, ORE);
    
    return 0;
}
