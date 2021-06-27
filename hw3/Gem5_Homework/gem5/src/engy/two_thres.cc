//
// Created by lf-z on 3/16/17.
//

#include "two_thres.hh"
#include "debug/EnergyMgmt.hh"

TwoThresSM::TwoThresSM(const Params *p)
    : BaseEnergySM(p), state(TwoThresSM::State::STATE_INIT),
      thres_high(p->thres_high), thres_low(p->thres_low)
{

}

void TwoThresSM::init()
{
    state = TwoThresSM::State::STATE_POWEROFF;
    EnergyMsg msg;
    msg.val = 0;
    msg.type = MsgType::POWEROFF;
    broadcastMsg(msg);
}

void TwoThresSM::update(double _energy)
{
    EnergyMsg msg;
    msg.val = 0;

    if (state == STATE_INIT) {
        state = STATE_POWERON;
    } else if (state == STATE_POWERON && _energy < thres_low) {
        DPRINTF(EnergyMgmt, "[TwoThresSM] State change: on->off state=%d, _energy=%lf, thres_low=%lf\n", state, _energy, thres_low);
        state = STATE_POWEROFF;
        msg.type = MsgType::POWEROFF;
        broadcastMsg(msg);
    } else if (state == STATE_POWEROFF && _energy > thres_high) {
        DPRINTF(EnergyMgmt, "[TwoThresSM] State change: off->on state=%d, _energy=%lf, thres_high=%lf\n", state, _energy, thres_high);
        state = STATE_POWERON;
        msg.type = MsgType::POWERON;
        broadcastMsg(msg);
    }

}

TwoThresSM *
TwoThresSMParams::create()
{
    return new TwoThresSM(this);
}
