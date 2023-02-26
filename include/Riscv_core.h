#ifndef RISCV_CORE_H
#define RISCV_CORE_H
#include <stdint.h>

class Riscv_core
{
    public:
        uint32_t regfile[32];
        uint8_t memory[65536];
        uint64_t pc = 0;

    void exec_nxt_ins(){

        //fetch instruction
        uint32_t* ins_ptr = (uint32_t*) &memory[pc]
        uint32_t ins = *ins_ptr
        cout << ins << endl;
    }

};

#endif // RISCV_CORE_H
