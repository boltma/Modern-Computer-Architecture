import m5
from m5.objects import *

system = System()
system.clk_domain = SrcClockDomain()
system.clk_domain.clock = '1MHz'
system.clk_domain.voltage_domain = VoltageDomain()
system.mem_mode = 'atomic'
system.mem_ranges = [AddrRange('512MB')]
system.has_vdev = 1
system.vdev_ranges = [AddrRange('512MB', '513MB')]
system.vaddr_vdev_ranges = [AddrRange('1000MB', '1001MB')]

system.energy_mgmt = EnergyMgmt(path_energy_profile = 'profile/energy_prof',
				energy_time_unit = '10us')
system.energy_mgmt.state_machine = TwoThresSM(thres_high = 20000, 
						thres_low = 10000)

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

system.vdev = VirtualDevice()
system.vdev.cpu = system.cpu
system.vdev.range = system.vdev_ranges[0]
system.vdev.delay_self = '10ms'
system.vdev.delay_cpu_interrupt = '100us'
system.vdev.delay_set = '200us'
system.vdev.delay_recover = '100us'
system.vdev.is_interruptable = 0
system.vdev.port = system.membus.master
system.vdev.s_energy_port = system.energy_mgmt.m_energy_port

process = LiveProcess()
process.cmd = ['tests/test-progs/vdev/bin/arm/linux/vdev']
system.cpu.workload = process
system.cpu.createThreads()


root = Root(full_system = False, system = system)
m5.instantiate()

print "Beginning simulation!"
exit_event = m5.simulate()
print 'Exiting @ tick %i because %s' % (m5.curTick(), exit_event.getCause())

