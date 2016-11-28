/****************************************
Authors: Justin Siekmann, Anthony Ciancio
NetIDs : jsiekmann, shjay
Date : 29 November 2016
Assignment #3

Netlist.cpp
*****************************************/

/*	ALAP calculation:
	*****************
	// note that outputs already have timeALAP
	latency = latencyInput;

	while (latency > 0) {

		for (i=0; i != logic.size(); i++) {
			
			if (logic.at(i)->get_Output()->get_timeALAP() == latency) {
				logic.at(i)->set_timeALAP() = latency;

				for (j=0; j != logic.at(i)->inputs.size(); j++) {
					logic.at(i)->get_Inputs.at(j)->set_timeALAP(latency);
				}
			}
			else {
			}
		}
	latency--;
	}


	- Note: need to produce error if latency constraint is too strict, can't be done




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
						this->outputs.push_back(new Connector(type_word, sign, size_word, name_word, -1, this->latency));
					}
					else if (type_word == "variable") {
						this->variables.push_back(new Connector(type_word, sign, size_word, name_word, -1, -1));
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
				/*else if (word4 == "") { //do we need this??
					tempInputs = createCompInputs(word3, "Clk", "Rst");
					tempOutputs = createCompOutputs(word4, type_word);
					if (tempOutputs.size() == 0) {
						return true;
					}
					tempsize = stoi(tempOutputs.at(0)->get_size());
					column = find(delayColumn.begin(), delayColumn.end(), tempsize) - delayColumn.begin();
					this->REGcomps.push_back(new Logic("REG", "reg_" + tempOutputs.at(0)->get_name(), tempInputs, tempOutputs, tempsize, false, delayArray[0][column], ""));
					REGs++;
				}*/
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
							this->logics.push_back(new Logic("ADD", "add" + std::to_string(ADDs), tempInputs, tempOutputs, false, 1, "", -1, -1));
							ADDs++;
						}
						else {
							tempInputs = createCompInputs(word3);
							tempOutputs = createCompOutputs(word4, type_word);
							if (tempOutputs.size() == 0 || tempInputs.size() == 0) {
								return true;
							}
							this->logics.push_back(new Logic("INC", "inc" + std::to_string(INCs), tempInputs, tempOutputs, false, 1, "", -1, -1));
							INCs++;
						}
						break;
					case 1:
						if (word5 != "1") {
							tempInputs = createCompInputs(word3, word5);
							tempOutputs = createCompOutputs(word4, type_word);
							if (tempOutputs.size() == 0 || tempInputs.size() == 0) {
								return true;
							}
							this->logics.push_back(new Logic("SUB", "sub" + std::to_string(SUBs), tempInputs, tempOutputs, false, 1, "", -1, -1));
							SUBs++;
						}
						else {
							tempInputs = createCompInputs(word3);
							tempOutputs = createCompOutputs(word4, type_word);
							if (tempOutputs.size() == 0 || tempInputs.size() == 0) {
								return true;
							}
							this->logics.push_back(new Logic("DEC", "dec" + std::to_string(DECs), tempInputs, tempOutputs, false, 1, "", -1, -1));
							DECs++;
						}
						break;
					case 2:
						tempInputs = createCompInputs(word3, word5);
						tempOutputs = createCompOutputs(word4, type_word);
						if (tempOutputs.size() == 0 || tempInputs.size() == 0) {
							return true;
						}
						this->logics.push_back(new Logic("MUL", "mul" + std::to_string(MULs), tempInputs, tempOutputs, false, 2, "", -1, -1));
						MULs++;
						break;
					case 3:
						tempInputs = createCompInputs(word3, word5);
						tempOutputs = createCompOutputs(word4, type_word);
						if (tempOutputs.size() == 0 || tempInputs.size() == 0) {
							return true;
						}
						this->logics.push_back(new Logic("COMP", "comp" + std::to_string(COMPs), tempInputs, tempOutputs, checkSign(tempInputs, tempOutputs), 1, "gt", -1, -1));
						COMPs++;
						break;
					case 4:
						tempInputs = createCompInputs(word3, word5);
						tempOutputs = createCompOutputs(word4, type_word);
						if (tempOutputs.size() == 0 || tempInputs.size() == 0) {
							return true;
						}
						this->logics.push_back(new Logic("COMP", "comp" + std::to_string(COMPs), tempInputs, tempOutputs, checkSign(tempInputs, tempOutputs), 1, "lt", -1, -1));
						COMPs++;
						break;
					case 5:
						tempInputs = createCompInputs(word3, word5);
						tempOutputs = createCompOutputs(word4, type_word);
						if (tempOutputs.size() == 0 || tempInputs.size() == 0) {
							return true;
						}
						this->logics.push_back(new Logic("COMP", "comp" + std::to_string(COMPs), tempInputs, tempOutputs, checkSign(tempInputs, tempOutputs), 1, "eq", -1, -1));
						COMPs++;
						break;
					case 6:
						tempInputs = createCompInputs(word3, word5);
						tempOutputs = createCompOutputs(word4, type_word);
						if (tempOutputs.size() == 0 || tempInputs.size() == 0) {
							return true;
						}
						this->logics.push_back(new Logic("SHR", "shr" + std::to_string(SHRs), tempInputs, tempOutputs, checkSign(tempInputs, tempOutputs), 1, "", -1, -1));
						SHRs++;
						break;
					case 7:
						tempInputs = createCompInputs(word3, word5);
						tempOutputs = createCompOutputs(word4, type_word);
						if (tempOutputs.size() == 0 || tempInputs.size() == 0) {
							return true;
						}
						this->logics.push_back(new Logic("SHL", "shl" + std::to_string(SHLs), tempInputs, tempOutputs, false, 1, "", -1, -1));
						SHLs++;
						break;
					case 8:
						tempInputs = createCompInputs(word3, word5);
						tempOutputs = createCompOutputs(word4, type_word);
						if (tempOutputs.size() == 0 || tempInputs.size() == 0) {
							return true;
						}
						this->logics.push_back(new Logic("DIV", "div" + std::to_string(DIVs), tempInputs, tempOutputs, checkSign(tempInputs, tempOutputs), 3, "", -1, -1));
						DIVs++;
						break;
					case 9:
						tempInputs = createCompInputs(word3, word5);
						tempOutputs = createCompOutputs(word4, type_word);
						if (tempOutputs.size() == 0 || tempInputs.size() == 0) {
							return true;
						}
						this->logics.push_back(new Logic("MOD", "mod" + std::to_string(MODs), tempInputs, tempOutputs, checkSign(tempInputs, tempOutputs), 3, "", -1, -1));
						MODs++;
						break;
					case 10:
						if (word6 == ":") {
							tempInputs = createCompInputs(word5, word7, word3);
							tempOutputs = createCompOutputs(word4, type_word);
							if (tempOutputs.size() == 0 || tempInputs.size() == 0) {
								return true;
							}
							this->logics.push_back(new Logic("MUX2x1", "mux" + std::to_string(MUX2x1s), tempInputs, tempOutputs, false, 1, "", -1, -1));
							MUX2x1s++;
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
	outFile << "\tinput Clk, Rst, Start;" << std::endl;
	outFile << "\toutput Done;" << std::endl;

	//inputs
	std::vector<std::string> inputSizes = getSizes(this->inputs);
	for (int i = 0; i != (int)inputSizes.size(); i++) {
		outFile << "\tinput ";
		if (inputSizes.at(i) != "1") {
			outFile << "[" << (stoi(inputSizes.at(i)) - 1) << ":0] ";
		}
		outFile << connectorsOfSize(this->inputs, inputSizes.at(i)) << ";" << std::endl;
	}

	//outputs
	std::vector<std::string> outputSizes = getSizes(this->outputs);
	for (int i = 0; i != (int)outputSizes.size(); i++) {
		outFile << "\toutput reg ";
		if (outputSizes.at(i) != "1") {
			outFile << "[" << (stoi(outputSizes.at(i)) - 1) << ":0] ";
		}
		outFile << connectorsOfSize(this->outputs, outputSizes.at(i)) << ";" << std::endl;
	}
	outFile << std::endl;

	//variables (regs)
	std::vector<std::string> variablesSizes = getSizes(this->variables);
	for (int i = 0; i != (int)variablesSizes.size(); i++) {
		outFile << "\treg ";
		if (variablesSizes.at(i) != "1") {
			outFile << "[" << (stoi(variablesSizes.at(i)) - 1) << ":0] ";
		}
		outFile << connectorsOfSize(this->variables, variablesSizes.at(i)) << ";" << std::endl;
	}
	outFile << std::endl;

	/*//logics
	for (int i = 0; i != (int)this->logics.size(); i++) {
		outFile << compInstanceStr(this->logics.at(i)); //CHANGE
	}*/

	//STATES

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
// 1. Calculate timeASAP for each node
//int Netlist::calcTimeASAP(std::vector<Connector*> vector) {

//}



// 2. Calculate ALAP for each node



// 3. Populate the resource probability matrix for each resource type
// note: column 0 is the node number

// add/sub
// mult
// div/mod
// logical



// 4. Populate the distribution graph for each resource
// note: column 0 is the node number

// add/sub
// mult
// div/mod
// logical




// 5. Populate the self-forces matrix
// note: column 0 is the node number




// 6. Populate the total forces matrix
// note: column 0 is the node number




// 7. Choose the lowest total force node and schedule it
// set bool scheduled = 1




// Repeat steps 3-7 until all nodes are scheduled









// STATE MACHINE ALGORITHM
// ***********************
// Populate states and transitions based on Force-Directed scheduling