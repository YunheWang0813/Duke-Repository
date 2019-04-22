#include "myShell.h"

/*
The function decide the FullCommand string from user input on 
the Shell and also judge if the Shell should exit. 
 */
bool IfExitShell(std::string & FullCommand) {
  getline(std::cin, FullCommand);
  /* If the user types the command "exit" or EOF is encountered reading
   from stdin, the shell should exit. Here, the function return true and
   in the main it get out of the loop.
  */
  if (std::cin.eof() || FullCommand == "exit") {
    return true;
  }
  return false;
}

/*
The function get the name of current directory and properly print the command prompt 
 */
void PrintPrompt() {
  char * curr_dir = get_current_dir_name();
  std::cout << "myShell:" << curr_dir << "$ ";
  free(curr_dir);
}

/*
The function passed in vector argv and convert to Char** form,
passed in string FullPath and convert to const char* form using argv_C_mod() function,
then execute ProgramExecution() and argv_C_delete() functions to properly execute the program     
 */
void Convert_And_Execute(std::vector<std::string> & argv,
                         const std::string & FullPath,
                         const int & RedirectOPNum,
                         const std::string & LastArgv) {
  // If there is a flag of redirection operation, pop "redirection op" and "file"
  // from passed arguments to ensure execution step works correctly
  if (RedirectOPNum != -1) {
    argv.pop_back();
    argv.pop_back();
  }
  const char * FullPath_c = FullPath.c_str();
  char ** newargv_c = argv_C_mod(argv);
  ProgramExecution(FullPath_c, newargv_c, RedirectOPNum, LastArgv);
  argv_C_delete(newargv_c, argv.size());
}

/*
The function convert vector<string> form into char** form
 */
char ** argv_C_mod(const std::vector<std::string> & argv) {
  // Need to delete later
  char ** argv_c = new char *[argv.size() + 1];
  for (int i = 0; i < int(argv.size()); i++) {
    // Process to convert const char* to char*
    const char * TempStr = argv[i].c_str();
    char * TempStr_c = new char[argv[i].size() + 1];
    strcpy(TempStr_c, TempStr);
    argv_c[i] = TempStr_c;
  }
  // Set last element to be NULL so that execve can properly work
  argv_c[argv.size()] = NULL;
  return argv_c;
}

/*
The function delete assigned spaces, put at the end of main loop
*/
void argv_C_delete(char ** argv, int n) {
  for (int i = 0; i < int(n); i++) {
    delete[] argv[i];
  }
  delete[] argv;
}

/*
The function utilizes fork(), execve() and waitpid() to execute program based on the fullpath,
its argument and environment variables. If the redirection flag was set, we need to operate 
redirection between fork() and execve() calls. Converted form of 
 */
