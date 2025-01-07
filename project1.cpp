#ifndef __PROJECT1_CPP__
#define __PROJECT1_CPP__

#include "project1.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>
#include <sstream>
#include <fstream>
using namespace std;

void process_rtype_instruction(std::string instruction_type, const std::vector<std::string> &terms, std::ofstream &output_file);
void process_shift_instruction(std::string instruction_type, const std::vector<std::string> &terms, std::ofstream &output_file);
void process_mult_div_instruction(std::string instruction_type, const std::vector<std::string> &terms, std::ofstream &output_file);
void process_move_from_instruction(std::string instruction_type, const std::vector<std::string> &terms, std::ofstream &output_file);
void process_itype_instruction(const std::string instruction_type, const std::vector<std::string> &terms, std::ofstream &output_file);
void process_load_store_instruction(std::string instruction_type, const std::vector<std::string> &terms, std::ofstream &output_file);
void process_jump_instruction(const std::string instruction_type, const std::vector<std::string> &terms, std::ofstream &output_file, const std::unordered_map<std::string, int> &instruction_labels);
void process_branch_instruction(const std::string instruction_type, const std::vector<std::string> &terms, int current_index, std::ofstream &output_file, const std::unordered_map<std::string, int> &instruction_labels);
void process_syscall_instruction(std::ofstream &output_file);

void process_move_instruction(const vector<std::string> &terms, std::ofstream &output_file);
void process_jump_register_instruction(const string &string, const vector<std::string> &terms, std::ofstream &output_file);
void process_la_instruction(const vector<std::string> &terms, const unordered_map<std::string, int> &pairs, std::ofstream &output_file);
void process_li_instruction(const vector<std::string> &terms, std::ofstream &output_file);
void process_lui_instruction(const vector<std::string> &terms, std::ofstream &output_file);

