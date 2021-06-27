from m5.SimObject import SimObject
from m5.params import *
from m5.proxy import *

class BaseEnergySM(SimObject):
    type = 'BaseEnergySM'
    cxx_header = "engy/state_machine.hh"