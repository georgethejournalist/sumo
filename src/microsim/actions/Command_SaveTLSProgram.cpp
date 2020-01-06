/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    Command_SaveTLSProgram.cpp
/// @author  Jakob Erdmann
/// @date    18.09.2019
///
// Writes the switch times of a tls into a file when the tls switches
/****************************************************************************/
// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include "Command_SaveTLSProgram.h"
#include <microsim/traffic_lights/MSTrafficLightLogic.h>
#include <microsim/MSEventControl.h>
#include <microsim/MSNet.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/iodevices/OutputDevice.h>


// ===========================================================================
// method definitions
// ===========================================================================
Command_SaveTLSProgram::Command_SaveTLSProgram(const MSTLLogicControl::TLSLogicVariants& logics, OutputDevice& od):
    myOutputDevice(od),
    myLogics(logics) {
    MSNet::getInstance()->getEndOfTimestepEvents()->addEvent(this);
    myOutputDevice.writeXMLHeader("additional", "additional_file.xsd");
}


Command_SaveTLSProgram::~Command_SaveTLSProgram() {
    writeCurrent();
}


SUMOTime
Command_SaveTLSProgram::execute(SUMOTime /*currentTime*/) {
    const std::string& state = myLogics.getActive()->getCurrentPhaseDef().getState();
    if (myLogics.getActive()->getProgramID() != myPreviousProgramID) {
        writeCurrent();
        myPreviousProgramID = myLogics.getActive()->getProgramID();
        myTLSID = myLogics.getActive()->getID();
    }
    if (myPreviousStates.size() == 0 || myPreviousStates.back().getState() != state) {
        myPreviousStates.push_back(MSPhaseDefinition(0, state));
    }
    myPreviousStates.back().duration += DELTA_T;
    return DELTA_T;
}

void
Command_SaveTLSProgram::writeCurrent() {
    if (myPreviousStates.size() > 0) {
        myOutputDevice.openTag(SUMO_TAG_TLLOGIC);
        myOutputDevice.writeAttr(SUMO_ATTR_ID, myTLSID);
        myOutputDevice.writeAttr(SUMO_ATTR_TYPE, "static");
        myOutputDevice.writeAttr(SUMO_ATTR_PROGRAMID, myPreviousProgramID);
        // write the phases
        for (const MSPhaseDefinition& p : myPreviousStates) {
            myOutputDevice.openTag(SUMO_TAG_PHASE);
            myOutputDevice.writeAttr(SUMO_ATTR_DURATION, STEPS2TIME(p.duration));
            if (p.duration < TIME2STEPS(10)) {
                myOutputDevice.writePadding(" ");
            }
            myOutputDevice.writeAttr(SUMO_ATTR_STATE, p.getState());
            myOutputDevice.closeTag();
        }
        // write params
        myOutputDevice.closeTag();
        myPreviousStates.clear();
    }
}


/****************************************************************************/

