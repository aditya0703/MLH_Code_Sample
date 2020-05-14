// This is the header file used for the inventory.c function
// It outlines all the needed methods and structs for the commands and variables
//
// @author: ak3064 Aditya Khanna
//
#ifndef INVENTORY_H
#define INVENTORY_H

extern char * trim(char *);
// extern int getline(char **, size_t *, FILE *);

#define ID_MAX 11 //max ID length

// This struct defines the contents in a part
struct part {
    char id[ID_MAX+1];        // ID_MAX plus NUL
    struct part * next; // the next part in the list of parts
};

// This struct defines the contents in an assembly
struct assembly {
    char id[ID_MAX+1];		 // ID_MAX plus NUL
    int capacity;		 // max capacity for this assembly
    int on_hand;		 // current number available for this assembly
    struct items_needed * items; // parts/sub-assemblies needed for this ID
    struct assembly * next;      // the next assembly in the inventory list
};

// This struct defines the contents in an item
struct item {
    char id[ID_MAX+1];           // ID_MAX plus NUL
    int quantity;		 // number of items
    struct item * next; // next item in the part/assembly list
};

// This struct defines the contents in an inventory (parts and assemblies)
struct inventory {
    struct part * part_list;         // list of parts by ID
    int part_count;                  // number of distinct parts
    struct assembly * assembly_list; // list of assemblies by ID
    int assembly_count;              // number of distinct assemblies
};

// This struct defines the items needed to make required assemblies
struct items_needed {
    struct item * item_list;
    int item_count;
};

// This struct is used for requesting a command
struct req {
    char * req_string;			// command 
    void (*req_fn)(struct inventory *); // request funtion pointer
};

typedef struct inventory inventory_t;		//typedef for inventory
typedef struct items_needed items_needed_t;	//typedef for items_needed
typedef struct item item_t;			//typedef for item
typedef struct part part_t;			//typedef for part
typedef struct assembly assembly_t;		//typedef for assembly

// This function checks to see if the id matches any id in the part list
//
// @param pp the part list to be traversed
// @param id the id that we're checking for
// @return the part if there is a match
//
part_t * lookup_part(part_t * pp, char * id);

// This function checks to see if the id matches any id in the assembly list
//
// @param ap the assembly list to be traversed
// @param id the id that we're checking for
// @return the assembly if there is a match
//
assembly_t * lookup_assembly(assembly_t * ap, char * id);

// This function checks to see if the id matches any id in the item list
//
// @param ip the part list to be traversed
// @param id the id that we're checking for
// @return the item if there is a match
//
item_t * lookup_item(item_t * ip, char * id);

// This function adds the part with the id to the beginning of the linked list
// It also increments the part count and moves the next list
//
// @param invp the inventory we're adding to
// @param id the id of the part we're adding
//
void add_part(inventory_t * invp, char * id);

// This function adds the assembly with the id to the beginning of the list
// It also increments the assembly count, sets on_hand and moves the next list
//
// @param invp the inventory we're adding to
// @param id the id of the assembly we're adding
// @param capacity the max capacity an assembly can hold
// @param items the items needed to manufacture an assmebly
//
void add_assembly(inventory_t * invp,
                  char * id,
                  int capacity,
                  items_needed_t * items);


// This function adds the item  with the id to the beginning of the linked list
// It also increments the item count and moves the next list
//
// @param items the items needed list we're adding to
// @param id the id of the item we're adding
// @param quantity the quantity of the items 
//
void add_item(items_needed_t * items, char * id, int quantity);

// these are used for sorting purposes

// This function converts the linked list to an array 
//
// @param count the number of elements in the list
// @param part_list the linked list to be converted
// @return an part array converted via the linked list 
//
part_t ** to_part_array(int count, part_t * part_list);

// This function converts the linked list to an array
//
// @param count the number of elements in the list
// @param assembly_list the linked list to be converted
// @return an assembly array converted via the linked list
//
assembly_t ** to_assembly_array(int count, assembly_t * assembly_list);

// This function converts the linked list to an array 
//
// @param count the number of elements in the list
// @param item_list the linked list to be converted
// @return an item array converted via the linked list 
// 
item_t ** to_item_array(int count, item_t * item_list);

// This function compares the two parts and returns a value used for sorting
//
// @param part1 the first part to be compared
// @param part2 the second part to be compared
// @return the string comparison of the ID values
//
int part_compare(const void * part1, const void * part2);

// This function compares the two assemblies and returns value used for sorting
//
// @param assembly1 the first assembly to be compared
// @param assembly2 the second assembly to be compared
// @return the string comparison of the ID values
// 
int assembly_compare(const void * assembly1, const void * assembly2);

// This function compares the two items and returns a value used for sorting
//
// @param item1 the first item to be compared
// @param item2 the second item to be compared
// @return the string comparison of the ID values
//
int item_compare(const void * item1, const void * item2);

// This function manufactures n instances of an assembly
//
// @param invp the inventory we're using to manufacture
// @param id the id of the assembly
// @param n amount of manufactures needed
// @param parts the items needed to manufacture
//
void make(inventory_t * invp, char * id, int n, items_needed_t * parts);

// This function retrieves enough units of a particular assembly to manufacture
// the given assembly
//
// @param invp the inventory we're using to manufacture
// @param id the id of the assembly
// @param n amount of manufactures needed
// @param parts the items needed to manufacture
//
void get(inventory_t * invp, char * id, int n, items_needed_t * parts);

// This functions checks conditions and the prints the appropriate inventory
//
// @param invp the inventory we're printing from
//
void print_inventory(inventory_t * invp);

// This function checks conditions and then prints the parts in the inventory
//
// @param invp the inventory we're printing from
//
void print_parts(inventory_t * invp);

// This functions prints the items needed to create an assembly
//
// @param items the list of items needed
//
void print_items_needed(items_needed_t * items);

// This function free's all the allocated memory in the inventory_t 
//
// @param invp the inventory_t that is being free'd
//
void free_inventory(inventory_t * invp);

#endif // INVENTORY_H
