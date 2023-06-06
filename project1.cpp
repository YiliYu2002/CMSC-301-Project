#ifndef __PROJECT1_CPP__
#define __PROJECT1_CPP__

#include "project1.h"
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 4) // Checks that at least 3 arguments are given in command line
    {
        std::cerr << "Expected Usage:\n ./assemble infile1.asm infile2.asm ... infilek.asm staticmem_outfile.bin instructions_outfile.bin\n" << std::endl;
        exit(1);
    }
    //Prepare output files
    std::ofstream inst_outfile, static_outfile;
    static_outfile.open(argv[argc - 2], std::ios::binary);
    inst_outfile.open(argv[argc - 1], std::ios::binary);

    std::vector<std::string> instructions;

    /* Label + Indices & Instructions
    labels<std::string, int>: 
    Key(string): LABELNAME
    Value(int): line indices of final instruction output
    */
    std::unordered_map<std::string, int> labels = {};
    //Static Label
    std::unordered_map<int, std::string> static_labels = {};
    // final instruction output(script style) 
    std::vector<std::string> instructs = {};
    // The line indices of final instruction output
    int line_num = 0;
    // static_memory(vector): basically a list of integers
    std::vector<int> static_memory = {};
    std::vector<std::string> static_lines = {};

    /**
     * Phase 1:
     * Rstead all instructions, clean them of comments and whitespace DONE
     * TODO: Determine the numbers for all static memory labels
     * (measured in bytes starting at 0)
     * TODO: Determine the line numbers of all instruction line labels
     * (measured in instructions) starting at 0
    */
    int dot = -1;
    int semi = -1;
    int statEndFlag = 0;
    //For each input file:
    for (int i = 1; i < argc - 2; i++) {
        //cout << "#### FILE ####" << endl;
        std::ifstream infile(argv[i]); //  open the input file for reading
        if (!infile) { // if file can't be opened, need to let the user know
            std::cerr << "Error: could not open file: " << argv[i] << std::endl;
            exit(1);
        }
        std::string str;
        while (getline(infile, str)){ //Read a line from the file
            str = clean(str); // remove comments and whitespace
            dot = str.find(".word");
            semi = str.find(":");
            if (str == "") { //Ignore empty lines
                continue;
            }
            else if (str == ".text" || str == ".data" || str == ".globl main" || str == ".align 2"){
                continue;
            }
            else if (dot < str.length()){
                static_lines.push_back(str);
                //cout << str << endl;
            }
            else if (semi < str.length()){
                if (statEndFlag == 0){
                    statEndFlag = 1;
                    std::string endStat = "__STATICMEMORYEND__";
                    labels[endStat] = line_num;
                }
                std::string s = str.substr(0, semi);
                labels[s] = line_num;
            }
            else{
                if (statEndFlag == 0){
                    statEndFlag = 1;
                    std::string endStat = "__STATICMEMORYEND__";
                    labels[endStat] = line_num;
                }
                instructs.push_back(str);
                line_num++;
            }
        }
        //infile.close();
    }

    int count_of_func = 0;
    for (std::string a: static_lines){
        std::vector<std::string> temp = split(a, WHITESPACE);
        static_labels[count_of_func * 4] = temp[0];
        for (int k = 2; k < temp.size(); k++){
            bool find = false;
            // if the static mem is label, find it in map
            // load it with a '* 4'
            for (auto x : labels){
                if (x.first == temp[k]){
                    static_memory.push_back((x.second) * 4);
                    find = true;
                    break;
                }
            }
            //cout << temp[k] << endl;
            // else load the number(int)
            if (!find){
                static_memory.push_back(std::stoi(temp[k]));
            }
            count_of_func ++;
        }
    }

        
        for (auto x: labels){
            cout << x.first << " " << x.second << endl;
        }
    /*
         //Check static_memory
        for(int i = 0;i < static_memory.size(); i++){
            cout << static_memory[i] << endl;
        }
        
        //Check instructs
        for(int i = 0;i < instructs.size(); i++){
            cout << instructs[i] << endl;
        } 
        //Check static_labels
        for(auto x: static_labels){
            cout << x.first << "---" << x.second << endl;
        }*/
        /*
         //Check static_memory
        for(int i = 0;i < static_memory.size(); i++){
            cout << static_memory[i] << endl;
        }
        
        //Check instructs
        for(int i = 0;i < instructs.size(); i++){
            cout << instructs[i] << endl;
        } 
        //Check static_labels
        for(auto x: static_labels){
            cout << x.first << "---" << x.second << endl;
        }*/

    // Outfile Instruction
    int cur_line_num = 0;
    for(std::string inst : instructs) {
        int sep = inst.find(" ");
        std::string inst_type = inst.substr(0,sep);
        std::string inst_type1 = inst.substr(sep+1);
        int sep1 = inst_type1.find(" ");
        
        if (inst_type == "add" || inst_type == "slt" || inst_type == "sub" || inst_type == "mult" || inst_type == "div" 
        || inst_type == "mflo" || inst_type == "mfhi" || inst_type == "jr") {
            write_binary(process_add(inst.substr(sep+1), inst_type), inst_outfile);
        }
        else if (inst_type == "addi" || inst_type == "sll" || inst_type == "srl") {
            write_binary(process_addi(inst.substr(sep+1), inst_type), inst_outfile);
        }
        else if (inst_type == "lw" || inst_type == "sw"){
            write_binary(process_lw(inst.substr(sep+1), inst_type), inst_outfile);
        }
        else if (inst_type == "beq"|| inst_type == "bne"){
            write_binary(process_eq(inst.substr(sep+1), inst_type, labels, cur_line_num), inst_outfile);
        }
        else if (inst_type == "j"|| inst_type == "jal"){
            write_binary(process_j(inst.substr(sep+1), inst_type, labels), inst_outfile);
        }
        else if (inst_type == "jalr"){
            write_binary(process_jalr(inst.substr(sep+1), inst_type), inst_outfile);
        }
        else if (inst_type == "la"){
            write_binary(process_la(inst.substr(sep+1), inst_type, static_labels), inst_outfile);

        }
        else if (inst_type == "syscall"){
            int t = 53260;
            write_binary(t, inst_outfile);
        }
        cur_line_num ++;  
    }

    // Outfile Static Memory
    for(int a: static_memory){
        int len_0 = 32- to_string(a).length();
        int res = a;
        for (int i = len_0; i < 32; i++){
            res += (0 << i);
        }
        write_binary(res, static_outfile);
    }

    
}

