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

# Rebuild network
netedit.rebuildNetwork()

# go to delete mode
netedit.selectTLSMode()

# select first junction
netedit.leftClick(referencePosition, 95, 225)

# create TLS
netedit.createTLS()

# select second junction
netedit.leftClick(referencePosition, 325, 225)

# create TLS
netedit.createTLS()

# select thrid junction
netedit.leftClick(referencePosition, 550, 225)

# create TLS
netedit.createTLS()

# TLS has to be saved using shortcut

# check undo redo
netedit.undo(referencePosition, 3)
netedit.redo(referencePosition, 3)

# save network
netedit.saveNetwork(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
