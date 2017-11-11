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

#include <assembler.h>

Assembler::Assembler(char *input, char *output) {
    initOpcodes();
    assemblerPass1(input);
    assemblerPass2(output);
}

void Assembler::assemblerPass1(char* filename) {
    //By default first instruction of the input assembly is assume to
    //be at address 0x0
    std::ifstream finput;
    finput.open(filename);
    if(finput.fail()) {
        std::cout << "Invalid input file" << std::endl;
        exit(1);
    }
    std::string line, buff;
    uint32 pc = 0; //pc stores the address for the instruction to be read
    uint32 data_addr = DATA_SEG;
    bool isInstSeg = true, isDataSeg = false;
    uint32 lineNum = 0;
    //pc value get updated after reading the instruction
    while(getline(finput, buff)) {
        line = buff;
        lineNum++;
        removeInitialWhiteSpace(line);
        removeDosChar(line);
        if(isComment(line)) {
            continue;
        }
        if(isEmpty(line)) {
            continue;
        }
        if(line.length() == 0) {
            continue;
        }

        lowerCase(line);
        if(line.find(".text") != std::string::npos) {
            isInstSeg = true;
            isDataSeg = false;
            continue;
        }
        if(line.find(".data") != std::string::npos) {
            std::string label;
            readLabel(line, label);
            if(label == ".data") {
                isInstSeg = false;
                isDataSeg = true;
                continue;
            }
        }
        if(line.find(".end") != std::string::npos) {
            std::string label;
            readLabel(line, label);
            if(label == ".end") {
                Instruction *newInst = new Instruction();
                isInstSeg = false;
                isDataSeg = false;
                mInstList.push_back(newInst);
                newInst->opcode = END;
                newInst->pc = pc;
                continue;
            }
        }
        if(isInstSeg) {
            Instruction *newInst = new Instruction();
            //find if it is a label
            if(line.find(':') != std::string::npos) {
                //There is a label
                std::string label;
                label = line.substr(0, line.find_first_of(':'));
                removeWhiteSpace(label); //remove white space so label is a single word
                mSymTab[label] = pc;

                std::string rest;
                rest = line.substr(line.find_first_of(':') + 1, std::string::npos);
                removeInitialWhiteSpace(rest);
                if(isEmpty(rest) || isComment(rest)) {
                    continue;
                }
                if(readInstruction(rest, newInst)){
                    mInstList.push_back(newInst);
                    newInst->pc = pc;
                    newInst->origInst = line;
                    newInst->lineNum = lineNum;
                    pc = pc + 4;
                    continue;
                }

            }
            if(readInstruction(line, newInst)) {
                mInstList.push_back(newInst);
                newInst->origInst = line;
                newInst->lineNum = lineNum;
                newInst->pc = pc;
                pc = pc + 4;
            } else {
                std::cout << "Wrong format:" << line << std::endl;
            }
        }
        if(isDataSeg) {
            if(line.find(':') != std::string::npos) {
                //there is an label
                std::string label;
                label = line.substr(0, line.find_first_of(':'));
                removeWhiteSpace(label); //remove white space so label is a single word
                lowerCase(label);
                mSymTab[label] = data_addr;
                Data* new_data = new Data(data_addr);
                mDataList.push_back(new_data);
                //check if space argument is given
                std::string rest;
                rest = line.substr(line.find_first_of(':') + 1, std::string::npos);
                removeInitialWhiteSpace(rest);
                if(isEmpty(rest) || isComment(rest)) {
                    continue;
                }
                if(rest.find("space") != std::string::npos) {
                    //allocate data space
                    std::string size = rest.substr(rest.find_first_of(' '));
                    int num = readNumber(size);
                    for(int i=0; i < num; i++) {
                        (mDataList.back())->data.push_back(0);
                        data_addr += 4;
                    }
                    continue;
                }
                (mDataList.back())->data.push_back(readNumber(rest));
                data_addr += 4;
                continue;
            }
            //assuming one data per line, and each data element is an word
            (mDataList.back())->data.push_back(readNumber(line));
            data_addr += 4;

        }
    } //end outer while
    finput.close();
}

