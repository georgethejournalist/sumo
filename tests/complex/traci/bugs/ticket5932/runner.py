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
# @author  Michael Behrisch
# @date    2017-01-23


from __future__ import print_function
from __future__ import absolute_import
import os
import sys
sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import traci  # noqa
import sumolib  # noqa


traci.start([sumolib.checkBinary("sumo"), '-c', 'sumo.sumocfg'])

traci.simulationStep()
traci.vehicle.moveTo("ego", "SC_1", 25)
traci.simulationStep()
traci.vehicle.moveTo("ego", "SC_1", 1)
traci.simulationStep()
try:
    # internal lane does not originate on the vehicles route
    traci.vehicle.moveTo("ego", "CW_0", 5)
    traci.simulationStep()
except traci.TraCIException as e:
    if traci.isLibsumo():
        print(e, file=sys.stderr)
try:
    # internal lane does not originate on the vehicles route
    traci.vehicle.moveTo("ego", ":C_1_0", 5)
    traci.simulationStep()
except traci.TraCIException as e:
    if traci.isLibsumo():
        print(e, file=sys.stderr)
try:
    # internal lane does not continue the vehicles route
    traci.vehicle.moveTo("ego", ":C_10_0", 5)
    traci.simulationStep()
except traci.TraCIException as e:
    if traci.isLibsumo():
        print(e, file=sys.stderr)
traci.vehicle.moveTo("ego", ":C_11_0", 5)
traci.simulationStep()
traci.vehicle.moveTo("ego", "CN_1", 30)
traci.simulationStep()
traci.vehicle.moveTo("ego", "SC_1", 5)
# check that the rest of the route is working normally after teleporting backwards
while traci.simulation.getMinExpectedNumber() > 0:
    traci.simulationStep()
traci.close()
