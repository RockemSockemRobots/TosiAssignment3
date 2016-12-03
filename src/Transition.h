/****************************************
Authors: Justin Siekmann, Anthony Ciancio
NetIDs : jsiekmann, shjay
Date : 7 December 2016
Assignment #3

State.h
*****************************************/
#ifndef TRANSITION_H
#define TRANSITION_H

#include <vector>
#include <string>

class State;

class Transition{
	public:
		// Constructor
		Transition(int src, int dst);

		//insert getters and setters
		int get_src() { return this->src; }
		int get_dst() { return this->dst; }
		//std::string get_condition() { return this->condition; }

		void set_dst(int dst) { this->dst = dst; }

	private:
		int src;
		int dst;
		//std::string condition;
};

#endif