int main(int argc, char *argv[])
{
    if (argc < 4) // Checks that at least 3 arguments are given in command line
    {
        std::cerr << "Expected Usage:\n ./assemble infile1.asm infile2.asm ... infilek.asm staticmem_outfile.bin instructions_outfile.bin\n"
                  << std::endl;
        exit(1);
    }
    // Prepare output files
    std::ofstream inst_outfile, static_outfile;
    static_outfile.open(argv[argc - 2], std::ios::binary);
    inst_outfile.open(argv[argc - 1], std::ios::binary);
    std::vector<std::string> instructions;

    /**
     * Phase 1:
     * Read all instructions, clean them of comments and whitespace DONE
     * TODO: Determine the numbers for all static memory labels
     * (measured in bytes starting at 0)
     * TODO: Determine the line numbers of all instruction line labels
     * (measured in instructions) starting at 0
     */

    std::unordered_map<std::string, int> static_memory_labels; // Stores the byte offset for static memory labels
    std::unordered_map<std::string, int> instruction_labels;   // Stores the instruction number for instruction labels
    std::vector<std::vector<std::string>> static_memory_data;

    int processed_instructions = 0;
    int static_offset = 0;

    // For each input file:
    for (int i = 1; i < argc - 2; i++)
    {
        std::ifstream infile(argv[i]); //  open the input file for reading
        if (!infile)
        { // if file can't be opened, need to let the user know
            std::cerr << "Error: could not open file: " << argv[i] << std::endl;
            exit(1);
        }

        std::string str;
        while (getline(infile, str))
        {                     // Read a line from the file
            str = clean(str); // remove comments, leading and trailing whitespace
            if (str == "")
            { // Ignore empty lines
                continue;
            }

            if (str == ".data")
            {
                std::string static_line;

                while (getline(infile, static_line))
                {
                    static_line = clean(static_line);
                    if (static_line == "")
                    {
                        continue;
                    }
                    if (static_line == ".text")
                    {
                        break;
                    }

                    vector<string> static_components = split(static_line, WHITESPACE + ":");
                    static_memory_data.push_back(static_components);

                    static_memory_labels[static_components[0]] = static_offset;
                    static_offset += (static_components.size() - 2) * 4;
                }
                continue;
            }

            // Ignore other directives
            if (str[0] == '.')
            {
                continue;
            }

            if (str.back() == ':')
            {
                string label = str.substr(0, str.size() - 1);
                instruction_labels[label] = processed_instructions;
                continue;
            }

            // Store the instruction for further processing
            instructions.push_back(str); // TODO This will need to change for labels
            processed_instructions++;
        }
        infile.close();
    }

    /** Phase 2
     * Process all static memory, output to static memory file
     * TODO: All of this
     */


    // Convert static memory directives to binary format and write them to the static memory output file
    for (const vector<string> &memoryDirective : static_memory_data)
    {
        // Iterate over the elements of the memory directive (starting at index 2, after the label and directive type)
        for (int elementIndex = 2; elementIndex < memoryDirective.size(); elementIndex++)
        {
            // If the memory element references an instruction label, retrieve the label's address
            if (instruction_labels.find(memoryDirective[elementIndex]) != instruction_labels.end())
            {
                int label_address = instruction_labels.at(memoryDirective[elementIndex]) * 4; // Convert instruction line number to byte address
                write_binary(label_address, static_outfile);
            }

            else
            {
                // Directly convert the element to an integer and write it to the binary file
                write_binary(stoi(memoryDirective[elementIndex]), static_outfile);
            }
        }
    }

    /** Phase 3
     * Process all instructions, output to instruction memory file
     * TODO: Almost all of this, it only works for adds
     */
    // for (std::string inst : instructions)
    // {
    //     std::vector<std::string> terms = split(inst, WHITESPACE + ",()");
    //     std::string inst_type = terms[0];
    //     if (inst_type == "add")
    //     {
    //         int result = encode_Rtype(0, registers[terms[2]], registers[terms[3]], registers[terms[1]], 0, 32);
    //         write_binary(encode_Rtype(0, registers[terms[2]], registers[terms[3]], registers[terms[1]], 0, 32), inst_outfile);
    //     }
    // }

    // PHASE 3: Process all instructions and output to instruction memory file
    int current_instruction_index = 0; // Tracks the line number of the instruction

    for (const std::string inst : instructions)
    {
        std::vector<std::string> terms = split(inst, WHITESPACE + ",()");
        std::string inst_type = terms[0]; // The first token is the instruction type

        // Process R-type instructions
        if (inst_type == "add" || inst_type == "sub" || inst_type == "slt" ||
            inst_type == "and" || inst_type == "or" || inst_type == "nor" || inst_type == "xor")
        {

            process_rtype_instruction(inst_type, terms, inst_outfile);
        }
        else if (inst_type == "sll" || inst_type == "srl")
        {
            process_shift_instruction(inst_type, terms, inst_outfile);
        }
        else if (inst_type == "mult" || inst_type == "div")
        {
            process_mult_div_instruction(inst_type, terms, inst_outfile);
        }
        else if (inst_type == "mflo" || inst_type == "mfhi")
        {
            process_move_from_instruction(inst_type, terms, inst_outfile);
        }

        // Process I-type instructions
        else if (inst_type == "addi" || inst_type == "andi" || inst_type == "ori" || inst_type == "xori")
        {
            process_itype_instruction(inst_type, terms, inst_outfile);
        }
        else if (inst_type == "lw" || inst_type == "sw")
        {
            process_load_store_instruction(inst_type, terms, inst_outfile);
        }

        // Process branching instructions
        else if (inst_type == "beq" || inst_type == "bne")
        {
            process_branch_instruction(inst_type, terms, current_instruction_index, inst_outfile, instruction_labels);
        }

        // Process jump instructions
        else if (inst_type == "j" || inst_type == "jal")
        {
            process_jump_instruction(inst_type, terms, inst_outfile, instruction_labels);
        }
        else if (inst_type == "jr" || inst_type == "jalr")
        {
            process_jump_register_instruction(inst_type, terms, inst_outfile);
        }

        // instruction handling (e.g., la, li, move)
        else if (inst_type == "la")
        {
            process_la_instruction(terms, static_memory_labels, inst_outfile);
        }
        else if (inst_type == "li")
        {
            process_li_instruction(terms, inst_outfile);
        }
        else if (inst_type == "lui")
        {
            process_lui_instruction(terms, inst_outfile);
        }
        else if (inst_type == "move")
        {
            process_move_instruction(terms, inst_outfile);
        }

        // Special instructions (syscall)
        else if (inst_type == "syscall")
        {
            process_syscall_instruction(inst_outfile);
        }

        // sge rd, rs, rt  # Set $rd to 1 if $rs >= $rt, otherwise set $rd to 0
        // Translated using slt (set less than) and xori (bitwise XOR immediate) instructions
        else if (inst_type == "sge")
        {
            // First, use slt to check if $rs < $rt and set the result in $rd
            write_binary(encode_Rtype(0, registers[terms[2]], registers[terms[3]], registers[terms[1]], 0, 42), inst_outfile); // slt rd, rs, rt
            // Then, invert the result in $rd to get $rs >= $rt
            write_binary(encode_Itype(14, registers[terms[1]], registers[terms[1]], 1), inst_outfile); // xori rd, rd, 1

            // Adjust label positions for any subsequent instructions
            for (auto &position : instruction_labels)
            {
                if (position.second > current_instruction_index)
                {
                    position.second += 1;
                }
            }
            current_instruction_index++; // Increment the instruction index to account for the added xori instruction
        }

        // sgt rd, rs, rt  # Set $rd to 1 if $rs > $rt, otherwise set $rd to 0
        // Translated using slt by swapping the operands of $rs and $rt
        else if (inst_type == "sgt")
        {
            write_binary(encode_Rtype(0, registers[terms[3]], registers[terms[2]], registers[terms[1]], 0, 42), inst_outfile); // slt rd, rt, rs
        }

        // sle rd, rs, rt  # Set $rd to 1 if $rs <= $rt, otherwise set $rd to 0
        // Translated using slt (set less than) and xori (bitwise XOR immediate) instructions
        else if (inst_type == "sle")
        {
            // First, use slt to check if $rs > $rt and set the result in $rd
            write_binary(encode_Rtype(0, registers[terms[3]], registers[terms[2]], registers[terms[1]], 0, 42), inst_outfile); // slt rd, rt, rs
            // Then, invert the result in $rd to get $rs <= $rt
            write_binary(encode_Itype(14, registers[terms[1]], registers[terms[1]], 1), inst_outfile); // xori rd, rd, 1

            // Adjust label positions for any subsequent instructions
            for (auto &position : instruction_labels)
            {
                if (position.second > current_instruction_index)
                {
                    position.second += 1;
                }
            }
            current_instruction_index++; // Increment the instruction index for the added xori instruction
        }

        // seq rd, rs, rt  # Set $rd to 1 if $rs == $rt, otherwise set $rd to 0
        // Translated using two slt (set less than) and a nor (bitwise NOR) instruction
        else if (inst_type == "seq")
        {
            // First, use slt to check if $rs < $rt and set the result in $rd
            write_binary(encode_Rtype(0, registers[terms[2]], registers[terms[3]], registers[terms[1]], 0, 42), inst_outfile); // slt rd, rs, rt
            // Then, use slt to check if $rt < $rs and store in $at (temporary register)
            write_binary(encode_Rtype(0, registers[terms[3]], registers[terms[2]], 1, 0, 42), inst_outfile); // slt at, rt, rs
            // Finally, perform nor to ensure that both conditions are false, setting $rd to 1 if $rs == $rt
            write_binary(encode_Rtype(0, registers[terms[1]], 1, registers[terms[1]], 0, 39), inst_outfile); // nor rd, rd, at

            // Adjust label positions for any subsequent instructions
            for (auto &position : instruction_labels)
            {
                if (position.second > current_instruction_index)
                {
                    position.second += 2; // Account for the additional slt and nor instructions
                }
            }
            current_instruction_index += 2; // Increment the instruction index for the added slt and nor instructions
        }

        // sne rd, rs, rt  # Set $rd to 1 if $rs != $rt, otherwise set $rd to 0
        // Translated using two slt (set less than) and a xor (bitwise XOR) instruction
        else if (inst_type == "sne")
        {
            // First, use slt to check if $rs < $rt and set the result in $rd
            write_binary(encode_Rtype(0, registers[terms[2]], registers[terms[3]], registers[terms[1]], 0, 42), inst_outfile); // slt rd, rs, rt
            // Then, use slt to check if $rt < $rs and store in $at (temporary register)
            write_binary(encode_Rtype(0, registers[terms[3]], registers[terms[2]], 1, 0, 42), inst_outfile); // slt at, rt, rs
            // Finally, perform xor to ensure the results differ, setting $rd to 1 if $rs != $rt
            write_binary(encode_Rtype(0, registers[terms[1]], 1, registers[terms[1]], 0, 38), inst_outfile); // xor rd, rd, at

            // Adjust label positions for any subsequent instructions
            for (auto &position : instruction_labels)
            {
                if (position.second > current_instruction_index)
                {
                    position.second += 2; // Account for the additional slt and xor instructions
                }
            }
            current_instruction_index += 2; // Increment the instruction index for the added slt and xor instructions
        }

        // bge rs, rt, label  # Branch if $rs >= $rt
        // Translated using slt (set less than), xori (bitwise XOR immediate), and bne (branch if not equal)
        else if (inst_type == "bge")
        {
            // First, use slt to check if $rs < $rt and set the result in $at
            write_binary(encode_Rtype(0, registers[terms[1]], registers[terms[2]], 1, 0, 42), inst_outfile); // slt at, rs, rt
            // Then, invert the result in $at to check if $rs >= $rt
            write_binary(encode_Itype(14, 1, 1, 1), inst_outfile); // xori at, at, 1
            // Finally, branch if $at != 0 (i.e., if $rs >= $rt)
            int offset = (instruction_labels[terms[3]] - current_instruction_index - 2);
            write_binary(encode_Itype(5, 1, 0, offset), inst_outfile); // bne at, $zero, label

            // Adjust label positions for any subsequent instructions
            for (auto &position : instruction_labels)
            {
                if (position.second > current_instruction_index)
                {
                    position.second += 2; // Account for the additional slt and xori instructions
                }
            }
            current_instruction_index += 2; // Increment the instruction index for the added slt and xori instructions
        }

        // ble rs, rt, label  # Branch if $rs <= $rt
        // Translated using slt (set less than), xori (bitwise XOR immediate), and bne (branch if not equal)
        else if (inst_type == "ble")
        {
            // First, use slt to check if $rt < $rs and set the result in $at
            write_binary(encode_Rtype(0, registers[terms[2]], registers[terms[1]], 1, 0, 42), inst_outfile); // slt at, rt, rs
            // Then, invert the result in $at to check if $rs <= $rt
            write_binary(encode_Itype(14, 1, 1, 1), inst_outfile); // xori at, at, 1
            // Finally, branch if $at != 0 (i.e., if $rs <= $rt)
            int offset = (instruction_labels[terms[3]] - current_instruction_index - 2);
            write_binary(encode_Itype(5, 1, 0, offset), inst_outfile); // bne at, $zero, label

            // Adjust label positions for any subsequent instructions
            for (auto &position : instruction_labels)
            {
                if (position.second > current_instruction_index)
                {
                    position.second += 2; // Account for the additional slt and xori instructions
                }
            }
            current_instruction_index += 2; // Increment the instruction index for the added slt and xori instructions
        }

        // bgt rs, rt, label  # Branch if $rs > $rt
        // Translated using slt (set less than) and bne (branch if not equal)
        else if (inst_type == "bgt")
        {
            // First, use slt to check if $rt < $rs and set the result in $at
            write_binary(encode_Rtype(0, registers[terms[2]], registers[terms[1]], 1, 0, 42), inst_outfile); // slt at, rt, rs
            // Finally, branch if $at != 0 (i.e., if $rs > $rt)
            int offset = (instruction_labels[terms[3]] - current_instruction_index - 1);
            write_binary(encode_Itype(5, 1, 0, offset), inst_outfile); // bne at, $zero, label

            current_instruction_index++; // Increment the instruction index for the added slt instruction
        }

        // blt rs, rt, label  # Branch if $rs < $rt
        // Translated using slt (set less than) and bne (branch if not equal)
        else if (inst_type == "blt")
        {
            // First, use slt to check if $rs < $rt and set the result in $at
            write_binary(encode_Rtype(0, registers[terms[1]], registers[terms[2]], 1, 0, 42), inst_outfile); // slt at, rs, rt
            // Finally, branch if $at != 0 (i.e., if $rs < $rt)
            int offset = (instruction_labels[terms[3]] - current_instruction_index - 1);
            write_binary(encode_Itype(5, 1, 0, offset), inst_outfile); // bne at, $zero, label

            current_instruction_index++; // Increment the instruction index for the added slt instruction
        }

        current_instruction_index++; // Move to the next instruction line
    }
}

