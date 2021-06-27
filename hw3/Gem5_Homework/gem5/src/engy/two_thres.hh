//
// Created by lf-z on 3/16/17.
//

#ifndef GEM5_TWO_THRES_HH
#define GEM5_TWO_THRES_HH

#include "state_machine.hh"
#include "params/TwoThresSM.hh"

class TwoThresSM : public BaseEnergySM
{

public:
    typedef TwoThresSMParams Params;
    const Params *params() const
    {
        return reinterpret_cast<const Params *>(_params);
    }
    TwoThresSM(const Params *p);
    ~TwoThresSM() {}
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
    double thres_high;
    double thres_low;

};
#endif //GEM5_TWO_THRES_HH