// PROCESS
int process_add(const std::string &inst, std::string um) {
    std::vector<std::string> reg_list = split(inst, WHITESPACE+",");
    if (um == "add"){
        return process_Rtype(0, registers[reg_list[1]], registers[reg_list[2]], registers[reg_list[0]], 0, 32); 
    }
    else if (um == "sub"){
        return process_Rtype(0, registers[reg_list[1]], registers[reg_list[2]], registers[reg_list[0]], 0, 34); 
    }
    else if (um == "mult"){
        return process_Rtype(0, registers[reg_list[0]], registers[reg_list[1]], 0, 0, 24); 
    }
    else if (um == "div"){
        return process_Rtype(0, registers[reg_list[0]], registers[reg_list[1]], 0, 0, 26); 
    }
    else if (um == "mflo"){
        return process_Rtype(0, 0, 0, registers[reg_list[0]], 0, 18); 
    }
    else if (um == "mfhi"){
        return process_Rtype(0, 0, 0, registers[reg_list[0]], 0, 16); 
    }
    else if (um == "slt"){
        return process_Rtype(0, registers[reg_list[1]], registers[reg_list[2]], registers[reg_list[0]], 0, 42);
    }
    else if(um == "jr"){
        return process_Rtype(0, registers[reg_list[0]], 0, 0, 0, 8);
    }
    return 0;    
}

int process_addi(const std::string &inst, std::string um) {
    std::vector<std::string> reg_list = split(inst, WHITESPACE+",");
    if (um == "addi"){
        int imm = stoi(reg_list[2]);
        return process_Itype(8, registers[reg_list[0]], (imm), registers[reg_list[1]]);
    }
    else if (um == "sll"){
        return process_Stype(0, registers[reg_list[0]], stoi(reg_list[2]), registers[reg_list[1]], 0); 
    }
    else if (um == "srl"){
        return process_Stype(0, registers[reg_list[0]], stoi(reg_list[2]), registers[reg_list[1]], 2); 
    }
    return 0;
}

