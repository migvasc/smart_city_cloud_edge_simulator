#include "ElectricityCO2eq.hpp"

 ElectricityCO2eq::ElectricityCO2eq() = default;
 ElectricityCO2eq::ElectricityCO2eq(std::string fileName)
 {		
    inputFile = fileName;        
    load_CSV_file();

} 

/*
Loads the CSV file containing the solar irradiation data in the following format:
instante_of_time,solar_irradiation
The instante_of_time is in the unit of seconds, and solar_irradiation in Watt-hour.
*/
void ElectricityCO2eq::load_CSV_file(){
    ifstream input = ifstream(inputFile);
    try {
        string  nextLine;
        // We skip the first line, as it only contains the header 
        getline(input,nextLine);
        //Example of header from electricity map:
        //Datetime (UTC),Country,Zone Name,Zone Id,Carbon Intensity gCO₂eq/kWh (direct),Carbon Intensity gCO₂eq/kWh (LCA),Low Carbon Percentage,Renewable Percentage,Data Source,Data Estimated,Data Estimation Method
        int hour = 0;

        while(getline(input,nextLine))
        {                                    
            std::vector<std::string> nextInput;				
            boost::algorithm::split(nextInput, nextLine, boost::is_any_of(","));
            pow_co2_eq_time_series[hour]= atof(nextInput[5].c_str());
            hour++;
        }
        input.close();
    } catch (const std::exception& e) 
    {            
        cout << "Error at ElectricityCO2eq  load_CSV_file " << simgrid::s4u::this_actor::get_name()  << " " << e.what() << endl;
    }		
}


double ElectricityCO2eq::get_current_co2_eq(double simulation_time){
    int hour =   (((int)simulation_time) / 3600) ;
    double co2_eq = 0.0;
    if (pow_co2_eq_time_series.find(hour)!=pow_co2_eq_time_series.end())
    {
        co2_eq = pow_co2_eq_time_series[hour];
    }
    return co2_eq;
}
