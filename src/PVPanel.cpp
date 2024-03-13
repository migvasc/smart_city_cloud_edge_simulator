#include "PVPanel.hpp"

 PVPanel::PVPanel() = default;
 PVPanel::PVPanel(std::string fileName){		
   
    inputFile = fileName;        
    time = 0;                  
    next_time=-1;  
    load_CSV_file();
    current_green_power_production = 0;
    advance_time(0);

} 


void PVPanel::load_CSV_file(){
    ifstream input = ifstream(inputFile);
    try {
        string  nextLine;
        while(getline(input,nextLine))
        {                                    
            std::vector<std::string> nextInput;				
            boost::algorithm::split(nextInput, nextLine, boost::is_any_of(";"));
            green_power_production.push_back(make_pair(atof(nextInput[0].c_str()),atof(nextInput[1].c_str())));
        }
        input.close();
    } catch (const std::exception& e) 
    {            
        cout << "Error at PVPanel advance_time " << simgrid::s4u::this_actor::get_name()  << " " << e.what() << endl;
    }		
}

/**
 * Advance to the new update of the power production -i.e. change the current/next time/production according to the trace.
 */
void PVPanel::advance_time(double simulation_time) {
     simgrid::s4u::CommPtr ptr = nullptr;
    try {
        string  nextLine;
        while(next_time<= simulation_time)
        {            
            if(green_power_production.empty()) break;
            current_green_power_production = next_green_power_production;        
            std::pair<double,double>  data = green_power_production[0];            
            time = next_time;
            next_time = data.first;
            next_green_power_production = data.second;            
            green_power_production.erase(green_power_production.begin());
        }
    } catch (const std::exception& e) 
    {            
        cout << "Error at PVPanel advance_time " << simgrid::s4u::this_actor::get_name()  << " " << e.what() << endl;
    }		
      
}


double PVPanel::get_current_green_power_production(double simulation_time){
    if(((int)simulation_time ) % 300 != 0)
    {
        double newTime = (1 + ((int)simulation_time ) / 300) *300; 
        simulation_time = newTime;
    }
    advance_time(simulation_time);
    return current_green_power_production;
}
