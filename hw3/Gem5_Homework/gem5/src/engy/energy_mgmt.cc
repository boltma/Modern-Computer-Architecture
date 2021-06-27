//
// Created by lf-z on 12/28/16.
//
#include <fstream>
#include <math.h>
#include "engy/energy_mgmt.hh"
#include "engy/state_machine.hh"
#include "engy/harvest.hh"
#include "debug/EnergyMgmt.hh"
#include "sim/eventq.hh"
#include "engy/DFS_LRY.hh"

EnergyMgmt::EnergyMgmt(const Params *p)
        : SimObject(p),
          event_msg(this, false, Event::Energy_Pri),
          event_energy_harvest(this, false, Event::Energy_Pri),
          state_machine(p->state_machine),
          harvest_module(p->harvest_module)
{
    msg_togo.resize(0);
}

EnergyMgmt::~EnergyMgmt()
{

}

void EnergyMgmt::init()
{
    /* Read energy profile. */
    energy_harvest_data = readEnergyProfile();
    /* Set mgmt pointer in state machine. */
    if (state_machine) {
        state_machine->mgmt = this;
    }

    /*DPRINTF(EnergyMgmt, "[EngyMgmt] Energy Management module initialized!\n");
    DPRINTF(EnergyMgmt, "[EngyMgmt] Energy profile: %s (Time unit: %d ticks)\n",
            _path_energy_profile.c_str(), time_unit);*/

    /* Trigger first energy harvest event here */
    //energyHarvest();
    state_machine->update(100);
}

int EnergyMgmt::consumeEnergy(double val)
{
    //state_machine->update(energy_remained);
    return 1;
}

void EnergyMgmt::broadcastMsg()
{
    /* Broadcast the first message in the msg queue. */
    _meport.broadcastMsg(msg_togo[0]);
    /* Delete the message we broadcast. */
    msg_togo.erase(msg_togo.begin());
    /* If there are still messages, schedule a new event. */
    if (!msg_togo.empty())
        schedule(event_msg, curTick());
}

int EnergyMgmt::broadcastMsgAsEvent(const EnergyMsg &msg)
{
    msg_togo.push_back(msg);
    /* Trigger first msg in the current tick. */
    if (msg_togo.size() == 1)
        schedule(event_msg, curTick());
    return 1;
}

int EnergyMgmt::handleMsg(const EnergyMsg &msg)
{
		DPRINTF(EnergyMgmt, "[EnergyMgmt] handleMsg called at %lu, msg.type=%d\n", curTick(), msg.type);
    /* msg type should be 0 here, for 0 represents energy consuming, */
    /* and EnergyMgmt module can only handle energy consuming msg*/
    if (msg.type != DFS_LRY::MsgType::CONSUMEENERGY)
        return 0;

    return consumeEnergy(msg.val);
}

std::vector<double> EnergyMgmt::readEnergyProfile()
{
    std::vector<double> data;
    data.resize(0);
    return data;
}

void EnergyMgmt::energyHarvest()
{
    return;
}

EnergyMgmt *
EnergyMgmtParams::create()
{
    return new EnergyMgmt(this);
}
