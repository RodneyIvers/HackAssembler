#pragma once
#include <string>
#include <map>
using namespace std;

class Computations
{
	map<string, string> computations;

public:
	Computations();
	string find(string);

};

