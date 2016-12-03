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
#include "Connector.h"

class State{
	public:
		// Constructor
		State(int name, std::vector<Transition*> inputTransitions, std::vector<Transition*> outputTransitions, std::vector<Logic*> operations);
		State(int name);

		//insert getters and setters
		int get_name() { return this->name; }
		int get_latency() { return this->latency; }
		std::vector<Connector*> get_conditions() { return this->conditions; }
		std::vector<Logic*> get_operations() { return this->operations; }
		bool get_lockCond() { return this->lockCond; }
		std::vector<Transition*> get_inputTransitions() { return this->inputTransitions; }
		std::vector<Transition*> get_outputTransitions() { return this->outputTransitions; }

		void set_latency(int latency) { this->latency = latency; }
		void set_conditions(std::vector<Connector*> conditions) { this->conditions = conditions; }
		void set_operations(std::vector<Logic*> operations) { this->operations = operations; }
		void add_condition(Connector* condition) { this->conditions.push_back(condition); }
		void set_lockCond(bool lockCond) { this->lockCond = lockCond; }

		void set_inputTransitions(std::vector<Transition*> inputTransitions) { this->inputTransitions = inputTransitions; }
		void set_outputTransitions(std::vector<Transition*> outputTransitions) { this->outputTransitions = outputTransitions; }

	private:
		int name;
		int latency;
		bool lockCond;
		std::vector<Connector*> conditions;
		std::vector<Transition*> inputTransitions;
		std::vector<Transition*> outputTransitions;
		std::vector<Logic*> operations;
};  

#endif