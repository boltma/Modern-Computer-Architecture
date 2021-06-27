from m5.SimObject import SimObject
from BaseEnergySM import BaseEnergySM
from m5.params import *
from m5.proxy import *

class DFS_LRY(BaseEnergySM):
    type = 'DFS_LRY'
    cxx_header = "engy/DFS_LRY.hh"

    thres_5_to_4 = Param.Float(Parent.thres_5_to_4 ,"energy state machine threshold")
    thres_4_to_3 = Param.Float(Parent.thres_4_to_3 ,"energy state machine threshold")
    thres_3_to_2 = Param.Float(Parent.thres_3_to_2 ,"energy state machine threshold")
    thres_2_to_1 = Param.Float(Parent.thres_2_to_1 ,"energy state machine threshold")
    thres_1_to_retention = Param.Float(Parent.thres_1_to_retention ,"energy state machine threshold")
    thres_retention_to_off = Param.Float(Parent.thres_retention_to_off ,"energy state machine threshold")
    thres_off_to_1 = Param.Float(Parent.thres_off_to_1 ,"energy state machine threshold")
    thres_retention_to_1 = Param.Float(Parent.thres_retention_to_1 ,"energy state machine threshold")
    thres_1_to_2 = Param.Float(Parent.thres_1_to_2 ,"energy state machine threshold")
    thres_2_to_3 = Param.Float(Parent.thres_2_to_3 ,"energy state machine threshold")
    thres_3_to_4 = Param.Float(Parent.thres_3_to_4 ,"energy state machine threshold")
    thres_4_to_5 = Param.Float(Parent.thres_4_to_5 ,"energy state machine threshold")