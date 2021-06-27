//
// Created by lf-z on 3/16/17.
//

#ifndef GEM5_DVFS_HH
#define GEM5_DVFS_HH

#include "state_machine.hh"
#include "params/DVFSSM.hh"

class DVFSSM : public BaseEnergySM
{

public:
    typedef DVFSSMParams Params;
    const Params *params() const
    {
        return reinterpret_cast<const Params *>(_params);
    }
    DVFSSM(const Params *p);
    ~DVFSSM() {}
    virtual void init();
    virtual void update(double _energy);

    enum State {
        STATE_INIT = 0,
        STATE_OFF = 1,
        STATE_LVL_LOW = 2,
        STATE_LVL_MIDDLE = 3,
        STATE_LVL_HIGH = 4
    };

    enum MsgType {
        CONSUMEENERGY = 0,
        POWEROFF = 1,
        DVFS_LOW = 2,
        DVFS_MIDDLE = 3,
        DVFS_HIGH = 4
    };
    
    //static double rateLow = 0.5;
    //static double rateMiddle = 1.0;
    //static double rateHigh = 1.5;

protected:
    State state;
    double thres_off_low;
    double thres_low_mid;
    double thres_mid_high;

};
#endif //GEM5_DVFS_HH
