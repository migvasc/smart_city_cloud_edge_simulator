#ifndef UTIL_HPP
#define UTIL_HPP
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

};
#endif