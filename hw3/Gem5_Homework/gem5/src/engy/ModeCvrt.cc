//
// Created by lf-z on 3/16/17.
//

#include "ModeCvrt.hh"
#include "debug/EnergyMgmt.hh"

ModeCvrtSM::ModeCvrtSM(const Params *p)
    : BaseEnergySM(p), state(ModeCvrtSM::State::STATE_INIT),
      thres_off_low(p->thres_off_low), thres_low_mid(p->thres_low_mid), 
      thres_mid_high(p->thres_mid_high)
{

}

void ModeCvrtSM::init()
{
    state = ModeCvrtSM::State::STATE_OFF;
    EnergyMsg msg;
    msg.val = 0;
    msg.type = MsgType::POWEROFF;
    broadcastMsg(msg);
}

void ModeCvrtSM::update(double _energy)
{
    EnergyMsg msg;
    msg.val = 0;

    if (state == STATE_INIT) {
        state = STATE_OFF;
    } else if (state != STATE_OFF && _energy < thres_off_low) {
        DPRINTF(EnergyMgmt, "[ModeCvrt] State change: **->off state=%d, _energy=%lf, thres=%lf\n", state, _energy, thres_off_low);
        state = STATE_OFF;
        msg.type = MsgType::POWEROFF;
        broadcastMsg(msg);
    } else if (state != STATE_LVL_LOW && _energy > thres_off_low && _energy < thres_low_mid) {
        DPRINTF(EnergyMgmt, "[ModeCvrt] State change: **->low state=%d, _energy=%lf\n", state, _energy);
        state = STATE_LVL_LOW;
        msg.type = MsgType::ModeCvrt_LOW;
        broadcastMsg(msg);
    } else if (state != STATE_LVL_MIDDLE && _energy > thres_low_mid && _energy < thres_mid_high) {
        DPRINTF(EnergyMgmt, "[ModeCvrt] State change: **->middle state=%d, _energy=%lf\n", state, _energy);
        state = STATE_LVL_MIDDLE;
        msg.type = MsgType::ModeCvrt_MIDDLE;
        broadcastMsg(msg);
    } else if (state != STATE_LVL_HIGH && _energy > thres_mid_high) {
        DPRINTF(EnergyMgmt, "[ModeCvrt] State change: **->high state=%d, _energy=%lf, thres=%lf\n", state, _energy, thres_mid_high);
        state = STATE_LVL_HIGH;
        msg.type = MsgType::ModeCvrt_HIGH;
        broadcastMsg(msg);
    }


}

ModeCvrtSM *
ModeCvrtSMParams::create()
{
    return new ModeCvrtSM(this);
}
