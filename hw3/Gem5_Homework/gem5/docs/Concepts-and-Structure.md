Concepts and Structure of gem5-NVP
===
This document file will introduce some basic ideas and structure 
used while developing gem5-NVP based on the original gem5 simulator. 
Ideally, by reading this file one can learn how to describe 
energy behavior of different modules with interfaces provided by gem5-NVP 
and finally be able to use gem5-NVP to simulate energy-aware systems.

# Contents
* <a href="#Overview">Overview</a>
* <a href="#Energy-Objects">Energy Objects</a>
  * <a href="#Energy-Objects-Members">Members</a>
  * <a href="#Energy-Objects-Interfaces">Interfaces</a>
* <a href="#Energy-Ports">Energy Ports</a>
  * <a href="#Energy-Ports-Types-of-Energy-Ports">Types of Energy Ports</a>
  * <a href="#Energy-Ports-Messages-Delivered-by-Energy-Port">Messages Delivered by Energy Port</a>
  * <a href="#Energy-Ports-How-to-Connect-Energy-Ports">How to Connect Energy Ports?</a>
* <a href="#Energy-Management-Module">Energy Management Module</a>
  * <a href="#Energy-Management-Module-Members-and-Interfaces">Members and Interfaces</a>
  * <a href="#Energy-Management-Module-State-Machine">State Machine</a>
  * <a href="#Energy-Management-Module-Harvesting-Module">Harvest Module</a>
  * <a href="#Energy-Management-Module-Python-Interfaces">Python Interfaces</a>
* <a href="#Other-Concepts">Other Concepts</a>
  * <a href="#Other-Concepts-Energy-Events">Energy Events</a>
  * <a href="#Other-Concepts-Energy-Profile">Energy Profile</a>
  * <a href="#Other-Concepts-Debug-Flags">Debug Flags</a>
  * <a href="#Other-Concepts-Statisitcs-and-Traces">Statistics and Traces</a>
* <a href="#Further-Reading">Further Reading</a>

<a name="Overview"/>

# Overview

This part mainly focuses on what is gem5-NVP, what features gem5-NVP has besides those of gem5, and how gem5-NVP achieves these functions.

##### What is gem5-NVP?

Gem5-NVP is a simulating tool based on gem5 simulator. At the very beginning, it is designed to simulate non-volatile systems. However, currently, it is proved to have capability of simulating all kinds of energy-aware behaviors, such as dynamic frequency according to energy remained, multi-level sleep behavior, etc.
 
##### What features gem5-NVP has?

Gem5-NVP provides each SimObject in gem5 the ability to consume energy and give respond to system's energy state changes. It also enables SimObjects to transfer energy to each other through a module called "Energy Port". Moreover, gem5-NVP provides a basic class for energy state controller in real energy-aware systems, named "EnergyMgmt". Users can use the interfaces of it to simulate custom power control behavior.

##### How gem5-NVP achieves these functions?

Gem5-NVP brings a new parent class to gem5's SimObject called "EnergyObject". EnergyObject has energy ports to transfer energy to other EnergyObjects, and also gives some interfaces to SimObject for consuming energy and responding to energy state changes.

Gem5-NVP introduces a new module (SimObject) in the system, named EnergyMgmt, as the power controller of the whole system. It communicates with other SimObjects through "Energy Ports". EnergyMgmt module handles the state changes of the whole system as well as the way the system harvests energy from external environment.

<a name="Energy-Objects"/>

# Energy Objects

Energy Object (aka "EnergyObject" in code) is a class that has members and 
interfaces related to energy behavior. EnergyObjects is implemented as one of 
SimObject's parent class, because every "Object" can have energy behavior.
EnergyObject empowers SimObjects to take actions when receiving energy messages 
(such as power-on, power-off, back-up, etc.). The following picture shows the 
inheritance diagram of SimObject.

[[images/sim_object.jpeg]]

<a name="Energy-Objects-Members"/>

### Members

|Member|Description|
|:-|:-|
|_seport|Slave Energy Port (see below) owned by the SimObject. It is used by the SimObject to connect energy management module as a consumer.|
|_meport|Master Energy Port (see below) owned by the SimObject. Normally only EnergyMgmt (see below) uses that port.|

<a name="Energy-Objects-Interfaces"/>

### Interfaces

|Interface|Description|
|:-|:-|
|setSimObject|Set owner for _seport and _meport, used at initialization stage. Not useful if not changing the structure of gem5-NVP.|
|getSlaveEnergyPort|Get _seport. Not useful if not changing the structure of gem5-NVP.|
|getMasterEnergyPort|Get _meport. Not useful if not changing the structure of gem5-NVP|
|consumeEnergy|Tell EnergyMgmt (see below) that this SimObject has consumed a certain amount of energy. It is used differently in EnergyMgmt module, but users don't need to care about it if not changing the structure of gem5-NVP.|
|handleMsg|Method to specify actions taken when a certain energy message is received. For instance, volatile SimObjects need to erase their data in case of power off, but non-volatile SimObjects only need to deschedule their events. Need to be rewritten by child classes.|

