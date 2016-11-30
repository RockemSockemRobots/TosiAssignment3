/****************************************
Authors: Justin Siekmann, Anthony Ciancio
NetIDs : jsiekmann, shjay
Date : 29 November 2016
Assignment #3

Netlist.cpp
*****************************************/
/*
	Force calculation:
	******************

	// 1. populate probs_ matrices for addsub, mult, div, log
			- column 1 stores the vertex number
	// 2. populate dist_ ""
			- ""
	// 3. populate selfForces matrix
			- if there is a 0 in the probs_ matrix for that slot, set to 0
			- else, calculate selfForce = dist1*(1/0 - probs)
	// 4. populate totalForces matrix
			- pred force = self force of immediate above node(s) that are forced into a time slot
			- succ force = self force of immediate below node(s) that are forced into a time slot
			- total force = self + pred + succ
	// 5. choose the lowest totalForce value
	// 6. overwrite timeASAP and timeALAP of that node with scheduled time; scheduleFlag = 1
	// 7. Check scheduleFlags of every node. Repeat 1-6 until every scheduleFlag == 1

*/

/*	NOTES
	*****
	- change output vector of a logic piece to just be one output
	- Logic
		- timeASAP
		- timeALAP
		- inputConnectors (vector)
		- outputConnectors (vector)
		- change the delay (from table to standard on logic type)
		- logicDependency
	- Connector
		- timeASAP
		- timeALAP
	- State (object/vector)
		- name
		- logics (vector)
		- input transitions (vector)
		- output transitions (vector)
	- Transition (object/vector)
		- name
		- startState
		- endState
		- condition
	- stateRegister
	- output Verilog code as states
	- algorithm to optimize
		- unscheduled graph
		- ASAP calculation
		- ALAP calculation
		- force directed algorithm
	- error for latency incompatability
*/
#include "Netlist.h"

Netlist::Netlist(std::string inputFile, std::string latency) {
	this->numAddSub = 0;
	this->numMult = 0;
	this->numDivMod = 0;
	this->numLog = 0;
	this->latency = stoi(latency);
	this->error = Netlist::readIn(inputFile);
}

