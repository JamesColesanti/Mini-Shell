# READ ME FIRST

**I strongly recommend** you perform your testing on your own linux VM  or the vagrant image we distributed. Executing a shell that forks indefinitely (i.e. a [fork bomb](https://en.wikipedia.org/wiki/Fork_bomb)) **slows down the servers for everyone** (and may prevent you from logging in). **We will not grant individual extensions because you blocked yourself and your classmates out from a shared resource, or Khoury Systems suspends your account.**

# Assignment Strategy 

You should be able to implement **Part 1.1** and **Part 1.2** of this assignment right away after a little web searching on the [string.h](http://www.cplusplus.com/reference/cstring/) C library. **I strongly suggest that you complete Parts 1.1 and 1.2 as soon as possible,** to give you more time to reflect on your design to complete Part 1.3.

Part 1.3 will require some knowledge of commands like `fork()` and `exec()`. Make sure you understand them before beginning (see Lab 6).

# Introduction

For this assignment, you will be writing your very own shell -- the mini-shell!

<img src="media/shell.png" alt="An example shell"/>

[Shells](https://en.wikipedia.org/wiki/Shell_(computing)) (or sometimes called command-line interfaces) are text based applications that serve as an intermediate between the user and the operating system. The shell is also a tool for how a user interacts with the operating system. The shell that you build with this assignment will be something you can continually expand on in the future, and perhaps distribute as your own one day! Let us take a closer look at the parts to get started.

# Part 1.1 - Lexer/Parser

For Part 1.1 of this assignment, you will perform string processing to build a simple lexer.  The lexer we are building now would be similar to the first part of building a compiler or interpeter for a programming language. We will build on our lexer to create a shell, which can be thought of as an interactive interpreter of a command-line language performing the following:

1. It takes in a line of code (**R**ead)
2. Breaks the line into tokens and parses the individual lexemes (**E**valuate)
3. Then executes the command and any arguments (**P**rint output)
4. Then repeats (**L**oop)

We call this a [REPL interpreter](https://en.wikipedia.org/wiki/Read–eval–print_loop)

For the first part of this assignment, we are going to break a string into individual tokens (or [lexemes](https://en.wikipedia.org/wiki/Lexical_analysis#Lexeme)).

- The first token will be the command
- The second through *N* number of tokens (i.e. lexemes) will be any number of arguments.
- (Optionally) The N+1 tokens through M tokens will be another command with arguments.

For those who like a more formal definition, a grammar describing how to break each string into individual lexemes looks like the following: 

```<command> <arg>* ( [;|] <commmand> <arg>* )*```

What this notation means:

- `<command>` is what you type in (e.g. *ls*)
- `<arg>*` are any number of arguments (e.g. *-l*)
- multiple commands can be combined using `;` or `|`
   - `;` sequences commands one after the other, that is, the left-hand side command is executed first to completion - once it's finished, the right-hand side command is executed
   - `|` between two commands runs both commands simultaneously, while redirecting the standard output of the left-hand side command to the standard input of the right-hand side command
- the `*` (star or asterisk) means that any number of arguments (zero to as infinite) could follow (See [Kleene star](https://en.wikipedia.org/wiki/Kleene_star)).
- the parentheses `(` `)` are just used to group the elements notationally and are not part of our shell's syntax

**For Part 1.1 of this assignment, write a program [parse.c](./parse.c) that contains a single function (there must be a function, do not implement everything in main) to lex a single line of input and and print out the individual tokens, one token per line.** You are allowed re-use as much of [parse.c](./parse.c) as you like for implementing your mini-shell in part 1.3.

### Example output for Part 1.1

Your [parse.c](./parse.c) program is executed with a single string as an argument and the output is the folowing: 

1. *./parse "cd ."*
  - ```
    cd
    .
    ```
2. *./parse "ls -l"*
  - ```
    ls
    -l
    ```
3. *./parse "cat parse.c | head -n2"*
  - ```
    cat
    parse.c
    |
    head
    -n2
    ```
  
- **Note:** Your program will read in from the command-line the arguments to parse (e.g. *./parse "cat parse.c | head 2"* 
	- **Hint:** Wrapping a part of the command line in quotes ensures it is treated as a single argument -- even if it contains spaces.
  
### Additional Hints for Part 1.1  
  
* The function [strtok](http://www.cplusplus.com/reference/cstring/strtok/) may be useful for splitting up a line of input.
  * `strtok` works by splitting up an input based on a delimeter that indicates a new token is starting(in our example a space). 
  * `strtok` is part of the [string.h](http://www.cplusplus.com/reference/cstring/) library, so be sure to include it. 
* You may use other strategies outside of strtok however--there are many ways to lex and parse your input.
* My expectation is Part 1.1 of this assignment should be relatively short and you may reuse the functions for implementing the mini-shell!
  
# Part 1.2 - Signal Handler (use this for Part 1.3)

In your shell, you will have to implement a signal handler in order to give the user some control of processes that are runninng. We have very briefly discussed signal handlers, so make sure you are familiar with the concept. 

### What is a signal handler?

As a refresher, a [signal handler](http://www.gnu.org/software/libc/manual/html_node/Signal-Handling.html) is a software interupt that is sent to a process. Provided below is an example of sending a software interrupt to a process running a shell when you press `Ctrl+C`. Signal handlers are handy for communicating with the operating system how to handle a process that may be misbehaving, or perhaps to [handle some other event](http://www.gnu.org/software/libc/manual/html_node/Kinds-of-Signals.html#Kinds-of-Signals).

```c
#include <stdio.h>
#include <stdlib.h>
#include <signal.h> // This is new!


// Create a signal handler
void sigint_handler(int sig){
	// Ask yourself why is 35 the last parameter here?
	write(1,"Terminating through signal handler\n",35); 
	exit(0);
}

int main(){

	// Install our signal handler
	signal(SIGINT, sigint_handler);

	printf("You can only terminate by pressing Ctrl+C\n");

	while(1){
		printf("Running forever!\n");
		sleep(1);
	}

	return 0;
}
```

* The following page provides details about signals: https://ftp.gnu.org/old-gnu/Manuals/glibc-2.2.3/html_chapter/libc_24.html
* The following article provides a nice introduction as well: https://www.thegeekstuff.com/2012/03/catch-signals-sample-c-code/

# Part 1.3 - Mini-Shell

<img src="./media/shell.gif" alt="An example of your minishell" width=400px/>

You are now ready to create Mini-Shell in [shell.c](./shell.c). Implementing a shell will teach you the fundamentals of how an operating system executes and controls new processes. The shell after all is our window into the operating sytsem. The Mini-Shell which you will be implementing is inspired by other popular shells! 

At a high level, implementing mini-shell takes the following components:
1. You will have a function that reads in commands a user types.
2. Your program will execute in an infinite loop.
3. Your program will execute child processes from the shell (the shell is the parent process) based on the command.

## The specifications for your shell

1. **You must have a prompt `mini-shell>` in front of each command that is typed out.**

2. **The maximum input size (i.e. BUFFER_SIZE) on a given line shall be at least 80 characters for our shell. You can specify this as a constant global variable.**
3. **Implement a signal handler so when the user presses 'Ctrl+C' the text 'mini-shell terminated' is printed out.**
	- Make sure that **all** child processes are terminated when this occurs
4. **When you launch a new child process from your shell, the child process should run in the foreground by default until it is completed.**
	- Here are some examples: https://www.tutorialspoint.com/unix/unix-processes.htm
5. **If the user adds the `;` symbol, that signifies the end of a command.  Other commands may follow.**
	- There can be an arbitrary number of `;` symbols within BUFFER_SIZE.
	- If a following command exists, it is executed after the preceding command completes.
7. **If the user adds the pipe `|` then the output of one process should be the input to the next process.**
	- There can be an arbitrary number of `|` symbols within BUFFER_SIZE.
	- As a hint, it can be helpful to have a separate function or use case that handles parsing a series of commands if a pipe is found when parsing the individual tokens.
	- Here are some examples: https://www.tutorialspoint.com/unix/unix-pipes-filters.htm
	- You may **NOT** use the [system](http://man7.org/linux/man-pages/man3/system.3.html) command (named `system()`) which makes handling pipes trivial. You must investigate [dup](https://linux.die.net/man/2/dup) and file descriptors. Remember to look at Example 6 from the fork/exec lab and Lab 7. 
8. **Your shell will have some built-in functions that you will have to implement (i.e. functions written in C code). These are:**
   1. **`exit` to terminate your shell,** 
	    - Note: If you launch another shell from within your shell, exit should only close out of the most recently launched shell.

   2. **`cd` so you may change directories** 
		  - You can test `cd` by moving up or down the directory tree.
		  - (Hint the C library has commands for this)

   3.  **`source` which takes a filename as input and processes each line of the file as a command.**

   4.  **`prev` which will print the previous command line and execute it again.**
	
   5. **`help` explaining all the built-in commands available in your shell.**
7. **You will want to think about how to store these built-in commands.**
   - I will hint that using an array with commands and an array of function pointers is one possibly effective solution.
   - What is a function pointer? https://www.geeksforgeeks.org/function-pointer-in-c/
   - You can further think about what an array of function pointers looks like and why this may be appropriate.
8. **If a command is not found, your shell should print out an error message, and resume execution.**
  
   For example:

   ```
   mini-shell> ano;wavu;
   Command not found
   mini-shell>
   ```

3. **System commands should not need a full path specification to run in the shell.**
   - For example, both `ls` and `/bin/ls` should work.

## A suggested development path for Part 1.3

1. Design `main()` of `shell.c` to include your `parser` function and your signal handler.
2. Test signal handler works.
3. Test `parser` on a string read from STDIN.
4. Implement `fork` and a flavor of `exec` to run a program supplied on STDIN and test. 
5. Implement support for `;` and test.
6. Implement support for `|` and test.
7. Implement each of the built-in functions and test.
8. Test your shell can run ./shell and exiting shell only exits the sub-shell.

## Some helpful things to ponder

1. You are running programs in your shell. The programs that are being run are found in the `bin` directory. For example, you would find a program called `cat` or `ls` there, which someone wrote in C for you.
2. There exists more than 1 [exec](https://linux.die.net/man/3/exec) command -- some are helpful for finding things on your [environment path](https://en.wikipedia.org/wiki/PATH_(variable)). Hmm!
3. You can also execute programs from a relative or absolute filepath (e.g. `./minishell`)
4. There is not a program called 'cd' in the 'bin' directory, which means it must be built into the actual shell.
	- Think about whether this would make sense to execute in the parent or child process and why.

Provided below is an example for program that forks a new process, executes it, and then resumes execution.

```c
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {

  char* myargv[16];
  myargv[0] = "/bin/ls"; // Try changing this to "ls" Will the program work?
                         // Note: There are many versions of exec that will look in
                         // environment variable paths. See execvpe for example.
  myargv[1] = "-F";
  myargv[2] = NULL;      // Terminate the argument list
  
  if (fork() == 0) {
    // Executes command from child then terminates our process
    execve(myargv[0], myargv, NULL);
    printf("Child: Should never get here\n");
    exit(1);
  } 
  else {
    wait(NULL); // This call makes the parent wait on its children.
  }

  printf("Finally, print from the parent\n");
  
  return 0;
}
```

### How do I test mini-shell? 

You can validate the behavior of your shell in a  Unix terminal to confirm it is working correcly. Some examples of tests you could use are:

```bash
$ ./shell
mini-shell> cat parse.c
.
.
.
mini-shell> nl parse.c
.
.
.
mini-shell> ls -l
.
.
.
mini-shell> cat parse.c | head 2
.
.
.
mini-shell> cat Makefile ; ps ef
.
.
.
mini-shell> ./shell # Running a mini-shell within a mini-shell and then executing commands
mini-shell> cd ..
mini-shell> ls
.
.
.
mini-shell> CRTL+C
mini-shell> pwd
.
.
.
```
# Deliverables
1. [parse.c](./parse.c)
1. [shell.c](./shell.c)

# Rubric

<table>
  <tbody>
    <tr>
      <th>Points</th>
      <th align="center">Description</th>
    </tr>
    <tr>
      <td>10% (Part 1.1 - Lexer)</td>
      <td align="left"><ul><li>Completed Lexer with a single function to parse</li></ul></td>
    </tr>
    <tr>
      <td>85% (Part 1.3 - Mini-Shell)</td>
      <td align="left"><ul><li>25% 5 built-in commands.</li><li>5% A working signal handler.</li><li>15% A shell that runs forever and has the 'mini-shell>' prompt and properly parses command lines.</li><li>40% Can successfully execute processes in the foreground and returns back to the prompt after the processes finish.
      <ul>
        <li>15% working pipe <code>|</code></li>
        <li>15% correctly execute arbitrarily long sequences of programs using <code>;</code></li>
      </ul></li></ul></td>
    </tr>    
    <tr>
      <td>5% Style</td>
      <td align="left"><ul><li>We don't require adherence to a particular style guide for this assignment, but we care about consistency, readability and good documentation. Follow the design recipe from Fundies I/II as a minimum. You can also use <a href="https://cs.brown.edu/courses/cs033/docs/guides/style.pdf">this style guide</a>. You might also want to look into <a href="https://clang.llvm.org/docs/ClangFormat.html">clang-format</a>.</li></ul></td>
    </tr>
  </tbody>
</table> 



# Resources to help

- (See links above)
- (See the SupportCode) folder
- And otherwise think about commands like fork, execvp, wait, waitpid, getpid, etc. :)
- [man 2 pipe](http://man7.org/linux/man-pages/man2/pipe.2.html)
- [dup](http://man7.org/linux/man-pages/man2/dup.2.html) **See example6.c in Lab 06.**

### Glossary and Tips
1. Commit your code changes to the repository often.  This is a good habit to practice.
2. **Do not commit your .o file or your executable file to the repository.** This is considered bad practice!
3. On Functionality/Style
	1. You should have comments in your code to explain what is going on, and use good style (80 characters per line maximum for example, functions that are less than 50 lines, well named variables, well named functions etc.).
4. You may use any stdio.h functions that are useful, such as getline. You are welcome to implement these functions from scratch if you like however.
	1. You will want to include 'signal.h' as well for your signal handlers.
5. Be careful when parsing. Things like newline characters '\n' like to sneak in and may make it difficult for `strcmp` to provide accurate results. `"teststring" != "teststring\n"`

# Going Further

You might consider some of the following optional features in your shell.
1. Switching processes between foreground and background (`fg` and `bg` commands).
3. Grouping command expressions. E.g.:

   ```
   ( cat prologue.txt ; ( cat names.txt | sort ) ; cat epilogue.txt ) | nl
   ```

# Feedback Loop

(An optional task that will reinforce your learning throughout the semester)

Implementing a shell is one of the more fun exercises in learning C programming (in my opinion). However, it is quite relevant, especially in the context of the web browser and other applications (video games for example, often have a real-time shell interface for debugging).

Choose one of the following 3 papers to scan.  (Note this is not graded and completely optional).

- Grocoli: https://dl.acm.org/citation.cfm?id=2442019
- Integrating Command Shell in Web Browser: https://www.usenix.org/techsessionssummary/integrating-command-shell-web-browser
- Inky: http://people.csail.mit.edu/emax/papers/uist08-inky/uist08-inky.pdf

# F.A.Q.

- Q: Do I have to implement every shell command?
- A: No--that would be completely unreasonable! Think about how the 'exec' family of commands work and PATH. You are only implementing a few built-in commands that will execute. If a command is not a built-in command, then you attempt to execute linux programs found on PATH.
