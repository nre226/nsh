#ifndef PROGRAM_CONTROL_H
#define PROGRAM_CONTROL_H

#include <string>
#include <vector>
#include <map>

void to_Var(std::vector<std::string> &tokens, std::map<std::string, std::string> &userVars);
void back_Command(std::vector<std::string> &tokens, std::map<int, std::string> &processes, std::vector<std::string> &procNames);
void do_Command(std::vector<std::string> &tokens);
int scanner_parser(std::string input, std::vector<std::string> &tokens, std::map<std::string, std::string> &userVars);
void split(std::vector<std::string> &currTokens, const std::string &item, char delim);
std::string toString(int item);

#endif
