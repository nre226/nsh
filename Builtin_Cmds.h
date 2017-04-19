#ifndef BUILTIN_CMDS_H
#define BUILTIN_CMDS_H

#include <string>
#include <vector>
#include <map>

void prompt_Change(std::string &prompt, std::vector<std::string> &tokens);
void procs(std::map<int, std::string> &processes, std::vector<std::string> &procNames);
void dir(std::vector<std::string> &tokens);
void setvar(std::vector<std::string> &tokens, std::map<std::string, std::string> &userVars);
void display_Vars(std::map<std::string, std::string> &userVars);

#endif
