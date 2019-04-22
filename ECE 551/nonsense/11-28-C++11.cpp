#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>  // used for wait
#include <unistd.h>    // used for fork, exec, ...

#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <unordered_map>  //used for variables
#include <vector>

extern char ** environ;  //external variable, used for execve()

bool Is_cd(const std::vector<std::string> & argv) {
  if (argv[0] == "cd") {
    if (argv.size() != 2) {
      std::cerr << "error: arguments number incorrect." << std::endl;
    }
    else if (chdir(argv[1].c_str()) == -1) {
      std::cerr << "error: No such file or directory." << std::endl;
    }
    return true;
  }
  return false;
}

bool Is_Var_Related(const std::vector<std::string> & argv,
                    std::unordered_map<std::string, std::string> & Var_Map) {
  std::string command = argv[0];
  int NumArgv = int(argv.size());
  if (command == "export" && NumArgv == 2) {
    std::unordered_map<std::string, std::string>::const_iterator it = Var_Map.find(argv[1]);
    if (it == Var_Map.end()) {
      std::cout << "Key was not found" << std::endl;
    }
    else {
      std::string Name_Value = it->first + "=" + it->second;
      if (putenv((char *)Name_Value.c_str()) == -1) {
        std::cerr << "export failed" << std::endl;
      }
    }
    return true;
  }
  else if (command == "inc" && NumArgv == 2) {
    std::unordered_map<std::string, std::string>::const_iterator it = Var_Map.find(argv[1]);
    if (it == Var_Map.end()) {
      std::cout << "Key was not found" << std::endl;
    }
    else {
      std::string value_str = it->second;
      int BaseTenFlag = 1;
      for (int i = 0; i < int(value_str.size()); i++) {
        if (!isdigit(value_str[i])) {
          BaseTenFlag = 0;
          break;
        }
      }
      if (BaseTenFlag) {
        int value = atoi(value_str.c_str());
        value++;
        std::stringstream ss;
        ss << value;
        std::string value_back = ss.str();
        Var_Map[it->first] = value_back;
      }
      else {
        Var_Map[it->first] = "1";
      }
    }
    return true;
  }
  else if (command == "set" && NumArgv == 3) {
    std::pair<std::string, std::string> var(argv[1], argv[2]);
    Var_Map.insert(var);
    return true;
  }

  return false;
}

bool PathSpecified(const std::vector<std::string> & argv, std::string & FullPath) {
  if (argv[0][0] == '.') {
    if (argv[0].size() >= 3) {  // make sure not to exceed the bound
      if (argv[0][1] == '/') {  // finally judge if it is ./sth form
        char * curr_dir = get_current_dir_name();
        FullPath = curr_dir;
        for (int i = 1; i < int(argv[0].size()); i++) {
          FullPath.push_back(argv[0][i]);
        }
        free(curr_dir);
        return true;
      }
    }
  }
  for (int i = 1; i < int(argv.size()); i++) {
    if (argv[i][0] == '/') {
      FullPath = argv[i];
      if (FullPath[FullPath.size() - 1] != '/') {
        FullPath.push_back('/');
      }
      FullPath += argv[0];
      return true;
    }
  }
  return false;
}

bool FileExist(const std::string & FullPath) {
  std::ifstream ifs(FullPath.c_str());
  if (ifs.good()) {
    ifs.close();
    return true;
  }
  return false;
}

bool FileExist(const std::vector<std::string> & PathList,
               const std::string & command,
               std::string & FullPath) {
  for (int i = 0; i < int(PathList.size()); i++) {
    FullPath = PathList[i] + "/" + command;
    std::ifstream ifs(FullPath.c_str());
    if (ifs.good()) {
      ifs.close();
      return true;
    }
  }
  return false;
}

char ** argv_C_mod(const std::vector<std::string> & argv) {
  char ** argv_c = new char *[argv.size() + 1];
  for (int i = 0; i < int(argv.size()); i++) {
    const char * TempStr = argv[i].c_str();
    char * TempStr_c = new char[argv[i].size() + 1];
    strcpy(TempStr_c, TempStr);
    argv_c[i] = TempStr_c;
  }
  argv_c[argv.size()] = NULL;
  return argv_c;
}

void argv_C_delete(char ** argv, int n) {
  for (int i = 0; i < int(n); i++) {
    delete[] argv[i];
  }
  delete[] argv;
}

