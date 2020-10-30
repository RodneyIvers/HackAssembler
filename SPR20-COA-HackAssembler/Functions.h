#pragma once
#include <vector>
#include <string>
using namespace std;

struct lbl_sym
{
    string _name;
    int _offset = 0;

    lbl_sym() 
    {
        _offset = 0; 
    }

    lbl_sym(string name, int offset, vector<lbl_sym>& vct)
    {
        _name = name;
        _offset = offset;
        vct.push_back(*this);
    }
};

struct instruction
{
    string _name;
    string _binary;

    instruction() {};
    instruction(string name, string binary, vector<instruction>& vct)
    {
        _name = name;
        _binary = binary;
        vct.push_back(*this);
    }
};

struct binHex
{
    string _binary;
    string _hex;
    binHex() {};
    binHex(string binary, string hex, vector<binHex>& vct)
    {
        _binary = binary;
        _hex = hex;
        vct.push_back(*this);
    }
};


string outStr;                      //final 16 bit binary instruction, saved in machineCode table

string inStr;                       //raw instruction direct from file

string destination;                 //binary destination code
string computation;                 //binary computation code
string jump;                        //binary jump code

vector<lbl_sym> symbols;            //symbol table, R0-R15 and user defined variables
vector<instruction> computations;   //HACK Computation Codes
vector<instruction> destinations;   //HACK Destination Codes
vector<instruction> jumps;          //HACK Jump Codes
vector<binHex> binToHex;            //Binary nibble to HEX Digit

void buildTables();

void writeBinaryHex(vector<string>);
string convertBinToHex(string);
string convertNibbleToHex(string);
string lookUpInst(string, vector<instruction>&);
int lookUpLblSym(string, vector<lbl_sym>&);



