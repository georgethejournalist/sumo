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
neteditProcess, referencePosition = netedit.setupAndStart(neteditTestRoot, ['--gui-testing-debug-gl'])

# go to shape mode
netedit.shapeMode()

# go to poly mode and select poly
netedit.changeElement("poly")

# create first polygon
netedit.createSquaredPoly(referencePosition, 200, 150, 200, True)

# go to inspect mode
netedit.inspectMode()

# inspect first polygon
netedit.leftClick(referencePosition, 100, 60)

# Change parameter 6 with a non valid value
netedit.modifyAttribute(6, "dummyLayer", False)

# Change parameter 6 with a valid value (negative)
netedit.modifyAttribute(6, "-2", False)

# Change parameter 6 with a valid value (negative)
netedit.modifyAttribute(6, "2.5", False)

# Check undos and redos
netedit.undo(referencePosition, 2)
netedit.redo(referencePosition, 2)

# save shapes
netedit.saveAdditionals(referencePosition)

# save network
netedit.saveNetwork(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
