//
// Created by lf-z on 3/13/17.
//

#ifndef GEM5_STATE_MACHINE_HH
#define GEM5_STATE_MACHINE_HH

#include "sim/sim_object.hh"
#include "engy/energy_mgmt.hh"
#include "params/BaseEnergySM.hh"
#include "params/SimpleEnergySM.hh"

class BaseEnergySM : public SimObject
{
    /* Set EnergyMgmt as friend class so that mgmt can be initialized. */
    friend class EnergyMgmt;
public:
    typedef BaseEnergySMParams Params;
    const Params *params() const
    {
        return reinterpret_cast<const Params *>(_params);
    }
    BaseEnergySM(const Params *p);
    virtual ~BaseEnergySM() {}
    virtual void init() {}
    virtual void update(double _energy) {}

protected:
    void broadcastMsg(const EnergyMsg &msg);
    EnergyMgmt *mgmt;
};

class SimpleEnergySM : public BaseEnergySM
{
public:
    typedef SimpleEnergySMParams Params;
    const Params *params() const
    {
        return reinterpret_cast<const Params *>(_params);
    }
    SimpleEnergySM(const Params *p);
    virtual ~SimpleEnergySM() {}
    virtual void init();

    virtual void update(double _energy);

    enum State {
        STATE_INIT = 0,
        STATE_POWEROFF = 1,
        STATE_POWERON = 2
    };

    enum MsgType {
        CONSUMEENERGY = 0,
        POWEROFF = 1,
        POWERON = 2
    };

protected:
    State state;

};

#endif //GEM5_STATE_MACHINE_HH