void process_rtype_instruction(const std::string instruction_type, const std::vector<std::string> &terms, std::ofstream &output_file)
{
    int funct_code;

    if (instruction_type == "add")
        funct_code = 32;
    else if (instruction_type == "sub")
        funct_code = 34;
    else if (instruction_type == "slt")
        funct_code = 42;
    else if (instruction_type == "and")
        funct_code = 36;
    else if (instruction_type == "or")
        funct_code = 37;
    else if (instruction_type == "nor")
        funct_code = 39;
    else if (instruction_type == "xor")
        funct_code = 38;

    int rs = registers[terms[2]];
    int rt = registers[terms[3]];
    int rd = registers[terms[1]];

    write_binary(encode_Rtype(0, rs, rt, rd, 0, funct_code), output_file);
}

void process_itype_instruction(const std::string instruction_type, const std::vector<std::string> &terms, std::ofstream &output_file)
{
    int opcode;

    if (instruction_type == "addi")
        opcode = 8;
    else if (instruction_type == "andi")
        opcode = 12;
    else if (instruction_type == "ori")
        opcode = 13;
    else if (instruction_type == "xori")
        opcode = 14;

    int rs = registers[terms[2]];
    int rt = registers[terms[1]];
    int16_t immediate = static_cast<int16_t>(stoi(terms[3]));

    write_binary(encode_Itype(opcode, rs, rt, static_cast<uint16_t>(immediate)), output_file);
}

