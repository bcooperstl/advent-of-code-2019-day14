#ifndef __ELEMENT_H__
#define __ELEMENT_H__

#define NAME_LENGTH 16
#define MAX_COMPONENTS 32
#define MAX_ELEMENTS 256

#define ORE "ORE"
#define FUEL "FUEL"

struct element {
    char name[NAME_LENGTH+1];
    long output_amount;
    long component_count;
    struct element * components[MAX_COMPONENTS]; // the list of all component elements needed to make this element
    char component_names[MAX_COMPONENTS][NAME_LENGTH+1]; // the list of names of components
    long component_amounts[MAX_COMPONENTS]; // amount of input for each component
    long needed;
    long produced;
    long consumed;
    long available;
};

typedef struct element element;

struct elements
{
    element list[MAX_ELEMENTS];
    long num_elements;
};

typedef struct elements elements;

element * add_element(elements * world, char * name);
element * find_element(elements * world, char * name);
void set_output_amount(element * element, long output_amount);
void add_component(element * element, char * component_name, long component_amount);
void fix_up_component_pointers(elements * world);
void process_line(elements * world, char * line);
long any_needed(elements * world);
void work_it(elements * world);
void reset_world(elements * world);
void dump_names(elements * world);

#endif
