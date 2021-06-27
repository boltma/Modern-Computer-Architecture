//
// Created by lf-z on 3/16/17.
//

#include "DVFS.hh"
#include "debug/EnergyMgmt.hh"

DVFSSM::DVFSSM(const Params *p)
    : BaseEnergySM(p), state(DVFSSM::State::STATE_INIT),
      thres_off_low(p->thres_off_low), thres_low_mid(p->thres_low_mid), 
      thres_mid_high(p->thres_mid_high)
{

}

void DVFSSM::init()
{
    state = DVFSSM::State::STATE_OFF;
    EnergyMsg msg;
    msg.val = 0;
    msg.type = MsgType::POWEROFF;
    broadcastMsg(msg);
}

void DVFSSM::update(double _energy)
{
    EnergyMsg msg;
    msg.val = 0;

    if (state == STATE_INIT) {
        state = STATE_OFF;
    } else if (state != STATE_OFF && _energy < thres_off_low) {
        DPRINTF(EnergyMgmt, "[DVFS] State change: **->off state=%d, _energy=%lf, thres=%lf\n", state, _energy, thres_off_low);
        state = STATE_OFF;
        msg.type = MsgType::POWEROFF;
        broadcastMsg(msg);
    } else if (state != STATE_LVL_LOW && _energy > thres_off_low && _energy < thres_low_mid) {
        DPRINTF(EnergyMgmt, "[DVFS] State change: **->low state=%d, _energy=%lf\n", state, _energy);
        state = STATE_LVL_LOW;
        msg.type = MsgType::DVFS_LOW;
        broadcastMsg(msg);
    } else if (state != STATE_LVL_MIDDLE && _energy > thres_low_mid && _energy < thres_mid_high) {
        DPRINTF(EnergyMgmt, "[DVFS] State change: **->middle state=%d, _energy=%lf\n", state, _energy);
        state = STATE_LVL_MIDDLE;
        msg.type = MsgType::DVFS_MIDDLE;
        broadcastMsg(msg);
    } else if (state != STATE_LVL_HIGH && _energy > thres_mid_high) {
        DPRINTF(EnergyMgmt, "[DVFS] State change: **->high state=%d, _energy=%lf, thres=%lf\n", state, _energy, thres_mid_high);
        state = STATE_LVL_HIGH;
        msg.type = MsgType::DVFS_HIGH;
        broadcastMsg(msg);
    }


}

DVFSSM *
DVFSSMParams::create()
{
    return new DVFSSM(this);
}