<a name="Energy-Ports"/>

# Energy Ports

Energy Ports (aka "EnergyPort" in code) is a class that is used by 
SimObjects (EnergyObjects) to communicate with each other. It is very 
similar to Port in the original gem5 (used by MemObjects to communicate 
with each other). Energy Ports can be inter-connected, and 
Energy Ports can send messages called EnergyMsg (see below) to 
connected Energy Ports. Normally, EnergyMsgs sent by master port are for 
notifying the slaves of state changes (power-on, power-off, back-up, etc) 
of the whole system, while reverse ones are for telling EnergyMgmt module 
that the SimObject is consuming a certain amount of energy in most time.

<a name="Energy-Ports-Types-of-Energy-Ports"/>

### Types of Energy Ports

There are two kinds of energy ports: master ports and slave ports. They can 
be connected with each other (master-slave), but cannot be connected within 
one kind (master-master, slave-slave). Functionally they have nearly no difference. 
The only different point of these two kinds is that one master port can be 
connected with multiple slave ports, while a slave port can only be connected 
to one master port. Master port has an array of slave ports it is connected with, 
and slave port saves the master port it is connected to.

<a name="Energy-Ports-Messages-Delivered-by-Energy-Port"/>

### Messages Delivered by Energy Port

Energy ports communicate with each other by sending a structure called EnergyMsg. 
```C
struct EnergyMsg
{
  int32_t type;
  double value;
};
```
This is a very simple data structure. "type" is used to determine which kind
of message itself is. In most time, what different values of "type" mean is 
defined by the state machine in EnergyMgmt module (see below). Actions taken 
by SimObjects should be in correspondence with certain energy state machine. 
Zero value of type is preserved because it means energy consuming no matter 
what energy state machine the whole system uses. The variance "value" is used 
to send energy values. For example, it means how much energy a SimObject has 
consumed when type is equal to zero.

<a name="Energy-Ports-How-to-Connect-Energy-Ports"/>

### How to Connect Energy Ports?

The next thing we need to concern is that how we determine how those SimObjects 
(with energy ports) is connected. This can be done in simulating scripts. 
Gem5-NVP has a swig interface in python to connect two SimObjects' energy ports.
```python
import m5.internal.pyobject connectEnergyPorts
# s1, s2: SimObjects in python.
connectEnergyPorts(s1.getCCObject(), s2.getCCObject)
```
The above code connects s1's master port with s2's slave port.

<a name="Energy-Management-Module"/>

# Energy Management Module

Energy Management module (aka EnergyMgmt in code) is a new kind of SimObject in 
gem5-NVP. Its function is to simulate the energy control module in real systems. 
It is able to simulate energy harvesting, voltage detection, system state control, 
and a lot of other functions in real systems. Normally EnergyMgmt is the only 
SimObject that uses the master port while running.

<a name="Energy-Management-Module-Members-and-Interfaces"/>

### Members and Interfaces

##### Members
|Member|Description|
|:-|:-|
|time_unit|Time unit of the energy profile.|
|energy_remained|Energy remained. Same function as capacitors in real non-volatile systems|
|energy_harvest_data|Energy data read from energy profile.|
|state_machine|The state machine the controls the system's energy state. (See below)|
|harvest_module|The module that determines how the system harvests energy from energy profile. (See below)|
|event_harvest|The event for energy harvesting. Happens per time unit.|
|event_msg|The event for broadcasting EnergyMsgs to slaves.|
|msg_togo|EnergyMsgs that are needed to broadcast to slaves at the current tick, which will be handled by event_msg.|
|path_energy_profile|The path to the energy profile.|

##### Interfaces
|Interface|Description|
|:-|:-|
|consumeEnergy|Derived from EnergyObject but used differently. Positive values mean energy is consumed (energy_remained goes down), while negative values mean that energy is harvested (energy_remained goes up).|
|broadcastMsg|The interface called when event_msg happens. It will broadcast a message to all the slaves.|
|broadcastAsEvent|Wrapper for broadcastMsg to make it into events. We broadcast messages in specify event because we don't want to mess up things in a single event.|
|handleMsg|The interface called when EnergyMgmt receives a message (Mostly energy consuming).|
|energyHarvest|The interface called when event_harvest happens. Harvest energy from energy_harvest_data/|
|readEnergyProfile|The function to read data from energy profile. Called at initialization stage.|

<a name="Energy-Management-Module-State-Machine"/>

### State Machine

