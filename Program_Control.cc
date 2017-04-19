#include "Program_Control.h"
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <sys/wait.h>	// waitpid()
#include <sys/types.h>	// waitpid()
#include <unistd.h>		// fork(),exec(),pid_t,chdir()
#include <stdlib.h>		// exit()
#include <fcntl.h>	// open("/dev/null", STDOUT_FILENO)

#define BUFFER 4096

void to_Var(std::vector<std::string> &tokens, std::map<std::string, std::string> &userVars) {
	char *argv[tokens.size()-1];
	for (int i = 2; i < tokens.size(); i+=1)	// start at 2 because that is the index of the cmd and parameters
		argv[i-2] = const_cast<char *>(tokens[i].c_str());
	argv[tokens.size()-2] = NULL;

	pid_t pid;
	int pipefd[2];
	char buffer[BUFFER];
	if (pipe(pipefd) == -1) {
		std::cerr << "ERROR: Pipe failed\n";
	}
	pid = fork();

	if (pid == 0) {
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[0]);
		close(pipefd[1]);

		//int fd = open("/tmp/output.o", std::ios_base::trunc);//O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
		//dup2(fd, 1);
		//dup2(fd, 2);
		//close(fd);

		if (tokens[1][0] == '/' || tokens[1].substr(0,2) == "./" || tokens[1].substr(0,3) == "../")
			execv(argv[0], argv);
		else
			execvp(argv[0], argv);
		std::cerr << "ERROR: Could not execute command \"" << argv[0] << "\"\n";
		exit(1);
	} else if (pid > 0) {
		close(pipefd[1]);
		userVars["$" + tokens[1]] = "";
		int nbytes = read(pipefd[0], &buffer, sizeof(buffer));
		userVars["$" + tokens[1]] = buffer;
		userVars["$" + tokens[1]].erase(nbytes-1);
		
		//std::ifstream file ("/tmp/output.o", std::ifstream::in);
		//std::string line;
		//userVars["$" + tokens[1]] = "";
		//while (std::getline(file, line))
		//	userVars["$" + tokens[1]] += line;


	} else {
		std::cerr << "ERROR: Fork failed\n";
		exit(1);
	}
}

void back_Command(std::vector<std::string> &tokens, std::map<int, std::string> &processes, std::vector<std::string> &procNames) {
	char *argv[tokens.size()];
	for (int i = 1; i < tokens.size(); i+=1)
		argv[i-1] = const_cast<char *>(tokens[i].c_str());
	argv[tokens.size()-1] = NULL;
	
	pid_t pid = fork();
	if (pid == 0) {	// child

		//int fd;
		//fd = open("/dev/null", O_WRONLY);
		//dup2(fd, 1);
		//close(fd);

		if (tokens[1][0] == '/' || tokens[1].substr(0,2) == "./" || tokens[1].substr(0,3) == "../")
			execv(argv[0], argv);
		else
			execvp(argv[0], argv);

		std::cerr << "ERROR: Could not execute command \"" << argv[0] << "\"\n";		// exec shouldn't return
		exit(1);
	} else if (pid < 0) {
		std::cerr << "ERROR: Could not execute fork for \"" << argv[0] << "\"\n";
		exit(1);
	} else {		// parent
		processes[pid] = "In Progress";
		procNames.push_back(toString(pid) + ":" + tokens[1]);
	}
	return;
}

void do_Command(std::vector<std::string> &tokens) {
	
	char *argv[tokens.size()];
	for (int i = 1; i < tokens.size(); i+=1)
		argv[i-1] = const_cast<char *>(tokens[i].c_str());
	argv[tokens.size()-1] = NULL;
	
	pid_t pid = fork();
	if (pid > 0) {			// parent
		int status;
		waitpid(pid, &status, 0);
	} else if (pid == 0) {	// child
		bool multiDirectories = false;
		std::vector<std::string> directories;
		char *directs[BUFFER];
		if (tokens[1].find(":") != std::string::npos) {
			split(directories, tokens[1], ':');
			multiDirectories = true;

			for (int i = 0; i < directories.size(); i+=1) {
				argv[i] = const_cast<char *>(directories[i].c_str());
			argv[directories.size()] = NULL;
			}
		}
		
		if (multiDirectories) {
			for (int i = 0; i < directories.size(); i+=1) {
				if (tokens[1][0] == '/' || tokens[1].substr(0,2) == "./" || tokens[1].substr(0,3) == "../")
					execv(directs[i], argv);
				else
					execvp(directs[i], argv);
			}
		} else {
			if (tokens[1][0] == '/' || tokens[1].substr(0,2) == "./" || tokens[1].substr(0,3) == "../")
				execv(argv[0], argv);
			else
				execvp(argv[0], argv);
		}

		std::cerr << "ERROR: Could not execute command \"" << argv[0] << "\"\n";		// exec shouldn't return
		exit(1);
	} else {
		std::cerr << "ERROR: Could not execute fork for \"" << argv[0] << "\"\n";
		exit(1);
	}
	return;
}