bool Netlist::readIn(std::string inputFile) {
	static std::vector<std::string> validComponents = { "+","-","*",">","<","==",">>","<<","/","%","?" };
	int ADDs = 0; int SUBs = 0; int MULs = 0; int COMPs = 0; int MUX2x1s = 0; int SHRs = 0; int SHLs = 0; int DIVs = 0; int MODs = 0; int INCs = 0; int DECs = 0;
	std::ifstream inFile;
	std::string line;
	std::string type_word;
	std::string sign_size_word;
	std::string name_word;
	std::string word2;
	std::string word3;
	std::string word4;
	std::string word5;
	std::string word6;
	std::string word7;
	std::vector<Connector*> tempInputs;
	std::vector<Connector*> tempOutputs;
	int tempsize;
	int column;
	
	inFile.open(inputFile);
	
	//insert empty file logic

	while (std::getline(inFile,line)) {
		line.erase(std::remove(line.begin(), line.end(), ','), line.end());
		std::stringstream line_stream(line);
		line_stream >> type_word;
		if (line != "") {
			if (type_word == "input" || type_word == "output" || type_word == "variable") {
				line_stream >> sign_size_word;
				std::size_t pos = sign_size_word.find_first_of("t");
				std::string sign_word = sign_size_word.substr(0, (pos + 1));
				bool sign = sign_word.find('U');
				std::string size_word = sign_size_word.substr((pos + 1), sign_size_word.length());
				while (line_stream >> name_word) {
					if (type_word == "input") {
						this->inputs.push_back(new Connector(type_word, sign, size_word, name_word, 1, -1)); //do we need delay?
					}
					else if (type_word == "output") {
						this->outputs.push_back(new Connector(type_word, sign, size_word, name_word, (this->latency + 1), this->latency));
					}
					else if (type_word == "variable") {
						this->variables.push_back(new Connector(type_word, sign, size_word, name_word, (this->latency + 1), -1));
					}
				}
				type_word = ""; sign_size_word = ""; name_word = ""; pos = 0; sign_word = ""; size_word = "";
			}

			// if statements

			// for loops
			
			else {
				//already have word1 as type_word
				line_stream >> word2;
				line_stream >> word3;
				line_stream >> word4;
				line_stream >> word5;
				line_stream >> word6;
				line_stream >> word7;
				std::vector<std::string>::iterator it = find(validComponents.begin(), validComponents.end(), word4);
				if (word2 != "=") {
					std::cerr << "Something went wrong: Check netlist";
					return true;
				}
				else if (it != validComponents.end()) {//if in validComponents
					int pos = it - validComponents.begin();
					switch (pos) {
					case 0:
						if (word5 != "1") {
							tempInputs = createCompInputs(word3, word5);
							tempOutputs = createCompOutputs(word4, type_word);
							if (tempOutputs.size() == 0 || tempInputs.size() == 0) {
								return true;
							}
							this->logics.push_back(new Logic("ADD", "add" + std::to_string(ADDs), tempInputs, tempOutputs, false, 1, "", -1, -1, "+"));
							ADDs++;
							this->numAddSub++;
						}
						else {
							tempInputs = createCompInputs(word3);
							tempOutputs = createCompOutputs(word4, type_word);
							if (tempOutputs.size() == 0 || tempInputs.size() == 0) {
								return true;
							}
							this->logics.push_back(new Logic("INC", "inc" + std::to_string(INCs), tempInputs, tempOutputs, false, 1, "", -1, -1, "+"));
							INCs++;
							this->numAddSub++;
						}
						break;
					case 1:
						if (word5 != "1") {
							tempInputs = createCompInputs(word3, word5);
							tempOutputs = createCompOutputs(word4, type_word);
							if (tempOutputs.size() == 0 || tempInputs.size() == 0) {
								return true;
							}
							this->logics.push_back(new Logic("SUB", "sub" + std::to_string(SUBs), tempInputs, tempOutputs, false, 1, "", -1, -1, "-"));
							SUBs++;
							this->numAddSub++;
						}
						else {
							tempInputs = createCompInputs(word3);
							tempOutputs = createCompOutputs(word4, type_word);
							if (tempOutputs.size() == 0 || tempInputs.size() == 0) {
								return true;
							}
							this->logics.push_back(new Logic("DEC", "dec" + std::to_string(DECs), tempInputs, tempOutputs, false, 1, "", -1, -1, "-"));
							DECs++;
							this->numAddSub++;
						}
						break;
					case 2:
						tempInputs = createCompInputs(word3, word5);
						tempOutputs = createCompOutputs(word4, type_word);
						if (tempOutputs.size() == 0 || tempInputs.size() == 0) {
							return true;
						}
						this->logics.push_back(new Logic("MUL", "mul" + std::to_string(MULs), tempInputs, tempOutputs, false, 2, "", -1, -1, "*"));
						MULs++;
						this->numMult++;
						break;
					case 3:
						tempInputs = createCompInputs(word3, word5);
						tempOutputs = createCompOutputs(word4, type_word);
						if (tempOutputs.size() == 0 || tempInputs.size() == 0) {
							return true;
						}
						this->logics.push_back(new Logic("COMP", "comp" + std::to_string(COMPs), tempInputs, tempOutputs, checkSign(tempInputs, tempOutputs), 1, "gt", -1, -1, ">"));
						COMPs++;
						this->numLog++;
						break;
					case 4:
						tempInputs = createCompInputs(word3, word5);
						tempOutputs = createCompOutputs(word4, type_word);
						if (tempOutputs.size() == 0 || tempInputs.size() == 0) {
							return true;
						}
						this->logics.push_back(new Logic("COMP", "comp" + std::to_string(COMPs), tempInputs, tempOutputs, checkSign(tempInputs, tempOutputs), 1, "lt", -1, -1, "<"));
						COMPs++;
						this->numLog++;
						break;
					case 5:
						tempInputs = createCompInputs(word3, word5);
						tempOutputs = createCompOutputs(word4, type_word);
						if (tempOutputs.size() == 0 || tempInputs.size() == 0) {
							return true;
						}
						this->logics.push_back(new Logic("COMP", "comp" + std::to_string(COMPs), tempInputs, tempOutputs, checkSign(tempInputs, tempOutputs), 1, "eq", -1, -1, "=="));
						COMPs++;
						this->numLog++;
						break;
					case 6:
						tempInputs = createCompInputs(word3, word5);
						tempOutputs = createCompOutputs(word4, type_word);
						if (tempOutputs.size() == 0 || tempInputs.size() == 0) {
							return true;
						}
						this->logics.push_back(new Logic("SHR", "shr" + std::to_string(SHRs), tempInputs, tempOutputs, checkSign(tempInputs, tempOutputs), 1, "", -1, -1, ">>"));
						SHRs++;
						this->numLog++;
						break;
					case 7:
						tempInputs = createCompInputs(word3, word5);
						tempOutputs = createCompOutputs(word4, type_word);
						if (tempOutputs.size() == 0 || tempInputs.size() == 0) {
							return true;
						}
						this->logics.push_back(new Logic("SHL", "shl" + std::to_string(SHLs), tempInputs, tempOutputs, false, 1, "", -1, -1, "<<"));
						SHLs++;
						this->numLog++;
						break;
					case 8:
						tempInputs = createCompInputs(word3, word5);
						tempOutputs = createCompOutputs(word4, type_word);
						if (tempOutputs.size() == 0 || tempInputs.size() == 0) {
							return true;
						}
						this->logics.push_back(new Logic("DIV", "div" + std::to_string(DIVs), tempInputs, tempOutputs, checkSign(tempInputs, tempOutputs), 3, "", -1, -1, "/"));
						DIVs++;
						this->numDivMod++;
						break;
					case 9:
						tempInputs = createCompInputs(word3, word5);
						tempOutputs = createCompOutputs(word4, type_word);
						if (tempOutputs.size() == 0 || tempInputs.size() == 0) {
							return true;
						}
						this->logics.push_back(new Logic("MOD", "mod" + std::to_string(MODs), tempInputs, tempOutputs, checkSign(tempInputs, tempOutputs), 3, "", -1, -1, "%"));
						MODs++;
						this->numDivMod++;
						break;
					case 10:
						if (word6 == ":") {
							tempInputs = createCompInputs(word5, word7, word3);
							tempOutputs = createCompOutputs(word4, type_word);
							if (tempOutputs.size() == 0 || tempInputs.size() == 0) {
								return true;
							}
							this->logics.push_back(new Logic("MUX2x1", "mux" + std::to_string(MUX2x1s), tempInputs, tempOutputs, false, 1, "", -1, -1, "?"));
							MUX2x1s++;
							this->numLog++;
						}
						else {
							std::cerr << "Incorrect Operator: " << word6 << std::endl;
							return true;
						}
						break;
					}
				}
				else {
					std::cerr << "Incorrect Operator: " << word4 << std::endl;
					return true;
				}
			}
			type_word = ""; word2 = ""; word3 = ""; word4 = ""; word5 = ""; word6 = ""; word7 = "";
		}
	}
	inFile.close();
	return false;
}

