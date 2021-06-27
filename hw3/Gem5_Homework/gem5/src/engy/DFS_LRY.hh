#ifndef GEM5_DFS_LRY_HH
#define GEM5_DFS_LRY_HH

#include "state_machine.hh"
#include "params/DFS_LRY.hh"

class DFS_LRY : public BaseEnergySM
{

public:
    typedef DFS_LRYParams Params;
    const Params *params() const
    {
        return reinterpret_cast<const Params *>(_params);
    }
    DFS_LRY(const Params *p);
    ~DFS_LRY() {}
    virtual void init();
    virtual void update(double _energy);

    enum State {
        STATE_INIT = 0,
        STATE_POWEROFF = 1,
        STATE_RETENTION = 2,
        STATE_FREQ1 = 3,
        STATE_FREQ2 = 4,
        STATE_FREQ3 = 5,
        STATE_FREQ4 = 6,
        STATE_FREQ5 = 7,
    };

    enum MsgType {
        CONSUMEENERGY = 0,
        POWEROFF = 1,
        POWERON = 2,
        FREQ1to2 = 3,
        FREQ2to3 = 4,
        FREQ3to4 = 5,
        FREQ4to5 = 6,
        FREQ5to4 = 7,
        FREQ4to3 = 8,
        FREQ3to2 = 9,
        FREQ2to1 = 10,
        RETENTION_BEG = 11,
        RETENTION_END = 12
    };

protected:
    State state;
    double thres_5_to_4;
    double thres_4_to_3;
    double thres_3_to_2;
    double thres_2_to_1;
    double thres_1_to_retention;
    double thres_retention_to_off;
    double thres_off_to_1;
    double thres_retention_to_1;
		double thres_1_to_2;
		double thres_2_to_3;
		double thres_3_to_4;
		double thres_4_to_5;
};
#endif //GEM5_TWO_THRES_HH
