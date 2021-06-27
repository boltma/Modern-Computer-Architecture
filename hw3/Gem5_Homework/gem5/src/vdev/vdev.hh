//
// Created by lf-z on 4/24/17.
//

#ifndef GEM5_VDEV_HH
#define GEM5_VDEV_HH

#include <fstream>

#include <string>
#include <vector>
#include "mem/mem_object.hh"
#include "cpu/base.hh"
#include "params/VirtualDevice.hh"

#include "./SimpleCNN/ai_chip.h"

class VirtualDevice : public MemObject
{
protected:
    /** Id of the virtual device */
    uint32_t id;

private:

    class DevicePort : public SlavePort
    {
    private:
        VirtualDevice* vdev;

    public:
        DevicePort(const std::string& _name, VirtualDevice* _vdev);

    protected:
        Tick recvAtomic(PacketPtr pkt);
        void recvFunctional(PacketPtr pkt);
        bool recvTimingReq(PacketPtr pkt);
        void recvRespRetry();
        AddrRangeList getAddrRanges() const;

    };

    DevicePort port;

public:

    typedef VirtualDeviceParams Params;
    const Params *params() const
    {
        return reinterpret_cast<const Params *>(_params);
    }
    VirtualDevice(const Params *p);
    virtual ~VirtualDevice() {}
    virtual void init();

    /** Flags for the first byte in the memory. */
    static const uint8_t VDEV_SET = 0x01;
    static const uint8_t VDEV_WORK = 0x02;
    static const uint8_t VDEV_READY = 0x04;
    static const uint8_t VDEV_FINISH = 0x08;   

    /** Method to trigger an interrupt after task finishes. */
    void triggerInterrupt();
    /** Simple method to access data. */
    Tick access(PacketPtr pkt);
    /** Record the execution state and energy consumption. **/
    void tick();
    /** Handle energy state changes. */
    virtual int handleMsg(const EnergyMsg &msg);
    /** Method for python scripts to get port. */
    BaseSlavePort& getSlavePort(const std::string& if_name,
                                PortID idx = InvalidPortID);
    /** Method to get addr range. */
    AddrRange getAddrRange() const;

    /** Methods to handle packet. */
    Tick recvAtomic(PacketPtr pkt);
    void recvFunctional(PacketPtr pkt);
    bool recvTimingReq(PacketPtr pkt);
    void recvRespRetry();

    /** State related parameters **/
    enum State {
        STATE_POWEROFF = 0,
        STATE_IDLE = 1,
        STATE_ACTIVE = 2
    };

    int access_time = 0;
protected:
    /*AI chip unit*/
    AIChip aichip;

    BaseCPU *cpu;
    /** Address range of the virtual device*/
    AddrRange range;
    /** Trace of the vdev, used to determine whether a task has succeeded */
    std::vector<std::string> trace;
    /** The delay for the CPU to set the vdev */
    Tick delay_set;
    /** The delay for the vdev to finish its task */
    Tick delay_self;
    /** The delay for the CPU to handle the interrupt caused by the vdev */
    Tick delay_cpu_interrupt;
    /** Execution states of vdev : [OFF, IDLE, ACTIVE]; **/
    State execution_state = STATE_POWEROFF;

    EventWrapper<VirtualDevice, &VirtualDevice::triggerInterrupt> event_interrupt;

    /** status register for the vdev.
     * |-high-----------------------low-|
     * |-4 bits-|finish|ready|work|set| */
    uint8_t pmem = VDEV_READY | VDEV_FINISH;
    
    double data_bandwidth;
};

#endif //GEM5_VDEV_HH
