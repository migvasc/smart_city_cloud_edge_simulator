#ifndef UTIL_HPP
#define UTIL_HPP

#include <stdlib.h>
#include <cmath>  
class Util{

public:

    static double convert_wh_to_joules(double energy_wh)
    {
    return energy_wh * 3600;
    }

    static double convert_joules_to_wh(double energy_joules)
    {
    return energy_joules/ 3600;
    }

    // Return the latency value in milisseconds
    static double getNetworkLatencyVivaldi(double x1, double y1, double z1, double x2, double y2, double z2)
    {
        double z_values = 0.0;
        if (z1!=z2)
        {
            z_values = z1+ z2;
        }
        return (sqrt( pow(x1-x2,2) + pow( y1-y2,2))  + z_values ) * 1/1000;

        
    }
};
#endif