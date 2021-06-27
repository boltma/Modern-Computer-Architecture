from m5.SimObject import SimObject
from m5.params import *
from m5.proxy import *
from SimpleEnergySM import SimpleEnergySM
from SimpleHarvest import SimpleHarvest

class EnergyMgmt(SimObject):
    type = 'EnergyMgmt'
    cxx_header = "engy/energy_mgmt.hh"      
    state_machine = Param.BaseEnergySM(SimpleEnergySM(), "energy state machine")
    harvest_module = Param.BaseHarvest(SimpleHarvest(), "energy harvesting module")