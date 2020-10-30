#include "Bin2Hex.h"
#include <boost/algorithm/string.hpp>

Bin2Hex::Bin2Hex()
{
    bin2Hex.insert(pair<string, char>("0000", '0'));
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
}

char Bin2Hex::find(string str)
{
    //return bin2Hex.find(str)->second;
    boost::to_upper(str);

    std::map<string, char>::iterator it;

    it = bin2Hex.find(str);
    if (it != bin2Hex.end())
    {
        return it->second;
    }
    else
    {
        return 'X';//not good error code but can only return char, needs work
    }

}
