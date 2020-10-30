#pragma once

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <iomanip>
#include <map>
using namespace std;

// Define Table Structs xx
class lbl_sym
{
public:
    string _name;
    int _offset = 0;

    lbl_sym()  //default constructor
    {
        _offset = 0;
    }

    //add new lbl_sym to label or symbol table
    lbl_sym(string name, int offset, vector<lbl_sym>& table)
    {
        _name = name;
        _offset = offset;
        table.push_back(*this);
    }
};

class instruction
{
public:
    string _name;
    string _binary;

    instruction() {};  //default constructor

    //add new instruction to instruction table
    instruction(string name, string binary, vector<instruction>& table)
    {
        _name = name;
        _binary = binary;
        table.push_back(*this);
    }
};

class binHex
{
public:
    string _binary;
    string _hex;

    binHex() {};  //default constructor

    //add new binHex to vector
    binHex(string binary, string hex, vector<binHex>& vct)
    {
        _binary = binary;
        _hex = hex;
        vct.push_back(*this);
    }
};

//Function Declarations
void buildLookupTables();
string lookUpInst(string, vector<instruction>&);
int lookUpLblSym(string, vector<lbl_sym>&);
void writeBinaryHex(vector<string>);
string convertBinToHex(string);
char cvtNibble2Hex(string);
//string convertNibbleToHex(string);
string removeSpaces(string);
void writeListingFile(vector<string>, vector<string>, vector<lbl_sym>, vector<lbl_sym>);


//Lookup Tables (vectors)
vector<lbl_sym> symbols;            //symbol table, R0-R15 and user defined variables
vector<lbl_sym> labels;             //label table, instructions wrapped in ()
vector<instruction> computations;   //HACK Computation Codes
vector<instruction> destinations;   //HACK Destination Codes
vector<instruction> jumps;          //HACK Jump Codes
//vector<binHex> binToHex;            //Binary nibble to HEX Digit
map<string, char> bin2Hex;          //Replacement for binToHex table

//Tables used for processing
vector<string> instructions;        //initial raw instructions, straight from file
vector<string> machineCode;         //processed binary instructions, to be written to file

int lineCount = 0;                  //count of valid instruction lines, no lables or blank lines are counted
int symCount = 16;                  //0-15 are reserved for R0-R15, 16 and beyond are for new user variables
string filename;

