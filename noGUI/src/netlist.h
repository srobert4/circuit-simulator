/*
Netlist.h
---------
Class to represent a netlist
*/
#include <string>
#include <vector>
#include <map>
#include <iostream>

#include "boundarycondition.h"

#ifndef __NETLIST_H__
#define __NETLIST_H__

class Netlist
{
public:
	Netlist(const std::string &name = "");
	~Netlist();

	char** get_netlist();
	double get_boundary_condition(const std::string &node_name, double time);

	void set_name(const std::string name);
	int add_element(
		const std::string &element, 
		const std::string &name, 
		const std::string &node_in, 
		const std::string &node_out, 
		const std::string &value, 
		const std::string &units = ""
	);
	int add_boundary_condition(
		const std::string &node_name, 
		const std::string &file_given = "", 
		const double &period_given = 0.0
	);
	int add_initial_condition(const std::string &node_name, const std::string &ic);
	int set_analysis(
		const std::string &type, 
		const std::string &step, 
		const std::string &step_units, 
		const std::string &time, 
		const std::string &time_units
	);
	int load_from_file(
		const std::string &filename,
		const std::map<std::string, std::string>* bc_files=NULL,
		const double period=0.0
	);
	void unload_netlist();
	size_t get_length();

// TODO: move functions to private
private:
	std::map<std::string, BoundaryCondition*> bcs;
	std::vector<std::string> netlist_vec;
	char** netlist;
	bool file_loaded;
	bool constructed;

	int construct_netlist();
	void request_boundary_condition(const std::string &node_name, std::string &file, double &period);
	bool check_file_loaded();
};


#endif