#include "Destinations.h"
#include <string>
#include <boost/algorithm/string.hpp>
using namespace std;

Destinations::Destinations()
{
	destinations.insert(pair<string, string>("null", "000"));
	destinations.insert(pair<string, string>("M", "001"));
	destinations.insert(pair<string, string>("D", "010"));
	destinations.insert(pair<string, string>("MD", "011"));
	destinations.insert(pair<string, string>("A", "100"));
	destinations.insert(pair<string, string>("AM", "101"));
	destinations.insert(pair<string, string>("AD", "110"));
	destinations.insert(pair<string, string>("AMD", "111"));
}

string Destinations::find(string str)
{
	//return destinations.find(str)->second;
	boost::to_upper(str);

	std::map<string, string>::iterator it;

	it = destinations.find(str);
	if (it != destinations.end())
	{
		return it->second;
	}
	else
	{
		return "Destination code not found: " + str;
	}

}