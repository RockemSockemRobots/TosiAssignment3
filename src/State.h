/****************************************
Authors: Justin Siekmann, Anthony Ciancio
NetIDs : jsiekmann, shjay
Date : 7 December 2016
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
		State(int name, std::vector<Transition*> inputTransitions, std::vector<Transition*> outputTransitions, std::vector<Logic*> operations);
		State(int name);

		//insert getters and setters
		int get_name() { return this->name; }
		int get_latency() { return this->latency; }
		std::vector<Logic*> get_operations() { return this->operations; }

		void set_latency(int latency) { this->latency = latency; }
		void set_operations(std::vector<Logic*> operations) { this->operations = operations; }

	private:
		int name;
		int latency;
		std::vector<Transition*> inputTransitions;
		std::vector<Transition*> outputTransitions;
		std::vector<Logic*> operations;
};  

#endif