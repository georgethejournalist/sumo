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

SUMO_HOME = os.path.join(os.path.dirname(__file__), "..", "..", "..", "..", "..", "..")
sys.path.append(os.path.join(os.environ.get("SUMO_HOME", SUMO_HOME), "tools"))
import traci  # noqa
import sumolib  # noqa


def checkMiddleEdges():
    now = traci.simulation.getTime()
    for edge in ["middle", "middle2", "middle3"]:
        print("edge=%s current=%s adapted=%s" % (edge, traci.edge.getTraveltime(edge),
                                                 traci.edge.getAdaptedTraveltime(edge, now)))


traci.start([sumolib.checkBinary('sumo'), "-c", "sumo.sumocfg"])
vehID = "ego"
traci.simulationStep()
checkMiddleEdges()
traci.edge.adaptTraveltime("middle", 20)
traci.edge.adaptTraveltime("middle3", 10)
checkMiddleEdges()
traci.vehicle.rerouteTraveltime(vehID, False)
while traci.simulation.getMinExpectedNumber() > 0:
    traci.simulationStep()
traci.close()
