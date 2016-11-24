/****************************************
Authors: Justin Siekmann, Anthony Ciancio
NetIDs : jsiekmann, shjay
Date : 25 October, 2016
Assignment #2

Logic.h
*****************************************/
#ifndef LOGIC_H
#define LOGIC_H

#include <string>
#include <vector>

#include "Connector.h"

class Logic {
	public:
		Logic(std::string type, std::string name, std::vector<Connector*> inputs, std::vector<Connector*> outputs, int dw, bool sign, float delay, std::string outType);
		//Get
		std::string get_type() { return this->type; }
		std::string get_name() { return this->name; }
		int get_dw() { return this->dw; }
		bool get_sign() { return this->sign; }
		double get_delay() { return this->delay; }
		std::string get_outType() { return this->outType; }
		std::vector<Connector*> get_inputs() { return this->inputs; }
		std::vector<Connector*> get_outputs() { return this->outputs; }
		//Set
		void set_type(std::string type) { this->type = type; }
		void set_name(std::string name) { this->name = name; }
		void set_dw(int dw) { this->dw = dw; }
		void set_sign(bool sign) { this->sign = sign; }
		void set_delay(double delay) { this->delay = delay; }
		void set_outType(std::string outType) { this->outType = outType; }
	private:
		std::string type;
		std::string name;
		std::vector<Connector*> inputs;
		std::vector<Connector*> outputs;
		int dw;
		bool sign;
		double delay;
		std::string outType;
};
#endif