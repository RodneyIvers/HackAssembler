#pragma once
#include <string>
#include <map>
using namespace std;

class Destinations
{
private:
	map<string, string> destinations;
public:
	Destinations();
	string find(string);
};

