//
// Created by lf-z on 3/16/17.
//

#ifndef GEM5_ModeCvrt_HH
#define GEM5_ModeCvrt_HH

#include "state_machine.hh"
#include "params/ModeCvrtSM.hh"

class ModeCvrtSM : public BaseEnergySM
{

public:
    typedef ModeCvrtSMParams Params;
    const Params *params() const
    {
        return reinterpret_cast<const Params *>(_params);
    } 
    ModeCvrtSM(const Params *p);
    ~ModeCvrtSM() {}
    virtual void init();
    virtual void update(double _energy);

    enum State {
        STATE_INIT = 0,
        STATE_OFF = 1,
        STATE_CPU = 2,
        STATE_SENSOR = 3,
        STATE_RF = 4
    };

    enum MsgType {
        CONSUMEENERGY = 0,
        POWEROFF = 1,
        ModeCvrt_CPU = 2,
        ModeCvrt_SENSOR = 3,
        ModeCvrt_RF = 4
    };

protected:
    State state;
    double thres_off_low;
    double thres_low_mid;
    double thres_mid_high;

};
#endif //GEM5_ModeCvrt_HH
