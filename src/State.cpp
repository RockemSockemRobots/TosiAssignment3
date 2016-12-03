/****************************************
Authors: Justin Siekmann, Anthony Ciancio
NetIDs : jsiekmann, shjay
Date : 7 December 2016
Assignment #3

State.cpp
*****************************************/
#include "State.h"

State::State(int name, std::vector<Transition*> inputTransitions, std::vector<Transition*> outputTransitions, std::vector<Logic*> operations) {
	this->name = name;
	this->inputTransitions = inputTransitions;
	this->outputTransitions = outputTransitions;
	this->operations = operations;
	this->lockCond = false;
}
State::State(int name) {
	this->name = name;
	this->lockCond = false;
}