/****************************************
Authors: Justin Siekmann, Anthony Ciancio
NetIDs : jsiekmann, shjay
Date : 29 November 2016
Assignment #3

State.cpp
*****************************************/
#include "State.h"

State::State(int name, std::vector<Transition*> inputTransitions, std::vector<Transition*> outputTransitions, std::vector<Logic*> operations) {
	this->name = name;
	this->inputTransitions = inputTransitions;
	this->outputTransitions = outputTransitions;
	this->operations = operations;
}