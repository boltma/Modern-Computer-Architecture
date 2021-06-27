//
// Created by lf-z on 1/6/17.
//

#ifndef GEM5_ENERGY_PORT_HH
#define GEM5_ENERGY_PORT_HH

#include <string>
#include <vector>

struct EnergyMsg
{
    int32_t type;
    double val;
};

enum EnergyPortType
{
    NOT_DECIDED = 0,
    MASTERPORT = 1,
    SLAVEPORT = 2
};

class SimObject;

class EnergyPort
{
protected:

    int port_id;
    std::string port_name;
    EnergyPortType port_type;

public:

    /* owner needs to be set when SimObject is created. */
    SimObject *owner;

    EnergyPort(SimObject *_owner = NULL)
            : port_id(-1), port_name(""), port_type(NOT_DECIDED), owner(_owner)
    { }

    void setOwner(SimObject *_owner)
    {
        owner = _owner;
    }

    void setPortId(int _port_id)
    {
        port_id = _port_id;
    }

    void setPortName(std::string _port_name)
    {
        port_name = _port_name;
    }

    int getPortId()
    {
        return port_id;
    }

    std::string getPortName()
    {
        return port_name;
    }

    int handleMsg(EnergyMsg msg);
};

class SlaveEnergyPort;

class MasterEnergyPort : public EnergyPort
{
    //friend class SlaveEnergyPort;
protected:
    std::vector<SlaveEnergyPort*> slave_list;

public:
    MasterEnergyPort(SimObject *_owner = NULL)
            : EnergyPort(_owner)
    {
        slave_list.resize(0);
        port_type = MASTERPORT;
    }

    int bindSlave(SlaveEnergyPort& _slave);
    int broadcastMsg(EnergyMsg msg);
};

class SlaveEnergyPort : public EnergyPort
{
    //friend class MasterEnergyPort;
protected:
    MasterEnergyPort* master;

public:
    SlaveEnergyPort(SimObject *_owner = NULL)
            : EnergyPort(_owner), master(NULL)
    {
        port_type = SLAVEPORT;
    }

    int setMaster(MasterEnergyPort& _master);
    int signalMsg(EnergyMsg msg);

};

#endif //GEM5_ENERGY_PORT_HH
