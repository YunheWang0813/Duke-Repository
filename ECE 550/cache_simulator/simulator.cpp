#define NONEXIST 0
#define UNIFIED 1
#define SPLIT 2

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
using namespace std;

#include "simulator.h"

// this function get the 3 parameters for cache from cin and returns them. first as A, second B, third C.
int * getParas() {
  int * paras = new int[3];
  cout << "Associativity: ";
  cin >> paras[0];
  cout << "Block size: ";
  cin >> paras[1];
  cout << "Capacity: ";
  cin >> paras[2];
  return paras;
}

Simulator::Simulator(int * l1_I$_p, int * l1_D$_p, int * l2_I$_p, int * l2_D$_p) :
    l1_I$(l1_I$_p[0], l1_I$_p[1], l1_I$_p[2]),
    l1_D$(l1_D$_p[0], l1_D$_p[1], l1_D$_p[2]),
    l2_I$(l2_I$_p[0], l2_I$_p[1], l2_I$_p[2]),
    l2_D$(l2_D$_p[0], l2_D$_p[1], l2_D$_p[2]) {
  l1_I$.setNextCaches(&l2_I$, &l2_D$);
  l1_D$.setNextCaches(&l2_I$, &l2_D$);
}

/*
vector<int> Simulator::Convert_and_Parse(const string & temp) {
  istringstream ss(temp);
  string token;
  vector<int> result;
  int FirstFlag = 0;
  while (getline(ss, token, ' ')) {
    if (!FirstFlag) {
      result.push_back(atoi(token.c_str()));
      FirstFlag = 1;
    }
    else {
      result.push_back(stoi(token, nullptr, 16));
    }
  }
  return result;
}
*/

vector<int> Simulator::Convert_and_Parse(const string & temp) {
  string _code = temp.substr(0, 1);
  string _addr = temp.substr(2);
  vector<int> result;
  result.push_back(atoi(_code.c_str()));
  stringstream ss;
  ss << std::hex << _addr;
  int addr;
  ss >> addr;
  result.push_back(addr);
  return result;
}

void Simulator::FillInput() {
  cout << "Input file: ";
  string Filename;
  cin >> Filename;
  ifstream ifs(Filename);
  for (string line; getline(ifs, line);) {
    vector<int> Temp_Pair = Convert_and_Parse(line);
    codes.push_back(Temp_Pair[0]);
    addrs.push_back(Temp_Pair[1]);
  }
}

void Simulator::run(int l1_mode, int l2_mode) {
  int clock = 0;
  for (size_t i = 0; i < addrs.size(); i++) {
    // is l1 is unified
    if (l1_mode == UNIFIED) {
      // only use the insn cache in l1
      l1_I$.access(addrs[i], codes[i], l2_mode, clock);
    }
    else if (l1_mode == SPLIT) {
      // if the code is fetch insns
      if (codes[i] == 2) {
        // use the insn cache in l1
        l1_I$.access(addrs[i], codes[i], l2_mode, clock);
      }
      // if the code is read/write data
      else {
        // use the data cache in l1
        l1_D$.access(addrs[i], codes[i], l2_mode, clock);
      }
    }
    clock++;
  }
}

