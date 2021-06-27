from m5.SimObject import SimObject
from m5.params import *
from m5.proxy import *

class BaseHarvest(SimObject):
    type = 'BaseHarvest'
    cxx_header = "engy/harvest.hh"