#include "DFS_LRY.hh"
#include "debug/EnergyMgmt.hh"

//这个全局变量用于通知energy_mgmt现在系统的状态
//不知道为啥energy_mgmt收不到msg，所以只能将就这么搞了
bool DFS_LRY_poweron_dirty_patch = false;


DFS_LRY::DFS_LRY(const Params *p)
    : BaseEnergySM(p), 
    	state(DFS_LRY::State::STATE_INIT),
    	thres_5_to_4(p->thres_5_to_4),
    	thres_4_to_3(p->thres_4_to_3),
    	thres_3_to_2(p->thres_3_to_2),
    	thres_2_to_1(p->thres_2_to_1),
    	thres_1_to_retention(p->thres_1_to_retention),
    	thres_retention_to_off(p->thres_retention_to_off),
    	thres_off_to_1(p->thres_off_to_1),
    	thres_retention_to_1(p->thres_retention_to_1),
    	thres_1_to_2(p->thres_1_to_2),
    	thres_2_to_3(p->thres_2_to_3),
    	thres_3_to_4(p->thres_3_to_4),
    	thres_4_to_5(p->thres_4_to_5)
{

}

void DFS_LRY::init()
{
    state = DFS_LRY::State::STATE_POWEROFF;
    EnergyMsg msg;
    msg.val = 0;
    msg.type = MsgType::POWEROFF;
    broadcastMsg(msg);
}

void DFS_LRY::update(double _energy)
{
    EnergyMsg msg;
    msg.val = 0;

//Initialization
    if (state == STATE_INIT)
    {
        state = STATE_POWEROFF;
    }

//change from higher freq to lower freq
    else if (state == STATE_FREQ5 && _energy < thres_5_to_4)
    {
        DPRINTF(EnergyMgmt, "State change: STATE_FREQ5->STATE_FREQ4 state=%d, _energy=%lf, thres_5_to_4=%lf\n", state, _energy, thres_5_to_4);
        state = STATE_FREQ4;
        msg.type = MsgType::FREQ5to4;
        broadcastMsg(msg);
    }

    else if (state == STATE_FREQ4 && _energy < thres_4_to_3)
    {
        DPRINTF(EnergyMgmt, "State change: STATE_FREQ4->STATE_FREQ3 state=%d, _energy=%lf, thres_4_to_3=%lf\n", state, _energy, thres_4_to_3);
        state = STATE_FREQ3;
        msg.type = MsgType::FREQ4to3;
        broadcastMsg(msg);
    }
    
    else if (state == STATE_FREQ3 && _energy < thres_3_to_2)
    {
        DPRINTF(EnergyMgmt, "State change: STATE_FREQ3->STATE_FREQ2 state=%d, _energy=%lf, thres_3_to_2=%lf\n", state, _energy, thres_3_to_2);
        state = STATE_FREQ2;
        msg.type = MsgType::FREQ3to2;
        broadcastMsg(msg);
    }
    
    else if (state == STATE_FREQ2 && _energy < thres_2_to_1)
    {
        DPRINTF(EnergyMgmt, "State change: STATE_FREQ2->STATE_FREQ1 state=%d, _energy=%lf, thres_2_to_1=%lf\n", state, _energy, thres_2_to_1);
        state = STATE_FREQ1;
        msg.type = MsgType::FREQ2to1;
        broadcastMsg(msg);
    }
    
//change from lower freq to higher freq
    else if (state == STATE_FREQ4 && _energy > thres_4_to_5)
    {
        DPRINTF(EnergyMgmt, "State change: STATE_FREQ4->STATE_FREQ5 state=%d, _energy=%lf, thres_4_to_5=%lf\n", state, _energy, thres_4_to_5);
        state = STATE_FREQ5;
        msg.type = MsgType::FREQ4to5;
        broadcastMsg(msg);
    }

    else if (state == STATE_FREQ3 && _energy > thres_3_to_4)
    {
        DPRINTF(EnergyMgmt, "State change: STATE_FREQ3->STATE_FREQ4 state=%d, _energy=%lf, thres_3_to_4=%lf\n", state, _energy, thres_3_to_4);
        state = STATE_FREQ4;
        msg.type = MsgType::FREQ3to4;
        broadcastMsg(msg);
    }
    
    else if (state == STATE_FREQ2 && _energy > thres_2_to_3)
    {
        DPRINTF(EnergyMgmt, "State change: STATE_FREQ2->STATE_FREQ3 state=%d, _energy=%lf, thres_2_to_3=%lf\n", state, _energy, thres_2_to_3);
        state = STATE_FREQ3;
        msg.type = MsgType::FREQ2to3;
        broadcastMsg(msg);
    }
    
    else if (state == STATE_FREQ1 && _energy > thres_1_to_2)
    {
        DPRINTF(EnergyMgmt, "State change: STATE_FREQ1->STATE_FREQ2 state=%d, _energy=%lf, thres_1_to_2=%lf\n", state, _energy, thres_1_to_2);
        state = STATE_FREQ2;
        msg.type = MsgType::FREQ1to2;
        broadcastMsg(msg);
    }

//beg retention
    else if (state == STATE_FREQ1 && _energy < thres_1_to_retention)
    {
        DPRINTF(EnergyMgmt, "State change: STATE_FREQ1->STATE_RETENTION state=%d, _energy=%lf, thres_1_to_retention=%lf\n", state, _energy, thres_1_to_retention);
        state = STATE_RETENTION;
        msg.type = MsgType::RETENTION_BEG;
        broadcastMsg(msg);
    }
    
//end retention
		else if (state == STATE_RETENTION && _energy > thres_retention_to_1)
    {
        DPRINTF(EnergyMgmt, "State change: STATE_RETENTION->STATE_FREQ1 state=%d, _energy=%lf, thres_retention_to_1=%lf\n", state, _energy, thres_retention_to_1);
        state = STATE_FREQ1;
        msg.type = MsgType::RETENTION_END;
        broadcastMsg(msg);
    }
    
//power on 
    else if (state == STATE_POWEROFF && _energy > thres_off_to_1)
    {
    		DFS_LRY_poweron_dirty_patch = true;
    		
        DPRINTF(EnergyMgmt, "State change: STATE_POWEROFF->STATE_FREQ1 state=%d, _energy=%lf, thres_off_to_1=%lf\n", state, _energy, thres_off_to_1);
        state = STATE_FREQ1;
        msg.type = MsgType::POWERON;
        broadcastMsg(msg);
    }

//pwoer off
    else if (state == STATE_RETENTION && _energy < thres_retention_to_off)
    {
    		DFS_LRY_poweron_dirty_patch = false;
    	    		
        DPRINTF(EnergyMgmt, "State change: STATE_RETENTION->STATE_POWEROFF state=%d, _energy=%lf, thres_retention_to_off=%lf\n", state, _energy, thres_retention_to_off);
        state = STATE_POWEROFF;
        msg.type = MsgType::POWEROFF;
        broadcastMsg(msg);
    }
}

DFS_LRY *
DFS_LRYParams::create()
{
    return new DFS_LRY(this);
}
