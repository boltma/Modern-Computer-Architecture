//
// Created by lf-z on 3/26/17.
//
#include "engy/harvest.hh"
#include "debug/EnergyMgmt.hh"

double SimpleHarvest::energy_harvest(double energy_harvested, double energy_remained)
{
    return energy_remained + energy_harvested;
}

void SimpleHarvest::init()
{
    DPRINTF(EnergyMgmt, "[EngyHarvest] simple energy harvest module is used in this system.\n");
}

BaseHarvest *
BaseHarvestParams::create()
{
    return new BaseHarvest(this);
}

SimpleHarvest *
SimpleHarvestParams::create()
{
    return new SimpleHarvest(this);
}
