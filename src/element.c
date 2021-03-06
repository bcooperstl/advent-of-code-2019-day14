#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "element.h"

element * add_element(elements * world, char * name)
{
    //printf("name %s num_elements %ld\n", name, world->num_elements);
    if (world->num_elements == MAX_ELEMENTS)
    {
        fprintf(stderr, "TOO MANY ELEMENTS IN WORLD; INCREASE MAX_ELEMENTS\n");
        exit(1);
    }
    element * ret = &world->list[world->num_elements];
    //printf("Added %s to the world in position #%d\n", name, world->num_elements);
    world->num_elements++;
    strncpy(ret->name, name, NAME_LENGTH);
    return ret;
}

void set_output_amount(element * element, long output_amount)
{
    element->output_amount=output_amount;
    //printf("Set the output amount for %s to %ld\n", element->name, output_amount);
}

void add_component(element * element, char * component_name, long component_amount)
{
    if (element->component_count == MAX_COMPONENTS)
    {
        fprintf(stderr, "TOO MANY COMPONENTS FOR ELEMENT %s; INCREASE MAX_COMPONENTS\n", element->name);
        exit(1);
    }
    
    //printf("Set element %s to have component #%d of %ld units of %s\n", element->name, element->component_count, component_amount, component_name);
    strncpy(element->component_names[element->component_count], component_name, NAME_LENGTH);
    element->component_amounts[element->component_count]=component_amount;
    element->component_count++;
}

element * find_element(elements * world, char * name)
{
    for (long i=0; i<world->num_elements; i++)
    {
        if (strncmp(world->list[i].name, name, NAME_LENGTH) == 0) // match found
        {
            return &world->list[i];
        }
    }
    return NULL;
}

void fix_up_component_pointers(elements * world)
{
    for (long i=0; i<world->num_elements; i++)
    {
        //printf("Fixing up components for %s (%d)\n", world->list[i].name, i);
        for (long j=0; j<world->list[i].component_count; j++)
        {
            element * found = find_element(world, world->list[i].component_names[j]);
            if (found == NULL)
            {
                fprintf(stderr, "ELEMENT %s NOT FOUND IN WORLD. CHECK LOGIC\n", world->list[i].component_names[j]);
                exit(1);
            }
            world->list[i].components[j] = found;
            //printf("  Fixed up component %ld to point to %s\n", j, world->list[i].components[j]->name);
        }
    }
}

void work_it(elements * world)
{
    while (any_needed(world))
    {
        for (long i=0; i<world->num_elements; i++)
        {
            element * elem = &world->list[i];
            if (elem->needed > 0)
            {
                //printf("element %s has %ld needed\n", elem->name, elem->needed);
                if (elem->available > 0)
                {
                    long avail_to_use=(elem->needed > elem->available ? elem->available : elem->needed);
                    //printf("  element %s has %ld available using %ld of them\n", elem->name, elem->available, avail_to_use);
                    elem->needed-=avail_to_use;
                    elem->available-=avail_to_use;
                    elem->consumed+=avail_to_use;
                    if (elem->needed==0)
                    {
                        //printf("   element %s had need filled due to available. no need to run reaction\n", elem->name);
                        continue;
                    }
                    //printf("   element %s now has %ld needed\n", elem->name, elem->needed);
                }
                
                long reaction_count=((elem->needed-1)/elem->output_amount)+1;
                //printf("  the reaction produces %ld, so it needs to run it %ld times\n", elem->output_amount, reaction_count);
                
                // increase the needed for each component
                for (long j=0; j<elem->component_count; j++)
                {
                    element * component=elem->components[j];
                    long component_needed_for_reaction=reaction_count*elem->component_amounts[j];
                    //printf("  need %ld of component %s\n", component_needed_for_reaction, component->name);
                    component->needed+=component_needed_for_reaction;
                }
                
                // increase the output for this element
                long num_produced=reaction_count*elem->output_amount;
                
                elem->produced+=num_produced;
                elem->available+=num_produced;
                //printf("  produces %ld of %s bring total available to %ld and total produced to %ld\n", num_produced, elem->name, elem->available, elem->produced);
                
                // consume those that are needed
                elem->available-=elem->needed;
                elem->consumed+=elem->needed;
                //printf("   %ld of element %s consumed, leaving %ld available and upping total of it consumed to %ld\n", elem->needed, elem->name, elem->available, elem->consumed);
                elem->needed=0;
            }
        }
    }
}

long any_needed(elements * world)
{
    for (long i=0; i<world->num_elements; i++)
    {
        if (world->list[i].needed > 0)
            return 1;
    }
    return 0;
}

// sample line: 12 HKGWZ, 1 GPVTF, 8 PSHF => 9 Q4DVJ
// assumption is that line has end of line characters removed
void process_line(elements * world, char * line)
{
    element * output;
    long qty;
    long len=strlen(line);
    char name[NAME_LENGTH+1];
    char * endptr;
    
    memset(&name, NAME_LENGTH+1, '\0');
    
    // first step - find the output component
    char * equals_pos = strchr(line, '=');
    char * num_pos=equals_pos+3;
    qty=strtol(num_pos, &endptr, 10);
    endptr++; // skip over the space
    strncpy(name, endptr, NAME_LENGTH);
    
    output=find_element(world, name);
    if (output == NULL)
    {
        output=add_element(world, name);
    }
    set_output_amount(output, qty);
    
    // find all the components
    char * pos = line;
    long done=0;
    while (!done)
    {
        qty=strtol(pos, &endptr, 10);
        pos=endptr+1; // skip over the space
        endptr=strchr(pos, ',');
        if (endptr == NULL) // no comma found. need to find the first space. this will be the last component
        {
            endptr=strchr(pos, ' ');
            done=1;
        }
        *endptr='\0';
        memset(&name, NAME_LENGTH+1, '\0');
        strncpy(name, pos, NAME_LENGTH);
        
        if (find_element(world, name) == NULL)
        {
            add_element(world, name);
        }
        add_component(output, name, qty);
        
        pos=endptr+2; // skip over the comma and the space
    }
}

void dump_names(elements * world)
{
    for (long i=0; i<world->num_elements; i++)
    {
        printf("%d [%s]\n", i, world->list[i].name);
        for (long j=0; j<world->list[i].component_count; j++)
        {
            printf("  %ld[%d] [%s]\n", i, j, world->list[i].component_names[j]);
        }
    }
}

void reset_world(elements * world)
{
    for (long i=0; i<world->num_elements; i++)
    {
        world->list[i].needed=0;
        world->list[i].produced=0;
        world->list[i].consumed=0;
        world->list[i].available=0;
    }
}
