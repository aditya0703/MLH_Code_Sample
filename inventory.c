// Description: Interative inventory system for a company. Inventory system
// keeps track of the parts used to manufacture assemblies and of the 
// assemblies that can be sold to customers.
//
// Author: ak3064 Aditya Khanna
//

//conflicting types
#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "inventory.h"
#include "trimit.h"

//Finds a part with an id
part_t * lookup_part( part_t * pp, char * id ) {;
	if( pp ) {
		for( part_t * copy = pp; copy; copy = copy->next ) {	
			if( strcmp( id, copy->id ) == 0 )
				return copy;
		}
	}
	return NULL;
}

//Finds an assembly with an id
assembly_t * lookup_assembly( assembly_t *ap, char * id ) {
	if( ap ) {
		for( assembly_t * copy = ap; copy; copy = copy->next ) {
			if( strcmp( id, copy->id ) == 0 )
				return copy;
		}
	}
	return NULL;
}

//Finds a part with an id
item_t * lookup_item( item_t * ip, char * id ) {
	if( ip ) {
                for( item_t * copy = ip; copy; copy = copy->next ) {
                        if( strcmp( id, copy->id ) == 0 )
                                return copy;
                }
        }
	return NULL;
}

//Adds a part to a list
void add_part( inventory_t * invp, char * id ) {
		part_t * newp = NULL;
		newp = calloc( 1, sizeof( part_t ) );
		strcpy( newp->id, id );
		newp->next = invp->part_list;
		invp->part_list = newp;
		invp->part_count++;
}

//Adds an assembly to a list
void add_assembly( inventory_t * invp, char * id, int capacity,
		items_needed_t * items ) {
		assembly_t * newp = calloc( 1,  sizeof( assembly_t ) );
		strcpy( newp->id, id );
		newp->capacity = capacity;
		newp->items = items;
		newp->on_hand = 0;
		newp->next = invp->assembly_list;
		invp->assembly_list = newp;
		invp->assembly_count++;
}

//Adds an item to a list
void add_item( items_needed_t * items, char * id, int quantity ) {
	item_t * isp = lookup_item( items->item_list, id );
	if( isp ) //if it already exists
		isp->quantity += quantity;
	else {
		item_t * newp = calloc( 1, sizeof( item_t ) );
		strcpy( newp->id, id );
		newp->quantity = quantity;
		items->item_count++;
		newp->next = items->item_list;
		items->item_list = newp;	
	}	
}

//Makes linked list an part array
part_t ** to_part_array( int count, part_t * part_list ) {
	part_t ** parts = calloc( count, sizeof( part_t ) );
	part_t * pp = part_list;
	for( int i = 0; i < count; i++ ) {
		parts[i] = pp;
		pp = pp->next;
	}
	return parts;
}

//Makes linked list an assembly array
assembly_t ** to_assembly_array( int count, assembly_t * assembly_list ) {
	assembly_t ** assem = calloc( count, sizeof( assembly_t ) );
	assembly_t * ap = assembly_list;
	for( int i = 0; i < count; i++ ) {
		assem[i] = ap;
		ap = ap->next;
	}
	return assem;
}

//Makes linked list an item array
item_t ** to_item_array( int count, item_t * item_list ) {
	if( item_list->id[0] == 'P' )
		return (item_t **) to_part_array( count, (part_t *)item_list );
	return (item_t **) to_assembly_array( count, (assembly_t *)item_list );
}

//Compares two parts
int part_compare( const void * part1, const void * part2 ) {
	part_t * temp1 = *( part_t ** ) part1;
	part_t * temp2 = *( part_t ** ) part2;
	return strcmp( temp1->id, temp2->id );
}

//Compares two assemblies
int assembly_compare( const void * assembly1, const void * assembly2 ) {
	assembly_t * temp1 = *( assembly_t ** ) assembly1;
	assembly_t * temp2 = *( assembly_t ** ) assembly2;
	return strcmp( temp1->id, temp2->id );
}

//Compares two items
int item_compare( const void * item1, const void * item2 ) {
	if( ( *( item_t ** )item1 )->id[0] == 'P' )
		return part_compare( item1, item2 );
	return assembly_compare( item1, item2);
}

