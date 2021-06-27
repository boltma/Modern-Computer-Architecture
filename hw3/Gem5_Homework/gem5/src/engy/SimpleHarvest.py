from m5.SimObject import SimObject
from BaseHarvest import BaseHarvest
from m5.params import *
from m5.proxy import *

class SimpleHarvest(BaseHarvest):
    type = 'SimpleHarvest'
    cxx_header = "engy/harvest.hh"