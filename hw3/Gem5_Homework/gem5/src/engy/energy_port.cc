//
// Created by lf-z on 1/6/17.
//

#include "engy/energy_port.hh"
#include "sim/sim_object.hh"
#include "debug/EnergyMgmt.hh"

int EnergyPort::handleMsg(EnergyMsg msg)
{
    return owner->handleMsg(msg);
}

int MasterEnergyPort::bindSlave(SlaveEnergyPort &_slave)
{
    slave_list.push_back(&_slave);
    _slave.setMaster(*this);
    return 1;
}

int MasterEnergyPort::broadcastMsg(EnergyMsg msg)
{
    int rlt = 1;
    unsigned long len = slave_list.size();

    if (len == 0)
        rlt = 0;

    for (int i = 0; i < len; i++) {
        if (!slave_list[i]->handleMsg(msg)) {
            rlt = 0;
        }
    }
    return rlt;
}

int SlaveEnergyPort::setMaster(MasterEnergyPort &_master)
{
    master = &_master;
    DPRINTF(EnergyMgmt, "[EngyPort] energy port connected. master: %s, slave: %s\n",
            master->owner->name().c_str(), owner->name().c_str());
    return 1;
}

int SlaveEnergyPort::signalMsg(EnergyMsg msg)
{
    if (!master)
        return 0;

    return master->handleMsg(msg);
}