void Netlist::writeOut(std::string outputFile) {
	std::ofstream outFile;
	outFile.open(outputFile);

	//first line
	outFile << "module HLSM " << "(Clk, Rst, Start, Done, ";
	for (int i = 0; i != (int)this->inputs.size(); i++) {
		outFile << this->inputs.at(i)->get_name() << ", ";
	}
	for (int i = 0; i != (int)this->outputs.size(); i++) {
		outFile << this->outputs.at(i)->get_name();
		if (i != (int)this->outputs.size() - 1) {
			outFile << ", ";
		}
	}
	outFile << ");" << std::endl;
	outFile << "\t" << "input Clk, Rst, Start;" << std::endl;
	outFile << "\t" << "output reg Done;" << std::endl << std::endl;

	//inputs
	std::vector<std::string> inputSizes = getSizes(this->inputs);
	for (int i = 0; i != (int)inputSizes.size(); i++) {
		outFile << "\t" << "input ";
		if (inputSizes.at(i) != "1") {
			outFile << "[" << (stoi(inputSizes.at(i)) - 1) << ":0] ";
		}
		outFile << connectorsOfSize(this->inputs, inputSizes.at(i)) << ";" << std::endl;
	}

	//outputs
	std::vector<std::string> outputSizes = getSizes(this->outputs);
	for (int i = 0; i != (int)outputSizes.size(); i++) {
		outFile << "\t" << "output reg ";
		if (outputSizes.at(i) != "1") {
			outFile << "[" << (stoi(outputSizes.at(i)) - 1) << ":0] ";
		}
		outFile << connectorsOfSize(this->outputs, outputSizes.at(i)) << ";" << std::endl;
	}

	//variables (regs)
	std::vector<std::string> variablesSizes = getSizes(this->variables);
	for (int i = 0; i != (int)variablesSizes.size(); i++) {
		outFile << "\t" << "reg ";
		if (variablesSizes.at(i) != "1") {
			outFile << "[" << (stoi(variablesSizes.at(i)) - 1) << ":0] ";
		}
		outFile << connectorsOfSize(this->variables, variablesSizes.at(i)) << ";" << std::endl;
	}
	outFile << std::endl;

	int stateRegBits = std::ceil(std::log2(this->states.size() + 2)) - 1;
	outFile << "\t" << "reg [" << stateRegBits << ":0] State;" << std::endl;
	outFile << "\t" << "parameter Wait = 0;" << std::endl;
	outFile << "\t" << "parameter Final = " << (this->states.size() + 1) << ";" << std::endl;
	outFile << std::endl;

	outFile << "\t" << "initial begin" << std::endl;
	outFile << "\t\t" << "State <= Wait;" << std::endl;
	for (int i = 0; i < (int)this->variables.size(); i++) {
		outFile << "\t\t" << this->variables.at(i)->get_name() << " <= 0;" << std::endl;
	}
	for (int i = 0; i < (int)this->outputs.size(); i++) {
		outFile << "\t\t" << this->outputs.at(i)->get_name() << " <= 0;" << std::endl;
	}
	outFile << "\t" << "end" << std::endl;
	outFile << std::endl;

	outFile << "\t" << "always @ (posedge Clk) begin" << std::endl;
	outFile << "\t\t" << "if (Rst) begin" << std::endl;
	outFile << "\t\t\t" << "State <= Wait;" << std::endl;
	for (int i = 0; i < (int)this->variables.size(); i++) {
		outFile << "\t\t\t" << this->variables.at(i)->get_name() << " <= 0;" << std::endl;
	}
	for (int i = 0; i < (int)this->outputs.size(); i++) {
		outFile << "\t\t\t" << this->outputs.at(i)->get_name() << " <= 0;" << std::endl;
	}
	outFile << "\t\t" << "end" << std::endl;
	outFile << std::endl;

	outFile << "\t\t" << "else begin" << std::endl;
	outFile << "\t\t\t" << "case (State)" << std::endl;
	outFile << "\t\t\t\t" << "Wait: begin" << std::endl;
	outFile << "\t\t\t\t\t" << "Done <= 0;" << std::endl;
	outFile << "\t\t\t\t\t" << "if (Start) begin" << std::endl;
	outFile << "\t\t\t\t\t\t" << "State <= 1;" << std::endl;
	outFile << "\t\t\t\t\t" << "end" << std::endl;
	outFile << "\t\t\t\t\t" << "else begin" << std::endl;
	outFile << "\t\t\t\t\t\t" << "State <= Wait;" << std::endl;
	outFile << "\t\t\t\t\t" << "end" << std::endl;
	outFile << "\t\t\t\t" << "end" << std::endl;

	for (int i = 0; i < (int)this->states.size(); i++) {
		outFile << "\t\t\t\t" << this->states.at(i)->get_name() << ": begin" << std::endl;
		for (int j = 0; j < (int)this->states.at(i)->get_operations().size(); j++) {
			outFile << "\t\t\t\t\t" << this->states.at(i)->get_operations().at(j)->get_outputs().at(0)->get_name();
			outFile << " <= ";
			if (this->states.at(i)->get_operations().at(j)->get_type() == "MUX2x1") {
				outFile << this->states.at(i)->get_operations().at(j)->get_inputs().at(2)->get_name();
				outFile << " ? ";
				outFile << this->states.at(i)->get_operations().at(j)->get_inputs().at(0)->get_name();
				outFile << " : ";
				outFile << this->states.at(i)->get_operations().at(j)->get_inputs().at(1)->get_name();
				outFile << ";" << std::endl;
			}
			else {
				outFile << this->states.at(i)->get_operations().at(j)->get_inputs().at(0)->get_name();
				outFile << " " << this->states.at(i)->get_operations().at(j)->get_typeSymbol() << " ";
				outFile << this->states.at(i)->get_operations().at(j)->get_inputs().at(1)->get_name();
				outFile << ";" << std::endl;
			}
		}
		if (this->states.at(i)->get_name() == (int)this->states.size()) {
			outFile << "\t\t\t\t\t" << "State <= Final;" << std::endl;
			outFile << "\t\t\t\t" << "end" << std::endl;
		}
		else {
			outFile << "\t\t\t\t\t" << "State <= " << this->states.at(i)->get_name() + 1 << ";" << std::endl;
			outFile << "\t\t\t\t" << "end" << std::endl;
		}
	}

	outFile << "\t\t\t\t" << "Final: begin" << std::endl;
	outFile << "\t\t\t\t\t" << "Done <= 1;" << std::endl;
	outFile << "\t\t\t\t\t" << "State <= Wait;" << std::endl;
	outFile << "\t\t\t\t" << "end" << std::endl;
	
	outFile << "\t\t\t" << "endcase" << std::endl;
	outFile << "\t\t" << "end" << std::endl;
	outFile << "\t" << "end" << std::endl;
	outFile << "endmodule";

	outFile.close();
}

std::vector<std::string> Netlist::getSizes(std::vector<Connector*> vector) {
	std::vector<std::string> result;
	for (int i = 0; i != (int)vector.size(); i++) {
		if (find(result.begin(), result.end(), vector.at(i)->get_size()) == result.end()) {
			result.push_back(vector.at(i)->get_size());
		}
	}
	return result;
}

std::string Netlist::connectorsOfSize(std::vector<Connector*> vector, std::string size) {
	std::string result = "";
	for (int i = 0; i != (int)vector.size(); i++) {
		if (size == vector.at(i)->get_size()) {
			result.append(vector.at(i)->get_name());
			result.append(", ");
		}
	}
	result.pop_back(); result.pop_back();
	return result;
}

