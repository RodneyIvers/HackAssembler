#pragma once
#include "Functions.h"
#include <fstream>

string convertNibbleToHex(string nibble)
{
    for (binHex bh : binToHex)
    {
        if (bh._binary == nibble)
            return bh._hex;
    }
    return "X";//should never happen
}

string convertBinToHex(string binary)
{
    string nibble03 = binary.substr(0, 4);
    string nibble02 = binary.substr(4, 4);
    string nibble01 = binary.substr(8, 4);
    string nibble00 = binary.substr(12, 4);

    string hex00 = convertNibbleToHex(nibble00);
    string hex01 = convertNibbleToHex(nibble01);
    string hex02 = convertNibbleToHex(nibble02);
    string hex03 = convertNibbleToHex(nibble03);

    return hex03 + hex02 + hex01 + hex00;
}

void buildTables()
{
    //Build R0-R15 Symbols
    for (int i = 0; i < 16; i++)
    {
        lbl_sym symbol("R" + to_string(i), i, symbols);
    }

    //Build Computation Codes
    {
        //a bit OFF
        instruction inst000("0", "101010", computations);
        instruction inst001("1", "111111", computations);
        instruction inst002("-1", "111010", computations);
        instruction inst003("D", "001100", computations);
        instruction inst004("A", "110000", computations);
        instruction inst005("!D", "001101", computations);
        instruction inst006("!A", "110001", computations);
        instruction inst007("-D", "001111", computations);
        instruction inst008("-A", "110011", computations);
        instruction inst009("D+1", "011111", computations);
        instruction inst010("A+1", "110111", computations);
        instruction inst011("D-1", "001110", computations);
        instruction inst012("A-1", "110010", computations);
        instruction inst013("D+A", "000010", computations);
        instruction inst014("D-A", "010011", computations);
        instruction inst015("A-D", "000111", computations);
        instruction inst016("D&A", "000000", computations);
        instruction inst017("D|A", "010101", computations);

        //a bit ON
        instruction inst018("M", "110000", computations);
        instruction inst019("!M", "110001", computations);
        instruction inst020("-M", "110011", computations);
        instruction inst021("M+1", "110111", computations);
        instruction inst022("M-1", "110010", computations);
        instruction inst023("D+M", "000010", computations);
        instruction inst024("D-M", "010011", computations);
        instruction inst025("M-D", "000111", computations);
        instruction inst026("D&M", "000000", computations);
        instruction inst027("D|M", "010101", computations);
    }

    //Build Destination Codes
    {
        instruction inst000("null", "000", destinations);
        instruction inst001("M", "001", destinations);
        instruction inst002("D", "010", destinations);
        instruction inst003("MD", "011", destinations);
        instruction inst004("A", "100", destinations);
        instruction inst005("AM", "101", destinations);
        instruction inst006("AD", "110", destinations);
        instruction inst007("AMD", "111", destinations);
    }

    //Build Jump Codes
    {
        instruction inst000("null", "000", jumps);
        instruction inst001("JGT", "001", jumps);
        instruction inst002("JEQ", "010", jumps);
        instruction inst003("JGE", "011", jumps);
        instruction inst004("JLT", "100", jumps);
        instruction inst005("JNE", "101", jumps);
        instruction inst006("JLE", "110", jumps);
        instruction inst007("JMP", "111", jumps);
    }

    //Build Binary to Hex Codes
    {
        binHex bin000("0000", "0", binToHex);
        binHex bin001("0001", "1", binToHex);
        binHex bin002("0010", "2", binToHex);
        binHex bin003("0011", "3", binToHex);
        binHex bin004("0100", "4", binToHex);
        binHex bin005("0101", "5", binToHex);
        binHex bin006("0110", "6", binToHex);
        binHex bin007("0111", "7", binToHex);
        binHex bin008("1000", "8", binToHex);
        binHex bin009("1001", "9", binToHex);
        binHex bin00A("1010", "A", binToHex);
        binHex bin00B("1011", "B", binToHex);
        binHex bin00C("1100", "C", binToHex);
        binHex bin00D("1101", "D", binToHex);
        binHex bin00E("1110", "E", binToHex);
        binHex bin00F("1111", "F", binToHex);
    }
}

string lookUpInst(string cmd, vector<instruction>& vct)
{
    for (instruction inst : vct)
    {
        if (inst._name == cmd)
            return inst._binary;
    }
    return "00000000"; //should never happen
}

int lookUpLblSym(string str, vector<lbl_sym>& vct)
{
    for (lbl_sym lblSym : vct)
    {
        if (lblSym._name == str)
            return lblSym._offset;
    }
    return -1;      //return -1 if instruction was not found
}

void writeBinaryHex(vector<string> machineCode)
{
    ofstream outFileBin("C:\\mydata\\maxOut.bin");
    ofstream outFileHex("C:\\mydata\\maxOut.hex");
    for (string line : machineCode)
    {
        char* cStr = const_cast<char*>(line.c_str());
        outFileBin.write(cStr, line.length());
        char eol[] = { '\n' };
        outFileBin.write(eol, 1);

        string hexLine = convertBinToHex(line);
        cStr = const_cast<char*>(hexLine.c_str());
        outFileHex.write(cStr, hexLine.length());
        outFileHex.write(eol, 1);
    }
    outFileBin.close();
    outFileHex.close();
}
