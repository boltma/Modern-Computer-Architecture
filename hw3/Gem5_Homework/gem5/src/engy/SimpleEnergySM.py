from m5.SimObject import SimObject
from BaseEnergySM import BaseEnergySM
from m5.params import *
from m5.proxy import *

class SimpleEnergySM(BaseEnergySM):
    type = 'SimpleEnergySM'
    cxx_header = "engy/state_machine.hh"