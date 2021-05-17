# -*- coding: utf-8 -*-
# Copyright (c) 2015 Mark D. Hill and David A. Wood
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met: redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer;
# redistributions in binary form must reproduce the above copyright
# notice, this list of conditions and the following disclaimer in the
# documentation and/or other materials provided with the distribution;
# neither the name of the copyright holders nor the names of its
# contributors may be used to endorse or promote products derived from
# this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# Authors: Jason Power

""" This file provides a few options to two CPU models, Minor and O3

This file allows you to set the latency and issue rate of the int div unit
and set the number of division and ALU units.

This file only provides models for Minor and O3 CPU. Minor is MyMinorCPU and O3 is MyO3CPU.

"""

from m5.objects import MinorCPU, MinorFUPool
from m5.objects import MinorDefaultIntFU, MinorDefaultIntMulFU
from m5.objects import MinorDefaultIntDivFU, MinorDefaultFloatSimdFU
from m5.objects import MinorDefaultMemFU, MinorDefaultFloatSimdFU
from m5.objects import MinorDefaultMiscFU

from m5.objects import DerivO3CPU, FUPool, FUDesc, OpDesc
from m5.objects import IntALU, IntMultDiv, FP_ALU, FP_MultDiv, ReadPort
from m5.objects import SIMD_Unit, WritePort, RdWrPort, IprPort

import options


# Note: since O3 and Minor CPU interface differs slightly, this can't be factored out
options.add_option('--alu-div-operation-latency', type=int, default=1,
                   help="Latency of integer divide operations. Default is 1")
options.add_option('--alu-div-issue-latency', type=int, default=1,
                   help="Latency of integer divide issue. Default is 1")
options.add_option('--alu-units', type=int, default=4,
                   help="Number of integer ALUs units. Default is 4")
options.add_option('--int-div-units', type=int, default=1,
                   help="Number of integer div ALUs units. Default is 1")

class MyMinorIntDiv(MinorDefaultIntDivFU):

    def __init__(self, options=None):
        super(MyMinorIntDiv, self).__init__()

        self.opLat = options.alu_div_operation_latency

        self.issueLat = options.alu_div_issue_latency


class MyFUPool(MinorFUPool):

    def __init__(self, options=None):
        super(MinorFUPool, self).__init__()

        alus = []
        for i in range(options.alu_units):
            alus.append(MinorDefaultIntFU())

        div_units = []
        for i in range(options.int_div_units):
            div_units.append(MyMinorIntDiv(options))

            # Copied from src/mem/MinorCPU.py
            self.funcUnits = [MinorDefaultIntMulFU(), 
                              MinorDefaultMemFU(), MinorDefaultMiscFU(),
                              MinorDefaultFloatSimdFU()]+alus+div_units


class MyMinorCPU(MinorCPU):

    def __init__(self, options=None):
        super(MinorCPU, self).__init__()
        
        self.executeFuncUnits = MyFUPool(options)


##############################################################################
# Out of order model

class MyIntALU(IntALU):

    def __init__(self, options=None):
        super(MyIntALU, self).__init__()

        self.count = options.alu_units

class MyIntMultDiv(FUDesc):

    opList = [ OpDesc(opClass='IntMult', opLat=3),
               OpDesc(opClass='IntDiv') ]

    def __init__(self, options=None):
        super(MyIntMultDiv, self).__init__()

        self.opList[1].opLat = options.alu_div_operation_latency

        self.count = options.int_div_units

class MyO3FUPool(FUPool):

    def __init__(self, options=None):
        super(MyO3FUPool, self).__init__()

        # Copied from src/cpu/o3/FUPool.py

        self.FUList = [ MyIntALU(options), MyIntMultDiv(options),
                ReadPort(), SIMD_Unit(), WritePort(), RdWrPort(), IprPort(),
                FP_ALU(),  FP_MultDiv() ]


class MyO3CPU(DerivO3CPU):

    def __init__(self, options=None):
        super(MyO3CPU, self).__init__()

        self.fuPool = MyO3FUPool(options)
        
        