bool Assembler::readInstruction(std::string &line, Instruction* inst) {
    std::string opString;
    bool modU = false, modH = false;
    //Finding first space is good for all cases, except ret. In that case,
    //we expect find_first_of will return end of the string, which is fine too.
    opString = line.substr(0, line.find_first_of(' '));
    lowerCase(opString);
    if(opString[opString.length() -1] == 'u') {
        modU = true;
        opString = opString.substr(0, opString.length() -1);
    }
    if(opString[opString.length() -1] == 'h') {
        modH = true;
        opString = opString.substr(0, opString.length() -1);
    }
    if(opcodes.find( opString) != opcodes.end()) {
        inst->opcode = opcodes[opString];
		if (line.find_first_of(' ') == std::string::npos)
        	inst->operands="";
        else
        	inst->operands = line.substr(line.find_first_of(' '));
        inst->modH = modH;
        inst->modU = modU;
        return true;

    } else {
        std::cout << "Error: Opcode: " << opString << " not correct." << std::endl
            << inst->lineNum << ":" << inst->origInst << std::endl;
        return false;
    }

    return false;
}

void Assembler::assemblerPass2(char* output) {
    std::ofstream outfile;
    outfile.open(output);
    if(outfile.fail()) {
        std::cout << "Error in opning output file" << std::endl;
        exit(1);
    }
    uint32 address =0;
    int i, j;

    for(i=0; i < mInstList.size(); i++) {
        Instruction *inst = mInstList[i];
        address = inst->pc;
        if(inst->opcode == END) {
            inst->encoding = 0xF8000000;
        }
        else if(isNoOperand(inst->opcode)) {
            //nop, ret, b
            inst->encoding = generateEncoding(inst->opcode);
        } else if(isSingleOperand(inst->opcode)) {
            //beq, bgt, call
            std::string label;
            readLabel(inst->operands, label);
            if(mSymTab.find(label) == mSymTab.end()) {
                std::cout << "Error: Label "<< label<<" not found in symbol table." << std::endl
                    << inst->lineNum << ":" << inst->origInst << std::endl;
                exit(1);
            }
            uint32 jumpPC = mSymTab[label];
            int offset = ( jumpPC  - inst->pc) >> 2;
            inst->encoding = generateEncoding(inst->opcode, offset);
        } else if(isMemoryOperand(inst->opcode)) {
            //special treatment for load/store because of
            //special parsing needs of memory operands
            //ld, st
            int destReg, src1Reg, offset;
            bool imm;
            readRegisterOperand(inst->operands, destReg, imm);
            readMemoryOffset(inst->operands, offset);
            readRegisterOperand(inst->operands, src1Reg, imm);
            inst->encoding = generateEncoding(inst->opcode, destReg, src1Reg, offset);

        } else if(isTwoOperand(inst->opcode)) {
            //cmp, move
            int destReg, src2;
            bool imm;
            readRegisterOperand(inst->operands, destReg, imm);
            readRegisterOperand(inst->operands, src2, imm);
            inst->encoding = generateEncoding(inst->opcode, destReg, src2, imm);
        } else {
            //default three operands
            int destReg, src1, src2;
            bool imm;
            readRegisterOperand(inst->operands, destReg, imm);
            readRegisterOperand(inst->operands, src1, imm);
            readRegisterOperand(inst->operands, src2, imm);
            inst->encoding = generateEncoding(inst->opcode, destReg, src1, src2, imm);
        }
        if(inst->modH) {
            inst->encoding = inst->encoding | (0x1 << 17);
        }
        if(inst->modU) {
            inst->encoding = inst->encoding | (0x1 << 16);
        }
        outfile << "0x" << std::hex << inst->pc << "  0x" << std::setfill('0') << std::setw(8) << inst->encoding<< std::endl;
    }
    if(mDataList.size() > 0) {
        address += 4;
        while(address != DATA_SEG) {
            outfile << "0x" << std::hex << address << " 0x00000000" << std::endl;
            address += 4;
        }
        for(i=0; i < mDataList.size(); i++) {
            for(j=0; j < mDataList[i]->data.size(); j++) {
                outfile << "0x" << std::hex << address << " 0x" << std::setfill('0') << std::setw(8) << mDataList[i]->data[j] << std::endl;
                address += 4;
            }
        }
    }
    outfile.close();
}