Connector* Netlist::findName(std::vector<Connector*> vector, std::string name) {
	for (int i = 0; i != (int)vector.size(); i++) {
		if (vector.at(i)->get_name() == name) {
			return vector.at(i);
		}
	}
	return NULL;
}
bool Netlist::checkSign(std::vector<Connector*> inputs, std::vector<Connector*> outputs) {
	for (int i = 0; i != (int)inputs.size(); i++) {
		if (inputs.at(i)->get_sign() == true) {
			return true;
		}
	}
	for (int i = 0; i != (int)outputs.size(); i++) {
		if (outputs.at(i)->get_sign() == true) {
			return true;
		}
	}
	return false;
}

std::vector<Connector*> Netlist::createCompInputs(std::string input1, std::string input2, std::string input3) {
	std::vector<Connector*> tempVector;
	std::vector<Connector*> errorVector;
	Connector* tempConnector = NULL;

	//input1
	tempConnector = findName(this->inputs, input1);
	if (tempConnector == NULL) {
		tempConnector = findName(this->variables, input1);
	}
	if (tempConnector == NULL) {
		std::cerr << "Missing input or variable" << std::endl;
		return errorVector;
	}
	tempVector.push_back(tempConnector);

	//input2
	if (input2 != "") {
		tempConnector = findName(this->inputs, input2);
		if (tempConnector == NULL) {
			tempConnector = findName(this->variables, input2);
		}
		if (tempConnector == NULL) {
			std::cerr << "Missing input or variable" << std::endl;
			return errorVector;
		}
		tempVector.push_back(tempConnector);
	}

	//input3
	if (input3 != "") {
		tempConnector = findName(this->inputs, input3);
		if (tempConnector == NULL) {
			tempConnector = findName(this->variables, input3);
		}
		if (tempConnector == NULL) {
			std::cerr << "Missing input or variable" << std::endl;
			return errorVector;
		}
		tempVector.push_back(tempConnector);
	}
	return tempVector;
}

std::vector<Connector*> Netlist::createCompOutputs(std::string word4, std::string input1, std::string input2, std::string input3) {
	std::vector<Connector*> tempInputs;
	std::vector<Connector*> tempOutputs;
	std::vector<Connector*> tempVector;
	std::vector<Connector*> errorVector;
	Connector* tempConnector = NULL;

	//input1
	tempConnector = findName(this->variables, input1);
	if (tempConnector == NULL) {
		tempConnector = findName(this->outputs, input1);
		tempVector.push_back(tempConnector);
	}
	else {
		tempVector.push_back(tempConnector);
	}
	if (tempConnector == NULL) {
		std::cerr << "Missing variable or output" << std::endl;
		return errorVector;
	}

	//input2
	if (input2 != "") {
		tempConnector = findName(this->variables, input2);
		if (tempConnector == NULL) {
			tempConnector = findName(this->outputs, input2);
			tempVector.push_back(tempConnector);
		}
		else {
			tempVector.push_back(tempConnector);
		}
		if (tempConnector == NULL) {
			std::cerr << "Missing variable or output" << std::endl;
			return errorVector;
		}
	}

	//input3
	if (input3 != "") {
		tempConnector = findName(this->variables, input3);
		if (tempConnector == NULL) {
			tempConnector = findName(this->outputs, input3);
			tempVector.push_back(tempConnector);
		}
		else {
			tempVector.push_back(tempConnector);
		}
		if (tempConnector == NULL) {
			std::cerr << "Missing variable or output" << std::endl;
			return errorVector;
		}
	}
	return tempVector;
}





// FORCE-DIRECTED SCHEDULING ALGORITHM
// ***********************************
void Netlist::ForceDir() {
	bool forceDirDone = false;
	int count = 0;
	for (int i = 0; i != (int)this->logics.size(); i++) {
		if (this->logics.at(i)->get_type() == "ADD" || this->logics.at(i)->get_type() == "INC" || this->logics.at(i)->get_type() == "SUB" || this->logics.at(i)->get_type() == "DEC") {
			this->addsubs.push_back(this->logics.at(i));
		}
		else if (this->logics.at(i)->get_type() == "MUL") {
			this->mults.push_back(this->logics.at(i));
		}
		else if (this->logics.at(i)->get_type() == "DIV" || this->logics.at(i)->get_type() == "MOD") {
			this->divmods.push_back(this->logics.at(i));
		}
		else if (this->logics.at(i)->get_type() == "COMP" || this->logics.at(i)->get_type() == "SHR" || this->logics.at(i)->get_type() == "SHL" || this->logics.at(i)->get_type() == "MUX2x1") {
			this->logs.push_back(this->logics.at(i));
		}
	}
	//loop here
	while (forceDirDone == false) {
		this->probAddSub = {}; this->probMult = {}; this->probDivMod = {}; this->probLog = {};
		this->distAddSub = {}; this->distMult = {}; this->distDivMod = {}; this->distLog = {};
		this->selfAddSub = {}; this->selfMult = {}; this->selfDivMod = {}; this->selfLog = {};
		this->totsAddSub = {}; this->totsMult = {}; this->totsDivMod = {}; this->totsLog = {};
		for (int i = 0; i != (int)this->logics.size(); i++) {
			this->logics.at(i)->set_schASAP(false);
			this->logics.at(i)->set_schALAP(false);
			this->logics.at(i)->set_selfForces({});
			this->logics.at(i)->set_totsForces({});
			if (this->logics.at(i)->get_schForce() == false) {
				this->logics.at(i)->set_timeASAP(-1);
				this->logics.at(i)->set_timeALAP(-1);
			}
		}
		for (int i = 0; i != (int)this->inputs.size(); i++) {
			this->inputs.at(i)->set_timeASAP(1);
			if (this->inputs.at(i)->get_schForce() == false) {
				this->inputs.at(i)->set_timeALAP(-1);
			}
		}
		for (int i = 0; i != (int)this->variables.size(); i++) {
			if (this->variables.at(i)->get_schForce() == false) {
				this->variables.at(i)->set_timeASAP((this->latency + 1));
				this->variables.at(i)->set_timeALAP(-1);
			}
		}
		for (int i = 0; i != (int)this->outputs.size(); i++) {
			if (this->outputs.at(i)->get_schForce() == false) {
				this->outputs.at(i)->set_timeASAP((this->latency + 1));
			}
			this->outputs.at(i)->set_timeALAP(this->latency);
		}
		if (this->calcTimeASAP() == false) {
			this->error = true;
			return;
		}
		if (this->calcTimeALAP() == false) {
			this->error = true;
			return;
		}
		this->calcProbMatrix();
		this->calcDistMatrix();
		this->calcSelfMatrix();
		this->calcTotsMatrix();
		this->schLowest();
		
		count = 0;
		for (int i = 0; i != (int)this->logics.size(); i++) {
			if (this->logics.at(i)->get_schForce() == true) {
				count++;
			}
		}
		if (count == (int)this->logics.size()) {
			forceDirDone = true;
		}
	}
}

