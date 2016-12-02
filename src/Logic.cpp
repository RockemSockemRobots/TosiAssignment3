/****************************************
Authors: Justin Siekmann, Anthony Ciancio
NetIDs : jsiekmann, shjay
Date : 7 December 2016
Assignment #3

Logic.cpp
*****************************************/
#include "Logic.h"

Logic::Logic(std::string type, std::string name, std::vector<Connector*> inputs, std::vector<Connector*> outputs, bool sign, int delay, std::string outType, int timeASAP, int timeALAP, std::string typeSymbol) {
	this->type = type;
	this->name = name;
	this->inputs = inputs;
	this->outputs = outputs;
	this->sign = sign;
	this->delay = delay;
	this->outType = outType;
	this->timeASAP = timeASAP;
	this->timeALAP = timeALAP;
	this->schASAP = false;
	this->schALAP = false;
	this->schForce = false;
	this->typeSymbol = typeSymbol;
}