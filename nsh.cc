#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sys/wait.h>
#include "Program_Control.h"
#include "Builtin_Cmds.h"

int main() {

	std::string prompt = "nsh> ", userin = "";
	std::vector<std::string> tokens;
	std::map<int, std::string> processes;
	std::vector<std::string> procNames;
	std::map<std::string, std::string> userVars;
	std::cout << prompt;
	std::getline(std::cin, userin);
	if (userin == "\0") userin = " ";
	while (userin != "done" && userin != "<control-D>") {
		if (!std::cin.bad() && std::cin.eof()) return(0);
		if (scanner_parser(userin, tokens, userVars) == 0) { 	// if return no error, proceed

			if (tokens[0] == "do")
				do_Command(tokens);
			else if (tokens[0] == "back")
				back_Command(tokens, processes, procNames);
			else if (tokens[0] == "tovar")
				to_Var(tokens, userVars);
			else if (tokens[0] == "prompt")
				prompt_Change(prompt, tokens);
			else if (tokens[0] == "set")
				setvar(tokens, userVars);
			else if (tokens[0] == "dir")
				dir(tokens);
			else if (tokens[0] == "procs")
				procs(processes, procNames);
			else if (tokens[0] == "display")
				display_Vars(userVars);
			else
				std::cerr << "Not a valid command input\n";
		}
		std::cout << prompt;
		std::getline(std::cin, userin);
		if (userin == "") userin = " ";
	}
	std::cout << '\n';
	return 0;
}
