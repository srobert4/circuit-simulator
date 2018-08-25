/*
BoundaryCondition.h
---------
Class to represent the boundary conditions for an element
*/
#include <string>
#include <map>

#ifndef __BOUNDARYCONDITION_H__
#define __BOUNDARYCONDITION_H__

class BoundaryCondition
{
public:
	BoundaryCondition(const std::string &filename, double period);
	~BoundaryCondition();

	double get_state(double time);


private:
	std::map<double, double> conditions;
	double period;

	double interpolate(double t_lower, double t_upper, double c_lower, double c_upper, double t);

};

#endif