#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2019 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    test.py
# @author  Pablo Alvarez Lopez
# @date    2016-11-25

# import common functions for netedit tests
import os
import sys

testRoot = os.path.join(os.environ.get('SUMO_HOME', '.'), 'tests')
neteditTestRoot = os.path.join(
    os.environ.get('TEXTTEST_HOME', testRoot), 'netedit')
sys.path.append(neteditTestRoot)
import neteditTestFunctions as netedit  # noqa

# Open netedit
neteditProcess, referencePosition = netedit.setupAndStart(neteditTestRoot)

# go to inspect mode
netedit.inspectMode()

# inspect edge
netedit.leftClick(referencePosition, 300, 180)

# Change parameter lengt
netedit.modifyAttribute(12, "25", False)

# inspect edge
netedit.leftClick(referencePosition, 300, 280)

# Change parameter lengt
netedit.modifyAttribute(12, "25", False)

# recompute
netedit.rebuildNetwork()

# save additionals
netedit.saveAdditionals(referencePosition)

# fix stopping place positions
netedit.fixStoppingPlace("fixPositions")

# save network
netedit.saveNetwork(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
