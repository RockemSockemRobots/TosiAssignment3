/****************************************
Authors: Justin Siekmann, Anthony Ciancio
NetIDs : jsiekmann, shjay
Date : 25 October, 2016
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

int main(int argc, char* argv[]) { //arguments: dpgen(cmd) netlistFile(input) verilogFile(output)	
	if (argc != 3) {
		std::cerr << "Usage: " << argv[0] << " netlistFile verilogFile" << std::endl; //get mad; u mad, bro?
		return -1;
	}
	Netlist netlist1(argv[1]); //pass netlistFile(input) to constructor
	if (netlist1.get_error()) {
		return -1;
	}
	netlist1.critPath();
	netlist1.writeOut(argv[2]);

	return 0;
}