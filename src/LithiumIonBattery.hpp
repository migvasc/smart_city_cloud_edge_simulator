#ifndef LITHIUMIONBATTERY_HPP
#define LITHIUMIONBATTERY_HPP


class LithiumIonBattery
{

private:
    
    // Capacity in Wh
    double capacity =0;
    
    // Current state of charge (SoC) of the battery
    double stateOfCharge=0;

    //Current Level of Energy in watt-hour
    double level_of_energy = 0.0;

    double min_level_of_energy = 0.0;
    double max_level_of_energy = 0.0;

    // Current depth Of discharge (DoD) of the battery. Initially 1 because the battery is fully discharged (100%)
    double depthOfDischarge=1;

    // The maximum DoD that the battery can reach, to increase its lifetime
    double maxDoD=0;

    double chargeEfficiency=0; 
    double dischargeEfficiency=0;

    double oneHourInSeconds = 3600.0;

    // Ration of energy loss of the battery per day
    double selfDischargeRatePerDay = 0;

public:    
        
    explicit LithiumIonBattery();
    
    /**
     * Constructor for the lithium ion battery object
     * @param aCapacity: Battery capacity in Watthours
     * @param aMaxDoD: Maxium Depth of Discharge of the battery (between 0 and 1)
     * @param aChargeEff: Efficiency of the charging process (between 0 and 1)
     * @param adischargeEff: Efficiency of the discharging process ( between 0 and 1)
     */
    LithiumIonBattery(double aCapacity, double aMaxDoD, double aChargeEff, double aDischargeEff);

    /**
     * Simulates the charging process
     * 
     * @param power: the energy in Watts hour that will be charged (considering the efficiency)
     * 
     */
    void charge(double energy);

    /**
     * Simulates the discharging process
     * 
     * @param power: the energy in Watts that will be discharged
     * @return the energy discharged in Watt hour
     */
    double discharge(double energy);

    /**
     * Return the battery state of charge (SoC)
     * @return a value between 0 and 1 that represents how full the battery is (0 is 0% and 1is 100%)
     */
    double getSOC();
    
    /**
     * Return the battery depth of discharge (DoD)
     * @return a value between 0 and 1 that represents how much the battery has been discharged (0 is 0% and 1 is 100%)
     */
    double getDOD();
    double getCurrentCapacityInWattsHour();
    double getCapacityInWattsHour();
    bool canDischarge();
    double getUsableWattsHour();
    double getChargeEfficiency(); 
    double getDischargeEfficiency();
    /**
     * Sets the battery state of charge manually 
     * @param aSoC a double that represents the battery state of charge
     */
    void setSoC(double aSoC);
};


#endif