// 1. Calculate timeASAP for each node
bool Netlist::calcTimeASAP() {
	int layer = 1;
	// loop until every layer up to latency is scheduled
	while (layer <= this->latency) {
		// check every logic node
		for (int i = 0; i != (int)this->logics.size(); i++) {
			bool goodgoodInputs = true;
			// check if the node is already scheduled
			if (this->logics.at(i)->get_schForce() == false) {
				if (this->logics.at(i)->get_schASAP() == false) {
					// if not scheduled yet, check if all inputs to that logic are <= layer
					for (int j = 0; j != this->logics.at(i)->get_inputs().size(); j++) {
						if (this->logics.at(i)->get_inputs().at(j)->get_timeASAP() > layer) {
							goodgoodInputs = false;
						}
					}
					if (goodgoodInputs == true) {
						//schedule the node, set the schASAP flag
						this->logics.at(i)->set_timeASAP(layer);
						this->logics.at(i)->set_schASAP(true);
						for (int k = 0; k != (int)this->logics.at(i)->get_outputs().size(); k++) {
							if (this->logics.at(i)->get_outputs().at(k)->get_schForce() == false) {
								this->logics.at(i)->get_outputs().at(k)->set_timeASAP(layer + this->logics.at(i)->get_delay());
							}
						}
					}
				}
			}
		}
		layer++;
	}
	// when done, check that every node has been scheduled
	// else, return error
	for (int i = 0; i != (int)this->logics.size(); i++) {
		if (this->logics.at(i)->get_schASAP() == false && this->logics.at(i)->get_schForce() == false) {
			std::cerr << "Input Latency Insufficient";
			return false;
		}
	}
	return true;
}

// 2. Calculate ALAP for each node
bool Netlist::calcTimeALAP() {
	int layer = this->latency;
	while (layer > 0) {
		for (int i = 0; i != (int)this->logics.size(); i++) {
			if (this->logics.at(i)->get_schForce() == false) {
				bool goodgoodOutputs = true;
				bool goodgoodLayer = false;
				for (int k = 0; k != (int)this->logics.at(i)->get_outputs().size(); k++) {
					if (this->logics.at(i)->get_outputs().at(k)->get_timeALAP() < layer) {
						goodgoodOutputs = false;
					}
					if (this->logics.at(i)->get_outputs().at(k)->get_timeALAP() == layer) {
						goodgoodLayer = true;
					}
				}
				if (goodgoodOutputs == true && goodgoodLayer == true) {
					this->logics.at(i)->set_timeALAP(layer - this->logics.at(i)->get_delay() + 1);
					this->logics.at(i)->set_schALAP(true);
					for (int j = 0; j != (int)this->logics.at(i)->get_inputs().size(); j++) {
						if (this->logics.at(i)->get_inputs().at(j)->get_schForce() == false) {
							this->logics.at(i)->get_inputs().at(j)->set_timeALAP(layer - this->logics.at(i)->get_delay());
						}
					}
				}
			}
		}
		layer--;
	}
	for (int i = 0; i != (int)this->logics.size(); i++) {
		if (this->logics.at(i)->get_schALAP() == false && this->logics.at(i)->get_schForce() == false) {
			std::cerr << "Input Latency Insufficient";
			return false;
		}
	}
	return true;
}

// 3. Populate the resource probability matrix for each resource type

void Netlist::calcProbMatrix() {	
	// add/sub
	int node = 0;
	int layer = 0;
	while (node != (int)this->addsubs.size()){
		layer = 0;
		std::vector<double> tempNodeVect = {};
		while (layer != latency) {
			if ((layer+1) < this->addsubs.at(node)->get_timeASAP() || (layer+1) > this->addsubs.at(node)->get_timeALAP()) {
				tempNodeVect.push_back(0);
			}
			else {
				tempNodeVect.push_back(1 / ((double)this->addsubs.at(node)->get_timeALAP() - (double)this->addsubs.at(node)->get_timeASAP() + 1));
			}
			layer++;
		}
		this->probAddSub.push_back(tempNodeVect);
		node++;
	}
	// mult
	node = 0;
	while (node != (int)this->mults.size()) {
		layer = 0;
		std::vector<double> tempNodeVect = {};
		while (layer != latency) {
			if ((layer + 1) < this->mults.at(node)->get_timeASAP() || (layer + 1) > this->mults.at(node)->get_timeALAP()) {
				tempNodeVect.push_back(0);
			}
			else {
				tempNodeVect.push_back(1 / ((double)this->mults.at(node)->get_timeALAP() - (double)this->mults.at(node)->get_timeASAP() + 1));
			}
			layer++;
		}
		this->probMult.push_back(tempNodeVect);
		node++;
	}
	// div/mod
	node = 0;
	while (node != (int)this->divmods.size()) {
		layer = 0;
		std::vector<double> tempNodeVect = {};
		while (layer != latency) {
			if ((layer + 1) < this->divmods.at(node)->get_timeASAP() || (layer + 1) > this->divmods.at(node)->get_timeALAP()) {
				tempNodeVect.push_back(0);
			}
			else {
				tempNodeVect.push_back(1 / ((double)this->divmods.at(node)->get_timeALAP() - (double)this->divmods.at(node)->get_timeASAP() + 1));
			}
			layer++;
		}
		this->probDivMod.push_back(tempNodeVect);
		node++;
	}
	// logical
	node = 0;
	while (node != (int)this->logs.size()) {
		layer = 0;
		std::vector<double> tempNodeVect = {};
		while (layer != latency) {
			if ((layer + 1) < this->logs.at(node)->get_timeASAP() || (layer + 1) > this->logs.at(node)->get_timeALAP()) {
				tempNodeVect.push_back(0);
			}
			else {
				tempNodeVect.push_back(1 / ((double)this->logs.at(node)->get_timeALAP() - (double)this->logs.at(node)->get_timeASAP() + 1));
			}
			layer++;
		}
		this->probLog.push_back(tempNodeVect);
		node++;
	}
}

