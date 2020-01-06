#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2019 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    runner.py
# @author  Jakob Erdmann
# @date    2017-01-23


from __future__ import print_function
from __future__ import absolute_import
import os
import sys

sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import traci  # noqa
import sumolib  # noqa


def check(vehID, steps=1):
    for i in range(steps):
        if i > 0:
            traci.simulationStep()
        try:
            print("%s vehicle %s on lane=%s pos=%s speed=%s" % (
                traci.simulation.getCurrentTime() / 1000.0,
                vehID,
                traci.vehicle.getLaneID(vehID),
                traci.vehicle.getLanePosition(vehID),
                traci.vehicle.getSpeed(vehID)))
        except traci.TraCIException:
            pass
    if steps > 1:
        print()


traci.start([sumolib.checkBinary('sumo'), "-c", "sumo.sumocfg",
             '--fcd-output', 'fcd.xml',
             '--fcd-output.signals'])
vehID = "v0"
traci.simulationStep()
traci.route.add("beg", ["beg"])
traci.vehicle.addLegacy(vehID, "beg")
traci.vehicle.setSpeed(vehID, 0)
for i in range(3):
    traci.simulationStep()
traci.vehicle.setSignals(vehID, 2048)
for i in range(3):
    traci.simulationStep()
traci.vehicle.setSignals(vehID, -1)
for i in range(4):
    traci.simulationStep()
traci.close()