std::vector<std::string> ParseCommandBySpace(std::string command) {
  command.push_back(' ');
  std::vector<std::string> result;
  std::string argument = "";
  int One_Argument_Flag = 0;
  int Escape_Flag = 0;
  for (int i = 0; i < int(command.size()); i++) {
    if (command[i] != ' ') {
      if (command[i] == '\\') {
        Escape_Flag = 1;
        continue;
      }
      argument.push_back(command[i]);
      One_Argument_Flag = 1;
    }
    else {  //if command[i] = ' '
      if (Escape_Flag) {
        argument.push_back(' ');
        Escape_Flag = 0;
        continue;
      }
      if (One_Argument_Flag) {
        result.push_back(argument);
        argument = "";
        One_Argument_Flag = 0;
      }
    }
  }
  return result;
}

std::vector<std::string> ParseCommandByComma(std::string PathList) {
  std::vector<std::string> result;
  std::string argument = "";
  int One_Argument_Flag = 0;
  for (int i = 0; i < int(PathList.size()); i++) {
    if (PathList[i] != ':') {
      argument.push_back(PathList[i]);
      One_Argument_Flag = 1;
    }
    else {
      if (One_Argument_Flag) {
        result.push_back(argument);
        argument = "";
      }
      One_Argument_Flag = 0;
    }
  }
  return result;
}

void ReplaceArguments(std::vector<std::string> & argv,
                      const std::unordered_map<std::string, std::string> & Var_Map) {
  for (int i = 0; i < int(argv.size()); i++) {
    if (argv[i][0] == '$') {
      std::string VariableName = argv[i].substr(1);  // only ignore '$'
      std::unordered_map<std::string, std::string>::const_iterator it = Var_Map.find(VariableName);
      if (it != Var_Map.end()) {
        argv[i] = it->second;  // Can we change at this point?????
      }
    }
  }
}

void ProgramExecution(const char * filename, char ** const argv) {
  pid_t pid = fork();
  pid_t w;
  int wstatus;
  if (pid < 0) {
    perror("fork");
    exit(EXIT_FAILURE);
  }
  else if (pid == 0) {  //child process
    /*int n = sizeof(argv) / sizeof(argv[0]);
    for (int i = 0; i < n; i++) {
      std::cout << "argv[" << i << "]: " << argv[i] << std::endl;
    }*/
    execve(filename, argv, environ);
    perror("execve");
    exit(EXIT_FAILURE);
  }
  else {  //parent process
    do {
      w = waitpid(pid, &wstatus, WUNTRACED | WCONTINUED);
      if (w == -1) {
        perror("waitpid");
        exit(EXIT_FAILURE);
      }
      if (WIFEXITED(wstatus)) {
        std::cout << "Program exited with status " << WEXITSTATUS(wstatus) << std::endl;
      }
      else if (WIFSIGNALED(wstatus)) {
        std::cout << "Program was killed by signal " << WTERMSIG(wstatus) << std::endl;
      }
    } while (!WIFEXITED(wstatus) && !WIFSIGNALED(wstatus));
  }
}

void Convert_And_Execute(const std::vector<std::string> & argv, const std::string & FullPath) {
  const char * FullPath_c = FullPath.c_str();
  char ** newargv_c = argv_C_mod(argv);
  ProgramExecution(FullPath_c, newargv_c);
  argv_C_delete(newargv_c, argv.size());
}

void PrintCommandNotFound(const std::string & command) {
  std::cout << "Command " << command << " not found" << std::endl;
}

void PrintPrompt() {
  char * curr_dir = get_current_dir_name();
  std::cout << "myShell:" << curr_dir << "$ ";
  free(curr_dir);
}

int main() {
  std::unordered_map<std::string, std::string> Var_Map;
  while (1) {
    PrintPrompt();
    std::string FullCommand;
    getline(std::cin, FullCommand);
    if (std::cin.eof() || FullCommand == "exit") {
      break;
    }

    std::vector<std::string> newargv = ParseCommandBySpace(FullCommand);
    ReplaceArguments(newargv, Var_Map);
    std::string command = newargv[0];  // newargv[0] must be a command
    std::string FullPath;

    if (Is_cd(newargv) || Is_Var_Related(newargv, Var_Map)) {
      continue;
    }
    else {
      if (PathSpecified(newargv, FullPath)) {
        if (FileExist(FullPath)) {
          Convert_And_Execute(newargv, FullPath);
        }
        else {
          PrintCommandNotFound(command);
        }
      }
      else {
        std::string PATHenv = std::getenv("PATH");
        std::vector<std::string> PathList = ParseCommandByComma(PATHenv);
        // if file exist, FullPath is updated to the actual path
        if (FileExist(PathList, command, FullPath)) {
          Convert_And_Execute(newargv, FullPath);
        }
        else {
          PrintCommandNotFound(command);
        }
      }
    }
  }
  return EXIT_SUCCESS;
}
