/****************************************
Authors: Justin Siekmann, Anthony Ciancio
NetIDs : jsiekmann, shjay
Date : 29 November 2016
Assignment #3

Transition.cpp
*****************************************/
#include "Transition.h"

Transition::Transition(int src, int dst, std::string condition) {
	this->src = src;
	this->dst = dst;
	this->condition = condition;
}