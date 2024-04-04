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
	double next_green_power_production =0.0;
	
	/**
	 * The current green power production according to the input trace.
	 */
	double current_green_power_production=0.0;
	
	/**
	 * the last time the power production was updated.
	 */
	double time=0.0;
	
	/**
	 * the next time the power production will be updated.
	 */
	double next_time=0.0;

    /**
	 * Advance to the new update of the power production -i.e. change the current/next time/production according to the trace.
	 */
	void advance_time(double time);
	void load_CSV_file();

    // Efficiency of the solar panel to convert solar irradiance into electricity
    double efficiency = 1;
    // PV Panel area in square meters
    double area = 1.0;
public:    
	explicit PVPanel();
	explicit PVPanel(std::string fileName, double efficiency, double area);
	double get_current_green_power_production(double time);
};

#endif