int main(int argc, char*argv[])
{
    char* test = argv[1];

    buildLookupTables();            //Build all the lookup tables

     //list lexicon
    if (argc > 1)
    {
        if (*argv[1] == 'L') //set first command line arg to 'L'
            //Properties\Debugging\Command Arguments
        {
            //Demo: Display Lexicon(computations)
            for (instruction inst : destinations)
            {
                cout << "Computations: \t" << inst._name << "\t" << inst._binary << endl;
            }
            //Demo: Display Lexicon(destinations)
            for (instruction inst : destinations)
            {
                cout << "Destinations: \t" << inst._name << "\t" << inst._binary << endl;
            }
            //Demo: Display Lexicon(jumps)
            for (instruction inst : jumps)
            {
                cout << "Jumps: \t\t" << inst._name << "\t" << inst._binary << endl;
            }

            //Demo: How to iterate through a MAP structure using iterator
            for (auto it = bin2Hex.cbegin(); it != bin2Hex.cend(); ++it)
            {
                std::cout << "Bin2Hex: \t" << it->first << "\t" << it->second << "\n";
            }
            //Demo: How to iterate through a MAP structure using forEach
            //for (auto elem : bin2Hex)
            //{
            //    std::cout << elem.first << " " << elem.second << "\n";
            //}
        }
    }


#pragma region Pass 1
    //Read lines from file, save labels with their addresses to lables table, 
    //instructions to instruction table, ignore white space, discard comments

    string inStr, firstChar;
    cout << "Enter input file name. Must be .asm file in C:\\mydata\ : ";
    cin >> filename;
    ifstream inFile("C:\\mydata\\" + filename + ".asm");


    cout << "Enter output file name (.bin, lis & .hex): ";
    cin >> filename;

    while (std::getline(inFile, inStr, '\r'))
    {
        //find comments in line and remove them
        int commentLoc = inStr.find("//");      //does line contain a comment
        if (commentLoc != string::npos)         //this line contains a comment
        {
            inStr = inStr.substr(0, commentLoc); //get text from loc 0 to commend loc
            inStr = removeSpaces(inStr);         //remove any left over spaces
        }
        
        //if line is zero length after removing comments or an empty line in source
        if (inStr.length() == 0)                
        {
            continue; //breaks out of while loop and gets next line from file
        }

        //TROUBLESHOOTING: cout << "Inst: " << inStr << " Line: " << lineCount << endl;

        firstChar = inStr.substr(0, 1);
        if (firstChar == "(" && inStr.find(")") != string::npos)     // is a LABEL instruction
        {
            string noParens = inStr.substr(1, inStr.length() - 2);   //strip the parans ()
            for (char& c : noParens) c = toupper(c);            //convert to uppercase
            lbl_sym lbl(noParens, lineCount, labels);           //add to labels vector
        }
        else  //an actual instruction, not a label             
        {
            //need to add code here to do a more robust validation of instruction
           instructions.push_back(inStr);                 //add to instructions vector
           lineCount++;
        }
    }
    inFile.close();
#pragma endregion

#pragma region Pass 2

    lineCount = 0;

    //Second pass, process all lines in instructions table, 
    //lines that begin with @ are A-Instructions,
    //lines that begin with D, M, A are C-Instructions

    for (string inst : instructions)
    {
        string firstChar = inst.substr(0, 1);                   //get first char of string
        string remainder = inst.substr(1, inst.length()-1);     //get remainder of string 
        for (char& c : remainder) c = toupper(c);               //convert to uppercase

        if (firstChar == "@")   //A Instruction
        {
            //search symbols and labels table for pre-existing symbol/label, return offset
            int offsetSym = 0;
            int offsetLbl = 0;
            try
            {
                offsetSym = lookUpLblSym(remainder, symbols);
            }
            catch (string e)
            {
                lbl_sym symbol(remainder, symCount, symbols);   //add previously unknown symbol to table
                symCount++;

                try
                {
                    offsetLbl = lookUpLblSym(remainder, labels);
                }
                catch (string e)
                {
                    
                    cout << e << endl;
                }
            }

            if ( offsetSym >= 0 || offsetLbl >= 0)  //if it exist in either table
            {
                char sym[32] = { 0 };               //buffer for binary string (_itoa_s)
                if (offsetSym >= 0)                 //is it a symbol
                {
                    _itoa_s(offsetSym, sym, 2);     //convert symbol offset to binary
                }
                else                                //is it a label
                {
                    _itoa_s(offsetLbl, sym, 2);     //convert label offset to binary
                }

                std::string newStr(sym);            //convert char[], sym to std::string
                //pad binary string with leading zeros
                std::string hexString = string(16 - newStr.length(), '0') + newStr;

                //save to machineCode vector
                machineCode.push_back(hexString);
            }
            else  //NOT found in symbol/label table
            {
                //create new symbol (variable) at next available location in symbol table
                lbl_sym lbl(remainder, symCount++, symbols);
            }
        }
        else    //C Instruction, first char is NOT '@'
        {
            //C-Instructions have 4 parts:
            //  1. Prefix, bits 15-12, bit 12 indicates if it is a 'Memory' related instruction
            //  2. Computation, bits 11-6 (see HACK instruction table)
            //  3. Destination, bits 5-3 (see HACK instruction table)
            //  4. Jump, bits 2-0 (see HACK instruction table)

            string prefix, leftSide, rightSide, jumpSide, destination, computation, jump;

            int eqPos = inst.find("=");                             //get position of '=' sign
            int scPos = inst.find(";");                             //get position of ';' 

            //C-Instructions have 4 possible formats:
            //  1. Has both an equal sign (=) and a semi-colon (;)
            //  2. Has NO equal sign, but does have a semi-colon
            //  3. Has an equal sign, but has NO semi-colon
            //  4. Has neither an equal sign nor a semi-colon

            //Format 1:
            if (eqPos != string::npos && scPos != string::npos)     //inst has both '=' & ';'
            {
                leftSide = inst.substr(0, eqPos);                   //get chars to left of '='
                rightSide = inst.substr(eqPos+1, scPos-eqPos-1);    //get chars between '=' and ';'
                jumpSide = inst.substr(scPos + 1, inst.length() - (scPos + 1));//get chars to the right of ';'               

                try
                {
                    destination = lookUpInst(leftSide, destinations);   //lookup destination code in table
                    computation = lookUpInst(rightSide, computations);  //lookup computation code in table
                    jump = lookUpInst(jumpSide, jumps);                 //lookup jump code in table
                }
                catch (string e)
                {
                    cout << e;
                }
            }
             //Format 2:
            else if (eqPos == string::npos && scPos != string::npos) //inst has NO '=' but has ';'
            {
                leftSide = inst.substr(0, scPos);                   //get left side of string (left of ';' sign)
                destination = "000";
                jumpSide = inst.substr(scPos + 1, inst.length() - (scPos + 1));//get chars to the right of ';'
                try
                {
                    computation = lookUpInst(leftSide, computations);   //lookup destination code in table               
                    jump = lookUpInst(jumpSide, jumps);                 //lookup jump code in table
                }
                catch (string e)
                {
                    cout << e;
                }
            }
            //Format 3:
            else if (eqPos != string::npos && scPos == string::npos) //inst has '=' but NO ';'
            {
                leftSide = inst.substr(0, eqPos);                    //get left side of string (left of '=' sign)
                rightSide = inst.substr(eqPos + 1, inst.length() - (eqPos + 1)); //get chars to right of '='
                jump = "000";

               try
                {
                    destination = lookUpInst(leftSide, destinations);    //lookup destination code in table
                    computation = lookUpInst(rightSide, computations);   //lookup computation code in table
                }
                catch (string e)
                {
                    cout << "Bad Source Code at Line: " << to_string(lineCount);
                }
            }
            //Format 4:
            else    //inst has neither '=' or ';'
            {
                //Should never get here, should build exception handler to handle possible bad commands
                //This loop is processing only C-Instructions which should all have some combination of '=' and ';'
            }

            //Get Instruction Type, first 4 bits, set prefix (Determine if 'a' bit (bit 12) should be set)
            if (rightSide.find("M") != string::npos)
                prefix = "1001";    //C-Instruction, a-bit(12) ON (Memory), computation (source) involves Memory
            else
                prefix = "1000";    //C-Instruction, a-bit(12) OFF (No Memory), computation (source) does not involve Memory

            //push binary to machineCode vector
            machineCode.push_back(prefix + computation + destination + jump);

            ////Testing purposes ONLY
            //cout << "Inst: " << inst;
            //cout << "\tBinary:  " << prefix + computation + destination + jump << endl;
        }
   }
#pragma endregion

    //write to both a binary and hex version of the instructions
    writeBinaryHex(machineCode);
}

