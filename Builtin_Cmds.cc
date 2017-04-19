#include "Builtin_Cmds.h"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <unistd.h>
#include <sstream>
#include <sys/param.h>
#include <sys/wait.h>
#include <sys/types.h>

void prompt_Change(std::string &prompt, std::vector<std::string> &tokens) {
	if (tokens.size() > 2)
		std::cerr << tokens[0] << ": too many arguments\n";
	else if (tokens.size() < 2)
		std::cerr << tokens[0] << ": not enough arguments\n";
	else {
		prompt = tokens[1];
		std::cout << prompt << std::endl;
		//prompt += "> ";
	}
	return;
}

void setvar(std::vector<std::string> &tokens, std::map<std::string, std::string> &userVars) {
	if(tokens.size() > 3)
		std::cerr << tokens[0] << ": too many arguments\n";
	else if(tokens.size() < 3)
		std::cerr << tokens[0] << ": not enough arguments\n";
	else 
		userVars["$" + tokens[1]] = tokens[2];
}

void dir(std::vector<std::string> &tokens) {
	if(tokens.size() > 2)
	{
		std::cerr << tokens[1] << ": too many arguments \n";
		return;
	}
	else if(tokens.size() < 2)
	{
		std::cerr << tokens[1] << ": not enough arguments\n";
		return;
	}
	else
	{
		int marker = chdir(tokens[1].c_str());
		if(marker == 0){
			chdir(tokens[1].c_str());
		}
		else{
			std::cerr << tokens[1] << ": No such file or directory \n";
		}
	}
}

void procs(std::map<int, std::string> &processes, std::vector<std::string> &procNames) {
	int status;
	if(processes.empty())
	{
		std::cout << "No processes are running in the background" << std::endl;
		return;
	}
	std::cout << "\n\tBackground Processes \nProcess ID\tStatus\t\t   Name\n";
	for (int i = 0; i < procNames.size(); i+=1) {
		std::istringstream buf (procNames[i].substr(0, procNames[i].find(":")));
		int currPid;
		buf >> currPid;
		if (waitpid(currPid, &status, WNOHANG))
			processes[currPid] = "Completed";
		std::cout << "   " << currPid;
		std::cout << "\t\t(" << processes[currPid] << ")\t   ";
		std::cout << procNames[i].substr(procNames[i].find(":")+1) << '\n';
	}
	std::cout << '\n';
}

void display_Vars(std::map<std::string, std::string> &userVars) {
	std::cout << "\nUser Variables:\n";
	for(std::map<std::string, std::string>::const_iterator i = userVars.begin(); i != userVars.end(); ++i)
		std::cout << i->first.substr(1) << " : " << i->second << '\n';
	std::cout << '\n';
}
