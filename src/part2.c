#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "element.h"

#define ONE_TRILLION 1000000000000l

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
    
    long prev=0;
    long curr=1;
    long found=0;
    while (found <= ONE_TRILLION)
    {
        reset_world(&world);
        prev=curr;
        curr*=2l; // double curr
        fuel->needed=curr;
        work_it(&world);
        found=ore->produced;
        printf("working - %ld resulted in %ld ore\n", curr, ore->produced);
    }
    printf("***answer is between %ld and %ld\n", prev, curr);
    
    // binary search for the win
    long low=prev;
    long high=curr;
    long last_less=prev;
    while (high-low>1)
    {
        long guess=(low+high)/2l;
        reset_world(&world);
        fuel->needed=guess;
        work_it(&world);
        printf("guessing %ld between %ld and %ld resulted in %ld ore\n", guess, low, high, ore->produced);
        if (ore->produced >= ONE_TRILLION) // too high
        {
            high=guess;
        }
        else
        {
            low=guess;
            last_less=guess;
        }
    }
    
    printf("****The best option produces %ld fuel\n", last_less);
    
    return 0;
}
