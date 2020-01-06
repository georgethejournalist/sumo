/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSInductLoop.cpp
/// @author  Christian Roessel
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @date    2004-11-23
///
// An unextended detector measuring at a fixed position on a fixed lane.
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include "MSInductLoop.h"
#include <cassert>
#include <numeric>
#include <utility>
#include <utils/common/WrappingCommand.h>
#include <utils/common/ToString.h>
#include <microsim/MSEventControl.h>
#include <microsim/MSLane.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSNet.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/StringUtils.h>
#include <utils/iodevices/OutputDevice.h>

#define HAS_NOT_LEFT_DETECTOR -1

// ===========================================================================
// method definitions
// ===========================================================================
MSInductLoop::MSInductLoop(const std::string& id, MSLane* const lane,
                           double positionInMeters,
                           const std::string& vTypes) :
    MSMoveReminder(id, lane),
    MSDetectorFileOutput(id, vTypes),
    myPosition(positionInMeters),
    myLastLeaveTime(SIMTIME),
    myVehicleDataCont(),
    myVehiclesOnDet() {
    assert(myPosition >= 0 && myPosition <= myLane->getLength());
    reset();
}


MSInductLoop::~MSInductLoop() {
}


void
MSInductLoop::reset() {
    myEnteredVehicleNumber = 0;
    myLastVehicleDataCont = myVehicleDataCont;
    myVehicleDataCont.clear();
}


bool
MSInductLoop::notifyEnter(SUMOTrafficObject& veh, Notification reason, const MSLane* /* enteredLane */) {
    if (!vehicleApplies(veh)) {
        return false;
    }
    if (reason == NOTIFICATION_DEPARTED ||
            reason == NOTIFICATION_TELEPORT ||
            reason == NOTIFICATION_PARKING ||
            reason == NOTIFICATION_LANE_CHANGE) {
        if (veh.getPositionOnLane() >= myPosition && veh.getBackPositionOnLane(myLane) < myPosition) {
            myVehiclesOnDet.insert(std::make_pair(&veh, SIMTIME));
            myEnteredVehicleNumber++;
        }
    }
    return true;
}


bool
MSInductLoop::notifyMove(SUMOTrafficObject& veh, double oldPos,
                         double newPos, double newSpeed) {
    if (newPos < myPosition) {
        // detector not reached yet
        return true;
    }
#ifdef HAVE_FOX
    FXConditionalLock lock(myNotificationMutex, MSGlobals::gNumSimThreads > 1);
#endif
    const double oldSpeed = veh.getPreviousSpeed();
    if (newPos >= myPosition && oldPos < myPosition) {
        // entered the detector by move
        const double timeBeforeEnter = MSCFModel::passingTime(oldPos, myPosition, newPos, oldSpeed, newSpeed);
        double entryTime = SIMTIME + timeBeforeEnter;
        enterDetectorByMove(veh, entryTime);
    }
    double oldBackPos = oldPos - veh.getVehicleType().getLength();
    double newBackPos = newPos - veh.getVehicleType().getLength();
    if (newBackPos > myPosition) {
        // vehicle passed the detector (it may have changed onto this lane somewhere past the detector)
        // assert(!MSGlobals::gSemiImplicitEulerUpdate || newSpeed > 0 || myVehiclesOnDet.find(&veh) == myVehiclesOnDet.end());
        // assertion is invalid in case of teleportation
        if (oldBackPos <= myPosition) {
            const double timeBeforeLeave = MSCFModel::passingTime(oldBackPos, myPosition, newBackPos, oldSpeed, newSpeed);
            const double leaveTime = SIMTIME + timeBeforeLeave;
            leaveDetectorByMove(veh, leaveTime);
        } else {
            // vehicle is already beyond the detector...
            // This can happen even if it is still registered in myVehiclesOnDet, e.g., after teleport.
            // XXX: would we need to call leaveDetectorByMove(veh, leaveTime) as it was done before
            //      I inserted this if-else differentiation? (Leo) It seems that such a call only resets
            //      the last leave Time, which seems inadequate to do for such a situation (though it actually
            //      appears in test output/e1/one_vehicle/lane_change). Moreover, if the vehicle was
            //      not removed, this call would tidy up.
            // XXX: Indeed, we need to tidy up, e.g., in case of teleport insertion behind detector
            // XXX: As a quickfix we just remove it. (should be discussed! Leo) Refs. #2579

            myVehiclesOnDet.erase(&veh);
        }
        return false;
    }
    // vehicle stays on the detector
    return true;
}


