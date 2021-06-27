//
// Created by lf-z on 1/9/17.
//

#ifndef GEM5_ENERGY_OBJECT_HH
#define GEM5_ENERGY_OBJECT_HH

#include "engy/energy_port.hh"

class SimObject;

class EnergyObject {
protected:
    // An energy object can be both consumer or provider,
    // so we have both slave port and master port.
    SlaveEnergyPort _seport;
    MasterEnergyPort _meport;
public:
    EnergyObject()
    { }

    // set SimObject of _seport and _meport
    void setSimObject(SimObject *_sim);

    SlaveEnergyPort& getSlaveEnergyPort();
    MasterEnergyPort& getMasterEnergyPort();

    // A method telling energy mgmt module this object has consume energy.
    // But it is used to do real changes to power amount in capacitor in energy_mgmt module
    virtual int consumeEnergy(double _energy);

    // Methods to implement actions taken when power on/off by child classes.
    virtual int handleMsg(const EnergyMsg &msg);
};
#endif //GEM5_ENERGY_OBJECT_HH
