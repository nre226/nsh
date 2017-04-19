# nsh
Names:Deavin Hester and Nicholas Reaves

Files: nsh.cc, Program_Control.cc, Program_Control.h, Builtin_Cmds.cc, Builtin_Cmds.h

Program Controls:
scanner_parser:
I first check for comments in input line and ignore them if there is. Then I check for quotes then add the string before the quotes to the temp tokens of vectors,
then the quote to the vector of temp tokens, then check for quotes in rest of input line and continue until all the way through. If no quotes then I just add the
whole line as one element to the temp tokens vector. Then for each element in that element that was not a quote, I send to the split funtion to be split at whitespaces
and added to the real token vector. Then at the very end I check each token to for a '$' symbol to denote a variable name then I replace it with its value that is
stored or return an error saying there is no variable.
split:
I put the entered string in a stringstream and then getline on that delimited by a space, and assign it to a temp variable. Then check if that variable is not an
empty string. If it isn't then I add that word to the end of the vector containing the real tokens.
do_Command:
I first make a char* array that point to c string versions of each token, just argv looks like. Then I check the cmd element for a ':' incase of multiple directories
I need to search for and try each one. If can't exec in any of them, I output an error to stderr. But other than that, I check to see if the cmd element starts with a
".", "./", or a "../" and use exev, otherwise I use execvp in the child. In the parent I just wait until the child is finished.
back_Command:
I do the same thing as in the do_Command function but instead of waiting for the child to finish I immediately return the prompt and add the pid to the list of 
processes. And add the name to a vector in the form of "pid:name" so I can match it to the pid num in the map I made so I can print out the name with the pid in 
the procs funciton.
to_Var:
I again, do the same thing as above with checking to see what to execute but in the child, I first redirect stdout and stderr to "output.o" in /tmp and then in the
parent I read this file into a buffer and add it to the map of variable I have.

Built-in Commands:
prompt_Change:
I check to see if there are the right number of variables. If there is, then I just change the string that I passed by reference to the prompt string that the 
user entered.
procs:
I loop the map I have containing all the processes and check waitpid to see if any have finished, if so, I change there status to completed and print them, if not I
just print them with the in progress status that they already had.
dir:
Check to make sure the correct number of tokens are right, then use chdir() to change directory or if not found, will output to stderr.
setvar:
I check for correct number of arguments and then change the string that is associated in my map to the value that the user specified.
display_Vars:
Additional command "display" can be input at prompt and this will display the names and values of the stored variable by the user so far.

Bugs:
If you just press enter on the prompt (the user doesn't enter anything before hand), the program will exit and dump it's core.

This project is a personal copy of the project wroked on by Deavin Heaster and myself. Logged here for personal documentation.