void process_shift_instruction(const std::string instruction_type, const std::vector<std::string> &terms, std::ofstream &output_file)
{
    int funct_code = (instruction_type == "sll") ? 0 : 2;

    int rt = registers[terms[2]];
    int rd = registers[terms[1]];
    int shift_amount = stoi(terms[3]);

    write_binary(encode_Rtype(0, 0, rt, rd, shift_amount, funct_code), output_file);
}

void process_load_store_instruction(const std::string instruction_type, const std::vector<std::string> &terms, std::ofstream &output_file)
{
    int opcode = (instruction_type == "lw") ? 35 : 43;

    int base_register = registers[terms[3]];
    int rt = registers[terms[1]];
    int16_t offset = static_cast<int16_t>(stoi(terms[2]));

    write_binary(encode_Itype(opcode, base_register, rt, static_cast<uint16_t>(offset)), output_file);
}

void process_jump_instruction(const std::string instruction_type, const std::vector<std::string> &terms, std::ofstream &output_file,
                              const std::unordered_map<std::string, int> &instruction_labels)
{
    int opcode = (instruction_type == "j") ? 2 : 3;
    int address = instruction_labels.at(terms[1]);

    write_binary(encode_Jtype(opcode, address), output_file);
}

void process_branch_instruction(const std::string instruction_type, const std::vector<std::string> &terms, int current_index,
                                std::ofstream &output_file, const std::unordered_map<std::string, int> &instruction_labels)
{
    int opcode = (instruction_type == "beq") ? 4 : 5;

    int rs = registers[terms[1]];
    int rt = registers[terms[2]];
    int16_t offset = static_cast<int16_t>(instruction_labels.at(terms[3]) - current_index - 1);

    write_binary(encode_Itype(opcode, rs, rt, static_cast<uint16_t>(offset)), output_file);
}