// 4. Populate the distribution for each resource
void Netlist::calcDistMatrix() {
	// add/sub
	for (int i = 0; i != latency; i++) {
		double tempProb = 0;
		for (int j = 0; j != (int)this->addsubs.size(); j++) {
			tempProb = tempProb + this->probAddSub.at(j).at(i);
		}
		this->distAddSub.push_back(tempProb);
	}
	// mult
	for (int i = 0; i != latency; i++) {
		double tempProb = 0;
		for (int j = 0; j != (int)this->mults.size(); j++) {
			tempProb = tempProb + this->probMult.at(j).at(i);
		}
		this->distMult.push_back(tempProb);
	}
	// div/mod
	for (int i = 0; i != latency; i++) {
		double tempProb = 0;
		for (int j = 0; j != (int)this->divmods.size(); j++) {
			tempProb = tempProb + this->probDivMod.at(j).at(i);
		}
		this->distDivMod.push_back(tempProb);
	}
	// logical
	for (int i = 0; i != latency; i++) {
		double tempProb = 0;
		for (int j = 0; j != (int)this->logs.size(); j++) {
			tempProb = tempProb + this->probLog.at(j).at(i);
		}
		this->distLog.push_back(tempProb);
	}
}


// 5. Populate the self-forces matrix
void Netlist::calcSelfMatrix() {
	//addsubs
	int node = 0;
	int layer = 0;
	while (node != (int)this->addsubs.size()) {
		layer = 0;
		std::vector<double> tempNodeVect = {};
		while (layer != latency) {
			if ((layer + 1) < this->addsubs.at(node)->get_timeASAP() || (layer + 1) > this->addsubs.at(node)->get_timeALAP()) {
				tempNodeVect.push_back(0);
			}
			else {
				double temp = 0;
					for (int j = 0; j != (int)this->distAddSub.size(); j++) {
						if (j == layer) {
							temp = temp + this->distAddSub.at(j)*(1 - this->probAddSub.at(node).at(j));
						}
						else {
							temp = temp + this->distAddSub.at(j)*(0 - this->probAddSub.at(node).at(j));
						}
					}
				tempNodeVect.push_back(temp);
			}
			layer++;
		}
		this->selfAddSub.push_back(tempNodeVect);
		this->addsubs.at(node)->set_selfForces(tempNodeVect);
		node++;
	}
	//mults
	node = 0;
	while (node != (int)this->mults.size()) {
		layer = 0;
		std::vector<double> tempNodeVect = {};
		while (layer != latency) {
			if ((layer + 1) < this->mults.at(node)->get_timeASAP() || (layer + 1) > this->mults.at(node)->get_timeALAP()) {
				tempNodeVect.push_back(0);
			}
			else {
				double temp = 0;
				for (int j = 0; j != (int)this->distMult.size(); j++) {
					if (j == layer) {
						temp = temp + this->distMult.at(j)*(1 - this->probMult.at(node).at(j));
					}
					else {
						temp = temp + this->distMult.at(j)*(0 - this->probMult.at(node).at(j));
					}
				}
				tempNodeVect.push_back(temp);
			}
			layer++;
		}
		this->selfMult.push_back(tempNodeVect);
		this->mults.at(node)->set_selfForces(tempNodeVect);
		node++;
	}
	//divmods
	node = 0;
	while (node != (int)this->divmods.size()) {
		layer = 0;
		std::vector<double> tempNodeVect = {};
		while (layer != latency) {
			if ((layer + 1) < this->divmods.at(node)->get_timeASAP() || (layer + 1) > this->divmods.at(node)->get_timeALAP()) {
				tempNodeVect.push_back(0);
			}
			else {
				double temp = 0;
				for (int j = 0; j != (int)this->distDivMod.size(); j++) {
					if (j == layer) {
						temp = temp + this->distDivMod.at(j)*(1 - this->probDivMod.at(node).at(j));
					}
					else {
						temp = temp + this->distDivMod.at(j)*(0 - this->probDivMod.at(node).at(j));
					}
				}
				tempNodeVect.push_back(temp);
			}
			layer++;
		}
		this->selfDivMod.push_back(tempNodeVect);
		this->divmods.at(node)->set_selfForces(tempNodeVect);
		node++;
	}
	//logs
	node = 0;
	while (node != (int)this->logs.size()) {
		layer = 0;
		std::vector<double> tempNodeVect = {};
		while (layer != latency) {
			if ((layer + 1) < this->logs.at(node)->get_timeASAP() || (layer + 1) > this->logs.at(node)->get_timeALAP()) {
				tempNodeVect.push_back(0);
			}
			else {
				double temp = 0;
				for (int j = 0; j != (int)this->distLog.size(); j++) {
					if (j == layer) {
						temp = temp + this->distLog.at(j)*(1 - this->probLog.at(node).at(j));
					}
					else {
						temp = temp + this->distLog.at(j)*(0 - this->probLog.at(node).at(j));
					}
				}
				tempNodeVect.push_back(temp);
			}
			layer++;
		}
		this->selfLog.push_back(tempNodeVect);
		this->logs.at(node)->set_selfForces(tempNodeVect);
		node++;
	}
}