//Ideally we should have taken care of other white spaces also
void Assembler::removeInitialWhiteSpace(std::string &str) {
    if(str.length() == 0) return;
    size_t first = str.find_first_not_of(" \t");
    if(first == std::string::npos) {
       str = "";
       return;
    }
    str = str.substr(first); //return the string from 'first' non space character till the end
    return;
}

//remove DOS style \r in the string
void Assembler::removeDosChar(std::string &input) {
    if( !input.empty() && input[input.size() - 1] == '\r')
        input.erase(input.size() - 1);
}

//remove all the white space found in the string
void Assembler::removeWhiteSpace(std::string &input) {
    std::string output;
    int i;
    for(i=0; i < input.length(); i++) {
        if(input[i] != ' ' && input[i] != '\t')
            output += input[i];
    }
    input = output;
}

// C++ style multi line comments not supported in this assembler
// Any line starting with %, # or @ would be considered comment.
bool Assembler::isComment(std::string &str) {
    if((str[0] == '%') || (str[0] == '#') || (str[0] == '@'))
        return true;
    if(str[0] == '/' && str[1] == '/')
        return true;
    if(str[0] == '/' && str[1] == '*') {
        std::cout << "C++ style multiline commands not supported. Please change your input" << std::endl;
        exit(1);
    }
    return false;
}

bool Assembler::isEmpty(std::string &str) {
    if(str.length() == 0) return true;
}

void Assembler::initOpcodes() {
    opcodes["add"] = ADD; //0
    opcodes["sub"] = SUB; //1
    opcodes["mul"] = MUL; //2
    opcodes["div"] = DIV; //3
    opcodes["mod"] = MOD; //4
    opcodes["cmp"] = CMP; //5
    opcodes["and"] = AND; //6
    opcodes["or"]  = OR;  //7
    opcodes["not"] = NOT; //8
    opcodes["mov"] = MOV; //9
    opcodes["lsl"] = LSL; //10
    opcodes["lsr"] = LSR; //11
    opcodes["asr"] = ASR; //12
    opcodes["nop"] = NOP; //13
    opcodes["ld"]  = LD;  //14
    opcodes["st"]  = ST;  //15
    opcodes["beq"] = BEQ; //16
    opcodes["bgt"] = BGT; //17
    opcodes["b"]   = B;   //18
    opcodes["call"]= CALL;//19
    opcodes["ret"] = RET; //20
    opcodes[".print"] = NOP; //Nullyfy the .print macro for now.
}

bool Assembler::isNoOperand(Opcode opcode) {
    if(opcode == RET || opcode == NOP)
        return true;
    return false;
}

bool Assembler::isSingleOperand(Opcode opcode) {
    if(opcode == B || opcode == BGT || opcode == BEQ || opcode == CALL)
        return true;
    return false;
}

bool Assembler::isMemoryOperand(Opcode opcode) {
    if(opcode == LD || opcode == ST)
        return true;
    return false;
}

bool Assembler::isTwoOperand(Opcode opcode) {
    if(opcode == CMP || opcode == NOT || opcode == MOV)
        return true;
    return false;
}

//input is operands string
//In this case, it will contain just label
void Assembler::readLabel(std::string &input, std::string &label) {
    removeInitialWhiteSpace(input);
    size_t pos = input.find_first_of(" \t");
    if(pos == std::string::npos) {
        label = input;
    } else {
        label = input.substr(0, pos);
    }
    return;
}