In most non-volatile system, there is an energy state machine to control the system's 
power state. Gem5-NVP has an energy state machine object to control the system's state. 
There is a base state machine class called "BaseEnergySM" defining the basic interface an 
energy state machine should have. Users of gem5-NVP should over-write the interface 
to achieve a custom state controller they want. An additional function is provided to 
help the energy state machine to inform the system of state changes.
```C
void update(double _energy)
```
This interface is used for the system to tell the energy state machine that the remaining 
energy in the capacitor of the system has changed. Once the system harvests or consumes some 
energy, EnergyMgmt module will call this api to inform the state controller. Users should 
rewrite this api in their derived state machine class. The state maintained in the object 
may be changed or maintained according to the energy provided.
```C
void broadcastMsg(const EnergyMsg &msg);
```
This function is to broadcast a EnergyMsg to the whole system. This interface is
useful because in most time the state machine has to inform the system to perform some kinds 
of operation when the state is changed. If such notification is needed, "broadcastMsg" is 
called inside "update". Normally the field "type" in EnergyMsg is defined when designing 
energy state machine, and the operation other parts in the system take is implemented 
according to those message types.
##### Example
Here is an example of a very straight-forward energy state machine, which is the default 
energy state machine gem5-NVP provides. There are only 2 states, power-on and power-off in 
the state machine. Once the energy goes below zero, power-off message is sent to the whole 
system, and when energy becomes positive, power-on message is sent. Below is the "update" 
function of such energy state machine.
```C
void SimpleEnergySM::update(double _energy)
{
  EnergyMsg msg;
  msg.val = 0;

  if (state == STATE_INIT) {
  state = STATE_POWERON;
  } else if (state == STATE_POWERON && _energy < 0) {
  state = STATE_POWEROFF;
  msg.type = MsgType ::POWEROFF;
  broadcastMsg(msg);
  } else if (state == STATE_POWEROFF && _energy > 0) {
  state = STATE_POWERON;
  msg.type = MsgType::POWERON;
  broadcastMsg(msg);
  }
}
```
The derived class of the state machine is called "SimpleEnergySM" in code.

<a name="Energy-Management-Module-Harvesting-Module"/>

### Harvesting Module

Real-world systems have complicated energy harvest module, which includes voltage transform 
module, rectification module, etc. Gem5-NVP does not care those complex part of energy flow. 
It only receives energy profile which contains power amplitude at discrete time. The harvesting 
module only needs to tell how much energy can be harvested given one item in the energy profile. 
The following interface is what a user of gem5-NVP should understand.
```C
double energy_harvest(double energy_harvested, double energy_remained)
```
The interface gets the current remained energy in the capacitor and the energy item in energy 
profile to perform its calculation. The interface returns the remained energy in the capacitor 
after the system has harvested the energy provided in the energy profile.

##### Example
Like the energy state machine module, energy harvesting module also has a default derived class, 
named "SimpleHarvest". The "energy_harvest" method in it is shown below.
```C
double SimpleHarvest::energy_harvest(double energy_harvested, double energy_remained)
{
  return energy_remained + energy_harvested;
}
```
All the energy in the energy profile goes into the capacitor. Simple, isn't it? In real systems, 
it is really hard to harvest more energy once there is abundant energy in the capacitor, and such 
limitation can be described in the "energy_harvest" method.

<a name="Energy-Management-Module-Python-Interfaces"/>

### Python Interfaces

It is possible to control some of the behavior of EnergyMgmt module through python scripts as well as 
arguments in shell command. The "Get Started" page provides some examples, and users can checkout 
"configs/example/se_engy.py" to find out how to change the config in python script.

<a name="Other-Concepts"/>

# Other Concepts

There are some other concepts in gem5-NVP that is important.

<a name="Other-Concepts-Energy-Events"/>

### Energy Events

The state changes of the system is implemented through events. Once the state should be changed, the 
energy state machine put a "state change" event at current tick through "broadcastMsg". Energy state-change
event is designed to have lower priority than other events so that state changes happens after all other 
events at current tick finish. In this way, state changes would not happen inside one process and bring 
conflicts.

<a name="Other-Concepts-Energy-Profile"/>

### Energy Profile

Energy profile is a file that contains amplitude of external power source at discrete time points. All the 
values in the profile should be separated by line break or space.

<a name="Other-Concepts-Debug-Flags"/>

### Debug Flags

A new debug flag, "EnergyMgmt", is introduced into gem5 by gem5-NVP. Users can use the following command to 
activate the debug flag:
```Bash
build/ARM/gem5.debug --debug-flag=EnergyMgmt --debug-file=your_output_file ...
```
If you need to add new debug outputs into the flag, you can simply include "debug/EnergyMgmt.hh" in your 
code and use "DPRINTF" to print the outputs.
```C
#include "debug/EnergyMgmt.hh"

...
  DPRINTF(EnergyMgmt, "Hello!\n");
...

```

<a name="Other-Concepts-Statistics-and-Traces"/>

### Statistics and Traces

TBD

<a name="Further-Reading"/>

# Further Reading

 - [Introduction to SimObject](http://gem5.org/SimObjects)
 - [Introduction to Mem System](http://gem5.org/Memory_System)
 - [Another Introduction to Mem System](http://gem5.org/General_Memory_System)
 - [How to Add Your Own SimObject](http://learning.gem5.org/book/part2/helloobject.html)
 - [How to Add Simple MemObject](http://learning.gem5.org/book/part2/memoryobject.html)
 - [Creating your first SimObject](http://pages.cs.wisc.edu/~david/courses/cs752/Spring2015/gem5-tutorial/part2/simobject.html)