// 6. Populate the total forces matrix
/*	- pred force = self force of immediate above node(s) that are forced into a time slot
	- succ force = self force of immediate below node(s) that are forced into a time slot
	- total force = self + pred + succ
*/
void Netlist::calcTotsMatrix() {
	//addsubs
	int node = 0;
	int layer = 0;
	double predForce = 0;
	double succForce = 0;
	int tempTimeASAP = -1;
	int tempTimeALAP = -1;
	while (node != (int)this->addsubs.size()) {
		layer = 0;
		predForce = 0;
		succForce = 0;
		std::vector<double> tempNodeVect = {};
		while (layer != latency) {
			if ((layer + 1) < this->addsubs.at(node)->get_timeASAP() || (layer + 1) > this->addsubs.at(node)->get_timeALAP()) {
				tempNodeVect.push_back(1000);
			}
			else {
				for (int i = 0; i != (int)this->addsubs.at(node)->get_inputs().size(); i++) {
					for (int j = 0; j != (int)this->logics.size(); j++) {
						for (int k = 0; k != (int)this->logics.at(j)->get_outputs().size(); k++) {
							if (this->addsubs.at(node)->get_inputs().at(i) == this->logics.at(j)->get_outputs().at(k)) {
								tempTimeASAP = this->logics.at(j)->get_timeASAP();
								tempTimeALAP = this->logics.at(j)->get_timeALAP();
								if (tempTimeALAP == (layer + 1) - (this->addsubs.at(node)->get_delay() - 1)) {
									tempTimeALAP = (layer + 1) - this->addsubs.at(node)->get_delay();
								}
								if ((tempTimeALAP - tempTimeASAP) == 0) {
									predForce = predForce + this->logics.at(j)->get_selfForces().at(tempTimeASAP - 1);
								}
							}
						}
					}
				}
				for (int i = 0; i != (int)this->addsubs.at(node)->get_outputs().size(); i++) {
					for (int j = 0; j != (int)this->logics.size(); j++) {
						for (int k = 0; k != (int)this->logics.at(j)->get_inputs().size(); k++) {
							if (this->addsubs.at(node)->get_outputs().at(i) == this->logics.at(j)->get_inputs().at(k)) {
								tempTimeASAP = this->logics.at(j)->get_timeASAP();
								tempTimeALAP = this->logics.at(j)->get_timeALAP();
								if (tempTimeASAP <= (layer + 1) + (this->addsubs.at(node)->get_delay() - 1)) {
									tempTimeASAP = (layer + 1) + this->addsubs.at(node)->get_delay();
								}
								if ((tempTimeALAP - tempTimeASAP) == 0) {
									succForce = succForce + this->logics.at(j)->get_selfForces().at(tempTimeALAP - 1);
								}
							}
						}
					}
				}
				tempNodeVect.push_back(this->selfAddSub.at(node).at(layer) + predForce + succForce);
			}
			layer++;
		}
		this->totsAddSub.push_back(tempNodeVect);
		this->addsubs.at(node)->set_totsForces(tempNodeVect);
		node++;
	}
	//mults
	node = 0;
	while (node != (int)this->mults.size()) {
		layer = 0;
		predForce = 0;
		succForce = 0;
		std::vector<double> tempNodeVect = {};
		while (layer != latency) {
			if ((layer + 1) < this->mults.at(node)->get_timeASAP() || (layer + 1) > this->mults.at(node)->get_timeALAP()) {
				tempNodeVect.push_back(1000);
			}
			else {
				for (int i = 0; i != (int)this->mults.at(node)->get_inputs().size(); i++) {
					for (int j = 0; j != (int)this->logics.size(); j++) {
						for (int k = 0; k != (int)this->logics.at(j)->get_outputs().size(); k++) {
							if (this->mults.at(node)->get_inputs().at(i) == this->logics.at(j)->get_outputs().at(k)) {
								tempTimeASAP = this->logics.at(j)->get_timeASAP();
								tempTimeALAP = this->logics.at(j)->get_timeALAP();
								if (tempTimeALAP == (layer + 1) - (this->mults.at(node)->get_delay() - 1)) {
									tempTimeALAP = (layer + 1) - this->mults.at(node)->get_delay();
								}
								if ((tempTimeALAP - tempTimeASAP) == 0) {
									predForce = predForce + this->logics.at(j)->get_selfForces().at(tempTimeASAP - 1);
								}
							}
						}
					}
				}
				for (int i = 0; i != (int)this->mults.at(node)->get_outputs().size(); i++) {
					for (int j = 0; j != (int)this->logics.size(); j++) {
						for (int k = 0; k != (int)this->logics.at(j)->get_inputs().size(); k++) {
							if (this->mults.at(node)->get_outputs().at(i) == this->logics.at(j)->get_inputs().at(k)) {
								tempTimeASAP = this->logics.at(j)->get_timeASAP();
								tempTimeALAP = this->logics.at(j)->get_timeALAP();
								if (tempTimeASAP == (layer + 1) + (this->mults.at(node)->get_delay() - 1)) {
									tempTimeASAP = (layer + 1) + this->mults.at(node)->get_delay();
								}
								if ((tempTimeALAP - tempTimeASAP) == 0) {
									succForce = succForce + this->logics.at(j)->get_selfForces().at(tempTimeALAP - 1);
								}
							}
						}
					}
				}
				tempNodeVect.push_back(this->selfMult.at(node).at(layer) + predForce + succForce);
			}
			layer++;
		}
		this->totsMult.push_back(tempNodeVect);
		this->mults.at(node)->set_totsForces(tempNodeVect);
		node++;
	}
	//divmods
	node = 0;
	while (node != (int)this->divmods.size()) {
		layer = 0;
		predForce = 0;
		succForce = 0;
		std::vector<double> tempNodeVect = {};
		while (layer != latency) {
			if ((layer + 1) < this->divmods.at(node)->get_timeASAP() || (layer + 1) > this->divmods.at(node)->get_timeALAP()) {
				tempNodeVect.push_back(1000);
			}
			else {
				for (int i = 0; i != (int)this->divmods.at(node)->get_inputs().size(); i++) {
					for (int j = 0; j != (int)this->logics.size(); j++) {
						for (int k = 0; k != (int)this->logics.at(j)->get_outputs().size(); k++) {
							if (this->divmods.at(node)->get_inputs().at(i) == this->logics.at(j)->get_outputs().at(k)) {
								tempTimeASAP = this->logics.at(j)->get_timeASAP();
								tempTimeALAP = this->logics.at(j)->get_timeALAP();
								if (tempTimeALAP == (layer + 1) - (this->divmods.at(node)->get_delay() - 1)) {
									tempTimeALAP = (layer + 1) - this->divmods.at(node)->get_delay();
								}
								if ((tempTimeALAP - tempTimeASAP) == 0) {
									predForce = predForce + this->logics.at(j)->get_selfForces().at(tempTimeASAP - 1);
								}
							}
						}
					}
				}
				for (int i = 0; i != (int)this->divmods.at(node)->get_outputs().size(); i++) {
					for (int j = 0; j != (int)this->logics.size(); j++) {
						for (int k = 0; k != (int)this->logics.at(j)->get_inputs().size(); k++) {
							if (this->divmods.at(node)->get_outputs().at(i) == this->logics.at(j)->get_inputs().at(k)) {
								tempTimeASAP = this->logics.at(j)->get_timeASAP();
								tempTimeALAP = this->logics.at(j)->get_timeALAP();
								if (tempTimeASAP == (layer + 1) + (this->divmods.at(node)->get_delay() - 1)) {
									tempTimeASAP = (layer + 1) + this->divmods.at(node)->get_delay();
								}
								if ((tempTimeALAP - tempTimeASAP) == 0) {
									succForce = succForce + this->logics.at(j)->get_selfForces().at(tempTimeALAP - 1);
								}
							}
						}
					}
				}
				tempNodeVect.push_back(this->selfDivMod.at(node).at(layer) + predForce + succForce);
			}
			layer++;
		}
		this->totsDivMod.push_back(tempNodeVect);
		this->divmods.at(node)->set_totsForces(tempNodeVect);
		node++;
	}
	//logs
	node = 0;
	while (node != (int)this->logs.size()) {
		layer = 0;
		predForce = 0;
		succForce = 0;
		std::vector<double> tempNodeVect = {};
		while (layer != latency) {
			if ((layer + 1) < this->logs.at(node)->get_timeASAP() || (layer + 1) > this->logs.at(node)->get_timeALAP()) {
				tempNodeVect.push_back(1000);
			}
			else {
				for (int i = 0; i != (int)this->logs.at(node)->get_inputs().size(); i++) {
					for (int j = 0; j != (int)this->logics.size(); j++) {
						for (int k = 0; k != (int)this->logics.at(j)->get_outputs().size(); k++) {
							if (this->logs.at(node)->get_inputs().at(i) == this->logics.at(j)->get_outputs().at(k)) {
								tempTimeASAP = this->logics.at(j)->get_timeASAP();
								tempTimeALAP = this->logics.at(j)->get_timeALAP();
								if (tempTimeALAP == (layer + 1) - (this->logs.at(node)->get_delay() - 1)) {
									tempTimeALAP = (layer + 1) - this->logs.at(node)->get_delay();
								}
								if ((tempTimeALAP - tempTimeASAP) == 0) {
									predForce = predForce + this->logics.at(j)->get_selfForces().at(tempTimeASAP - 1);
								}
							}
						}
					}
				}
				for (int i = 0; i != (int)this->logs.at(node)->get_outputs().size(); i++) {
					for (int j = 0; j != (int)this->logics.size(); j++) {
						for (int k = 0; k != (int)this->logics.at(j)->get_inputs().size(); k++) {
							if (this->logs.at(node)->get_outputs().at(i) == this->logics.at(j)->get_inputs().at(k)) {
								tempTimeASAP = this->logics.at(j)->get_timeASAP();
								tempTimeALAP = this->logics.at(j)->get_timeALAP();
								if (tempTimeASAP == (layer + 1) + (this->logs.at(node)->get_delay() - 1)) {
									tempTimeASAP = (layer + 1) + this->logs.at(node)->get_delay();
								}
								if ((tempTimeALAP - tempTimeASAP) == 0) {
									succForce = succForce + this->logics.at(j)->get_selfForces().at(tempTimeALAP - 1);
								}
							}
						}
					}
				}
				tempNodeVect.push_back(this->selfLog.at(node).at(layer) + predForce + succForce);
			}
			layer++;
		}
		this->totsLog.push_back(tempNodeVect);
		this->logs.at(node)->set_totsForces(tempNodeVect);
		node++;
	}
	int end = 1;
}



