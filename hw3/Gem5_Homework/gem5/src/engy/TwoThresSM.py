from m5.SimObject import SimObject
from BaseEnergySM import BaseEnergySM
from m5.params import *
from m5.proxy import *

class TwoThresSM(BaseEnergySM):
    type = 'TwoThresSM'
    cxx_header = "engy/two_thres.hh"
    thres_high = Param.Float(Parent.thres_high, "high threshold of energy state machine")
    thres_low = Param.Float(Parent.thres_low, "low threshold of energy state machine")
