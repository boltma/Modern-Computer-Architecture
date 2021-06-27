Example: Simulating a System with Two-Threshold Energy State Machine
===

This page will introduce an example on how to simulate a non-volatile system with a two-threshold energy state machine using gem5-NVP. To get the example more simple, we will use AtomicSimpleCPU as the cpu model, and ignore how the back-up module restores the value in the memory system. THU-1010n is the real-world prototype of such system. We will begin by introducing THU-1010n first.

# Introduction to THU-1010n

THU-1010n is a single-cycle non-volatile MCU prototype fabricated using .13um CMOS-ferroelectric hybrid process. It has 8KB SRAM as memory together with a 1607-bit FeFF non-volatile storage to ensure critical data can be stored in case of lacking power. Below is some properties of THU-1010n.

In this simulation, we assume that the micro processor works at 1MHz with a 160uW active power. At that frequency, the MCU uses 7 cycles to store data and fall into sleep mode and uses 3 cycles to wake up from sleep. We would simplify the state controller of THU-1010n in this simulation. Two thresholds of energy remained in the capacitor decides when the system will switch from power-off to power-on and from power-on to power-off.

# Creating Two-Threshold Energy State Machine

We need to create a two-threshold energy state machine mentioned above. All instances of energy state machine in gem5-NVP is derived from BaseEnergySM. Gem5-NVP uses SimpleEnergySM as its default state machine, and that state machine cannot meet our demand in this case. Therefore, we are going to derive another state machine class and override "update" method to achieve a two-thres behavior.

### Python Side
Each SimObject has a corresponding class on the python side, and so does our state machine class. On the python side, we need to inherit all the parameters from BaseEnergySM class and add two parameters named "thres_low" and "thres_high" furthermore. In this way, we can control the threshold from simulating scripts. Below is the code on python side:

<b>src/engy/TwoThresSM.py</b>
```python
from m5.SimObject import SimObject
from BaseEnergySM import BaseEnergySM
from m5.params import *
from m5.proxy import *

class TwoThresSM(BaseEnergySM):
    type = 'TwoThresSM'
    cxx_header = "engy/two_thres.hh"
    thres_high = Param.Float(Parent.thres_high, "high threshold of energy state machine")
    thres_low = Param.Float(Parent.thres_low, "low threshold of energy state machine")
```

### CPP Side
When creating new SimObject, there are some formality sakes that need to be implemented in order to pass the python parameters to CPP code. We don't need to fully understand those codes, and we should only focus on the critical part. Below is the CPP code for the class:

<b>src/engy/two_thres.hh</b>
```CPP
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
```

<b>src/engy/two_thres.cc</b>
```CPP
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
        DPRINTF(EnergyMgmt, "State change: on->off state=%d, _energy=%lf, thres_low=%lf\n", state, _energy, thres_low);
        state = STATE_POWEROFF;
        msg.type = MsgType::POWEROFF;
        broadcastMsg(msg);
    } else if (state == STATE_POWEROFF && _energy > thres_high) {
        DPRINTF(EnergyMgmt, "State change: off->on state=%d, _energy=%lf, thres_high=%lf\n", state, _energy, thres_high);
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
```

### SConscript
Next we should add something to SConscript to tell scons about our changes.

<b>src/engy/SConscript</b>
```python
Import('*')

Source('energy_mgmt.cc')
Source('energy_port.cc')
Source('energy_object.cc')
Source('state_machine.cc')
Source('two_thres.cc')

SimObject('EnergyMgmt.py')
SimObject('BaseEnergySM.py')
SimObject('SimpleEnergySM.py')
SimObject('TwoThresSM.py')

DebugFlag('EnergyMgmt')
```

# Implementing the Actions Taken When State Has Changed
Next, we should tell other parts of the system what they should do when the system's state changes. We are using AtomicSimpleCPU in the simulation, and in that CPU module all the processes of the system are triggered by AtomicSimpleCPU::tick() method. Since we don't care about details like how back-up module store the values in SRAM, we can only implement actions of AtomicSimpleCPU in case of state changes. We assume that all the critical data can be properly saved and restored when power-off and power-on occur, so the process of the CPU seems to be "delayed" when the system goes from power-on to power-off. Therefore, the action taken when receiving power-off message is descheduling the "tick" event, the one taken when receiving power-on message is scheduling "tick" event. In the following code, we carefully take restore/back-up time into consideration.

<b>src/cpu/simple/atomic.cc</b>
```CPP

...

#include "engy/two_thres.hh"

...

void
AtomicSimpleCPU::tick()
{
    DPRINTF(SimpleCPU, "Tick\n");

    consumeEnergy(16);

    ...

}

...

int
AtomicSimpleCPU::handleMsg(const EnergyMsg &msg)
{
    int rlt = 1;
    DPRINTF(EnergyMgmt, "handleMsg called at %lu, msg.type=%d\n", curTick(), msg.type);
    switch(msg.type){
        case (int) TwoThresSM::MsgType::POWEROFF:
            if (tickEvent.scheduled())
                consumeEnergy(160*7);
                deschedule(tickEvent);
            break;
        case (int) TwoThresSM::MsgType::POWERON:
            if (!tickEvent.scheduled())
                consumeEnergy(160*3);
                schedule(tickEvent, curTick() + 3 * clockPeriod());
            break;
        default:
            rlt = 0;
    }
    return rlt;
}

...
```

# Changing Simulating Scripts
Make some changes to simulating scripts, and link our energy state machine to the energy management module. Also, we can add some options to scripts to make simulation easier.

<b>configs/example/se_engy.py</b>
```python
...
system.energy_mgmt = EnergyMgmt(path_energy_profile = options.energy_profile,
                                energy_time_unit = options.energy_time_unit,
                                energy_modules = options.energy_modules,
                                state_machine = TwoThresSM(thres_high = options.thres_high,
				thres_low = options.thres_low))
...
```

<b>configs/common/Options.py</b>
```python
...
def addEngyOptions(parser):
	# Energy Management Options

...

    parser.add_option("--thres-high", type="float", default=100,
                      help="high threshold of energy state machine")
    parser.add_option("--thres-low", type="float", default=50,
                      help="low threshold of energy state machine")

...
```

# Simulation
### Build System
From the gem5 repository, run:
```Bash
scons build/ARM/gem5.debug
```

### Run Simulation
From the gem5 repository, run:
```Bash
build/ARM/gem5.debug --debug-flags=EnergyMgmt --debug-file=a.out configs/example/se_engy.py -c tests/test-progs/hello/bin/arm/linux/hello --cpu-clock='1MHz' --energy-modules='cpu' --thres-low=5000 --thres-high=10000 --energy-prof=./profile/energy_prof --energy-time-unit=10000000
```

