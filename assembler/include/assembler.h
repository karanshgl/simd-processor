/*Copyright (C) 2016, IIT Ropar
 * This file is part of SimpleRISC assembler.
 *
 * SimpleRISC assembler is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SimpleRISC assembler is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Neeraj Goel (email: neeraj@iitrpr.ac.in)
 */

#ifndef __ASSEMBLER_H__
#define __ASSEMBLER_H__

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <stdlib.h>
#include <iomanip>
#include <stdint.h>

//Remove this comment if you are using 64 bit system
//#define BITS64

#ifdef BITS64
typedef uint32_t uint32;
#else
typedef unsigned int uint32;
#endif

#define DATA_SEG 0x500
enum Opcode {
    ADD,    //0
    SUB,    //1
    MUL,    //2
    DIV,    //3
    MOD,    //4
    CMP,    //5
    AND,    //6
    OR,     //7
    NOT,    //8
    MOV,    //9
    LSL,    //10
    LSR,    //11
    ASR,    //12
    NOP,    //13
    LD,     //14
    ST,     //15
    BEQ,    //16
    BGT,    //17
    B,      //18
    CALL,   //19
    RET,    //20
    VMOV1,  //21
    VMOV2,  //22
    VADD,   //23
    VSUB,   //24
    VMUL,   //25
    VDIV,   //26
    VMOD,   //27
    VAND,   //28
    VLD,    //29
    VST,    //30
    END
};

struct Instruction {
    uint32 pc;
    Opcode opcode;
    uint32 encoding;
    std::string operands;
    bool modU;
    bool modH;
    std::string origInst;
    uint32 lineNum;
    Instruction() {
        pc = encoding = 0;
    }
};

struct Data {
    uint32 address;
    std::vector<int> data; //array of values
    Data(uint32 add) {
        address = add;
    }
};

class Assembler {
        public:
            Assembler(char *input, char *output);
        private:
            void assemblerPass1(char *);
            void assemblerPass2(char *);
            void removeInitialWhiteSpace(std::string &);
            void removeDosChar(std::string &);
            void removeWhiteSpace(std::string &);
            bool isComment(std::string &);
            bool isEmpty(std::string &);
            void initOpcodes();
            void readLabel(std::string &input, std::string &label) ;
            void readMemoryOffset(std::string &input, int &offset);
            void readRegisterOperand(std::string &input, int &regIndex, bool &imm, char &reg_vec);
            void lowerCase(std::string &input);
            uint32 generateEncoding(Opcode opcode);
            uint32 generateEncoding(Opcode opcode, int offset);
            uint32 generateEncoding(Opcode opcode, int reg1, int reg2, int imm);
            uint32 generateEncoding(Opcode opcode, int reg1, int src2, bool imm);
            uint32 generateEncoding(Opcode opcode, int reg1, int src1, int src2, bool imm);
            bool readInstruction(std::string &line, Instruction* inst);
            bool isNoOperand(Opcode opcode);
            bool isSingleOperand(Opcode opcode);
            bool isMemoryOperand(Opcode opcode);
            bool isTwoOperand(Opcode opcode);
            void isValid(Opcode opcode, char reg_vec, int opNum);
            int readNumber(std::string &);

        private:
            std::map<std::string, uint32> mSymTab;
            std::vector<Instruction*> mInstList;
            std::vector<Data*> mDataList;
            std::map<std::string, Opcode> opcodes;
};

#endif
