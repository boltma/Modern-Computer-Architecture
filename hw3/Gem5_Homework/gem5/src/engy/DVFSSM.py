from m5.SimObject import SimObject
from BaseEnergySM import BaseEnergySM
from m5.params import *
from m5.proxy import *

class DVFSSM(BaseEnergySM):
    type = 'DVFSSM'
    cxx_header = "engy/DVFS.hh"
    thres_off_low = Param.Float(Parent.thres_off_low, "thres_off_low threshold of DVFS state machine")
    thres_low_mid = Param.Float(Parent.thres_low_mid, "thres_low_mid threshold of DVFS state machine")
    thres_mid_high = Param.Float(Parent.thres_mid_high, "thres_mid_high threshold of DVFS state machine")
 