//Used for manufaturing assemblies
void make( inventory_t * invp, char * id, int n, items_needed_t * parts ) {
	assembly_t * assem = lookup_assembly( invp->assembly_list, id );
	printf( ">>> make %d units of assembly %s\n", n, assem->id );
	for( item_t * itm = assem->items->item_list; itm; itm = itm->next ) {
		if( lookup_part( invp->part_list, itm->id ) )
			add_item( parts, itm->id, n * ( itm->quantity ) );
		else
			get( invp, itm->id, n * ( itm->quantity ), parts );

	}	
}

//Used for utilizing resources for manufacturing
void get( inventory_t * invp, char * id, int n, items_needed_t * parts ) {
	assembly_t * assem = lookup_assembly( invp->assembly_list, id );
	if( assem->on_hand >= n ) //more on hand than capacity
		assem->on_hand = ( assem->on_hand ) - n;
	else {
		make( invp, id, n - ( assem->on_hand ), parts ); //recursive
		assem->on_hand = 0; //reset on hand
	}

}

//Used for printing the inventory
void print_inventory( inventory_t * invp ) {
	char * id = strtok( NULL, " \n\t" );
	int shift1, shift2 = 8; //alignment variables
	if( id ) { //specific for a particular id
		assembly_t * assem = lookup_assembly( invp->assembly_list, id );
		if( !assem ) {
			fprintf( stderr, "!!! %s: assembly ID is not in the " 
					"inventory\n", id );
			return;
		}
	      	printf( "Assembly ID: %*s\nbin capacity: %*d\non hand: %*d",3, 
				assem->id, 2, assem->capacity, 6, 
				assem->on_hand );

		if ( !( assem->items->item_list) ) //no items
			return;
		
		item_t ** itm = to_item_array( assem->items->item_count,
				assem->items->item_list );
		qsort( itm, assem->items->item_count, sizeof( item_t * ),
				item_compare );
		printf( "\nParts list:\n-----------\nPart ID%13s\n"
				"=========== ========", "quantity" );
		for( int i = 0; i < assem->items->item_count; i++ ) {
			shift1 = 20 - strlen( itm[i]->id );
			printf( "\n%s%*d", itm[i]->id, shift1, 
					itm[i]->quantity );
		}
		printf( "\n" ); //formatting
		return;
	}

	printf( "Assembly inventory:\n-------------------\n" );
	if( !( invp->assembly_list ) ) { //no assemblies
		printf( "EMPTY INVENTORY\n" );
		return;
	}
	printf( "Assembly ID Capacity On Hand\n=========== ======== =======" );
	assembly_t ** assem = to_assembly_array( invp->assembly_count,
                        invp->assembly_list );
        qsort( assem, invp->assembly_count, sizeof( assembly_t * ),
                        assembly_compare );
	for( int i = 0; i < invp->assembly_count; i++ ) {
		shift1 = 20 - strlen( assem[i]->id );
		printf( "\n%s%*d%*d%s ", assem[i]->id, shift1 , 
				assem[i]->capacity,
				shift2, assem[i]->on_hand, 
				( assem[i]->on_hand && assem[i]->on_hand * 2 >
				  assem[i]->capacity ? "" : "*" ) );
	}
	free( assem ); //free resources
	printf("\n"); //formatting

}

//Used for printing the parts 
void print_parts( inventory_t * invp ) {
	printf( "Part inventory:\n---------------" );
	if( !( invp->part_list ) ) { //empty inventory
		printf( "\nNO PARTS\n" );
		return;
	}
	printf( "\nPart ID\n===========" );
	part_t ** parts = to_part_array( invp->part_count, invp->part_list );
	qsort( parts, invp->part_count, sizeof( part_t *), part_compare );
	for( int i = 0; i < invp->part_count; i++ )
		printf( "\n%s", parts[i]->id );
	free( parts ); //free resources
	printf("\n"); //formatting 
}

//Used for printing the items needed in requests
void print_items_needed( items_needed_t * items ) {
	int shift1; //alignment variable
	printf( "Parts needed:\n-------------\n" );
	if( !(items->item_list) ) //no items
		return;
	printf( "Part ID%13s\n=========== ========", "quantity" );
	item_t ** itm = to_item_array( items->item_count, items->item_list );
	qsort( itm, items->item_count, sizeof( item_t * ), item_compare );
	for( int i = 0; i < items->item_count; i++ ) {
		shift1 = 20 - strlen( itm[i]->id ); //num of '=' minus len
		printf("\n%s%*d", itm[i]->id, shift1, itm[i]->quantity );
	}
	free( itm ); //free resource
	printf( "\n" ); //formatting print
}

