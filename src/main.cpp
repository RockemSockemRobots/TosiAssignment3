/****************************************
Authors: Justin Siekmann, Anthony Ciancio
NetIDs : jsiekmann, shjay
Date : 29 November 2016
Assignment #2

main.cpp
*****************************************/
//srcs
#include "Connector.h"
#include "Logic.h"
#include "Netlist.h"

//inout
#include <iostream>
#include <fstream>

int main(int argc, char* argv[]) { //arguments: hlsyn(cmd) cFile(input) latency verilogFile(output)	
	if (argc != 4) {
		std::cerr << "Usage: " << argv[0] << " cFile latency verilogFile" << std::endl; //get mad; u mad, bro?
		return -1;
	}
	Netlist netlist1(argv[1], argv[2]); //pass cFile(input) to constructor
	if (netlist1.get_error()) {
		return -1;
	}
	netlist1.calcTimeALAP();
	netlist1.writeOut(argv[3]);

	return 0;
}