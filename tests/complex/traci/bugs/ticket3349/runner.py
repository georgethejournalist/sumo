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
sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import traci  # noqa
import sumolib  # noqa
import traci.constants as tc  # noqa


traci.start([sumolib.checkBinary("sumo"), '-n', 'input_net.net.xml', '-r', 'input_routes.rou.xml', '--no-step-log', '--begin', '5', '-S', '-Q'])
vehID = "v0"
traci.vehicle.add(vehID, "r0")
traci.vehicle.subscribeContext(vehID, tc.CMD_GET_VEHICLE_VARIABLE,
                               dist=20, begin=0, end=99999999)
traci.simulationStep(10.)
traci.close()