// This function free's the allocated memory for the needed items list
//
// @param itm the items needed list to be free'd
//
static void free_items( items_needed_t * itm ) {
        //basically the code from free_inventory above
        items_needed_t * ip = itm;
        item_t * temp;
        item_t * copy = ip->item_list;
        while( copy ) {
                temp = copy;
                copy = copy->next;
                free( temp );
        }
        free( ip );
}

//Used to free allocated resources in inventory
void free_inventory( inventory_t * invp ) {
	//goes through the parts and frees them
        part_t * pp;
        for( int i = 0; i < invp->part_count; i++ ){
                pp = invp->part_list;
                invp->part_list = invp->part_list->next;
                free( pp );
        }
        free( invp->part_list );

        //goes through the assemblies and frees them
        assembly_t * ap;
        for( int i = 0; i < invp->assembly_count; i++ ){
                ap = invp->assembly_list;
                invp->assembly_list = invp->assembly_list->next;

		free_items( ap->items );
                free( ap );
        }
        free( invp->assembly_list );
}

// This function processes the add part request.
// It basically just error checks and then calls add part.
//
// @param invp the inventory_t needed to call add part and error check
//
static void add_part_request( inventory_t * invp ) {
	char * id = strtok( NULL, " \n\t" );
	//error checks
	if( id[0] != 'P' ) { 
		fprintf( stderr, "!!! %s: part ID must start with 'P'\n", id );
		return;
	} 
	else if( strlen( id ) > ID_MAX ) { 
		fprintf( stderr, "!!! %s: part ID too long", id );
		return;
	} 
	else if( lookup_part( invp->part_list, id ) ) {
		fprintf( stderr, "!!! %s: duplicate part ID", id );
		return;
	}
	add_part( invp, id );
}

// This function processes the add assembly request. It goes through the variou
// error checks and then adds assemblies to items
//
// @param invp the inventory_t needed to access the different lists
//
static void add_assembly_request( inventory_t * invp ) {
	char * id = strtok( NULL, " \n\t" );
	//error checks
	if( id[0] != 'A' ) {
		fprintf( stderr, "\n!!! %s: assembly ID must start with 'A'",
				id );
		return;
	} 
	else if( strlen( id ) > ID_MAX ) {
		fprintf( stderr, "\n!!! %s: assembly ID too long", id );
		return;
	} 
	else if( lookup_assembly( invp->assembly_list, id ) ) {
		fprintf( stderr, "\n!!! %s: duplicate assembly ID\n", id );
		return;
	}

	char item_id[ID_MAX+1];
	strcpy( item_id, id );
	id = strtok( NULL, " \n\t" );
	int capacity = 0;
	capacity = strtol( id, NULL, 10 );
	if( capacity < 0 ) {
		fprintf( stderr, "\n!!! %d: illegal capacity for ID%s\n", 
				capacity, item_id );
		return;
	}

	// get items
	items_needed_t * items = calloc( 1, sizeof( items_needed_t ) );
	char item[ID_MAX+1];
	int quantity;
	while( ( id = strtok( NULL, " \n\t" ) ) ) { 
		if( !lookup_assembly( invp->assembly_list, id ) && 
				!lookup_part( invp->part_list, id ) ) {
			fprintf( stderr, "\n!!! %s: part/assembly ID is not"
					" in the inventory", id );
			free( items );
			return;
		}
		strcpy( item, id );
		id = strtok( NULL, " \n\t" );
		quantity = strtol( id, NULL, 10 );
		if( quantity <= 0 ) {
			fprintf( stderr, "\n!!! %d: illegal quantity for ID %s"
					, quantity, item );
			free_items( items );
			return;
		}
		add_item( items, item, quantity );
	}
	add_assembly( invp, item_id, capacity, items );
}


