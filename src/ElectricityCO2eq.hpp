#ifndef ELECTRICITYCO2EQ_HPP
#define ELECTRICITYCO2EQ_HPP

#include <simgrid/s4u.hpp>
#include <string>
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <stdlib.h>
#include <limits>

using namespace std;

class ElectricityCO2eq 
{
	/**
	 * The input file containing the power production trajectory.
	 */
    string inputFile;
	
    std::map<int,double> pow_co2_eq_time_series;
	void load_CSV_file();

public:
	explicit ElectricityCO2eq();
	explicit ElectricityCO2eq(std::string fileName);
	double get_current_co2_eq(double time);
};

#endif