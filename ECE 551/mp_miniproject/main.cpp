#include "myShell.h"

int main() {
  // set Var_Map to store set variables
  std::map<std::string, std::string> Var_Map;
  // Initialize ECE551PATH environment variable with value of PATH
  const char * ECE551 = "ECE551PATH";
  setenv(ECE551, getenv("PATH"), 1);
  // Storing process
  StoreEnvVar(Var_Map);
  // Loop and Loop for shell to continue
  while (1) {
    // Print prompt
    PrintPrompt();
    std::string FullCommand;
    // Exit under some condition
    if (IfExitShell(FullCommand)) {
      break;
    }
    // Judge if the command is full of space or empty, if so, do nothing
    int OnlySpaceFlag = 1;
    for (int i = 0; i < int(FullCommand.size()); i++) {
      if (FullCommand[i] != ' ') {
        OnlySpaceFlag = 0;
        break;
      }
    }
    if (OnlySpaceFlag) {
      continue;
    }
    // newargv stores separated strings by space
    std::vector<std::string> newargv = ParseCommandBySpace(FullCommand);
    // replace contents in newargv
    ReplaceArguments(newargv, Var_Map);
    // set for redirection operation
    bool RedirectionFlag = RedirectionContained(newargv);
    int RedirectOPNum = -1;
    std::string LastArgv = newargv[newargv.size() - 1];
    if (RedirectionFlag) {
      if (newargv.size() > 2) {
        // check no.2 last char
        std::string check_char = newargv[newargv.size() - 2];
        if (!(check_char == "<" || check_char == ">" || check_char == "2>")) {
          std::cerr << "Wrong redirection operation!" << std::endl;
          continue;
        }
        RedirectOPNum = RedirectionNumber(check_char);
      }
      else {
        std::cerr << "No enough argument for redirection!" << std::endl;
        continue;
      }
    };

    // newargv[0] is command program
    std::string command = newargv[0];
    std::string FullPath;

    // case 1: cd/set/inc/export
    if (Is_cd(newargv) || Is_Var_Related(newargv, Var_Map, FullCommand)) {
      continue;
    }
    // case 2: execute program
    else {
      // see if path is specified
      if (PathSpecified(newargv, FullPath)) {
        if (FileExist(FullPath)) {
          Convert_And_Execute(newargv, FullPath, RedirectOPNum, LastArgv);
        }
        else {
          std::cout << "Path " << newargv[0] << " not found" << std::endl;
        }
      }
      else {
        std::string PATHenv = std::getenv("ECE551PATH");
        std::vector<std::string> PathList = ParseCommandByComma(PATHenv);
        // if file exist, FullPath will be updated to the actual path
        if (FileExist(PathList, command, FullPath)) {
          Convert_And_Execute(newargv, FullPath, RedirectOPNum, LastArgv);
        }
        else {
          std::cout << "Command " << command << " not found" << std::endl;
        }
      }
    }
  }
  return EXIT_SUCCESS;
}