//***FUNCTION DEFINITIONS ***

void buildLookupTables() //HACK Lexicon
{
    //Symbols Table R0-R15, user symbols(variables) added during Pass 2
    for (int i = 0; i < 16; i++)
    {
        lbl_sym symbol("R" + to_string(i), i, symbols);
    }

    //These tables are all CONSTANT data from the HACK instruction set. 
    //Used here as lookup tables for equating Assembler Text instructions 
    //to their binary equivalent

    //Computation Codes Table
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

        //a bit ON (Memory is part of Computation field of instruction)
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

    //Destination Codes Table
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

    //Jump Codes Table
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

    //Retreive char from Map
    //  char test = bin2Hex.find("0000")->second << '\n';
    //std::map<string, char> bin2Hex;
    bin2Hex["0000"] = '0';
    bin2Hex["0001"] = '1';
    bin2Hex["0010"] = '2';
    bin2Hex["0011"] = '3';
    bin2Hex["0100"] = '4';
    bin2Hex["0101"] = '5';
    bin2Hex["0110"] = '6';
    bin2Hex["0111"] = '7';
    bin2Hex["1000"] = '8';
    bin2Hex["1001"] = '9';
    bin2Hex["1010"] = 'A';
    bin2Hex["1011"] = 'B';
    bin2Hex["1100"] = 'C';
    bin2Hex["1101"] = 'D';
    bin2Hex["1110"] = 'E';
    bin2Hex["1111"] = 'F';


    //Used to convert binary code to HEX code
    //Build Binary Nibble to Hex Codes
    //This version is replaced by the MAP above
    //{
    //    binHex bin000("0000", "0", binToHex);
    //    binHex bin001("0001", "1", binToHex);
    //    binHex bin002("0010", "2", binToHex);
    //    binHex bin003("0011", "3", binToHex);
    //    binHex bin004("0100", "4", binToHex);
    //    binHex bin005("0101", "5", binToHex);
    //    binHex bin006("0110", "6", binToHex);
    //    binHex bin007("0111", "7", binToHex);
    //    binHex bin008("1000", "8", binToHex);
    //    binHex bin009("1001", "9", binToHex);
    //    binHex bin00A("1010", "A", binToHex);
    //    binHex bin00B("1011", "B", binToHex);
    //    binHex bin00C("1100", "C", binToHex);
    //    binHex bin00D("1101", "D", binToHex);
    //    binHex bin00E("1110", "E", binToHex);
    //    binHex bin00F("1111", "F", binToHex);
    //}
}

string lookUpInst(string cmd, vector<instruction>& vct)
{
    for (instruction inst : vct)
    {
        if (inst._name == cmd)
            return inst._binary;
    }
    string excep = "Code Not found in Lookup Table--Code: " + cmd + "  Line #: " + to_string(lineCount);
    throw excep;
}

int lookUpLblSym(string str, vector<lbl_sym>& vct)
{
    for (lbl_sym lblSym : vct)
    {
        if (lblSym._name == str)
            return lblSym._offset;
    }
    string excep = "Code not found in Symbol/Lables Lookup Table: " + str + " at line: " + to_string(lineCount);
    throw excep;
}