bool
MSInductLoop::notifyLeave(SUMOTrafficObject& veh, double lastPos, MSMoveReminder::Notification reason, const MSLane* /* enteredLane */) {
    if (reason != MSMoveReminder::NOTIFICATION_JUNCTION) {
        leaveDetectorByLaneChange(veh, lastPos);
        return false;
    }
    return true;
}


double
MSInductLoop::getSpeed(const int offset) const {
    const std::vector<VehicleData>& d = collectVehiclesOnDet(SIMSTEP - offset);
    return d.empty() ? -1. : std::accumulate(d.begin(), d.end(), 0.0, speedSum) / (double) d.size();
}


double
MSInductLoop::getVehicleLength(const int offset) const {
    const std::vector<VehicleData>& d = collectVehiclesOnDet(SIMSTEP - offset);
    return d.empty() ? -1. : std::accumulate(d.begin(), d.end(), 0.0, lengthSum) / (double)d.size();
}


double
MSInductLoop::getOccupancy() const {
    const SUMOTime tbeg = SIMSTEP - DELTA_T;
    double occupancy = 0;
    const double csecond = SIMTIME;
    for (const VehicleData& i : collectVehiclesOnDet(tbeg)) {
        const double leaveTime = i.leaveTimeM == HAS_NOT_LEFT_DETECTOR ? csecond : MIN2(i.leaveTimeM, csecond);
        const double entryTime = MAX2(i.entryTimeM, STEPS2TIME(tbeg));
        occupancy += MIN2(leaveTime - entryTime, TS);
    }
    return occupancy / TS * 100.;
}


double
MSInductLoop::getPassedNumber(const int offset) const {
    return (double)collectVehiclesOnDet(SIMSTEP - offset).size();
}


std::vector<std::string>
MSInductLoop::getVehicleIDs(const int offset) const {
    const std::vector<VehicleData>& d = collectVehiclesOnDet(SIMSTEP - offset);
    std::vector<std::string> ret;
    for (const VehicleData& i : d) {
        ret.push_back(i.idM);
    }
    return ret;
}


double
MSInductLoop::getTimeSinceLastDetection() const {
    if (myVehiclesOnDet.size() != 0) {
        // detector is occupied
        return 0;
    }
    return SIMTIME - myLastLeaveTime;
}


SUMOTime
MSInductLoop::getLastDetectionTime() const {
    if (myVehiclesOnDet.size() != 0) {
        return MSNet::getInstance()->getCurrentTimeStep();
    }
    return TIME2STEPS(myLastLeaveTime);
}


void
MSInductLoop::writeXMLDetectorProlog(OutputDevice& dev) const {
    dev.writeXMLHeader("detector", "det_e1_file.xsd");
}


