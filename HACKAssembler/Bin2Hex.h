#pragma once
#include <string>
#include <map>
using namespace std;


class Bin2Hex
{
	map<string, char> bin2Hex;
public:
	Bin2Hex();
	char find(string);
};