int process_la(const std::string &inst, std::string um, std::unordered_map<int, std::string> static_labels){
    std::vector<std::string> reg_list = split(inst, WHITESPACE+","+"(");
    for(auto x: static_labels){
        int a = x.second.find(":");
        std::string str = x.second.substr(0, a);
        if (str == reg_list[1]){
            return (8 << 26) + (0 << 21) + (registers[reg_list[0]] << 16) + (x.first);
        }
    }
    return 0;
}

int process_lw(const std::string &inst, std::string um){
    std::vector<std::string> reg_list = split(inst, WHITESPACE+","+"(");
    if (um == "lw"){
        return process_Itype(35, registers[reg_list[0]], stoi(reg_list[1]), registers[reg_list[2].substr(0, reg_list[2].find(")"))]);
    }
    else if (um == "sw"){
        return process_Itype(43, registers[reg_list[0]], stoi(reg_list[1]), registers[reg_list[2].substr(0, reg_list[2].find(")"))]);
    }
    return 0;
}

int process_eq(const std::string &inst, std::string um, std::unordered_map<std::string, int> labels, int ln){
    std::vector<std::string> reg_list = split(inst, WHITESPACE+",");
    int num_of_func = labels.find(reg_list[2]) -> second;
    num_of_func = num_of_func - ln - 1;

    if (num_of_func < 0){
        num_of_func = 65535 & num_of_func;
    } 
    if (um == "beq"){
        return process_Etype(4, registers[reg_list[0]], registers[reg_list[1]], num_of_func);
    }
    else if(um == "bne"){
        return process_Etype(5, registers[reg_list[0]], registers[reg_list[1]], num_of_func);
    }
    return 0;
}

int process_j(const std::string &inst, std::string um, std::unordered_map<std::string, int> labels){
    std::vector<std::string> reg_list = split(inst, WHITESPACE+",");
    int num_of_func = (labels.find(reg_list[0]) -> second);
    if (um == "j"){
        return process_Jtype(2, num_of_func);
    }
    else if(um == "jal"){
        return process_Jtype(3, num_of_func);
    }
    return 0;
}

int process_jalr(const std::string &inst, std::string um){
    std::vector<std::string> reg_list = split(inst, WHITESPACE+",");
    if (reg_list.size() > 1){
         return process_Xtype(0, registers[reg_list[0]], registers[reg_list[1]], 9);
    }
    else{
        return process_Xtype(0, registers[reg_list[0]], registers["$ra"], 9);
    }
    return 0;
}


// TYPES
int process_Rtype(int opcode, int rs, int rt, int rd, int shftamt, int funccode) {  
    return (opcode << 26) + (rs << 21) + (rt << 16) + (rd << 11) + (shftamt << 6) + funccode;
}

int process_Itype(int opcode, int rt, int imm, int rs) { //rt = t0, imm = 9, rs = s0
    //return (opcode << 25) + (rs << 20) + (rt << 17) + (imm);
    if (imm < 0){
            int j = 65536;
            imm = j + imm;
        }
    return (opcode << 26) + (rs << 21) + (rt << 16) + (imm);
}

int process_Stype(int opcode, int rt, int imm, int rs, int funccode){
    return (opcode << 26) + (rs << 16) + (rt << 11) + (imm << 6) + funccode;
}

int process_Ltype(int opcode, int rt, int rs, int off){
    return (opcode << 26) + (rs << 16) + (rt << 11) + off;
}

int process_Etype(int opcode, int rt, int rs, int off){
    return (opcode << 26) + (rt << 21) + (rs << 16) + off;
}

int process_Jtype(int opcode, int off){
    return (opcode << 26) + off;
}

int process_Xtype(int opcode, int rs, int rd, int funccode){
    return (opcode << 26) + (rs<<21) + (0<<16) + (rd<<11) + (0<<6) + funccode;
}
#endif