void process_syscall_instruction(std::ofstream &output_file)
{
    write_binary(encode_Rtype(0, 0, 0, 26, 0, 12), output_file);
}

void process_la_instruction(const std::vector<std::string> &terms, const std::unordered_map<std::string, int> &static_memory_labels, std::ofstream &output_file)
{
    int static_address = static_memory_labels.at(terms[2]);                             // Load the static memory address
    write_binary(encode_Itype(8, 0, registers[terms[1]], static_address), output_file); // la is typically an addi with $zero
}

void process_li_instruction(const std::vector<std::string> &terms, std::ofstream &output_file)
{
    int destination_register = registers[terms[1]];
    int immediate_value = stoi(terms[2]);                                                 // Convert immediate value to integer
    write_binary(encode_Itype(8, 0, destination_register, immediate_value), output_file); // li is an addi with $zero
}

// Helper function to process multiplication and division instructions (e.g., mult, div)
void process_mult_div_instruction(const std::string instruction_type, const std::vector<std::string> &terms, std::ofstream &output_file)
{
    int funct_code = (instruction_type == "mult") ? 24 : 26; // funct code for mult is 24, for div is 26

    int rs = registers[terms[1]];
    int rt = registers[terms[2]];

    write_binary(encode_Rtype(0, rs, rt, 0, 0, funct_code), output_file); // Destination is in hi/lo, so rd is 0
}

