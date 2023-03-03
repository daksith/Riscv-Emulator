#ifndef RISCV_CORE_H
#define RISCV_CORE_H
#include <stdint.h>
#include <stdio.h>
#include <memory.h>
#include "Decode_defs.h"

using namespace std;

class Riscv_core
{
    public:
        uint64_t regfile[32] = {0};
        uint8_t memory[1<<16];
        uint64_t pc = 0;
        bool ecall=false;

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
            imm_b = (~4095) | (ins_7 << 11) | (ins_30_25 << 5) | (ins_11_8 << 1);
            imm_u = (ins_31 << 31) | (ins_30_20 << 20) | (ins_19_12 << 12);
            imm_j = (4294967295 & ~1048525) | (ins_20 << 11) | (ins_30_25 << 5) | (ins_24_21 << 1);
        }else{
            imm_i = (ins_30_25 << 5) | (ins_24_21 << 1) | (ins_20);
            imm_s = (ins_30_25 << 5) | (ins_11_8 << 1) | (ins_7);
            imm_b = (ins_7 << 11) | (ins_30_25 << 5) | (ins_11_8 << 1);
            imm_u = (ins_30_20 << 20) | (ins_19_12 << 12);
            imm_j = (ins_20 << 11) | (ins_30_25 << 5) | (ins_24_21 << 1);
        }

        //sign extend immediates to 64 bits
        uint64_t imm_i_64 = (int64_t) (int32_t)imm_i;
        uint64_t imm_s_64 = (int64_t) (int32_t)imm_s;
        uint64_t imm_b_64 = (int64_t) (int32_t)imm_b;
        uint64_t imm_u_64 = (int64_t) (int32_t)imm_u;
        uint64_t imm_j_64 = (int64_t) (int32_t)imm_j;

        //fence
        if (opcode == 0b0001111 || opcode == 0b1110011){
            if(fun3==0 && imm_i ==0){
            ecall=true;
            }
            pc+=4;
            return;
        }

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
                    pc += 4;
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
            }else if(fun3==6){
                //OR
                regfile[rd] = regfile[rs1] | regfile[rs2];
                pc += 4;
            }else if(fun3==7){
                //AND
                regfile[rd] = regfile[rs1] & regfile[rs2];
                pc += 4;
            }

        }else if(is_i){
            if(fun3 == 0){
                //ADDI
                regfile[rd] = regfile[rs1] + imm_i_64;
                pc += 4;
            }else if(fun3 == 2){
                //SLTI
                regfile[rd] = ((int64_t)regfile[rs1]) < ((int64_t)imm_i_64);
                pc += 4;
            }else if(fun3==3){
                //SLTIU
                regfile[rd] = regfile[rs1] < imm_i_64;
                pc += 4;
            }else if (fun3==4){
                //XORI
                regfile[rd] = regfile[rs1] ^ imm_i_64;
                pc += 4;
            }else if (fun3==6){
                //ORI
                regfile[rd] = regfile[rs1] | imm_i_64;
                pc += 4;
            }else if (fun3==7){
                //XORI
                regfile[rd] = regfile[rs1] & imm_i_64;
                pc += 4;
            }else if (fun3==1){
                //SLLI
                regfile[rd] = regfile[rs1] << (imm_i_64 & 63);
                pc += 4;
            }else if (fun3==5){
                if (ins < (1<<30)){
                    //SRLI
                    regfile[rd] = regfile[rs1] >> (imm_i_64 & 63);
                    pc += 4;
                }else{
                    //SRAI
                    regfile[rd] = ((int64_t)regfile[rs1]) >> (imm_i_64 & 63);
                    pc += 4;
                }
            }

        }else if (is_i_load){
            if (fun3==0){
                //LB
                regfile[rd] = (int64_t) ((int8_t) memory[regfile[rs1] + imm_i_64]);
                pc += 4;
            }else if (fun3==1){
                //LH
                uint16_t* t = (uint16_t*) &memory[regfile[rs1] + imm_i_64];
                int16_t t1 = *t;
                regfile[rd] = (int64_t) t1;
//                if (pc == 0x1a4){
//                    cout<<(int64_t)regfile[rd]<<endl;
//                }
                pc += 4;
            }else if (fun3==2){
                //LW
                uint32_t* t = (uint32_t*) &memory[regfile[rs1] + imm_i_64];
                int32_t t1 = *t;
                regfile[rd] = (int64_t) t1;
                pc += 4;
            }else if (fun3==4){
                //LBU
                regfile[rd] = memory[regfile[rs1] + imm_i_64];
                pc += 4;
            }else if (fun3==5){
                //LHU
                uint16_t* t = (uint16_t*) &memory[regfile[rs1] + imm_i_64];
                uint16_t t1 = *t;
                regfile[rd] = (uint64_t) t1;
                pc += 4;
            }else if (fun3==3){
                //LD
                uint64_t* t = (uint64_t*) &memory[regfile[rs1] + imm_i_64];
                regfile[rd] = *t;
                pc += 4;
            }else if (fun3==6){
                //LWU
                uint32_t* t = (uint32_t*) &memory[regfile[rs1] + imm_i_64];
                uint32_t t1 = *t;
                regfile[rd] = (uint64_t) t1;
                pc += 4;
            }
        }else if(is_s){
            if (fun3==0){
                //SB
                memory[regfile[rs1] + imm_s_64] = regfile[rs2];
                pc += 4;
            }else if (fun3==1){
                //SH
                memcpy(&memory[regfile[rs1] + imm_s_64],&regfile[rs2],2);
                pc += 4;
            }else if (fun3==2){
                //SW
                memcpy(&memory[regfile[rs1] + imm_s_64],&regfile[rs2],4);
                pc += 4;
            }else if (fun3 == 3){
                //SD
                memcpy(&memory[regfile[rs1] + imm_s_64],&regfile[rs2],8);
                pc += 4;
            }
        }else if (is_b){
            if (fun3==0){
                //BEQ
                if(regfile[rs1]==regfile[rs2]){
                    pc = pc + imm_b_64;
                }else{
                    pc += 4;
                }
            }else if (fun3==1){
                //BNE
                if(regfile[rs1]!=regfile[rs2]){
                    pc = pc + imm_b_64;
                }else{
                    pc += 4;
                }
            }else if (fun3==4){
                //BLT
                if( ((int64_t)regfile[rs1]) < ((int64_t)regfile[rs2])){
                    pc = pc + imm_b_64;
                }else{
                    pc += 4;
                }
            }else if (fun3==5){
                //BGE
                if( ((int64_t)regfile[rs1]) >= ((int64_t)regfile[rs2])){
                    pc = pc + imm_b_64;
                }else{
                    pc += 4;
                }
            }else if (fun3==6){
                //BLTU
                if(regfile[rs1] < regfile[rs2]){
                    pc = pc + imm_b_64;
                }else{
                    pc += 4;
                }
            }else if (fun3==7){
                //BGEU
                if(regfile[rs1] >= regfile[rs2]){
                    pc = pc + imm_b_64;
                }else{
                    pc += 4;
                }
            }
        }else if (is_lui){
            //LUI
            regfile[rd] = imm_u_64;
            pc += 4;
        }else if (is_auipc){
            //AUIPC
            regfile[rd] = pc + imm_u_64;
            pc += 4;
        }else if(is_jal){
            //JAL
            regfile[rd] = pc + 4;
            pc = pc + imm_j_64;
        }else if (is_jalr){
            //JALR
            uint64_t temp = pc;
            pc = (regfile[rs1] + imm_i_64) & -2;
            regfile[rd] = temp + 4;
        }else if (is_r32){
            if (fun3==0){
                if (fun7==0){
                    //ADDW
                    regfile[rd] = (int64_t) (int32_t) ( (int32_t)regfile[rs1] + (int32_t)regfile[rs2] );
                    pc += 4;
                }else{
                    //SUBW
                    regfile[rd] = (int64_t) (int32_t) ( (int32_t)regfile[rs1] - (int32_t)regfile[rs2] );
                    pc += 4;
                }

            }else if (fun3==1){
                //SLLW
                regfile[rd] = (int64_t) (int32_t) ((uint32_t)regfile[rs1] << (uint32_t)(regfile[rs2] & 31));
                pc += 4;
            }else if (fun3==5){
                if (fun7==0){
                    //SRLW
                    regfile[rd] = (int64_t) (int32_t) ((uint32_t)regfile[rs1] >> (uint32_t)(regfile[rs2] & 31));
                    pc += 4;
                }else{
                    //SRAW
                    regfile[rd] = (int64_t) (int32_t) ((int32_t)regfile[rs1] >> (uint32_t)(regfile[rs2] & 31));
                    pc += 4;
                }
            }
        }else if (is_i32){
            if (fun3==0){
                //ADDIW
                regfile[rd] = (int64_t) (int32_t) ( (int32_t)regfile[rs1] + (int32_t)imm_i );
                pc += 4;
            }else if(fun3==1){
                //SLLIW
                regfile[rd] = (int64_t) (int32_t) ((uint32_t)regfile[rs1] <<  (imm_i & 31));
                pc += 4;
            }else if (fun3==5){
                if (ins < (1<<30)){
                    //SRLIW
                    regfile[rd] = (int64_t) (int32_t) ((uint32_t)regfile[rs1] >> (imm_i & 31));
                    pc += 4;
                }else {
                    //SRAIW
                    regfile[rd] = (int64_t) (int32_t) ((int32_t)regfile[rs1] >> (imm_i & 31));
                    pc += 4;
                }
            }
        }
        //hardwire x0 to zero
        regfile[0] = 0;

    }


};

#endif // RISCV_CORE_H