void ProgramExecution(const char * filename,
                      char ** const argv,
                      const int & RedirectOPNum,
                      const std::string LastArgv) {
  pid_t pid = fork();
  pid_t w;
  int wstatus;
  // used for redirection
  int fd = -1;

  // fork error
  if (pid < 0) {
    perror("fork");
    exit(EXIT_FAILURE);
  }
  // Child Process (pid = 0)
  else if (pid == 0) {
    // Redirects standard input for the command
    if (RedirectOPNum == 0) {
      // set file descripter. O_RDONLY (read only), S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH (permission setting)
      if ((fd = open(LastArgv.c_str(), O_RDONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) == -1) {
        // open error
        perror("open");
        exit(EXIT_FAILURE);
      }
      // here, dup2() function specify the file descriptors with RedirectOPnum, and do corresponding operation
      if (dup2(fd, RedirectOPNum) == -1) {
        // dup2 error
        perror("dup2");
        exit(EXIT_FAILURE);
      }
      if (close(fd) == -1) {
        // close error
        perror("close");
        exit(EXIT_FAILURE);
      }
    }
    // redirects standard output(1)/error(2)
    else if (RedirectOPNum == 1 || RedirectOPNum == 2) {
      // O_WRONLY (write only), O_CREAT (create file)
      if ((fd =
               open(LastArgv.c_str(), O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) ==
          -1) {
        perror("open");
        exit(EXIT_FAILURE);
      }
      if (dup2(fd, RedirectOPNum) == -1) {
        perror("dup2");
        exit(EXIT_FAILURE);
      }
      if (close(fd) == -1) {
        perror("close");
        exit(EXIT_FAILURE);
      }
    }

    execve(filename, argv, environ);
    // execve() returns only on error
    perror("execve");
    exit(EXIT_FAILURE);
  }
  // Parent Process
  else {
    do {
      // suspends execution of the calling process until a child specified by pid argument has changed state
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

/*
This function see if the path for executing command is specified. The path can be relative 
or absolute, and for both cases, if the path found, it should return true; if the path not 
found, it should return false.
 */
bool PathSpecified(const std::vector<std::string> & argv, std::string & FullPath) {
  // relative path case
  if (argv[0][0] == '.') {
    // make sure not to exceed the bound
    if (argv[0].size() >= 3) {
      // finally judge if it is ./sth form
      if (argv[0][1] == '/') {
        // get name of current directory
        char * curr_dir = get_current_dir_name();
        FullPath = curr_dir;
        // make FullPath string to be actually a fullpath
        for (int i = 1; i < int(argv[0].size()); i++) {
          FullPath.push_back(argv[0][i]);
        }
        free(curr_dir);
        return true;
      }
      else {
        return false;
      }
    }
    else {
      return false;
    }
  }
  // absolute path case
  else if (argv[0][0] == '/') {
    // argv[0] must be a fullpath
    FullPath = argv[0];
    return true;
  }

  return false;
}

/*
The function is used for the case FullPath is already specified:
it check if the program is actually at that path, and if so, return true.
 */
bool FileExist(const std::string & FullPath) {
  // use ifstream.good() to see if the file can be open at that directory
  std::ifstream ifs(FullPath.c_str());
  if (ifs.good()) {
    ifs.close();
    return true;
  }
  return false;
}

/*
The function is used for the case path is not specified and need to search for 
paths in ECE551PATH. It searches for every path and if the fullpath can be use
ifstream to open, then return true.
*/
bool FileExist(const std::vector<std::string> & PathList,
               const std::string & command,
               std::string & FullPath) {
  for (int i = 0; i < int(PathList.size()); i++) {
    // combine partial path and command to make a temporary fullpath
    FullPath = PathList[i] + "/" + command;
    std::ifstream ifs(FullPath.c_str());
    if (ifs.good()) {
      ifs.close();
      return true;
    }
  }
  return false;
}

/*
The function parse a string by spaces inside it, and return a vector including 
the substring parsed. Space can be single or multiple.When a escaped symbol appears, 
it need to judge if a space need to be included.
 */
std::vector<std::string> ParseCommandBySpace(std::string command) {
  // if string that do not contain a space is passed, just return a
  // vector with only that element.
  if (command.find(' ') == std::string::npos) {
    std::vector<std::string> for_rtn;
    for_rtn.push_back(command);
    return for_rtn;
  }
  // Firstly push back a space to make it easier to judge when operation end
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
      // update argument with new char
      argument.push_back(command[i]);
      // make it consective
      One_Argument_Flag = 1;
    }
    // once meet ' '
    else {
      // if '\' is before that ' '
      if (Escape_Flag) {
        argument.push_back(' ');
        Escape_Flag = 0;
        continue;
      }
      // if already start extracting arguments
      if (One_Argument_Flag) {
        result.push_back(argument);
        // reset argument, One_Argument_Flag
        argument = "";
        One_Argument_Flag = 0;
      }
    }
  }
  return result;
}

/*
The function parse a string by ':' inside that string, and return a vector 
including that substrings. Similar to ParseCommandbyspace, but this function is 
specifically useful to parse ECE551PATH string.
 */
std::vector<std::string> ParseCommandByComma(std::string PathList) {
  if (PathList.find(':') == std::string::npos) {
    std::vector<std::string> for_rtn;
    for_rtn.push_back(PathList);
    return for_rtn;
  }
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

/*
The function specifically handle "cd" opperation. Chdir() function is utilized to 
do the operation. If "cd" return true. 
 */
bool Is_cd(const std::vector<std::string> & argv) {
  if (argv[0] == "cd") {
    // must be only one arguments
    if (argv.size() != 2) {
      std::cerr << "error: arguments number incorrect." << std::endl;
    }
    // change directory, or fail
    else if (chdir(argv[1].c_str()) == -1) {
      std::cerr << "error: No such file or directory." << std::endl;
    }
    return true;
  }
  return false;
}

/*
The function handles "set" operation. Variable is set to the string on the rest of the 
command line. Always return true.
 */
bool Set_op(const std::vector<std::string> & argv,
            std::map<std::string, std::string> & Var_Map,
            const std::string & FullCommand) {
  std::string Var_Name = argv[1];
  // find the Var_Name in Var_Map
  std::size_t found = FullCommand.find(Var_Name);
  // find the index to use substr to cut the command
  found += Var_Name.size() + 1;
  // substr from found index to store required string as value
  std::string Var_Value = FullCommand.substr(found);
  // judge if the name is valid
  for (int i = 0; i < int(Var_Name.size()); i++) {
    if (!(isalpha(Var_Name[i]) || isdigit(Var_Name[i]) || Var_Name[i] == '_')) {
      std::cerr << "Invalid variable name!" << std::endl;
      return true;
    }
  }
  // if Var_Name is already in Var_Map, replace it with new value
  std::map<std::string, std::string>::const_iterator it = Var_Map.find(Var_Name);
  if (it != Var_Map.end()) {
    Var_Map[Var_Name] = Var_Value;
    return true;
  }
  // update Var_Map with Var_Name and Var_Value
  std::pair<std::string, std::string> var(Var_Name, Var_Value);
  Var_Map.insert(var);
  return true;
}

/*
The function handles "export" operation. It put the specific var's current value 
into the environment for other programs.
 */
bool Export_op(const std::vector<std::string> & argv,
               std::map<std::string, std::string> & Var_Map) {
  std::map<std::string, std::string>::const_iterator it = Var_Map.find(argv[1]);
  if (it == Var_Map.end()) {
    std::cout << "Key was not found" << std::endl;
  }
  else {
    // use setenv() function to export the key and value found in the map.
    if (setenv(it->first.c_str(), it->second.c_str(), 1)) {
      std::cerr << "export failed" << std::endl;
    }
  }
  return true;
}

/*
The function handles "inc" operation. It increments the value of var as a number.
If var is not currently set or does not represent a number in base10, then the initial
value is treated as 0 and it will be "1". 
 */
bool Inc_op(const std::vector<std::string> & argv, std::map<std::string, std::string> & Var_Map) {
  std::map<std::string, std::string>::const_iterator it = Var_Map.find(argv[1]);
  // if the argument is not set, set it and value = "1"
  if (it == Var_Map.end()) {
    Var_Map[argv[1]] = "1";
  }
  else {
    std::string value_str = it->second;
    int BaseTenFlag = 1;
    int NegativeFlag = 0;
    std::string value_back;
    for (int i = 0; i < int(value_str.size()); i++) {
      if (i == 0) {
        // negative flag
        if (value_str[0] == '-') {
          NegativeFlag = 1;
          // set it as negative
          value_back.push_back('-');
          continue;
        }
      }
      // if it contains character which is not a number,
      // judge it as a non-number string
      if (!isdigit(value_str[i])) {
        BaseTenFlag = 0;
        break;
      }
    }
    // if negativeflag, and we know no characters other than first '-' in the
    // later string, we simply get the substring of later
    if (NegativeFlag) {
      if (value_str == "-") {
        Var_Map[it->first] = "1";
        return true;
      }
      value_str = value_str.substr(1);
    }
    // if
    if (BaseTenFlag) {
      int value = atoi(value_str.c_str());
      if (NegativeFlag) {
        // addition for negative number
        value--;
        if (value == 0) {
          Var_Map[it->first] = "0";
          return true;
        }
      }
      // normal addition
      else {
        value++;
      }
      // convert from int to string
      std::stringstream ss;
      ss << value;
      value_back += ss.str();
      Var_Map[it->first] = value_back;
    }
    else {
      // set var_value to 1 if it is not considered ans number
      Var_Map[it->first] = "1";
    }
  }
  return true;
}

/*
Judge if above "set", "export", "inc" operation is needed. If need, get
into the corresponding function and return true.
 */
bool Is_Var_Related(const std::vector<std::string> & argv,
                    std::map<std::string, std::string> & Var_Map,
                    const std::string & FullCommand) {
  std::string command = argv[0];
  int NumArgv = int(argv.size());
  if (command == "export" && NumArgv == 2) {
    return Export_op(argv, Var_Map);
  }
  else if (command == "inc" && NumArgv == 2) {
    return Inc_op(argv, Var_Map);
  }
  else if (command == "set") {
    return Set_op(argv, Var_Map, FullCommand);
  }

  return false;
}

/*
The function stores environment variables into own map. It performs Parsing by 
'=' mark operation inside to separate a string to name and value, from the form of
"name=value"
 */
void StoreEnvVar(std::map<std::string, std::string> & Var_Map) {
  int i = 0;
  while (environ[i] != NULL) {
    std::string temp_var = environ[i];
    std::size_t EqualPointer = temp_var.find('=');
    if (EqualPointer != std::string::npos) {
      // separate just by '='
      std::string Key_str = temp_var.substr(0, EqualPointer);
      std::string Value_str = temp_var.substr(EqualPointer + 1);
      Var_Map[Key_str] = Value_str;
      i++;
      continue;
    }
    // bad error if there is no '=' char in argument of environ
    else {
      std::cerr << "Fatal Error!" << std::endl;
      exit(EXIT_FAILURE);
    }
  }
}

/*
The function replace the '$'-initial string in argv from the value found from map.
Special cases are considered. 
 */
void ReplaceArguments(std::vector<std::string> & argv,
                      const std::map<std::string, std::string> & Var_Map) {
  // Loop over the argv
  for (int i = 0; i < int(argv.size()); i++) {
    // if string first char is '$'
    if (argv[i][0] == '$') {
      std::string Full_Argument_Name;
      std::string VariableName;
      // Pointer to track the replacement place
      int CharPointer = 0;
      // flag to manage special characters
      int SpecialCharFlag = 0;
      int j = 1;
      // check if the rest of string contains '$'
      for (; j < int(argv[i].size()); j++) {
        if (SpecialCharFlag && (argv[i][j] != '$')) {
          Full_Argument_Name.push_back(argv[i][j]);
        }
        // judge validity
        else if (!(isalpha(argv[i][j]) || isdigit(argv[i][j]) || argv[i][j] == '_')) {
          if (argv[i][j] == '$' && SpecialCharFlag) {
            SpecialCharFlag = 0;
            CharPointer = j;
            continue;
          }
          // substr argv[i] by a calculated region
          VariableName = argv[i].substr(CharPointer + 1, j - 1 - CharPointer);
          std::map<std::string, std::string>::const_iterator it = Var_Map.find(VariableName);
          if (it != Var_Map.end()) {
            Full_Argument_Name += it->second;
          }
          // If the variable is not found, just do nothing
          if (argv[i][j] != '$') {
            SpecialCharFlag = 1;
            Full_Argument_Name.push_back(argv[i][j]);
          }
          CharPointer = j;
        }
      }
      // If the special case is not need to consider
      if (!SpecialCharFlag) {
        VariableName = argv[i].substr(CharPointer + 1, j - 1 - CharPointer);
        std::map<std::string, std::string>::const_iterator it = Var_Map.find(VariableName);
        if (it != Var_Map.end()) {
          Full_Argument_Name += it->second;
        }
      }
      // finally set argv[i] to be new one
      argv[i] = Full_Argument_Name;
    }
  }
}

/*
The function judge if redirection operation is necessary for the command.
 */
bool RedirectionContained(const std::vector<std::string> & argv) {
  // loop over argv to check
  for (int i = 0; i < int(argv.size()); i++) {
    if (argv[i] == "<" || argv[i] == ">" || argv[i] == "2>") {
      return true;
    }
  }
  return false;
}

/* 
The function assign redirection according to the redirection character specified
 */
int RedirectionNumber(std::string OP) {
  // stdin
  if (OP == "<") {
    return 0;
  }
  //stdout
  if (OP == ">") {
    return 1;
  }
  //stderr
  if (OP == "2>") {
    return 2;
  }
  return -1;
}
