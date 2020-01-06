/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSDevice_Emissions.cpp
/// @author  Daniel Krajzewicz
/// @author  Laura Bieker
/// @author  Michael Behrisch
/// @date    Fri, 30.01.2009
///
// A device which collects vehicular emissions
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <microsim/MSNet.h>
#include <microsim/MSLane.h>
#include <microsim/MSVehicleControl.h>
#include <utils/options/OptionsCont.h>
#include <utils/emissions/PollutantsInterface.h>
#include <utils/iodevices/OutputDevice.h>
#include "MSDevice_Emissions.h"


// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// static initialisation methods
// ---------------------------------------------------------------------------
void
MSDevice_Emissions::insertOptions() {
    insertDefaultAssignmentOptions("emissions", "Emissions", OptionsCont::getOptions());
}


void
MSDevice_Emissions::buildVehicleDevices(SUMOVehicle& v, std::vector<MSVehicleDevice*>& into) {
    if (equippedByDefaultAssignmentOptions(OptionsCont::getOptions(), "emissions", v, false)) {
        // build the device
        MSDevice_Emissions* device = new MSDevice_Emissions(v, "emissions_" + v.getID());
        into.push_back(device);
    }
}


// ---------------------------------------------------------------------------
// MSDevice_Emissions-methods
// ---------------------------------------------------------------------------
MSDevice_Emissions::MSDevice_Emissions(SUMOVehicle& holder, const std::string& id)
    : MSVehicleDevice(holder, id), myEmissions() {
}


MSDevice_Emissions::~MSDevice_Emissions() {
}


bool
MSDevice_Emissions::notifyMove(SUMOTrafficObject& veh, double /*oldPos*/, double /*newPos*/, double newSpeed) {
    const SUMOEmissionClass c = veh.getVehicleType().getEmissionClass();
    const double a = veh.getAcceleration();
    const double slope = veh.getSlope();
    myEmissions.addScaled(PollutantsInterface::computeAll(c, newSpeed, a, slope,
                          static_cast<const SUMOVehicle&>(veh).getEmissionParameters()), TS);
    return true;
}


void
MSDevice_Emissions::notifyMoveInternal(const SUMOTrafficObject& veh,
                                       const double /* frontOnLane */,
                                       const double timeOnLane,
                                       const double /* meanSpeedFrontOnLane */,
                                       const double meanSpeedVehicleOnLane,
                                       const double /* travelledDistanceFrontOnLane */,
                                       const double /* travelledDistanceVehicleOnLane */,
                                       const double /* meanLengthOnLane */) {

    // called by meso (see MSMeanData_Emissions::MSLaneMeanDataValues::notifyMoveInternal)
    const double a = veh.getAcceleration();
    myEmissions.addScaled(PollutantsInterface::computeAll(veh.getVehicleType().getEmissionClass(),
                          meanSpeedVehicleOnLane, a, veh.getSlope(),
                          static_cast<const SUMOVehicle&>(veh).getEmissionParameters()), timeOnLane);
}



void
MSDevice_Emissions::generateOutput(OutputDevice* tripinfoOut) const {
    if (tripinfoOut != nullptr) {
        const int precision = MAX2(6, gPrecision);
        tripinfoOut->openTag("emissions");
        tripinfoOut->writeAttr("CO_abs", OutputDevice::realString(myEmissions.CO, precision));
        tripinfoOut->writeAttr("CO2_abs", OutputDevice::realString(myEmissions.CO2, precision));
        tripinfoOut->writeAttr("HC_abs", OutputDevice::realString(myEmissions.HC, precision));
        tripinfoOut->writeAttr("PMx_abs", OutputDevice::realString(myEmissions.PMx, precision));
        tripinfoOut->writeAttr("NOx_abs", OutputDevice::realString(myEmissions.NOx, precision));
        tripinfoOut->writeAttr("fuel_abs", OutputDevice::realString(myEmissions.fuel, precision));
        tripinfoOut->writeAttr("electricity_abs", OutputDevice::realString(myEmissions.electricity, precision));
        tripinfoOut->closeTag();
    }
}


/****************************************************************************/

