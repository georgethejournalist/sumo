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
# @date    2018-09-27

import os
import sys

sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))

import traci  # noqa
import sumolib  # noqa

sumoBinary = sumolib.checkBinary('sumo')
traci.start([sumoBinary,
             "-n", "input_net.net.xml",
             "-r", "input_routes.rou.xml",
             "--no-step-log",
             ])
vehID = "v0"
traci.vehicle.add(vehID, "r0")
traci.vehicle.setLaneChangeMode(vehID, 0)
for i in range(5):
    traci.simulationStep()
traci.vehicle.changeLaneRelative(vehID, 1, 0)
traci.simulationStep()
traci.simulationStep()
pos = traci.vehicle.getPosition(vehID)
traci.vehicle.moveToXY(vehID, "", 0, pos[0], pos[1]-3.2)
for i in range(5):
    traci.simulationStep()

traci.close()
