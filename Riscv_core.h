#ifndef RISCV_CORE_H
#define RISCV_CORE_H
#include <stdint.h>
#include "Decode_defs.h"

class Riscv_core
{
    public:
        uint64_t regfile[32] = {0};
        uint8_t memory[65536];
        uint64_t pc = 4;

    void exec_nxt_ins(){

        //fetch instruction
        uint32_t* ins_ptr = (uint32_t*) &memory[pc];
        uint32_t ins = *ins_ptr;

        //decode
        uint32_t opcode = ins & 127;
        uint32_t rs1 = (ins >> 15) & 31;
        uint32_t rs2 = (ins >> 20) & 31;
        uint32_t fun3 = (ins >> 12) & 7;
        uint32_t fun7 = (ins >> 25) & 127;
        uint32_t rd = (ins >> 7) & 31;

        //opcode defs
        bool is_r = false;
        bool is_s = false;
        bool is_lui = false;
        bool is_auipc = false;
        bool is_i = false;
        bool is_i_load = false;
        bool is_b = false;
        bool is_jalr = false;
        bool is_jal = false;
        bool is_r32 = false;
        bool is_i32 = false;

        //extract immediates
        uint32_t imm_i;
        uint32_t imm_s;
        uint32_t imm_b;
        uint32_t imm_u;
        uint32_t imm_j;

        uint32_t ins_30_25 = (ins >> 25) & 63;
        uint32_t ins_24_21 = (ins >> 21) & 15;
        uint32_t ins_7 = (ins >> 7) & 1;
        uint32_t ins_11_8 = (ins >> 8) & 15;
        uint32_t ins_20 = (ins >> 20) & 1;
        uint32_t ins_30_20 = (ins >> 20) & 2047;
        uint32_t ins_19_12 = (ins >> 12) & 255;
        uint32_t ins_31 = (ins >> 31) & 1;

        if (ins_31 == 1){
            imm_i = (4294967295 & ~2047) | (ins_30_25 << 5) | (ins_24_21 << 1) | (ins_20);
            imm_s = (4294967295 & ~2047) | (ins_30_25 << 5) | (ins_11_8 << 1) | (ins_7);
            imm_b = (4294967295 & ~4095) | (ins_30_25 << 5) | (ins_11_8 << 1) | (ins_7);
            imm_u = (ins_31 << 31) | (ins_30_20 << 20) | (ins_19_12 << 12);
            imm_j = (4294967295 & ~1048525) | (ins_20 << 11) | (ins_30_25 << 5) | (ins_24_21 << 1);
        }else{
            imm_i = (ins_30_25 << 5) | (ins_24_21 << 1) | (ins_20);
            imm_s = (ins_30_25 << 5) | (ins_11_8 << 1) | (ins_7);
            imm_b = (ins_30_25 << 5) | (ins_11_8 << 1) | (ins_7);
            imm_u = (ins_30_20 << 20) | (ins_19_12 << 12);
            imm_j = (ins_20 << 11) | (ins_30_25 << 5) | (ins_24_21 << 1);
        }

        //sign extend immediates to 64 bits
        uint64_t imm_i_64 = (int64_t) (int)imm_i;
        uint64_t imm_s_64 = (int64_t) (int)imm_s;
        uint64_t imm_b_64 = (int64_t) (int)imm_b;
        uint64_t imm_u_64 = (int64_t) (int)imm_u;
        uint64_t imm_j_64 = (int64_t) (int)imm_j;

        //Decode opcode
        switch (opcode){
            case r_type:
                is_r = true;
                break;
            case s_type:
                is_s = true;
                break;
            case lui:
                is_lui = true;
                break;
            case auipc:
                is_auipc = true;
                break;
            case i_type:
                is_i = true;
                break;
            case i_type_load:
                is_i_load = true;
                break;
            case b_type:
                is_b = true;
                break;
            case jalr:
                is_jalr = true;
                break;
            case jal:
                is_jal = true;
                break;
            case rops32:
                is_r32 = true;
                break;
            case iops32:
                is_i32 = true;
                break;

        }

        //execute stage
        if (is_r){
            if(fun3==0){
                if (fun7==0){
                    //ADD
                    regfile[rd] = regfile[rs1] + regfile[rs2];
                    pc += 4;
                }else{
                    //SUB
                    regfile[rd] = regfile[rs1] - regfile[rs2];
                }
            }else if(fun3==1){
                //SLL
                regfile[rd] = regfile[rs1] << (regfile[rs2] & 63);
                pc += 4;
            }else if(fun3==2){
                //SLT
                regfile[rd] = ((int64_t)regfile[rs1]) < ((int64_t)regfile[rs2]);
                pc +=4;
            }else if(fun3==3){
                //SLTU
                regfile[rd] = regfile[rs1] < regfile[rs2];
                pc += 4;
            }else if(fun3==4){
                //XOR
                regfile[rd] = regfile[rs1] ^ regfile[rs2];
                pc += 4;
            }else if(fun3==5){
                if(fun7==0){
                    //SRL
                    regfile[rd] = regfile[rs1] >> (regfile[rs2] & 63);
                    pc += 4;
                }else{
                    //SRA
                    regfile[rd] = ((int64_t)regfile[rs1]) >> (regfile[rs2] & 63);
                    pc += 4;
                }
            }

        }


    }

};

#endif // RISCV_CORE_H
