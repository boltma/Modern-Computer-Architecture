//
// Created by lf-z on 3/26/17.
//

#ifndef GEM5_HARVEST_HH
#define GEM5_HARVEST_HH

#include "sim/sim_object.hh"
#include "params/BaseHarvest.hh"
#include "params/SimpleHarvest.hh"

class BaseHarvest : public SimObject
{
public:
    typedef BaseHarvestParams Params;
    const Params *params() const
    {
        return reinterpret_cast<const Params *>(_params);
    }
    BaseHarvest(const Params *p) : SimObject(p) {}
    virtual ~BaseHarvest() {}
    virtual void init() {}
public:
    /* Return energy remained after harvesting energy */
    virtual double energy_harvest(double energy_harvested, double energy_remained)
    {
        return 0;
    }
};

class SimpleHarvest : public BaseHarvest
{
public:
    typedef SimpleHarvestParams Params;
    const Params *params() const
    {
        return reinterpret_cast<const Params *>(_params);
    }
    SimpleHarvest(const Params *p) : BaseHarvest(p) {}
    virtual ~SimpleHarvest() {}
    virtual void init();
public:
    double energy_harvest(double energy_harvested, double energy_remained);
};

#endif //GEM5_HARVEST_HH