void Simulator::print(int l2_mode) {
  float _Total = l1_I$.getReadAccess() + l1_I$.getWriteAccess() + l1_I$.getInsnAccess() +
                 l1_D$.getReadAccess() + l1_D$.getWriteAccess() + l1_D$.getInsnAccess();
  float _Instrn = l1_I$.getInsnAccess() + l1_D$.getInsnAccess();
  float _Data = l1_I$.getReadAccess() + l1_D$.getReadAccess() + l1_I$.getWriteAccess() +
                l1_D$.getWriteAccess();
  float _Read = l1_I$.getReadAccess() + l1_D$.getReadAccess();
  float _Write = l1_I$.getWriteAccess() + l1_D$.getWriteAccess();
  float _Total_M = l1_I$.getReadMiss() + l1_I$.getWriteMiss() + l1_I$.getInsnMiss() +
                   l1_D$.getReadMiss() + l1_D$.getWriteMiss() + l1_D$.getInsnMiss();
  float _Instrn_M = l1_I$.getInsnMiss() + l1_D$.getInsnMiss();
  float _Data_M =
      l1_I$.getReadMiss() + l1_D$.getReadMiss() + l1_I$.getWriteMiss() + l1_D$.getWriteMiss();
  float _Read_M = l1_I$.getReadMiss() + l1_D$.getReadMiss();
  float _Write_M = l1_I$.getWriteMiss() + l1_D$.getWriteMiss();
  printf("L1:\n");
  printf("Metrics                      Total           Instrn                 Data            "
         "Read           Write            Misc\n");
  printf("------                       -----           ------                 ----            "
         "----           -----            ----\n");
  printf("Demand Fetches              %d             %d                  %d           %d "
         "           %d               0\n",
         int(_Total),
         int(_Instrn),
         int(_Data),
         int(_Read),
         int(_Write));
  printf("  Fraction of total          %.4f           %.4f                %.4f         %.4f    "
         "      %.4f          0.0000\n",
         _Total / _Total,
         _Instrn / _Total,
         _Data / _Total,
         _Read / _Total,
         _Write / _Total);
  printf("\n");
  printf("Demand Misses                 %d              %d                   %d             %d     "
         "        %d               0\n",
         int(_Total_M),
         int(_Instrn_M),
         int(_Data_M),
         int(_Read_M),
         int(_Write_M));
  printf("  Demand Misses rate         %.4f           %.4f                %.4f         %.4f    "
         "      %.4f          0.0000\n",
         _Total_M / _Total,
         _Instrn_M / _Instrn,
         _Data_M / _Data,
         _Read_M / _Read,
         _Write_M / _Write);

  if (l2_mode != 0) {
    float _Total_2 = l2_I$.getReadAccess() + l2_I$.getWriteAccess() + l2_I$.getInsnAccess() +
                     l2_D$.getReadAccess() + l2_D$.getWriteAccess() + l2_D$.getInsnAccess();
    float _Instrn_2 = l2_I$.getInsnAccess() + l2_D$.getInsnAccess();
    float _Data_2 = l2_I$.getReadAccess() + l2_D$.getReadAccess() + l2_I$.getWriteAccess() +
                    l2_D$.getWriteAccess();
    float _Read_2 = l2_I$.getReadAccess() + l2_D$.getReadAccess();
    float _Write_2 = l2_I$.getWriteAccess() + l2_D$.getWriteAccess();
    float _Total_M_2 = l2_I$.getReadMiss() + l2_I$.getWriteMiss() + l2_I$.getInsnMiss() +
                       l2_D$.getReadMiss() + l2_D$.getWriteMiss() + l2_D$.getInsnMiss();
    float _Instrn_M_2 = l2_I$.getInsnMiss() + l2_D$.getInsnMiss();
    float _Data_M_2 =
        l2_I$.getReadMiss() + l2_D$.getReadMiss() + l2_I$.getWriteMiss() + l2_D$.getWriteMiss();
    float _Read_M_2 = l2_I$.getReadMiss() + l2_D$.getReadMiss();
    float _Write_M_2 = l2_I$.getWriteMiss() + l2_D$.getWriteMiss();
    printf("\n");
    printf("L2:\n");
    printf("Metrics                      Total           Instrn                 Data            "
           "Read           Write            Misc\n");
    printf("------                       -----           ------                 ----            "
           "----           -----            ----\n");
    printf("Demand Fetches              %d             %d                  %d           %d "
           "           %d               0\n",
           int(_Total_2),
           int(_Instrn_2),
           int(_Data_2),
           int(_Read_2),
           int(_Write_2));
    printf("  Fraction of total          %.4f           %.4f                %.4f         %.4f    "
           "      %.4f          0.0000\n",
           _Total_2 / _Total_2,
           _Instrn_2 / _Total_2,
           _Data_2 / _Total_2,
           _Read_2 / _Total_2,
           _Write_2 / _Total_2);
    printf("\n");
    printf(
        "Demand Misses                 %d              %d                   %d             %d     "
        "        %d               0\n",
        int(_Total_M_2),
        int(_Instrn_M_2),
        int(_Data_M_2),
        int(_Read_M_2),
        int(_Write_M_2));
    printf("  Demand Misses rate         %.4f           %.4f                %.4f         %.4f    "
           "      %.4f          0.0000\n",
           _Total_M_2 / _Total_2,
           _Instrn_M_2 / _Instrn_2,
           _Data_M_2 / _Data_2,
           _Read_M_2 / _Read_2,
           _Write_M_2 / _Write_2);
  }
}

int main() {
  int l1_mode, l2_mode;
  cout << "Give parameters for l1 insn cache:" << endl;
  int * l1_I$_p = getParas();

  cout << "Give parameters for l1 data cache:" << endl;
  int * l1_D$_p = getParas();

  cout << "Give parameters for l2 insn cache:" << endl;
  int * l2_I$_p = getParas();

  cout << "Give parameters for l2 data cache:" << endl;
  int * l2_D$_p = getParas();

  Simulator simulator(l1_I$_p, l1_D$_p, l2_I$_p, l2_D$_p);

  delete[] l1_I$_p;
  delete[] l1_D$_p;
  delete[] l2_I$_p;
  delete[] l2_D$_p;
  std::cout << "Type in mode for l1 (0 for none, 1 for unified, 2 for split): ";
  std::cin >> l1_mode;
  std::cout << "Type in mode for l2: ";
  std::cin >> l2_mode;
  simulator.FillInput();
  simulator.run(l1_mode, l2_mode);
  simulator.print(l2_mode);
  return EXIT_SUCCESS;
}
