#include <iostream>
#include <bitset>
#include <fstream>
#include <string>
#include <stdio.h>
#include "Riscv_core.h"

using namespace std;

std::string exec(const char* cmd) {
    char buffer[128];
    std::string result = "";
    FILE* pipe = popen(cmd, "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        while (fgets(buffer, sizeof buffer, pipe) != NULL) {
            result += buffer;
        }
    } catch (...) {
        pclose(pipe);
        throw;
    }
    pclose(pipe);
    return result;
}

int main()
{
   Riscv_core cpu = Riscv_core();

   //Read file and initialize memory
   char input_file[35] = "elf2hex/tests/rv64ui-p-xori";
   char a[150] = "elf2hex/elf2hex --bit-width 32 --input ";
   strcat(a, input_file);
    strcat(a, " --output data_hex.txt");
    string ou = exec(a);

   ifstream inFile;
   inFile.open("data_hex.txt");
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

    //execute instructions
    while (!cpu.ecall){
    cout << std::hex<< cpu.pc <<endl;
    cpu.exec_nxt_ins();

    }
    cout << cpu.regfile[gp] <<endl;
    return 0;
}
