#pragma once
#include <map>
#include <string>
using namespace std;

class Jumps
{
private:
	map<string, string> jumps;

public:
	Jumps(); //default constructor
	string find(string);
};

