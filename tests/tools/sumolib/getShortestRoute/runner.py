#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2019 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    runner.py
# @author  Jakob Erdmann
# @date

from __future__ import absolute_import
from __future__ import print_function

import os
import sys
if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(tools)
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")
import sumolib.net  # noqa

net = sumolib.net.readNet(sys.argv[1], withInternal=True, withLatestPrograms=True)
SC = net.getEdge("SC")
CE = net.getEdge("CE")
print(net.getShortestPath(SC, CE))
print(net.getShortestPath(SC, CE, vClass="passenger"))
print(net.getShortestPath(SC, CE, vClass="bus"))
