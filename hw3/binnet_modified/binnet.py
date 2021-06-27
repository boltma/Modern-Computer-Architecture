import m5
from m5.objects import *

import sys
import optparse

parser = optparse.OptionParser()
parser.add_option('--noise', type=float, default=0, help="Noise of RRAM array, standard deviation. Default is zero.")
parser.add_option('--load_once', default=False, dest='load_once', action='store_true', help="Flag to load weight only once. Default is false")
(options, args) = parser.parse_args()

system = System()
system.clk_domain = SrcClockDomain()
system.clk_domain.clock = '3GHz'
system.clk_domain.voltage_domain = VoltageDomain()
system.mem_mode = 'atomic'
system.mem_ranges = [AddrRange('512MB')]

#vdev
system.has_vdev = 1	
system.vdev_ranges = [AddrRange('512MB', '522MB')]
system.vaddr_vdev_ranges = [AddrRange('1000MB', '1010MB')]
###

#energy mgmt
system.energy_mgmt = EnergyMgmt()
###

#set some parameters for the CPU
system.cpu = AtomicSimpleCPU()

system.cpu.s_energy_port = system.energy_mgmt.m_energy_port

system.membus = SystemXBar()

system.cpu.icache_port = system.membus.slave
system.cpu.dcache_port = system.membus.slave

system.cpu.createInterruptController()

system.mem_ctrl = DDR3_1600_x64()
system.mem_ctrl.range = system.mem_ranges[0]
system.mem_ctrl.port = system.membus.master

system.system_port = system.membus.slave

system.vdev1 = VirtualDevice()
system.vdev1.cpu = system.cpu
system.vdev1.range = system.vdev_ranges[0]
system.vdev1.energy_consumed_per_cycle_vdev = 1
system.vdev1.delay_cpu_interrupt = '1000t'
system.vdev1.delay_set = '1000t'
system.vdev1.port = system.membus.master
system.vdev1.s_energy_port = system.energy_mgmt.m_energy_port
system.vdev1.data_bandwidth = 10000  #bandwidth, ticks per byte # 10000 = 100MB/s
system.vdev1.noise_rram = options.noise
system.vdev1.load_once = options.load_once

process = LiveProcess()
process.cmd = ['tests/test-progs/binnet/main']
if options.load_once:
     process.cmd = process.cmd + ['--load_once']
system.cpu.workload = process
system.cpu.createThreads()

root = Root(full_system = False, system = system)
m5.instantiate()

print "Beginning simulation!"
exit_event = m5.simulate()
print 'Exiting @ tick %i because %s' % (m5.curTick(), exit_event.getCause())
