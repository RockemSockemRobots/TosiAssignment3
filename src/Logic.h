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
		Logic(std::string type, std::string name, std::vector<Connector*> inputs, std::vector<Connector*> outputs, bool sign, int delay, std::string outType, int timeASAP, int timeALAP);
		//Get
		std::string get_type() { return this->type; }
		std::string get_name() { return this->name; }
		bool get_sign() { return this->sign; }
		int get_delay() { return this->delay; }
		std::string get_outType() { return this->outType; }
		std::vector<Connector*> get_inputs() { return this->inputs; }
		std::vector<Connector*> get_outputs() { return this->outputs; }
		int get_timeASAP() { return this->timeASAP; }
		int get_timeALAP() { return this->timeALAP; }

		bool get_schASAP() { return this->schASAP; }
		bool get_schALAP() { return this->schALAP; }
		bool get_schForce() { return this->schForce; }

		std::vector<double> get_selfForces() { return this->selfForces; }
		std::vector<double> get_totsForces() { return this->totsForces; }

		//Set
		void set_type(std::string type) { this->type = type; }
		void set_name(std::string name) { this->name = name; }
		void set_sign(bool sign) { this->sign = sign; }
		void set_delay(int delay) { this->delay = delay; }
		void set_outType(std::string outType) { this->outType = outType; }

		void set_timeASAP(int timeASAP) { this->timeASAP = timeASAP; }
		void set_timeALAP(int timeALAP) { this->timeALAP = timeALAP; }
		void set_schASAP(bool schASAP) { this->schASAP = schASAP; }
		void set_schALAP(bool schALAP) { this->schALAP = schALAP; }
		void set_schForce(bool schForce) { this->schForce = schForce; }

		void set_selfForces(std::vector<double> selfForces) { this->selfForces = selfForces; }
		void set_totsForces(std::vector<double> totsForces) { this->totsForces = totsForces; }

	private:
		std::string type;
		std::string name;
		std::vector<Connector*> inputs;
		std::vector<Connector*> outputs;
		bool sign;
		int delay;
		std::string outType;
		int timeASAP;
		int timeALAP;
		bool schASAP;
		bool schALAP;
		bool schForce;
		std::vector<double> selfForces;
		std::vector<double> totsForces;
};
#endif