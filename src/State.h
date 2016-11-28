/****************************************
Authors: Justin Siekmann, Anthony Ciancio
NetIDs : jsiekmann, shjay
Date : 29 November, 2016
Assignment #3

State.h
*****************************************/
#ifndef STATE_H
#define STATE_H

#include <vector>
#include <string>

#include "Transition.h"
#include "Logic.h"

class State{
	public:
		// Constructor
		//State(std::string name, std::vector<Transition*> inputTransitions, std::vector<Transition*> outputTransitions, std::vector<Logic*> operations);

		//insert getters and setters

	private:
		std::string name;
		//std::vector<Transition*> inputTransitions;
		//std::vector<Transition*> outputTransitions;
		std::vector<Logic*> operations;
};  

#endif