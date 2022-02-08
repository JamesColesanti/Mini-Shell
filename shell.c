#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <assert.h>

/* Maximum number of characters user can input in a single line */
#define BUFFER_SIZE 80

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
 * Creates a new vector. The data array will be large enough to hold BUFFER_SIZE number
 * of characters.
 */
vect_t *vect_new() {
	vect_t *v = (vect_t *) malloc(sizeof(vect_t));
	v->data = (char **) calloc(BUFFER_SIZE, sizeof(char *)); 
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
 * Frees all data allocated to array of array of string.
 */
void delete_array_of_array(char*** array) {
	int char_ind = 0;
	int string_ind = 0;
	while (array[string_ind] != NULL) {
		while (array[char_ind] != NULL) {
			free(array[string_ind][char_ind]);
			char_ind++;
		}
		string_ind++;
	}
	free(array);
}

/*
 * Parses the given character array to create a vect_t struct. This struct will
 * be used to execute the user's commands.
 */
vect_t *parser(char* str) {

	char **token_list;
	char *char_list = strtok(str, " ");
	int index = 0;
	vect_t *commands = vect_new();

	while (char_list != NULL) {
		commands->data[index] = char_list;
        	index++;
        	commands->size++;     
        	char_list = strtok(NULL, " ");
		// remove new line character from last string
		if (char_list == NULL) {
	        	char* fixLast = (char *) malloc(BUFFER_SIZE * sizeof(char));
                	fixLast = strncpy(fixLast, commands->data[index - 1], strlen(commands->data[index - 1]) - 1);
			strcpy(commands->data[index - 1], fixLast);
			free(fixLast);
		}
	}
	return commands;
}

/*
 * Signal handler used to write termination message and exit
 */
void sigint_handler(int sig) {
	write(1, "mini-shell terminated\n", 23);
	exit(0);
}

/*
 * Determines if multiple commands are present in a single line. Multiple commands
 * are present if the line contains at least one ";" character. Returns 1 if this
 * character is found and 0 otherwise.
 */

/*
 * Determines if user input contains given string. Returns 1 if true and 0 otherwise.
 */
int contains_string(vect_t *commands, char* str) {
        for (int i = 0; i < commands->size; i++) {
                if (strcmp(commands->data[i], str) == 0) {
                        return 1;
                }
        }
        return 0;
}

/*
 * Returns number of pipes in user input.
 */
int get_num_pipes(char** data) {
	int count = 0;
	int i = 0;
        while (data[i] != NULL) {
                if (strcmp(data[i], "|") == 0 || strcmp(data[i], "|\n") == 0) {
                        count++;
                }
		i++;
        }
        return count;
}

/*
 * Copies data from given vector starting at given index. Returns new vector
 * with all data from _start_ to end of given vector.
 */
vect_t *copy_data(int start, vect_t *v) {
        vect_t *output = vect_new();
        int count = start;
        for (int i = 0; i < (v->size - start); i++) {
                output->data[i] = (char *) malloc(BUFFER_SIZE * sizeof(char));
		strcpy(output->data[i], v->data[count]);
                output->size++;
                count++;
        }
        return output;
}

/*
 * Find a portion of the given data array starting and ending at the given indices.
 */
char** find_portion(char** data, int start, int end) {
        char** output = (char **) calloc(BUFFER_SIZE, sizeof(char *));
        int ind = 0;
        for (int i = start; i < end; i++) {
		output[i] = (char *) malloc(BUFFER_SIZE * sizeof(char));
                strcpy(output[ind], data[i]);
                ind++;
        }
        return output;
} 

/*
 * Update the previous command.
 */
void set_prev(vect_t *new_command, vect_t *prev_command) {
	prev_command->size = new_command->size;
     	for (int i = 0; i < new_command->size; i++) {
		prev_command->data[i] = (char *) malloc(BUFFER_SIZE * sizeof(char));
               	strcpy(prev_command->data[i], new_command->data[i]);
        }
}

/*
 * Forward declaration of execute_all. Explained in more detail below.
 */
void execute_all(vect_t *v);

/*
 * Forward declaration of execute_single. Explained in more detail below.
 */
void execute_single(char** data);

/*
 * Create an array of an array of strings for all commands in the given string array.
 * Ignore any pipe characters.
 */
char*** get_commands_no_pipes(char** data) {
        char*** output = (char ***) calloc(BUFFER_SIZE, sizeof(char **));
	char* str;
        int cmd_ind = 0;
        int data_ind = 0;
	int output_ind = 0;
	char** cmd = (char **) malloc(80 * sizeof(char *));
        while (data[data_ind] != NULL) {
		// while there are no pipes, continue adding to same string
                if (strcmp(data[data_ind], "|") != 0) {
        	        cmd[cmd_ind] = (char *) malloc(BUFFER_SIZE * sizeof(char));
	                strcpy(cmd[cmd_ind], data[data_ind]);
			cmd_ind++;
                } 
		// once a pipe is found, add array of string to output and start adding to
		// next array of strings.
		else {
			output[output_ind] = (char **) malloc(BUFFER_SIZE * sizeof(char *));

			for (int i = 0; i < cmd_ind; i++) {
				str = (char *) malloc(BUFFER_SIZE * sizeof(char));
				strcpy(str, cmd[i]);
				free(cmd[i]);
	                        output[output_ind][i] = (char *) malloc(BUFFER_SIZE * sizeof(char));
				strcpy(output[output_ind][i], str);
			}
			output_ind++;
			cmd_ind = 0;
		}
                data_ind++;
        }

	// run block one more time to add command after last pipe to output
        output[output_ind] = (char **) malloc(BUFFER_SIZE * sizeof(char *));
        for (int i = 0; i < cmd_ind; i++) {	
	        str = (char *) malloc(BUFFER_SIZE * sizeof(char));
        	strcpy(str, cmd[i]);
        	output[output_ind][i] = (char *) malloc(BUFFER_SIZE * sizeof(char));
        	strcpy(output[output_ind][i], str);
        }
	free(cmd);
        return output;
}

/*
 * Carries out same procedure as above, but instead outputs the number of commands
 * in the array of array of strings.
 */
int get_num_commands_no_pipes(char** data) {
        int data_ind = 0;
        int output_ind = 0;
        while (data[data_ind] != NULL) {
                if (strcmp(data[data_ind], "|") == 0) {
                          output_ind++;
                }
                data_ind++;
        }
	// add 1 more for last commands after pipe
	output_ind++;
	return output_ind;
}

/* Handles cd built-in command. Changes the directory based on user specifications */
void cd_case(char** data) {
        char* dir = (char *) malloc(BUFFER_SIZE * sizeof(char));
	// get name of new directory (could also be ".." indicating that the user wants
	// to go back one step in the directory tree
        dir = strncpy(dir, data[1], strlen(data[1]));
	// change directory
        int newDir = chdir(dir);
	free(dir);
        if (newDir < 0) {
                printf("Error occurred while changing directory.\n");
        }
}

/*
 * Handles source built-in command. Copies all contents of the file specificed by the
 * user and runs each lines as a command.
 */
void source_case(char** data) {

        FILE *source;
        char input[80];
        char* fileName = (char *) malloc(BUFFER_SIZE * sizeof(char));
        // get name of file
	fileName = strncpy(fileName, data[1], strlen(data[1]));
        source = fopen(fileName, "r");
	// while there are more lines, execute each line as a command
        while (fgets(input, sizeof(input), source) != NULL) {
                vect_t *v = parser(input);
                execute_all(v);
        }
	free(fileName);
        fclose(source);
}

/*
 * Handles cases where pipes are found. A pipe is set up between each command
 * allowing the output of one to be the input for the next. This continues while
 * more pipes are found.
 */
void pipe_case(char** data) {
	int pipe_count = get_num_pipes(data);
        int pipe_fds[pipe_count * 2];
        // setup pipes
	// indexing: pipes start at intervals of 2
	// Example: 2 pipes
	// pipe 1 - read end is 0, write end is 1
	// pipe 2 - read end is 2, write end is 3
        for (int i = 0; i < pipe_count; i++) {
		pipe(pipe_fds + i * 2);
        }
        int pipe_index = 0;
	int total_commands = get_num_commands_no_pipes(data); 
        int pid;
        char*** array = get_commands_no_pipes(data);
        for (int cmd_index = 0; cmd_index < total_commands; cmd_index++) {
                pid = fork();
                if (pid == 0) {
                        // if we are not at the last command, replace command's stdout with
			// write end of next pipe
                        if(cmd_index != total_commands - 1) {
				close(1);
				dup(pipe_fds[pipe_index + 1]);
                        }
			// if we are not at first command, replace command's stdin with
			// read end of previous pipe
                        if (cmd_index != 0) {
				close(0);
				dup(pipe_fds[pipe_index - 2]);
                        }
			// close pipes
                        for (int i = 0; i < pipe_count * 2; i++) {
                                close(pipe_fds[i]);
                        }
                        // execute current command
                        execute_single(array[cmd_index]);
                }
                pipe_index = pipe_index + 2;
        }
        // close all pipes
	for (int i = 0; i < pipe_count * 2; i++) {
                close(pipe_fds[i]);
        }
        // wait for all children to finish execution
        for (int i = 0; i <= pipe_count; i++) {
                wait(NULL);
        }
}

/* Function for handling case where command is not found in built-in commands. The only
 * options for this case are that the user wants to start a new shell or execute a 
 * command found in bin.
 */
void else_case(char** data, int more_commands, vect_t *all_commands, int new_start) {
        pid_t child = fork();
        if (child == 0) {
                // execute command in child
                execute_single(data);
        }
        // in parent, follow appropriate wait/signal procedure based on commands
        else {
                if (strcmp(data[0], "./shell") == 0) {
                        // ignore signal while child is running, go back to listening once
                        // it has completed execution
                        signal(SIGINT, SIG_IGN);
                        wait(NULL);
                        signal(SIGINT, sigint_handler); 
			// execute rest of commands if there are more
                        if (more_commands) {
				//copy over data starting at new_start 
	                        vect_t *rest_commands = copy_data(new_start, all_commands);
				delete_vect(all_commands);
                                execute_all(rest_commands);
                        }
                }
                else {
                        wait(NULL);
                        if (more_commands) {
                                vect_t *rest_commands = copy_data(new_start, all_commands);
				delete_vect(all_commands);
                                execute_all(rest_commands);
                        }
                }
         }
}

/*
 * Delegate commands to appropriate function. Specifically, check for pipes/built-in
 * commands that require special attention.
 */
void delegate_command(char** data, int more_commands, vect_t *all_commands, int new_start) {
	// case for when one or more pipes are found
	// In the pipe case, we switch to an array of an array of strings. This helps
	// with the index arithmetic that must be done while setting up pipes between
	// complete sets of commands. It is cumbersome to attempt to do this arithmetic
	// using an array of strings, where it is difficult to tell where one command
	// ends and the next begins.
	if (contains_string(all_commands, "|")) {
		pipe_case(data);
	}
	// exit case
        else if (strcmp(data[0], "exit") == 0) {
	        write(1, "mini-shell terminated\n", 23);
		exit(0);
        }
	// cd case
        else if (strcmp(data[0], "cd") == 0) {
		cd_case(data);
        }
	// source case
	else if (strcmp(data[0], "source") == 0) {
        	source_case(data);
        }
        // in the help case, write help message to stout
        else if (strcmp(data[0], "help") == 0) {
                free(data);
                write(1, "James' mini-shell\nBuilt-in Commands are:\nexit\ncd\nsource\nprev\nhelp\n", 66);
        }
	// else case, command must either be to start a new shell to use a bin commands at this
	// point
	else {
		else_case(data, more_commands, all_commands, new_start);
	}
}

/*
 * Execute a single command. Data holds the command.
 */
void execute_single(char** data) {
		// in the shell case, execute shell
            	if (strcmp(data[0], "./shell") == 0) {
			char* myargv[16];
			myargv[0] = "shell";
			myargv[1] = NULL;
			free(data);
			execvp(myargv[0], myargv);
		}
		// in other cases, attempt to execute command from bin
		// return message if unsuccessful
		else {
                      	int status = execvp(data[0], data);
			if (status == -1) {
				write(1, "Command not found\n", 18);
			}
			exit(0);
                }
}

/*
 * Executes all commands in the given command vector. Delegates based on whether or not
 * multiple commands are present.
 */
void execute_all(vect_t *all_commands) {
        int new_start = 0;
	// when multiple commands are present, must be executed one at a time
	if (contains_string(all_commands, ";")) {
		for (int i = 0; i < all_commands->size; i++) {
			if (strcmp(all_commands->data[i], ";") == 0) {
				// find first command (everything before a ";")
				char** portion;
                                portion = find_portion(all_commands->data, 0, i);
				// update new_start so that program knows to copy over
				// next command next time around
				new_start = i + 1;
				// delegate command to appropriate function
				delegate_command(portion, 1, all_commands, new_start);
				break; 
                        }
                }
	}
	// when just a single command is present, delegate that command to the appropriate
	// function
        else {
                delegate_command(all_commands->data, 0, all_commands, 0);
        }
}

/*
 * Replace all instances of prev in current command with the "value" of prev. The "value"
 * of prev is the seqeuence of commands prev currently refes to.
 */
vect_t *replace_prev(vect_t *all_commands, vect_t *prev) {
	vect_t *output = vect_new();
	int index = 0;
	// loop through all command
	for (int i = 0; i < all_commands->size; i++) {
		// when prev is found, replace it with sequence of commands that were in previous
		// input line 
		if (strcmp(all_commands->data[i], "prev") == 0) {
			// j will be used to loop through commands in prev
			for (int j = 0; j < prev->size; j++) {
                                output->data[index] = (char *) malloc(BUFFER_SIZE * sizeof(char));
				strcpy(output->data[index], prev->data[j]);
				output->size++;
				index++;
			}
		}
		// when prev is not found, simply copy over command to output vector
		else {
			output->data[index] = (char *) malloc(80 * sizeof(char));
                        strcpy(output->data[index], all_commands->data[i]); 
                        output->size++; 
                        index++;  
		}
	}
	return output;
}

/* Main function for program */
int main(int argc, char** argv){
 	signal(SIGINT, sigint_handler);

	vect_t *prev_command = vect_new();
	char input[80];
	while(1) {
		printf("mini-shell>");
		fgets(input, sizeof(input), stdin);
		vect_t *v = parser(input);
		// when prev is not present, update prev_command and execute commands
	        if (contains_string(v, "prev") == 0) {
        	        set_prev(v, prev_command);
			execute_all(v);
        	}
		// when prev is present, run commands, substituting prev for the sequence of commands
		// it refers to
		// update prev
		else {
			vect_t *original_prev = copy_data(0, prev_command);
			vect_t *new_prev = replace_prev(v, prev_command); 
			set_prev(new_prev, prev_command);
			execute_all(new_prev);
		}
	}

	return 0;
}
