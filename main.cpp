#include <iostream>
#include <bitset>
#include <fstream>
#include <string>
#include "Riscv_core.h"

using namespace std;

int main()
{
   Riscv_core cpu = Riscv_core();

   //Read file and initialize memory
   ifstream inFile;
   inFile.open("add.txt");
   if (!inFile) {
    cerr << "Unable to open file";
    exit(1);   // call system to stop
    }
    string ins_str;
    //int i = 0;
    uint32_t* ptr= (uint32_t*) &cpu.memory[0];
    while (getline(inFile, ins_str)) {
        uint32_t ins = stoul(ins_str,nullptr,16);
        memcpy(ptr,&ins,4);
        //std::cout << *ptr << <<std::endl;
        //i++;
        ptr++;
    }

    for (int i=0;i<1000;i++){
    cout << std::hex<< cpu.pc <<endl;
    cpu.exec_nxt_ins();

    }
    cout << cpu.regfile[3] <<endl;
    return 0;
}
