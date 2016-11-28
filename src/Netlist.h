/****************************************
Authors: Justin Siekmann, Anthony Ciancio
NetIDs : jsiekmann, shjay
Date : 29 November 2016
Assignment #3

Netlist.h
*****************************************/
#ifndef NETLIST_H
#define NETLIST_H

#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

//file inout
#include <iostream>
#include <fstream>

#include "Connector.h"
#include "Logic.h"
#include "Transition.h"
#include "State.h"

class Netlist {
	public:
		//Constructor
		Netlist(std::string inputFile, std::string latency);

		std::vector<Connector*> createCompInputs(std::string input1, std::string input2 = "", std::string input3 = "");
		std::vector<Connector*> createCompOutputs(std::string word4, std::string input1, std::string input2 = "", std::string input3 = "");
		Connector* findName(std::vector<Connector*> vector, std::string name);
		bool checkSign(std::vector<Connector*> inputs, std::vector<Connector*> outputs);
		std::vector<std::string> getSizes(std::vector<Connector*> vector);
		std::string connectorsOfSize(std::vector<Connector*> vector, std::string size);

		//int calcTimeASAP(std::vector<Connector*> vector);

		//Get
		std::vector<Connector*> get_inputs() { return this->inputs; }
		std::vector<Connector*> get_outputs() { return this->outputs; }
		std::vector<Connector*> get_variables() { return this->variables; }
		std::vector<Logic*> get_logics() { return this->logics; }
		std::vector<Transition*> get_transitions() { return this->transitions; }
		std::vector<State*> get_states() { return this->states; }
		bool get_error() { return this->error; }
		//File input
		bool readIn(std::string inputFile);
		//File output
		void writeOut(std::string outputFile);
	private:
		int latency;
		std::vector<Connector*> inputs;
		std::vector<Connector*> outputs;
		std::vector<Connector*> variables;
		std::vector<Logic*> logics;
		std::vector<Transition*> transitions;
		std::vector<State*> states;
		bool error;
};

#endif