void
MSInductLoop::writeXMLOutput(OutputDevice& dev,
                             SUMOTime startTime, SUMOTime stopTime) {
    const double t(STEPS2TIME(stopTime - startTime));
    const double flow = ((double)myVehicleDataCont.size() / t) * (double) 3600.0;
    double occupancy = 0.;
    double speedSum = 0.;
    double lengthSum = 0.;
    // to approximate the space mean speed
    double inverseSpeedSum = 0.;
    for (std::deque< VehicleData >::const_iterator i = myVehicleDataCont.begin(); i != myVehicleDataCont.end(); ++i) {
        const double timeOnDetDuringInterval = i->leaveTimeM - MAX2(STEPS2TIME(startTime), i->entryTimeM);
        occupancy += MIN2(timeOnDetDuringInterval, t);
        speedSum += i->speedM;
        assert(i->speedM > 0);
        inverseSpeedSum += 1. / i->speedM;
        lengthSum += i->lengthM;
    }
    for (std::map< SUMOTrafficObject*, double >::const_iterator i = myVehiclesOnDet.begin(); i != myVehiclesOnDet.end(); ++i) {
        occupancy += STEPS2TIME(stopTime) - MAX2(STEPS2TIME(startTime), i->second);
    }
    occupancy = occupancy / t * (double) 100.;
    const double meanSpeed = myVehicleDataCont.size() != 0 ? speedSum / (double)myVehicleDataCont.size() : -1;
    const double harmonicMeanSpeed = myVehicleDataCont.size() != 0 ? (double)myVehicleDataCont.size() / inverseSpeedSum : -1;
    const double meanLength = myVehicleDataCont.size() != 0 ? lengthSum / (double)myVehicleDataCont.size() : -1;
    dev.openTag(SUMO_TAG_INTERVAL).writeAttr(SUMO_ATTR_BEGIN, STEPS2TIME(startTime)).writeAttr(SUMO_ATTR_END, STEPS2TIME(stopTime));
    dev.writeAttr(SUMO_ATTR_ID, StringUtils::escapeXML(getID())).writeAttr("nVehContrib", myVehicleDataCont.size());
    dev.writeAttr("flow", flow).writeAttr("occupancy", occupancy).writeAttr("speed", meanSpeed).writeAttr("harmonicMeanSpeed", harmonicMeanSpeed);
    dev.writeAttr("length", meanLength).writeAttr("nVehEntered", myEnteredVehicleNumber).closeTag();
    reset();
}


void
MSInductLoop::enterDetectorByMove(SUMOTrafficObject& veh, double entryTimestep) {
    myVehiclesOnDet.insert(std::make_pair(&veh, entryTimestep));
    myEnteredVehicleNumber++;
}


void
MSInductLoop::leaveDetectorByMove(SUMOTrafficObject& veh, double leaveTimestep) {
    std::map<SUMOTrafficObject*, double>::iterator it = myVehiclesOnDet.find(&veh);
    if (it != myVehiclesOnDet.end()) {
        const double entryTimestep = it->second;
        myVehiclesOnDet.erase(it);
        assert(entryTimestep < leaveTimestep);
        myVehicleDataCont.push_back(VehicleData(veh.getID(), veh.getVehicleType().getLength(), entryTimestep, leaveTimestep, veh.getVehicleType().getID()));
    }
    // XXX: why is this outside the conditional block? (Leo)
    myLastLeaveTime = leaveTimestep;
}


void
MSInductLoop::leaveDetectorByLaneChange(SUMOTrafficObject& veh, double /* lastPos */) {
    // Discard entry data
    myVehiclesOnDet.erase(&veh);
}


std::vector<MSInductLoop::VehicleData>
MSInductLoop::collectVehiclesOnDet(SUMOTime tMS, bool leaveTime) const {
    const double t = STEPS2TIME(tMS);
    std::vector<VehicleData> ret;
    for (const VehicleData& i : myVehicleDataCont) {
        if (i.entryTimeM >= t || (leaveTime && i.leaveTimeM >= t)) {
            ret.push_back(i);
        }
    }
    for (const VehicleData& i : myLastVehicleDataCont) {
        if (i.entryTimeM >= t || (leaveTime && i.leaveTimeM >= t)) {
            ret.push_back(i);
        }
    }
    for (const auto& i : myVehiclesOnDet) {
        if (i.second >= t || leaveTime) { // no need to check leave time, they are still on the detector
            SUMOTrafficObject* const v = i.first;
            VehicleData d(v->getID(), v->getVehicleType().getLength(), i.second, HAS_NOT_LEFT_DETECTOR, v->getVehicleType().getID());
            d.speedM = v->getSpeed();
            ret.push_back(d);
        }
    }
    return ret;
}


/****************************************************************************/