// 7. Choose the lowest total force node and schedule it
void Netlist::schLowest() {
	double tempLowest = 1000;
	int tempLayer = 0;
	Logic* tempNode;
	for (int i = 0; i != (int)this->logics.size(); i++) {
		for (int layer = 0; layer != this->latency; layer++) {
			if (this->logics.at(i)->get_totsForces().at(layer) < tempLowest && this->logics.at(i)->get_schForce() == false) {
				tempLowest = this->logics.at(i)->get_totsForces().at(layer);
				tempLayer = layer;
				tempNode = this->logics.at(i);
			}
		}
	}
	tempNode->set_timeASAP(tempLayer + 1);
	tempNode->set_timeALAP(tempLayer + 1);
	tempNode->set_schForce(true);
	for (int i = 0; i != (int)tempNode->get_outputs().size(); i++) {
		tempNode->get_outputs().at(i)->set_timeASAP(tempLayer + 1 + tempNode->get_delay());
		tempNode->get_outputs().at(i)->set_timeALAP(tempLayer + 1 + tempNode->get_delay());
		tempNode->get_outputs().at(i)->set_schForce(true);
	}
	for (int i = 0; i != (int)tempNode->get_inputs().size(); i++) {
		tempNode->get_inputs().at(i)->set_timeASAP(tempLayer + 1 - tempNode->get_delay());
		tempNode->get_inputs().at(i)->set_timeALAP(tempLayer + 1 - tempNode->get_delay());
		tempNode->get_inputs().at(i)->set_schForce(true);
	}
}


// STATE MACHINE ALGORITHM
// ***********************
// Populate states and transitions based on Force-Directed scheduling

void Netlist::populate() {
	int numTrans = this->latency + 1;
	int numStates = this->latency;
	for (int i = 1; i <= numTrans; i++) {
		this->transitions.push_back(new Transition(i - 1, i, ""));
	}
	std::vector<Logic*> tempOps;
	std::vector<Transition*> tempIns;
	std::vector<Transition*> tempOuts;
	for (int layer = 1; layer <= numStates; layer++) {
		tempOps.clear(); tempIns.clear(); tempOuts.clear();
		for (int i = 0; i < (int)this->logics.size(); i++) {
			if (this->logics.at(i)->get_timeASAP() == layer) {
				tempOps.push_back(this->logics.at(i));
			}
		}
		for (int i = 0; i < (int)this->transitions.size(); i++) {
			if (this->transitions.at(i)->get_dst() == layer) {
				tempIns.push_back(this->transitions.at(i));
			}
			if (this->transitions.at(i)->get_src() == layer) {
				tempOuts.push_back(this->transitions.at(i));
			}
		}
		this->states.push_back(new State(layer, tempIns, tempOuts, tempOps));
	}
}