void writeBinaryHex(vector<string> machineCode)
{
    ofstream outFileBin("C:\\mydata\\" + filename + ".bin");
    ofstream outFileHex("C:\\mydata\\" + filename + ".hex");
    char* cStr;

    //Write header to .hex file for Logisim compatibility
    string header = "v2.0 raw";
    char eol[] = { '\n' };
    char tab[] = { '\t'};
    cStr = const_cast<char*>(header.c_str());
    outFileHex.write(cStr, header.length());
    outFileHex.write(eol, 1);

    for (string line : machineCode)
    {
        //write binary instructions to .bin file and console
        cStr = const_cast<char*>(line.c_str());
        outFileBin.write(cStr, line.length());
        outFileBin.write(eol, 1);
        cout.write(cStr, line.length());
        cout.write(tab, 1);

        //write HEX instructions to .hex file and console
        string hexLine = convertBinToHex(line);
        cStr = const_cast<char*>(hexLine.c_str());
        outFileHex.write(cStr, hexLine.length());
        outFileHex.write(eol, 1);
        cout.write(cStr, hexLine.length());
        cout.write(eol, 1);

    }

    outFileBin.close();
    outFileHex.close();

    writeListingFile(machineCode, instructions, labels, symbols);

    cout << "***********************************" << endl;
    cout << "Wrote " << to_string(machineCode.size()) << " instructions to .bin and .hex files" << endl;
    cout << "***********************************" << endl;

}

//Replaced by cvtNibble2Hex below
//string convertNibbleToHex(string nibble)
//{
//    for (binHex bh : binToHex)
//    {
//        if (bh._binary == nibble)
//            return bh._hex;
//    }
//    return "X";//should never happen, invalid nibble value
//}

char cvtNibble2Hex(string nibble)
{
    return bin2Hex.find(nibble)->second;

}

string convertBinToHex(string binary)
{
    string nibble03 = binary.substr(0, 4); //extract first 4 bits
    string nibble02 = binary.substr(4, 4);
    string nibble01 = binary.substr(8, 4);
    string nibble00 = binary.substr(12, 4);

    //string hex00 = convertNibbleToHex(nibble00);  //convert binary nibble to hex
    //string hex01 = convertNibbleToHex(nibble01);
    //string hex02 = convertNibbleToHex(nibble02);
    //string hex03 = convertNibbleToHex(nibble03);

    char hex00 = cvtNibble2Hex(nibble00);//get Hex digit
    char hex01 = cvtNibble2Hex(nibble01);
    char hex02 = cvtNibble2Hex(nibble02);
    char hex03 = cvtNibble2Hex(nibble03);

    //Return string created from 4 Hex Characters
    return string(1, hex03) + string(1, hex02) + string(1, hex01) + string(1, hex00);
}

void writeListingFile(vector<string> machineCode, vector<string> instructions, vector<lbl_sym> labels, vector<lbl_sym> symbols)
{
    ofstream outFileLis("C:\\mydata\\" + filename + ".lis");

    outFileLis << "*******************************************" << endl;
    outFileLis << "Addr   Binary           HEX     Instruction " << endl;
    outFileLis << "*******************************************" << endl;

    for (unsigned int i = 0; i < machineCode.size(); i++)
    {
            char* cStr;
            char addr[32] = { 0 };               //buffer for binary string (_itoa_s)

            _itoa_s(i, addr, 16);               //convert address to hex
            outFileLis << addr << "\t";         //write address

            outFileLis << machineCode[i] << "\t"; //write binary

            string hexLine = convertBinToHex(machineCode[i]); //write hex instruction
            cStr = const_cast<char*>(hexLine.c_str());
            outFileLis << hexLine << "\t";

            outFileLis << instructions[i] << endl;  //write original instruction
    }

    outFileLis << "**********************************" << endl;
    outFileLis << "         LABELS/Line #            " << endl;
    outFileLis << "**********************************" << endl;
    for (lbl_sym lbl : labels)  //write all lables and addresses
        outFileLis << lbl._name << ":\t\t " << lbl._offset << endl;

    outFileLis << "**********************************" << endl;
    outFileLis << "         SYMBOLS/Offsets          " << endl;
    outFileLis << "**********************************" << endl;
    for (lbl_sym sym : symbols) //write all symbols and addresses
        outFileLis << sym._name << ":\t\t " << sym._offset << endl;

    outFileLis.close();
}

string removeSpaces(string str)
{
    //Removes all spaces from string and returns new string
    string newStr;
    for (unsigned int i = 0; i< str.length(); i++)
    {
        if (str[i] != ' ')
            newStr += str[i];
    }
    return newStr;
}

