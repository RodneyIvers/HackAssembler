// HACKAssembler.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Jumps.h"
#include "Destinations.h"
#include "Computations.h"
#include "Bin2Hex.h"
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <boost/algorithm/string.hpp>
using namespace std;

int main(int argc, char* argv[])
{
	Jumps jumpTbl;
	Destinations destTbl;
	Computations compTbl;
	Bin2Hex b2hTbl;
	vector<string> binaryInstructions, instructions;
	string inFileName, outFileName, inStr;
	int symbolCount = 16, lineCount = 0;

	//Maps for symbols and labels and iterators
	map<string, int> symbols;
	map<string, int>::iterator itSymbols;
	map<string, int> labels;
	map<string, int>::iterator itLabels;


	//Load R0-R15 into Symbol table
	for (int i = 0; i < 16; i++)
	{
		symbols.insert(pair<string, int>("R" + to_string(i), i));
	}

	//Getting input and output file names (from command line)
	if (argc <= 1)//user provided no file names
	{
		cout << "You must enter two paramaters, input file and output file" << endl;
	}
	else  //get file names from command line arguments
	{
		inFileName = "C:\\mydata\\" + string(argv[1]) + ".asm";
		//outFileName = "C:\\mydata\\" + string(argv[2]) + ".bin";
	}
	ifstream inFile(inFileName); //open the input file



	//*** Begin Pass 1 ***//
	/* This pass removes comments, blank lines, identifies Labels and saves
	   them with the line count */
	while (getline(inFile, inStr, '\r'))
	{
		// Comment Processing
		int commentLoc = inStr.find("//");
		if (commentLoc != string::npos)
		{
			inStr = inStr.substr(0, commentLoc);
			boost::erase_all(inStr, " ");
		}

		// Skip empty lines
		if (inStr.size() == 0)
			continue;

		// Process labels
		if (inStr.find("(") != string::npos && inStr.find(")") != string::npos)
		{
			int leftLoc = inStr.find("(");
			int rightLoc = inStr.find(")");
			string label = inStr.substr(leftLoc + 1, rightLoc - (leftLoc + 1));
			boost::to_upper(label);

			itLabels = labels.find(label);
			if (itLabels == labels.end())
			{
				labels.insert(pair<string, int>(label, lineCount));
			}
		}
		else
		{
			//Save A and C instructions
			instructions.push_back(inStr);
			lineCount++;
		}
	}	//end of while

	//*** End of Pass 1 *** //


	//*** Begin Pass 2 *** //
	/* This pass processes the A and C instructions and coverts them to binary */
	//Process Instructions
	for (string inst : instructions)
	{

		//Start Prep 110320

	}//end of FOR loop
}	//end of program



