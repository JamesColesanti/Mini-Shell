#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Maximum number of characters user can input */
#define BUFFER_SIZE 80

// Implement a lexer parser in this file that splits text into individual tokens.
// You may reuse any functions you write for your main shell.
// The point is that you get something small working first!
//
//


// Implement your function parse here
// Parse should accept 1 string as an argument.
// Think carefully about the return type
// --what would be a useful type to store a
// --collection of tokens?

/*
 * The vect_t struct from Assignment 2 is used to help with storing data about the
 * given commands. The main reason for using this kind of struct is to help with
 * looping through the commands, as the size field serves as an indication of
 * where the data array ends.
 */
typedef struct string_vect {
         //string array storing user input
	 char **data;

         //size field indicating number of strings currently in array
    	 unsigned int size;
} vect_t;

/*
 * Creates a new vector. The data array will be large enough to hold BUFFER_SIZE 
 * number of characters.
 */
vect_t *vect_new() {
	vect_t *v = (vect_t *) malloc(sizeof(vect_t));
	v->data = (char **) calloc(80, sizeof(char *)); 
	v->size = 0;
	return v;
} 

/*
 * Frees all data allocated to vector.
 */
void delete_vect(vect_t *v) {
        free(v);
        free(v->data);
}

/*
 * Lexes a single line of input and prints out the individual token, one token
 * per line. Stores token in vect_t struct.
 */
vect_t *parse(char* str) {
          
   	char **token_list;
    	char *char_list = strtok(str, " ");
    	int index = 0;
   	vect_t *v = vect_new();
	
	// char_list will be null when there are no more token to process
    	while (char_list != NULL) {
      		v->data[index] = char_list;
        	index++;
        	v->size++;     
		char_list = strtok(NULL, " ");
    	}

	// print strings in vector
    	index = 0;
    	while(v->size > index) {
        	printf("%s\n", v->data[index]);
         	index++;
    	}
    
    	return v;
}

/*
 * Main method for program. Calls parse and frees vector used to store
 * tokens while printing.
 */
int main(int argc, char** argv){
 
    	vect_t *tokens = parse(argv[1]);
	delete_vect(tokens);
   	
	return 0;
}
