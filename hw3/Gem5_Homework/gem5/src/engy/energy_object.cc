//
// Created by lf-z on 1/9/17.
//

#include "engy/energy_object.hh"
#include "engy/energy_port.hh"

void EnergyObject::setSimObject(SimObject *_sim)
{
    _seport.setOwner(_sim);
    _meport.setOwner(_sim);
}
int EnergyObject::consumeEnergy(double _energy)
{
    EnergyMsg msg;
    /* Zero is the default value of msg.type, indicating energy consuming*/
    msg.type = 0;
    msg.val = _energy;
    _seport.signalMsg(msg);
    return 1;
}

MasterEnergyPort& EnergyObject::getMasterEnergyPort()
{
    return _meport;
}

SlaveEnergyPort& EnergyObject::getSlaveEnergyPort()
{
    return _seport;
}

int EnergyObject::handleMsg(const EnergyMsg &msg)
{
    return 1;
}