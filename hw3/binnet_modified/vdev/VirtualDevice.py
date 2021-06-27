from m5.params import *
from MemObject import MemObject
from AtomicSimpleCPU import AtomicSimpleCPU

class VirtualDevice(MemObject):
    type = 'VirtualDevice'
    cxx_header = 'vdev/vdev.hh'
    port = SlavePort("Slave port of virtual device.")
    delay_set = Param.Clock('0t', "delay to set up the device")
    delay_cpu_interrupt = Param.Clock('1ms', "time that the cpu need to take when the device finishes its work")
    cpu = Param.BaseCPU(NULL, "The cpu of the system")
    range = Param.AddrRange('10MB', "Address range")
    energy_consumed_per_cycle_vdev = Param.Float(1, "The power-off, idle and active energy consumption, where the first one (power-off consumption) must be zero.")   
    data_bandwidth = Param.Float(1, "bandwidth, ticks per byte")
    noise_rram = Param.Float(0, "RRAM noise, standard deviation")
    load_once = Param.Bool(True, "Load weight once")