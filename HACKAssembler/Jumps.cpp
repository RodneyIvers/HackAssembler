#include "Jumps.h"
#include <map>
#include <string>
#include <boost/algorithm/string.hpp>

using namespace std;

Jumps::Jumps()
{
	jumps.insert(pair<string, string>("NULL", "000"));
	jumps.insert(pair<string, string>("JGT", "001"));
	jumps.insert(pair<string, string>("JEQ", "010"));
	jumps.insert(pair<string, string>("JGE", "011"));
	jumps.insert(pair<string, string>("JLT", "100"));
	jumps.insert(pair<string, string>("JNE", "101"));
	jumps.insert(pair<string, string>("JLE", "110"));
	jumps.insert(pair<string, string>("JMP", "111"));
}

string Jumps::find(string str)
{
	//return jumps.find(str)->second;
	boost::to_upper(str);

	std::map<string, string>::iterator it;

	it = jumps.find(str);
	if (it != jumps.end())
	{
		return it->second;
	}
	else
	{
		return "JUMP code not found: " + str;
	}
}