int scanner_parser(std::string input, std::vector<std::string> &tokens, std::map<std::string, std::string> &userVars) {
	std::vector<std::string>().swap(tokens); // free mem & replace with empty 1
	std::vector<std::string> currTokens;	// tokens split at quotes
	std::vector<bool> quoteTokens;		// keeps track of if each token is a quote or not

	// if (%) in input, then truncate at that pos, as it is a comment
	if (input.find("%") != std::string::npos)
		input = input.substr(0,input.find("%"));

	// Check for usage of quotes
	// one "two \" two"
	std::size_t pos1 = input.find("\""), pos2=1, firstQuote=-1, secondQuote=-1;
	bool contSearch = true;
	while (pos1 != std::string::npos && contSearch) {			// if (") is found in input
		if (pos1 == 0) {		// if first (") is in begin of input
			firstQuote = pos1;
			input.erase(pos1,1);
			pos1 = input.find("\"");
		} else if (pos1 == input.size()-1 && firstQuote != -1) {
			std::cerr << "Error: Unmatched quotations\n";
			return(1);
		} else {				// if (") not first char in innut
			if (input[pos1-1] != '\\') {			// if first (") isn't escaped
				if (firstQuote != -1) {
					secondQuote = pos1;
					input.erase(pos1,1);
					currTokens.push_back(input.substr(firstQuote, secondQuote-firstQuote));
					quoteTokens.push_back(true);
					input = input.substr(secondQuote, input.size()-secondQuote);
					pos1 = input.find("\"");
					if (pos1 == std::string::npos && input.size() != 0) {		// if string w/ no quotes exists
						currTokens.push_back(input);
						quoteTokens.push_back(false);
					} else if (pos1 != std::string::npos && input.size() != 0)
						firstQuote = -1;
					continue;
				}
				firstQuote = pos1; 		// save pos1
				input.erase(pos1,1);	// get that (") out of here!!!!
				pos2 = input.find("\"");	// find next quotation
				if (input[pos2-1] != '\\' && pos2 != std::string::npos) {		// if next (") not escaped
					secondQuote = pos2;
					input.erase(pos2,1);
					currTokens.push_back(input.substr(0, firstQuote-1));	// add string b4 first (") to currTokens
					quoteTokens.push_back(false);
					currTokens.push_back(input.substr(firstQuote, secondQuote-firstQuote));	// add string b/w (") to currTokens
					quoteTokens.push_back(true);
					if (pos2+1 != input.size()) {	// if last (") not last char of input
						if (input.find("\"", pos2+1) == std::string::npos) {	// if no more (") left in input
							currTokens.push_back(input.substr(secondQuote, input.size()-secondQuote-1));
							quoteTokens.push_back(false);
							contSearch = false;
						} else { 					// if >=1 (") left in input
							input = input.substr(secondQuote, input.size()-secondQuote-1);
							pos1 = input.find("\"");
						}
					} else {						// if last (") is last char of input
						contSearch = false;
					}
				} else if (input[pos2-1] == '\\' && pos2 != std::string::npos) {			// if next (") is escaped
					input.erase(pos2-1,1);
					pos1 = input.find("\"", pos2);
				} else { 	// if firstQuote is found but not second one
					if (firstQuote != -1) {
						std::cerr << "Error: Unmatched quotations\n";
						return(1);
					}
				}
			} else {								// if first (") is escaped
				pos1 = input.find("\"", pos1+1);
			}
		}
	}

	if (currTokens.empty()) { // if no char(") was found
		currTokens.push_back(input);
		quoteTokens.push_back(false);
	}
	// Split each not quoted token and keep order
	for (int i = 0; i < currTokens.size(); i+=1) {
		if (!quoteTokens[i]) 		// if 'false' for no quotes in that element
			split(tokens, currTokens[i], ' ');
		else
			tokens.push_back(currTokens[i]);
	}

	// check for variables in tokens
	//for(std::map<int, std::string>::const_iterator i = processes.begin(); i != processes.end(); ++i)
	bool varFlag = false, varFound = false;
	for (int i = 0; i < tokens.size(); i+=1)
		if (tokens[i].find("$") != std::string::npos) {
			varFlag = true;
			int varLength = 0;
			std::size_t endPos = tokens[i].find(" ", tokens[i].find("$")), startPos = tokens[i].find("$");
			if (startPos != std::string::npos)
				startPos += 1;
			if (endPos != std::string::npos) {
				varLength = endPos - startPos;
				if (userVars.find(tokens[i].substr(startPos-1, varLength+1)) != userVars.end()) {
					tokens[i] = tokens[i].substr(0, startPos-1) + userVars[tokens[i].substr(startPos, varLength)] + tokens[i].substr(endPos);
					varFound = true;
				}
			} else {
				varLength = tokens[i].size() - startPos;
				if (userVars.find(tokens[i].substr(startPos-1)) != userVars.end()) {
					tokens[i] = tokens[i].substr(0, startPos-1) + userVars[tokens[i].substr(startPos-1)];
					varFound = true;
				}
			}
		}
	if (varFlag && !varFound) {
		std::cerr << "ERROR: no matching variable name found\n";
		return(1);
	}
	return(0);
}

void split(std::vector<std::string> &tokens, const std::string &item, char delim) {
	std::stringstream ss;
	ss.str(item);
	std::string temp;
	while (std::getline(ss, temp, delim))
		if (temp != "")
			tokens.push_back(temp);
}

std::string toString(int item) {
	std::ostringstream strstreamO;
	strstreamO << item;
	return strstreamO.str();
}