// Helper function to move from hi/lo registers (e.g., mflo, mfhi)
void process_move_from_instruction(const std::string instruction_type, const std::vector<std::string> &terms, std::ofstream &output_file)
{
    int funct_code = (instruction_type == "mflo") ? 18 : 16; // funct code for mflo is 18, for mfhi is 16

    int rd = registers[terms[1]];

    write_binary(encode_Rtype(0, 0, 0, rd, 0, funct_code), output_file);
}

void process_move_instruction(const std::vector<std::string> &terms, std::ofstream &output_file)
{
    int source_register = registers[terms[2]];
    int destination_register = registers[terms[1]];
    write_binary(encode_Rtype(0, 0, source_register, destination_register, 0, 32), output_file); // move is equivalent to add $rd, $rs, $zero
}

void process_jump_register_instruction(const std::string &instruction_type, const std::vector<std::string> &terms, std::ofstream &output_file)
{
    int funct_code = (instruction_type == "jr") ? 8 : 9; // jr has funct code 8, jalr has funct code 9
    int rs = registers[terms[1]];                        // Register to jump to

    if (instruction_type == "jalr")
    {
        write_binary(encode_Rtype(0, rs, 0, 31, 0, funct_code), output_file); // Save return address in $ra (register 31)
    }
    else
    {
        write_binary(encode_Rtype(0, rs, 0, 0, 0, funct_code), output_file); // No return address
    }
}

void process_lui_instruction(const std::vector<std::string> &terms, std::ofstream &output_file)
{
    int rt = registers[terms[1]];
    int immediate = stoi(terms[2]);
    write_binary(encode_Itype(15, 0, rt, immediate), output_file); // opcode for lui is 15
}

#endif
