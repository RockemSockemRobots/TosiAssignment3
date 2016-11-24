/****************************************
Authors: Justin Siekmann, Anthony Ciancio
NetIDs : jsiekmann, shjay
Date : 25 October, 2016
Assignment #2

Netlist.cpp
*****************************************/
#include "Netlist.h"

Netlist::Netlist(std::string inputFile) {
	this->inputs.push_back(new Connector("", false, "1", "Clk", 0));
	this->inputs.push_back(new Connector("", false, "1", "Rst", 0));
	this->error = Netlist::readIn(inputFile);
}

bool Netlist::readIn(std::string inputFile) {
	static std::vector<int> delayColumn = { 1, 2, 8, 16, 32, 64 };
	static std::vector<std::string> validComponents = { "+","-","*",">","<","==",">>","<<","/","%","?" };
	int REGs = 0; int ADDs = 0; int SUBs = 0; int MULs = 0; int COMPs = 0; int MUX2x1s = 0; int SHRs = 0; int SHLs = 0; int DIVs = 0; int MODs = 0; int INCs = 0; int DECs = 0;
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
	while (std::getline(inFile,line)) {
		line.erase(std::remove(line.begin(), line.end(), ','), line.end());
		std::stringstream line_stream(line);
		line_stream >> type_word;
		if (line != "") {
			if (type_word == "input" || type_word == "output" || type_word == "wire" || type_word == "register") {
				line_stream >> sign_size_word;
				std::size_t pos = sign_size_word.find_first_of("t");
				std::string sign_word = sign_size_word.substr(0, (pos + 1));
				bool sign = sign_word.find('U');
				std::string size_word = sign_size_word.substr((pos + 1), sign_size_word.length());
				while (line_stream >> name_word) {
					if (type_word == "input") {
						this->inputs.push_back(new Connector(type_word, sign, size_word, name_word, 0));
					}
					else if (type_word == "output") {
						this->outputs.push_back(new Connector(type_word, sign, size_word, name_word, -1));
					}
					else if (type_word == "wire") {
						this->wires.push_back(new Connector(type_word, sign, size_word, name_word, -1));
					}
					else if (type_word == "register") {
						this->wires.push_back(new Connector(type_word, sign, size_word, name_word, -1));
						this->registers.push_back(new Connector(type_word, sign, size_word, name_word, -1));
					}
				}
				type_word = ""; sign_size_word = ""; name_word = ""; pos = 0; sign_word = ""; size_word = "";
			}
			else {
				//word1 already have as type_word
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
				else if (word4 == "") {
					tempInputs = createCompInputs(word3, "Clk", "Rst");
					tempOutputs = createCompOutputs(word4, type_word);
					if (tempOutputs.size() == 0) {
						return true;
					}
					tempsize = stoi(tempOutputs.at(0)->get_size());
					column = find(delayColumn.begin(), delayColumn.end(), tempsize) - delayColumn.begin();
					this->REGcomps.push_back(new Logic("REG", "reg_" + tempOutputs.at(0)->get_name(), tempInputs, tempOutputs, tempsize, false, delayArray[0][column], ""));
					REGs++;
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
							tempsize = stoi(tempOutputs.at(0)->get_size());
							column = find(delayColumn.begin(), delayColumn.end(), tempsize) - delayColumn.begin();
							this->logics.push_back(new Logic("ADD", "add" + std::to_string(ADDs), tempInputs, tempOutputs, tempsize, false, delayArray[1][column], ""));
							ADDs++;
						}
						else {
							tempInputs = createCompInputs(word3);
							tempOutputs = createCompOutputs(word4, type_word);
							if (tempOutputs.size() == 0 || tempInputs.size() == 0) {
								return true;
							}
							tempsize = stoi(tempOutputs.at(0)->get_size());
							column = find(delayColumn.begin(), delayColumn.end(), tempsize) - delayColumn.begin();
							this->logics.push_back(new Logic("INC", "inc" + std::to_string(INCs), tempInputs, tempOutputs, tempsize, false, delayArray[10][column], ""));
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
							tempsize = stoi(tempOutputs.at(0)->get_size());
							column = find(delayColumn.begin(), delayColumn.end(), tempsize) - delayColumn.begin();
							this->logics.push_back(new Logic("SUB", "sub" + std::to_string(SUBs), tempInputs, tempOutputs, tempsize, false, delayArray[2][column], ""));
							SUBs++;
						}
						else {
							tempInputs = createCompInputs(word3);
							tempOutputs = createCompOutputs(word4, type_word);
							if (tempOutputs.size() == 0 || tempInputs.size() == 0) {
								return true;
							}
							tempsize = stoi(tempOutputs.at(0)->get_size());
							column = find(delayColumn.begin(), delayColumn.end(), tempsize) - delayColumn.begin();
							this->logics.push_back(new Logic("DEC", "dec" + std::to_string(DECs), tempInputs, tempOutputs, tempsize, false, delayArray[11][column], ""));
							DECs++;
						}
						break;
					case 2:
						tempInputs = createCompInputs(word3, word5);
						tempOutputs = createCompOutputs(word4, type_word);
						if (tempOutputs.size() == 0 || tempInputs.size() == 0) {
							return true;
						}
						tempsize = stoi(tempOutputs.at(0)->get_size());
						column = find(delayColumn.begin(), delayColumn.end(), tempsize) - delayColumn.begin();
						this->logics.push_back(new Logic("MUL", "mul" + std::to_string(MULs), tempInputs, tempOutputs, tempsize, false, delayArray[3][column], ""));
						MULs++;
						break;
					case 3:
						tempInputs = createCompInputs(word3, word5);
						tempOutputs = createCompOutputs(word4, type_word);
						if (tempOutputs.size() == 0 || tempInputs.size() == 0) {
							return true;
						}
						tempsize = std::max(stoi(tempInputs.at(0)->get_size()), stoi(tempInputs.at(1)->get_size()));
						column = find(delayColumn.begin(), delayColumn.end(), tempsize) - delayColumn.begin();
						this->logics.push_back(new Logic("COMP", "comp" + std::to_string(COMPs), tempInputs, tempOutputs, tempsize, checkSign(tempInputs, tempOutputs), delayArray[4][column], "gt"));
						COMPs++;
						break;
					case 4:
						tempInputs = createCompInputs(word3, word5);
						tempOutputs = createCompOutputs(word4, type_word);
						if (tempOutputs.size() == 0 || tempInputs.size() == 0) {
							return true;
						}
						tempsize = std::max(stoi(tempInputs.at(0)->get_size()), stoi(tempInputs.at(1)->get_size()));
						column = find(delayColumn.begin(), delayColumn.end(), tempsize) - delayColumn.begin();
						this->logics.push_back(new Logic("COMP", "comp" + std::to_string(COMPs), tempInputs, tempOutputs, tempsize, checkSign(tempInputs, tempOutputs), delayArray[4][column], "lt"));
						COMPs++;
						break;
					case 5:
						tempInputs = createCompInputs(word3, word5);
						tempOutputs = createCompOutputs(word4, type_word);
						if (tempOutputs.size() == 0 || tempInputs.size() == 0) {
							return true;
						}
						tempsize = std::max(stoi(tempInputs.at(0)->get_size()), stoi(tempInputs.at(1)->get_size()));
						column = find(delayColumn.begin(), delayColumn.end(), tempsize) - delayColumn.begin();
						this->logics.push_back(new Logic("COMP", "comp" + std::to_string(COMPs), tempInputs, tempOutputs, tempsize, checkSign(tempInputs, tempOutputs), delayArray[4][column], "eq"));
						COMPs++;
						break;
					case 6:
						tempInputs = createCompInputs(word3, word5);
						tempOutputs = createCompOutputs(word4, type_word);
						if (tempOutputs.size() == 0 || tempInputs.size() == 0) {
							return true;
						}
						tempsize = stoi(tempOutputs.at(0)->get_size());
						column = find(delayColumn.begin(), delayColumn.end(), tempsize) - delayColumn.begin();
						this->logics.push_back(new Logic("SHR", "shr" + std::to_string(SHRs), tempInputs, tempOutputs, tempsize, checkSign(tempInputs, tempOutputs), delayArray[6][column], ""));
						SHRs++;
						break;
					case 7:
						tempInputs = createCompInputs(word3, word5);
						tempOutputs = createCompOutputs(word4, type_word);
						if (tempOutputs.size() == 0 || tempInputs.size() == 0) {
							return true;
						}
						tempsize = stoi(tempOutputs.at(0)->get_size());
						column = find(delayColumn.begin(), delayColumn.end(), tempsize) - delayColumn.begin();
						this->logics.push_back(new Logic("SHL", "shl" + std::to_string(SHLs), tempInputs, tempOutputs, tempsize, false, delayArray[7][column], ""));
						SHLs++;
						break;
					case 8:
						tempInputs = createCompInputs(word3, word5);
						tempOutputs = createCompOutputs(word4, type_word);
						if (tempOutputs.size() == 0 || tempInputs.size() == 0) {
							return true;
						}
						tempsize = stoi(tempOutputs.at(0)->get_size());
						column = find(delayColumn.begin(), delayColumn.end(), tempsize) - delayColumn.begin();
						this->logics.push_back(new Logic("DIV", "div" + std::to_string(DIVs), tempInputs, tempOutputs, tempsize, checkSign(tempInputs, tempOutputs), delayArray[8][column], ""));
						DIVs++;
						break;
					case 9:
						tempInputs = createCompInputs(word3, word5);
						tempOutputs = createCompOutputs(word4, type_word);
						if (tempOutputs.size() == 0 || tempInputs.size() == 0) {
							return true;
						}
						tempsize = stoi(tempOutputs.at(0)->get_size());
						column = find(delayColumn.begin(), delayColumn.end(), tempsize) - delayColumn.begin();
						this->logics.push_back(new Logic("MOD", "mod" + std::to_string(MODs), tempInputs, tempOutputs, tempsize, checkSign(tempInputs, tempOutputs), delayArray[9][column], ""));
						MODs++;
						break;
					case 10:
						if (word6 == ":") {
							tempInputs = createCompInputs(word5, word7, word3);
							tempOutputs = createCompOutputs(word4, type_word);
							if (tempOutputs.size() == 0 || tempInputs.size() == 0) {
								return true;
							}
							tempsize = stoi(tempOutputs.at(0)->get_size());
							column = find(delayColumn.begin(), delayColumn.end(), tempsize) - delayColumn.begin();
							this->logics.push_back(new Logic("MUX2x1", "mux" + std::to_string(MUX2x1s), tempInputs, tempOutputs, tempsize, false, delayArray[5][column], ""));
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
	
	std::size_t posStart = std::min(outputFile.find_last_of("\\"), outputFile.find_last_of("/"));
	std::size_t posEnd = outputFile.find_last_of(".");
	std::string moduleName;
	if (outputFile.substr(posStart + 1, 5) == "474a_" || outputFile.substr(posStart + 1, 5) == "574a_") {
		moduleName = outputFile.substr((posStart + 6), posEnd - (posStart+6));
	}
	else {
		moduleName = outputFile.substr(posStart + 1, posEnd - posStart - 1);
	}

	//first line
	outFile << "module " << moduleName << "(";
	for (int i = 0; i != (int)this->inputs.size(); i++) {
		outFile << this->inputs.at(i)->get_name() << ", ";
	}
	for (int i = 0; i != (int)this->outputs.size(); i++) {
		outFile << this->outputs.at(i)->get_name();
		if (i != (int)this->outputs.size() - 1) {
			outFile << ", ";
		}
	}
	outFile << ");" << std::endl;;
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
		outFile << "\toutput ";
		if (outputSizes.at(i) != "1") {
			outFile << "[" << (stoi(outputSizes.at(i)) - 1) << ":0] ";
		}
		outFile << connectorsOfSize(this->outputs, outputSizes.at(i)) << ";" << std::endl;
	}
	outFile << std::endl;
	//wires
	std::vector<std::string> wiresSizes = getSizes(this->wires);
	for (int i = 0; i != (int)wiresSizes.size(); i++) {
		outFile << "\twire ";
		if (wiresSizes.at(i) != "1") {
			outFile << "[" << (stoi(wiresSizes.at(i)) - 1) << ":0] ";
		}
		outFile << connectorsOfSize(this->wires, wiresSizes.at(i)) << ";" << std::endl;
	}
	outFile << std::endl;
	//logics
	for (int i = 0; i != (int)this->logics.size(); i++) {
		outFile << compInstanceStr(this->logics.at(i));
	}
	//REGs
	for (int i = 0; i != (int)this->REGcomps.size(); i++) {
		outFile << compInstanceStr(this->REGcomps.at(i));
	}
	outFile << "endmodule";

	outFile.close();
}

std::string Netlist::compInstanceStr(Logic* logic) {
	std::string result = "\t";
	if (logic->get_sign() == true) {
		result.append("S");
	}
	result.append(logic->get_type() + " #(.DATAWIDTH(" + std::to_string(logic->get_dw()) + ")) " + logic->get_name() + "(");
	for (int i = 0; i != (int)logic->get_inputs().size(); i++) {
		if (logic->get_type() == "MUX2x1" && i == 2) {
			result.append(logic->get_inputs().at(i)->get_name());
			result.append(", ");
		}
		// pad
		else if (logic->get_dw() > stoi(logic->get_inputs().at(i)->get_size()) && logic->get_inputs().at(i)->get_name() != "Clk" && logic->get_inputs().at(i)->get_name() != "Rst") {
			// signed
			if (logic->get_inputs().at(i)->get_sign() == true && !((logic->get_type() == "SHR" || logic->get_type() == "SHL") && i == 1)) {
				result.append("{{" + std::to_string(logic->get_dw() - stoi(logic->get_inputs().at(i)->get_size())) + "{" + logic->get_inputs().at(i)->get_name() + "[" + std::to_string(stoi(logic->get_inputs().at(i)->get_size()) - 1) + "]}}," + logic->get_inputs().at(i)->get_name() + "}");
				result.append(", ");
			}
			// unsigned
			else {
				result.append("{{" + std::to_string(logic->get_dw() - stoi(logic->get_inputs().at(i)->get_size())) + "{0}}," + logic->get_inputs().at(i)->get_name() + "}");
				result.append(", ");
			}
		}
		// truncate
		else if (logic->get_dw() < stoi(logic->get_inputs().at(i)->get_size()) && logic->get_inputs().at(i)->get_name() != "Clk" && logic->get_inputs().at(i)->get_name() != "Rst") { 
			result.append(logic->get_inputs().at(i)->get_name() + "[" + std::to_string(logic->get_dw() - 1) + ":0]");
			result.append(", ");
		}
		else {
			result.append(logic->get_inputs().at(i)->get_name());
			result.append(", ");
		}
	}
	if (logic->get_type() == "COMP") {
		if (logic->get_outType() == "gt") {
			result.append(logic->get_outputs().at(0)->get_name() + ",,");
		}
		else if (logic->get_outType() == "lt") {
			result.append(", "+ logic->get_outputs().at(0)->get_name() + ",");
		}
		else {
			result.append(",,"+logic->get_outputs().at(0)->get_name());
		}
	}
	else {
		for (int i = 0; i != (int)logic->get_outputs().size(); i++) {
			result.append(logic->get_outputs().at(i)->get_name());
			if (i != ((int)logic->get_outputs().size() - 1)) {
				result.append(", ");
			}
		}
	}
	result.append(");\n");
	return result;
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
		tempConnector = findName(this->wires, input1);
	}
	if (tempConnector == NULL) {
		std::cerr << "Missing input or wire" << std::endl;
		return errorVector;
	}
	tempVector.push_back(tempConnector);
	//input2
	if (input2 != "") {
		tempConnector = findName(this->inputs, input2);
		if (tempConnector == NULL) {
			tempConnector = findName(this->wires, input2);
		}
		if (tempConnector == NULL) {
			std::cerr << "Missing input or wire" << std::endl;
			return errorVector;
		}
		tempVector.push_back(tempConnector);
	}
	//input3
	if (input3 != "") {
		tempConnector = findName(this->inputs, input3);
		if (tempConnector == NULL) {
			tempConnector = findName(this->wires, input3);
		}
		if (tempConnector == NULL) {
			std::cerr << "Missing input or wire" << std::endl;
			return errorVector;
		}
		tempVector.push_back(tempConnector);
	}
	return tempVector;
}

std::vector<Connector*> Netlist::createCompOutputs(std::string word4, std::string input1, std::string input2, std::string input3) {
	static std::vector<int> delayColumn = { 1, 2, 8, 16, 32, 64 }; int tempsize; int column;
	std::vector<Connector*> tempInputs;
	std::vector<Connector*> tempOutputs;
	std::vector<Connector*> tempVector;
	std::vector<Connector*> errorVector;
	Connector* tempConnector = NULL;
	//input1
	tempConnector = findName(this->wires, input1);
	if (tempConnector == NULL) {
		tempConnector = findName(this->outputs, input1);
		if(word4 == ""){
			tempVector.push_back(tempConnector);
		}
		else if(tempConnector != NULL){
			this->wires.push_back(new Connector("wire", tempConnector->get_sign(), tempConnector->get_size(), tempConnector->get_name() + "wire", -1));
			tempVector.push_back(this->wires.at(this->wires.size() - 1));
			tempInputs.push_back(this->wires.at(this->wires.size() - 1));
			tempInputs.push_back(this->inputs.at(0));
			tempInputs.push_back(this->inputs.at(1));
			tempOutputs.push_back(tempConnector);
			tempsize = stoi(tempOutputs.at(0)->get_size());
			column = find(delayColumn.begin(), delayColumn.end(), tempsize) - delayColumn.begin();
			this->REGcomps.push_back(new Logic("REG", "reg_" + tempOutputs.at(0)->get_name(), tempInputs, tempOutputs, tempsize, false, delayArray[0][column], ""));
		}
	}
	else {
		tempVector.push_back(tempConnector);
	}
	if (tempConnector == NULL) {
		std::cerr << "Missing wire or output" << std::endl;
		return errorVector;
	}
	//input2
	if (input2 != "") {
		tempConnector = findName(this->wires, input2);
		if (tempConnector == NULL) {
			tempConnector = findName(this->outputs, input2);
			if (word4 == "") {
				tempVector.push_back(tempConnector);
			}
			else {
				this->wires.push_back(new Connector("wire", tempConnector->get_sign(), tempConnector->get_size(), tempConnector->get_name() + "wire", -1));
				tempVector.push_back(this->wires.at(this->wires.size() - 1));
				tempInputs.push_back(this->wires.at(this->wires.size() - 1));
				tempInputs.push_back(this->inputs.at(0));
				tempInputs.push_back(this->inputs.at(1));
				tempOutputs.push_back(tempConnector);
				tempsize = stoi(tempOutputs.at(0)->get_size());
				column = find(delayColumn.begin(), delayColumn.end(), tempsize) - delayColumn.begin();
				this->REGcomps.push_back(new Logic("REG", "reg_" + tempOutputs.at(0)->get_name(), tempInputs, tempOutputs, tempsize, false, delayArray[0][column], ""));
			}
		}
		else {
			tempVector.push_back(tempConnector);
		}
		if (tempConnector == NULL) {
			std::cerr << "Missing wire or output" << std::endl;
			return errorVector;
		}
	}
	//input3
	if (input3 != "") {
		tempConnector = findName(this->wires, input3);
		if (tempConnector == NULL) {
			tempConnector = findName(this->outputs, input3);
			if (word4 == "") {
				tempVector.push_back(tempConnector);
			}
			else {
				this->wires.push_back(new Connector("wire", tempConnector->get_sign(), tempConnector->get_size(), tempConnector->get_name() + "wire", -1));
				tempVector.push_back(this->wires.at(this->wires.size() - 1));
				tempInputs.push_back(this->wires.at(this->wires.size() - 1));
				tempInputs.push_back(this->inputs.at(0));
				tempInputs.push_back(this->inputs.at(1));
				tempOutputs.push_back(tempConnector);
				tempsize = stoi(tempOutputs.at(0)->get_size());
				column = find(delayColumn.begin(), delayColumn.end(), tempsize) - delayColumn.begin();
				this->REGcomps.push_back(new Logic("REG", "reg_" + tempOutputs.at(0)->get_name(), tempInputs, tempOutputs, tempsize, false, delayArray[0][column], ""));
			}

		}
		else {
			tempVector.push_back(tempConnector);
		}
		if (tempConnector == NULL) {
			std::cerr << "Missing wire or output" << std::endl;
			return errorVector;
		}
	}
	return tempVector;
}

void Netlist::critPath() {
	std::vector<Logic*> problemConnectors;
	bool problem = false;
	double tempMax = 0;
	for (int i = 0; i != (int)this->logics.size(); i++) {
		tempMax = 0;
		problem = false;
		for (int j = 0; j != (int)this->logics.at(i)->get_inputs().size(); j++) {
			if (this->logics.at(i)->get_inputs().at(j)->get_delay() >= 0) {
				tempMax = std::max(this->logics.at(i)->get_inputs().at(j)->get_delay(), tempMax);
			}
			else {
				problem = true;
			}
		}
		if (problem == false) {
			this->logics.at(i)->get_outputs().at(0)->set_delay(this->logics.at(i)->get_delay() + tempMax);
		}
		else {
			problemConnectors.push_back(this->logics.at(i));
		}
	}
	for (int i = 0; i != (int)this->REGcomps.size(); i++) {
		this->REGcomps.at(i)->get_outputs().at(0)->set_delay(this->REGcomps.at(i)->get_delay());
	}
	while (!problemConnectors.empty()) {
		bool problem = false;
		for (int i = 0; i != (int)problemConnectors.size(); i++) {
			tempMax = 0;
			problem = false;
			for (int j = 0; j != (int)problemConnectors.at(i)->get_inputs().size(); j++) {
				if (problemConnectors.at(i)->get_inputs().at(j) >= 0) {
					tempMax = std::max(problemConnectors.at(i)->get_inputs().at(j)->get_delay(), tempMax);
				}
				else {
					problem = true;
				}
			}
			if (problem == false) {
				problemConnectors.at(i)->get_outputs().at(0)->set_delay(problemConnectors.at(i)->get_delay() + tempMax);
				problemConnectors.erase(problemConnectors.begin()+i);
				i--;
			}
		}
	}
	tempMax = 0;
	for (int i = 0; i != (int)this->REGcomps.size(); i++) {
		tempMax = std::max(this->REGcomps.at(i)->get_inputs().at(0)->get_delay(), tempMax);
	}
	std::cout << "Critical Path : " << tempMax << " ns" << std::endl;
}