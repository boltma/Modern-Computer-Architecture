//
// Created by lf-z on 12/28/16.
//

#ifndef GEM5_ENGY_HH
#define GEM5_ENGY_HH

#include "sim/sim_object.hh"
#include "params/EnergyMgmt.hh"

class BaseEnergySM;
class BaseHarvest;

class EnergyMgmt : public SimObject
{
public:
    typedef EnergyMgmtParams Params;
    const Params *params() const
    {
        return reinterpret_cast<const Params *>(_params);
    }
    EnergyMgmt(const Params *p);
    virtual ~EnergyMgmt();
    virtual void init();
    /* Harvest energy if val < 0 */
    virtual int consumeEnergy(double val);
    void broadcastMsg();
    int broadcastMsgAsEvent(const EnergyMsg &msg);
    int handleMsg(const EnergyMsg &msg);

		double energy_profile_mult;
protected:
    /* msg_togo is changed into a queue to prevent bugs in case that multiple state changes occurs in one tick. */
    std::vector<EnergyMsg> msg_togo;
    EventWrapper<EnergyMgmt, &EnergyMgmt::broadcastMsg> event_msg;
    std::vector<double> energy_harvest_data;
    void energyHarvest();
    EventWrapper<EnergyMgmt, &EnergyMgmt::energyHarvest> event_energy_harvest;
    BaseEnergySM *state_machine;
    BaseHarvest *harvest_module;

private:
    std::vector<double> readEnergyProfile();

};

#endif //GEM5_ENGY_HH
