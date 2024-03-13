#ifndef PVPANEL_HPP
#define PVPANEL_HPP

#include <simgrid/s4u.hpp>
#include <string>
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <stdlib.h>
#include <limits>

using namespace std;

class PVPanel 
{
	/**
	 * The input file containing the power production trajectory.
	 */
    string inputFile;
	
    std::vector<std::pair<double,double>> green_power_production;

	string host_name; 
	
	/**
	 * The next green power production according to the input trace.
	 */
	double next_green_power_production;
	
	/**
	 * The current green power production according to the input trace.
	 */
	double current_green_power_production;
	
	/**
	 * the last time the power production was updated.
	 */
	double time;
	
	/**
	 * the next time the power production will be updated.
	 */
	double next_time;

    /**
	 * Advance to the new update of the power production -i.e. change the current/next time/production according to the trace.
	 */
	void advance_time(double time);
	void load_CSV_file();
public:    
	explicit PVPanel();
    explicit PVPanel(std::string fileName);
	double get_current_green_power_production(double time);
};

#endif