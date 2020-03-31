#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "element.h"

element * add_element(elements * world, char * name)
{
    if (world->num_elements == MAX_ELEMENTS)
    {
        fprintf(stderr, "TOO MANY ELEMENTS IN WORLD; INCREASE MAX_ELEMENTS\n");
        exit(1);
    }
    element * ret = &world->list[world->num_elements];
    printf("Added %s to the world in position #%d\n", name, world->num_elements);
    world->num_elements++;
    strncpy(ret->name, name, NAME_LENGTH);
    return ret;
}

void set_output_amount(element * element, int output_amount)
{
    element->output_amount=output_amount;
    printf("Set the output amount for %s to %d\n", element->name, output_amount);
}

void add_component(element * element, char * component_name, int component_amount)
{
    if (element->component_count == MAX_COMPONENTS)
    {
        fprintf(stderr, "TOO MANY COMPONENTS FOR ELEMENT %s; INCREASE MAX_COMPONENTS\n", element->name);
        exit(1);
    }
    
    printf("Set element %s to have component #%d of %d units of %s\n", element->name, element->component_count, component_amount, component_name);
    strncpy(element->component_names[element->component_count], component_name, NAME_LENGTH);
    element->component_amounts[element->component_count]=component_amount;
    element->component_count++;
}

element * find_element(elements * world, char * name)
{
    for (int i=0; i<world->num_elements; i++)
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
    for (int i=0; i<world->num_elements; i++)
    {
        printf("Fixing up components for %s (%d)\n", world->list[i].name, i);
        for (int j=0; j<world->list[i].component_count; j++)
        {
            element * found = find_element(world, world->list[i].component_names[j]);
            if (found == NULL)
            {
                fprintf(stderr, "ELEMENT %s NOT FOUND IN WORLD. CHECK LOGIC\n", world->list[i].component_names[j]);
                exit(1);
            }
            world->list[i].components[j] = found;
            printf("  Fixed up component %d to point to %s\n", j, world->list[i].components[j]->name);
        }
    }
}

// sample line: 12 HKGWZ, 1 GPVTF, 8 PSHF => 9 Q4DVJ
// assumption is that line has end of line characters removed
void process_line(elements * world, char * line)
{
    element * output;
    long qty;
    int len=strlen(line);
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
    int done=0;
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
