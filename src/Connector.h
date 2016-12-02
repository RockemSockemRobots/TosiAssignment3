/****************************************
Authors: Justin Siekmann, Anthony Ciancio
NetIDs: jsiekmann, shjay
Date: 25 October 2016
Assignment #2

Connector.h
*****************************************/
#ifndef CONNECTOR_H
#define CONNECTOR_H

#include <string>

class Connector{
	public:
		Connector();
		Connector(std::string type, bool sign, std::string size, std::string name, int timeASAP, int timeALAP);
		//Get
		std::string get_type() { return this->type; }
		std::string get_name() { return this->name; }
		std::string get_size() { return this->size; }
		bool get_sign() { return this->sign; }
		int get_timeASAP() { return this->timeASAP; }
		int get_timeALAP() { return this->timeALAP; }
		int get_minASAP() { return this->minASAP; }
		int get_maxALAP() { return this->maxALAP; }
		bool get_schForce() { return this->schForce; }
		//Set
		void set_type(std::string type) { this->type = type; }
		void set_name(std::string name) { this->name = name; }
		void set_size(std::string size) { this->size = size; }
		void set_sign(bool sign) { this->sign = sign; }
		void set_timeASAP(int timeASAP) { this->timeASAP = timeASAP; }
		void set_timeALAP(int timeALAP) { this->timeALAP = timeALAP; }
		void set_minASAP(int minASAP) { this->minASAP = minASAP; }
		void set_maxALAP(int maxALAP) { this->maxALAP = maxALAP; }
		void set_schForce(bool schForce) { this->schForce = schForce; }
	private:
		std::string type;
		std::string name;
		std::string size;
		bool sign;
		int timeASAP;
		int timeALAP;
		int minASAP;
		int maxALAP;
		bool schForce;
};

#endif