// This function performs the fulfill order command. 
// It continues while there is an id and adds that item to the list.
// It then calls get on each of those items and prints the desired output
//
// @param invp the inventory_t needed to get the required lists for calls
//
static void fulfill_order_request( inventory_t * invp) {
	char * id;
	items_needed_t * items = calloc( 1, sizeof( items_needed_t ) );
	char item_id[ID_MAX+1];
	int quantity, flag = 0; 

	while( ( id = strtok( NULL, " \n\t" ) ) ) {
		if( !lookup_assembly( invp->assembly_list, id ) ) {
			fprintf( stderr, "\n!!! %s: assembly ID is not in the "
					"inventory -- order canceled", id );
			free_items( items );	
			return;
		}
		strcpy( item_id, id );
		id = strtok( NULL, " \n\t" );
		quantity = strtol( id, NULL, 10 );
		if( quantity <= 0 ) { //error check
			fprintf( stderr, "\n!!! %d: illegal order quantity for"
				       " ID %s -- order canceled", quantity,
				       item_id );
			free_items( items ); //free resource
			return;
		}
		add_item( items, item_id, quantity );
		flag++; //assembly flag
	}
	if( !flag ) { //no assemblies
		free_items( items );
		return;
	}
	
	items_needed_t * itm = calloc( 1, sizeof( items_needed_t ) );
	for( item_t * i = items->item_list; i; i = i->next )
		get( invp, i->id, i->quantity, itm );
	//prints and free's everything
	print_items_needed( itm );
	free_items( itm ); 
	free_items( items );
}

// This function processes the stock request. It checks conditions and 
// calls make if those conditions were met. It also modifies on_hand
//
// @param invp the inventory_t needed to call make and check assembly
//
static void stock_request( inventory_t * invp ) {
	char * id = strtok( NULL, " \n\t" );
	int quantity;
	assembly_t * assem = lookup_assembly( invp->assembly_list, id );
	if( !assem ) {
		fprintf( stderr, "\n!!! %s: assembly ID is not in the inventory"
				, id );
		return;
	}
	id = strtok( NULL, " \n\t" );
	quantity = strtol( id, NULL, 10 );
	if( quantity <= 0 ) {
		fprintf( stderr, "\n!!! %d: illegal quantity for ID %s",
				quantity, assem->id );
		return;
	}
	items_needed_t * itm = calloc( 1, sizeof( items_needed_t ) );
	if( assem->capacity == assem->on_hand ) {
		free_items( itm ); //free's before return
		return;
	}
	else if( assem->on_hand + quantity > assem->capacity ) {
		make( invp, assem->id, assem->capacity - assem->on_hand, itm );
		assem->on_hand += assem->capacity - assem->on_hand;
	} 
	else {
		make( invp, assem->id, quantity, itm );
		assem->on_hand += quantity;
	}
        free_items( itm ); //free's the items
}

// This function is a helper to restock_request function
// It uses an inventory_t and an assembly_t to check on hand and call make
//
// @param invp the inventory_t with the neeed lists for make
// @param ap the assembly_t with the on_hand and capacity values
//
static void restock_help( inventory_t * invp, assembly_t * ap, 
		items_needed_t * itm ) {
	if( ap->on_hand == 0 || ( ( ap->on_hand * 2 ) <  ap->capacity ) ) {
		int num = ap->capacity - ap->on_hand;
		printf( ">>> restocking assembly %s with %d items\n", ap->id, 
				num );
		make( invp, ap->id, num, itm ); //restocks
		ap->on_hand = ap->capacity; 
	}
}

// This function processes the restock request
// It uses the inventory_t passed in and performs error checking
// If no errors are found, it calls a helper than calls make on condition
//
// @param invp the inventory_t with the needed lists
//
static void restock_request( inventory_t * invp ) {
	char * id = strtok( NULL, " \n\t" );
	assembly_t * ap;
	items_needed_t * itm = calloc( 1, sizeof( items_needed_t ) );
	if( id ) { //Specific for one inventory request
		ap = lookup_assembly( invp->assembly_list, id );
		if( !ap ) {
			fprintf( stderr, "!!! %s: assembly ID is not in the "
					"inventory", id );
			free_items( itm );
			return;
		}
		restock_help( invp, ap, itm ); //helper function
	}
	else {
		ap = invp->assembly_list;
		for( assembly_t * copy = ap; copy; copy = copy->next )		
			restock_help( invp, copy, itm ); //helper function
	}
	//if there's an item then we print
	if( itm->item_count )
		print_items_needed( itm );
	free_items( itm ); //free's resources
}

