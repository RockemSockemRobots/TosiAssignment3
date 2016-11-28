/****************************************
Authors: Justin Siekmann, Anthony Ciancio
NetIDs : jsiekmann, shjay
Date : 29 November 2016
Assignment #3

Logic.h
*****************************************/
#ifndef LOGIC_H
#define LOGIC_H

#include <string>
#include <vector>

#include "Connector.h"

class Logic {
	public:
		Logic(std::string type, std::string name, std::vector<Connector*> inputs, std::vector<Connector*> outputs, bool sign, float delay, std::string outType, int timeASAP, int timeALAP);
		//Get
		std::string get_type() { return this->type; }
		std::string get_name() { return this->name; }
		bool get_sign() { return this->sign; }
		double get_delay() { return this->delay; }
		std::string get_outType() { return this->outType; }
		std::vector<Connector*> get_inputs() { return this->inputs; }
		std::vector<Connector*> get_outputs() { return this->outputs; }
		int get_timeASAP() { return this->timeASAP; }
		int get_timeALAP() { return this->timeALAP; }
		bool get_schASAP() { return this->schASAP; }
		bool get_schALAP() { return this->schALAP; }
		bool get_schForce() { return this->schForce; }
		//Set
		void set_type(std::string type) { this->type = type; }
		void set_name(std::string name) { this->name = name; }
		void set_sign(bool sign) { this->sign = sign; }
		void set_delay(double delay) { this->delay = delay; }
		void set_outType(std::string outType) { this->outType = outType; }
		int set_timeASAP(int timeASAP) { this->timeASAP = timeASAP; }
		int set_timeALAP(int timeALAP) { this->timeALAP = timeALAP; }
		bool set_schASAP(bool schASAP) { this->schASAP = schASAP; }
		bool set_schALAP(bool schALAP) { this->schALAP = schALAP; }
		bool set_schForce(bool schForce) { this->schForce = schForce; }
	private:
		std::string type;
		std::string name;
		std::vector<Connector*> inputs;
		std::vector<Connector*> outputs;
		bool sign;
		double delay;
		std::string outType;
		int timeASAP;
		int timeALAP;
		bool schASAP;
		bool schALAP;
		bool schForce;
};
#endif