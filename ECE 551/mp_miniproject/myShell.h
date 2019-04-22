#ifndef _MYSHELL__
#define _MYSHELL__

#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <sstream>
#include <string>
#include <vector>

extern char ** environ;  //external environment variable declared

//Step 1: Read a command name and, run it
bool IfExitShell(std::string & FullCommand);
void PrintPrompt();
void Convert_And_Execute(std::vector<std::string> & argv,
                         const std::string & FullPath,
                         const int & RedirectOPNum,
                         const std::string & LastArgv);
char ** argv_C_mod(const std::vector<std::string> & argv);
void argv_C_delete(char ** argv, int n);
void ProgramExecution(const char * filename,
                      char ** const argv,
                      const int & RedirectOPNum,
                      const std::string LastArgv);

//Step 2: Improving commands

bool PathSpecified(const std::vector<std::string> & argv, std::string & FullPath);
bool FileExist(const std::string & FullPath);
bool FileExist(const std::vector<std::string> & PathList,
               const std::string & command,
               std::string & FullPath);
std::vector<std::string> ParseCommandBySpace(std::string command);
std::vector<std::string> ParseCommandByComma(std::string PathList);

//Step 3: Directories and variables
bool Is_cd(const std::vector<std::string> & argv);
bool Set_op(const std::vector<std::string> & argv,
            std::map<std::string, std::string> & Var_Map,
            const std::string & FullCommand);
bool Export_op(const std::vector<std::string> & argv, std::map<std::string, std::string> & Var_Map);
bool Inc_op(const std::vector<std::string> & argv, std::map<std::string, std::string> & Var_Map);
bool Is_Var_Related(const std::vector<std::string> & argv,
                    std::map<std::string, std::string> & Var_Map,
                    const std::string & FullCommand);
void StoreEnvVar(std::map<std::string, std::string> & Var_Map);
void ReplaceArguments(std::vector<std::string> & argv,
                      const std::map<std::string, std::string> & Var_Map);

//Step 4: Pipes and redirection

bool RedirectionContained(const std::vector<std::string> & argv);
int RedirectionNumber(std::string OP);

#endif
