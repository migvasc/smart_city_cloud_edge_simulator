#include "LithiumIonBattery.hpp"

LithiumIonBattery::LithiumIonBattery(double aCapacity, double aMaxDoD, double aChargeEff, double aDischargeEff)
{
    capacity = aCapacity;
    maxDoD = aMaxDoD;
    stateOfCharge = 1-maxDoD;
    level_of_energy = stateOfCharge*capacity;// starts discharged
    min_level_of_energy = stateOfCharge * capacity;
    max_level_of_energy = maxDoD* capacity;
    chargeEfficiency = aChargeEff;
    dischargeEfficiency = aDischargeEff;    

}

void LithiumIonBattery::charge(double energy)
{    
    // Cannot overcharge the battery
    if (level_of_energy == max_level_of_energy) return;    


    level_of_energy += energy * chargeEfficiency;

    if (level_of_energy >= max_level_of_energy )
        level_of_energy = max_level_of_energy;

}

double LithiumIonBattery::discharge(double energy)
{    
    // Energy that will really be discharged from the batteries considering the efficiency
    double energy_to_be_discharged = energy/dischargeEfficiency;            
    

    if ( level_of_energy - energy_to_be_discharged < min_level_of_energy)
    {
        energy_to_be_discharged = level_of_energy - min_level_of_energy;
    }


    level_of_energy = level_of_energy - energy_to_be_discharged;

    stateOfCharge = level_of_energy/capacity;
    // Energy that we succed to discharge :
    double energy_discharged = energy_to_be_discharged * dischargeEfficiency;
    return energy_discharged;

}

double LithiumIonBattery::getSOC()
{
    return stateOfCharge;
}

double LithiumIonBattery::getDOD()
{
    return depthOfDischarge;
}

double LithiumIonBattery::getCurrentCapacityInWattsHour()
{  
    return stateOfCharge * capacity;
}

double LithiumIonBattery::getUsableWattsHour()
{  
    return (level_of_energy - min_level_of_energy) * dischargeEfficiency;
}

double LithiumIonBattery::getCapacityInWattsHour()
{  
    return capacity;
}

bool LithiumIonBattery::canDischarge()
{
    return level_of_energy >  (1-maxDoD)*capacity;
}

void LithiumIonBattery::setSoC(double aSoC)
{
    stateOfCharge = aSoC;
}


double LithiumIonBattery::getChargeEfficiency()
{
    return chargeEfficiency;
} 

double LithiumIonBattery::getDischargeEfficiency()
{
    return dischargeEfficiency;    
}


double LithiumIonBattery::getLevelOfEnergy()
{
    return level_of_energy;
}

void LithiumIonBattery::setLevelOfEnergy(double loe)
{
    level_of_energy = loe;
}