//assume the operands as a string
//outputs the register number, or immediate, as the case be
//trims the operand string, by removing the operand given as output
void Assembler::readRegisterOperand(std::string &input, int &regIndex, bool &imm) {
    removeInitialWhiteSpace(input);
    std::string operString = input.substr(0, input.find_first_of(','));
    input = input.substr(input.find_first_of(',') + 1); //input is trimmed
    lowerCase(operString);
    if(operString[0] == 'r' && operString[1] == 'a') {
        regIndex = 15;
        imm = false;
        return;
    } else if(operString[0] == 'r') {
        imm = false;
        if(operString[2] >= '0' && operString[2] <= '5'){
            regIndex = operString[2] - '0' + 10 * (operString[1] - '0');
        } else {
            //single digit operand
            regIndex = operString[1] - '0';
        }
        return;
    } else if(operString[0] == 's' && operString[1] == 'p') {
        regIndex = 14;
        imm = false;
        return;
    } else if(operString[0] == '.') {
        std::string label;
        readLabel(operString, label);
        if(mSymTab.find(label) == mSymTab.end()) {
            std::cout  << "Error: While reading operand " << input <<  " Label " << operString << " not found in symbol table" << std::endl;
            exit(1);
        }
        regIndex = mSymTab[label];
        imm = true;
        return;
    }
    //not a register, so must be an immediate
    regIndex = readNumber(operString);;
    imm = true;
    return;
}

int Assembler::readNumber(std::string &in) {
    int num;
    //Idealy it could be in decimal as well as in hex
    lowerCase(in);
    bool hex = in.find('x') == std::string::npos ? false:true;
    std::stringstream ss(in);
    if(hex) {
        if((ss >> std::hex >>  num).fail()) {
            std::cout << "Error while reading immediate operand:"  << in << std::endl;
            exit(1);
        }
    } else {
        if((ss >> num).fail()) {
            std::cout << "Error while reading immediate operand:" << in  << std::endl;
            exit(1);
        }
    }
    return num;
}

void Assembler::readMemoryOffset(std::string &input, int &offset) {
    removeInitialWhiteSpace(input);
    std::string offsetStr = input.substr(0, input.find_first_of('['));
    removeWhiteSpace(offsetStr);
    if(offsetStr.size() == 0) {
        offset = 0;
    } else {
        std::stringstream ss(offsetStr);
        bool hex = offsetStr.find('x') == std::string::npos ? false:true;
        if(hex) {
            if((ss >> std::hex >> offset).fail()) {
                std::cout << "Error while reading memory offset:" << input  << std::endl;
                exit(1);
            }
        } else {
            if((ss >> offset).fail()) {
                std::cout << "Error while reading memory offset:" << input << std::endl;
                exit(1);
            }
        }
    }
    std::size_t pos1 = input.find_first_of('[');
    std::size_t pos2 = input.find_first_of(']');
    input = input.substr(pos1+1, pos2); //trim the operand string
    return;
}

void Assembler::lowerCase(std::string &input) {
    int i;
    for(i=0; i < input.size(); i++) {
        input[i] = std::tolower(input[i]);
    }
}

//No operand
uint32 Assembler::generateEncoding(Opcode opcode) {
    return (opcode << 27);
}

//single operand, branch
uint32 Assembler::generateEncoding(Opcode opcode, int offset) {
    return (opcode << 27) | offset & 0x7FFFFFF;
}

//ld/st
uint32 Assembler::generateEncoding(Opcode opcode, int reg1, int reg2, int imm) {
    return (opcode << 27) | (reg1 << 22) | (reg2 << 18) | (imm & 0xFFFF) | (1 << 26);
}
//two operands cmp/not/mov
uint32 Assembler::generateEncoding(Opcode opcode, int reg1, int src2, bool imm) {
    if(opcode == CMP)
    	return imm  ? ((opcode << 27) | (reg1 << 18)  | (src2 & 0xFFFF) | (1 << 26)) : ((opcode << 27) |  (reg1 << 18) | (src2 << 14));
    else
    	return imm  ? ((opcode << 27) | (reg1 << 22)  | (src2 & 0xFFFF) | (1 << 26)) : ((opcode << 27) |  (reg1 << 22) | (src2 << 14));

}
uint32 Assembler::generateEncoding(Opcode opcode, int reg1, int src1, int src2, bool imm) {

    return imm  ? ((opcode << 27) | (reg1 << 22)  | (src1 << 18) | (src2 & 0xFFFF) | (1 << 26)) : ((opcode << 27) |  (reg1 << 22) | (src1 << 18) | (src2 << 14));
}