// This function resets the number of on hand items available
//
// @param invp the inventory_t containing the list needed to reset on hand 
//
static void empty( inventory_t * invp ) {
        char * id = strtok( NULL, " \n\t" );
	if( id[0] != 'A' ) { //error check
		fprintf( stderr, "\n!!! %s: ID not an assembly", id );
		return;
	}
        assembly_t * ap = lookup_assembly( invp->assembly_list, id );
	if( ap )
                ap->on_hand = 0; //emptying the current assembly available
        else
                fprintf( stderr, "\n!!! %s: assembly ID is not in the inventory"
                                , id );
}

// This function prints out all the possible requests that the user can enter
//
static void help(){
	printf("Requests:\n    addPart"
			"\n    addAssembly ID capacity [x1 n1 [x2 n2 ...]]"
			"\n    fufillOrder [x1 n1 [x2 n2 ...]]"
			"\n    stock ID n\n    restock [ID]\n    empty ID"
			"\n    inventory [ID]\n    parts\n    help\n    clear"
			"\n    quit\n");
}

// This function clesrs the exisitng inventory_t that's passed in
// It also properly allocated space for another inventory_t
//
// @param invp the inventory_t that needs to be cleared
//
static void clear( inventory_t * invp ) {
	free_inventory( invp ); //clear exisiting memory
	invp->part_count = 0, invp->assembly_count = 0;
	invp->part_list = NULL, invp->assembly_list = NULL;
}

// This function quits the program and then free's all allocated memory
//
// @param invp the inventory_t with all allocated memory
//
static void quit( inventory_t * invp ) {
        free_inventory( invp );
	free( invp ); //free's the inventory
}

// This struct is used the process the commands passed in via the stream
// It uses the keywords and then calls the appropriate function
//
static struct req table[] = {
          { "addPart",          add_part_request },
	  { "addAssembly",      add_assembly_request },
	  { "fulfillOrder",     fulfill_order_request },
	  { "stock",            stock_request },
	  { "restock",          restock_request },
	  { "empty",            empty },
	  { "inventory",        print_inventory },
	  { "parts",            print_parts },
	  { "help",             help },
	  { "clear",            clear }
//	  { "quit",             quit }
};

// This functions uses the request struct and processes all the commands
// It strips the commands of certain characters and processes it through table
// This function and req table idea used from Steele material
//
// @param fp the file stream being used to read input (stdin or a file)
// @param invp the initial inventory_t struct with all the needed list
//
static void process( FILE * fp, inventory_t * invp ) {
	size_t count = 0, size = 0;
	char * tok;
	char * buf = NULL;
	int flag; //Checks command validity

	while( ( count = getline( &buf, &size, fp ) ) > 0 ) {
		if( feof( fp ) ) { //file error checks
			fclose( fp );
			quit( invp );
			free( buf );
			return;
		}
		flag = 0; //used for unknown command
		buf = trim( buf );
		int len = strlen( buf );
		for( int i = 0; i < len; i++ ) {
			if( buf[i] == '#' ) {
				buf[i] = '\0';
				break;
			}
		}
		if( strcmp( buf, "\0" ) == 0 )
			continue;
		printf( "+ %s\n", buf );
		tok = strtok( buf, " \n\t" );
		if( !strcmp( tok, "quit" ) ) {
			fclose( fp );
			quit( invp );
			free( buf );
			return;
		}
		for( size_t i = 0; i < sizeof( table ) / sizeof( struct req );
				i++ ) {
			if( !strcmp( tok, table[i].req_string ) ) {
				table[i].req_fn( invp ); //Calls functions
				flag++;
			}
		}
		if( !flag && ( strlen( tok ) != 0 ) )
			fprintf( stderr, "\n!!! %s: unknown command\n", tok );
	}
	free( buf ); //free resource
	if( fp != stdin )
		fclose( fp ); //close file
}


// Main function that uses it's parameters to determine file stream and
// call appropriate methods using a process request helper
//
// @param argc the number of arguments from command line
// @param argv the specific arguments on command line
// @return EXIT_STATUS (either SUCCESS or FAILURE)
//
int main( int argc, char * argv[] ) {
	FILE * fp;
	inventory_t * invp = calloc( 1, sizeof( inventory_t ) ); //initialize
	if( argc == 1 ) 
		fp = stdin; 
	else if ( argc == 2 ) {
		fp = fopen( argv[1], "r" );
		if( !fp ) {
			fprintf( stderr, "usage: " );
			return EXIT_FAILURE;
		}
	}
	else {
		fprintf( stderr, "usage: " );
		return EXIT_FAILURE;
	}
	process( fp, invp ); //process everything
	return 0;
}
