#ifndef ENERGYSTORAGEDEVICE_HPP
#define ENERGYSTORAGEDEVICE_HPP

class EnergyStorageDevice{

public:
    virtual void charge(double power,double time) =0;
    virtual double discharge(double power,double time)=0;
    virtual double getSOC()=0;
    virtual double getDOD()=0;
    virtual double getCurrentCapacityInWattsHour()=0;
    virtual double getCapacityInWattsHour()=0;
};


#endif
