/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2013-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSDevice_Taxi.cpp
/// @author  Jakob Erdmann
/// @date    16.12.2019
///
// A device which controls a taxi
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <utils/common/StringUtils.h>
#include <utils/common/StaticCommand.h>
#include <utils/options/OptionsCont.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/vehicle/SUMOVehicle.h>
#include <utils/router/SUMOAbstractRouter.h>
#include <microsim/transportables/MSTransportable.h>
#include <microsim/MSEventControl.h>
#include <microsim/MSGlobals.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSEdge.h>
#include "MSDispatch.h"
#include "MSRoutingEngine.h"
#include "MSDevice_Taxi.h"

// ===========================================================================
// static member variables
// ===========================================================================
SUMOTime MSDevice_Taxi::myDispatchPeriod(0);
/// @brief the dispatch algorithm
MSDispatch* MSDevice_Taxi::myDispatcher(nullptr);
/// @brief The repeated call to the dispatcher
Command* MSDevice_Taxi::myDispatchCommand(nullptr);
// @brief the list of available taxis
std::vector<MSDevice_Taxi*> MSDevice_Taxi::myFleet;

#define TAXI_SERVICE "taxi"

// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// static initialisation methods
// ---------------------------------------------------------------------------
void
MSDevice_Taxi::insertOptions(OptionsCont& oc) {
    oc.addOptionSubTopic("Taxi Device");
    insertDefaultAssignmentOptions("taxi", "Taxi Device", oc);

    oc.doRegister("device.taxi.dispatch-algorithm", new Option_String("greedy"));
    oc.addDescription("device.taxi.dispatch-algorithm", "Taxi Device", "The dispatch algorithm [greedy|greedyClosest]");

    oc.doRegister("device.taxi.dispatch-period", new Option_String("60", "TIME"));
    oc.addDescription("device.taxi.dispatch-period", "Taxi Device", "The period between successive calls to the dispatcher");
}


void
MSDevice_Taxi::buildVehicleDevices(SUMOVehicle& v, std::vector<MSVehicleDevice*>& into) {
    OptionsCont& oc = OptionsCont::getOptions();
    if (equippedByDefaultAssignmentOptions(oc, "taxi", v, false)) {
        // build the device
        MSDevice_Taxi* device = new MSDevice_Taxi(v, "taxi_" + v.getID());
        into.push_back(device);
        myFleet.push_back(device);
        if (v.getParameter().line == "") {
            // automatically set the line so that persons are willing to enter
            // (see MSStageDriving::isWaitingFor)
            const_cast<SUMOVehicleParameter&>(v.getParameter()).line = TAXI_SERVICE;
        }
    }
}

void
MSDevice_Taxi::initDispatch() {
    OptionsCont& oc = OptionsCont::getOptions();
    myDispatchPeriod = string2time(oc.getString("device.taxi.dispatch-period"));
    // init dispatch algorithm
    std::string algo = oc.getString("device.taxi.dispatch-algorithm");
    if (algo == "greedy") {
        myDispatcher = new MSDispatch_Greedy();
    } else if (algo == "greedyClosest") {
        myDispatcher = new MSDispatch_GreedyClosest();
    } else {
        throw ProcessError("Dispatch algorithm '" + algo + "' is not known");
    }
    myDispatchCommand = new StaticCommand<MSDevice_Taxi>(&MSDevice_Taxi::triggerDispatch);
    MSNet::getInstance()->getEndOfTimestepEvents()->addEvent(myDispatchCommand);
}

void
MSDevice_Taxi::addReservation(MSTransportable* person,
        const std::set<std::string>& lines,
        SUMOTime reservationTime, 
        SUMOTime pickupTime,
        const MSEdge* from, double fromPos,
        const MSEdge* to, double toPos) 
{
    if (lines.size() == 1 && *lines.begin() == TAXI_SERVICE) {
        if (myDispatchCommand == nullptr) {
            initDispatch();
        }
        myDispatcher->addReservation(person, reservationTime, pickupTime, from, fromPos, to, toPos);
    }
}


SUMOTime
MSDevice_Taxi::triggerDispatch(SUMOTime currentTime) {
    myDispatcher->computeDispatch(currentTime, myFleet);
    return myDispatchPeriod;
}

bool
MSDevice_Taxi::hasServableReservations() {
    return myDispatcher != nullptr && myDispatcher->hasServableReservations();
}

void
MSDevice_Taxi::cleanup() {
    if (myDispatcher != nullptr) {
        delete myDispatcher;
        myDispatcher = nullptr;
    }
    myDispatchCommand = nullptr;
}

// ---------------------------------------------------------------------------
// MSDevice_Taxi-methods
// ---------------------------------------------------------------------------
MSDevice_Taxi::MSDevice_Taxi(SUMOVehicle& holder, const std::string& id) :
    MSVehicleDevice(holder, id),
    myServiceEnd(string2time(getStringParam(holder, OptionsCont::getOptions(), "taxi.end", toString(1e15), false)))
{
}


MSDevice_Taxi::~MSDevice_Taxi() {
    myFleet.erase(std::find(myFleet.begin(), myFleet.end(), this));
}


void
MSDevice_Taxi::dispatch(const Reservation& res) {
    if (isEmpty()) {
        if (MSGlobals::gUseMesoSim) {
            throw ProcessError("Dispatch for meso not yet implemented");
        }
        MSVehicle* veh = static_cast<MSVehicle*>(&myHolder);
        veh->abortNextStop();
        ConstMSEdgeVector tmpEdges;
        tmpEdges.push_back(myHolder.getEdge());
        tmpEdges.push_back(res.from);
        tmpEdges.push_back(res.to);
        veh->replaceRouteEdges(tmpEdges, -1, 0, "taxi:prepare_dispatch", false, false, false);

        std::string error;
        double fromPos = MAX2(res.fromPos, MIN2(myHolder.getVehicleType().getLength(), res.from->getLength()));
        SUMOVehicleParameter::Stop pickup;
        pickup.lane = getStopLane(res.from)->getID();
        pickup.startPos = res.fromPos;
        pickup.endPos = fromPos;
        pickup.triggered = true;
        //pickup.awaitedPersons.insert(res.person->getID());
        pickup.parking = true;
        pickup.index = STOP_INDEX_END;
        myHolder.addStop(pickup, error);
        if (error != "") {
            WRITE_WARNINGF("Could not add taxi stop for vehicle '%' to pick up person '%'. time=% error=%", myHolder.getID(), res.person->getID(), SIMTIME, error)
        }

        error = "";
        double toPos = MAX2(res.toPos, MIN2(myHolder.getVehicleType().getLength(), res.to->getLength()));
        SUMOVehicleParameter::Stop destination;
        destination.lane = getStopLane(res.to)->getID();
        destination.startPos = res.toPos;
        destination.endPos = toPos;
        destination.duration = TIME2STEPS(60); // pay and collect bags
        destination.triggered = true; // wait until next dispatch but let the simulation end when there are no more persons
        destination.parking = true;
        destination.index = STOP_INDEX_END;
        myHolder.addStop(destination, error);
        if (error != "") {
            WRITE_WARNINGF("Could not add taxi stop for vehicle '%' to deliver person '%'. time=% error=%", myHolder.getID(), res.person->getID(), SIMTIME, error)
        }

        SUMOAbstractRouter<MSEdge, SUMOVehicle>& router = MSRoutingEngine::getRouterTT(0);
        // SUMOAbstractRouter<MSEdge, SUMOVehicle>& router = myHolder.getInfluencer().getRouterTT(veh->getRNGIndex())
        myHolder.reroute(MSNet::getInstance()->getCurrentTimeStep(), "taxi:dispatch", router, false);
    } else {
        throw ProcessError("Dispatch for busy taxis not yet implemented");
    }
    myState = PICKUP;
    myCustomer = res.person;
}

MSLane*
MSDevice_Taxi::getStopLane(const MSEdge* edge) {
    const std::vector<MSLane*>* allowedLanes = edge->allowedLanes(myHolder.getVClass());
    if (allowedLanes == nullptr) {
        throw ProcessError("Taxi '" + myHolder.getID() + "' cannot pick up person on edge '" + edge->getID() + "'");
    }
    return allowedLanes->front();
}

bool
MSDevice_Taxi::isEmpty() {
    return myState == EMPTY;
}


bool
MSDevice_Taxi::allowsBoarding(MSTransportable* t) const {
    return t == myCustomer;
}


bool
MSDevice_Taxi::notifyMove(SUMOTrafficObject& /*tObject*/, double oldPos,
                             double newPos, double /*newSpeed*/) {
    if (myHolder.getPersonNumber() > 0) {
        myOccupiedDistance += (newPos - oldPos);
        myOccupiedTime += DELTA_T;
    }
    if (myHolder.isStopped()) {
        if (!myIsStopped) {
            // limit duration of stop
            // @note: stops are not yet added to the vehicle so we can change the loaded parameters. Stops added from a route are not affected
            if (!MSGlobals::gUseMesoSim) {
                MSVehicle& veh = static_cast<MSVehicle&>(myHolder);
                veh.getNextStop().endBoarding = myServiceEnd;
            }
        }
    }
    myIsStopped = myHolder.isStopped();
    return true; // keep the device
}


bool
MSDevice_Taxi::notifyEnter(SUMOTrafficObject& /*veh*/, MSMoveReminder::Notification /*reason*/, const MSLane* /* enteredLane */) {
    return true; // keep the device
}


bool
MSDevice_Taxi::notifyLeave(SUMOTrafficObject& /*veh*/, double /*lastPos*/, MSMoveReminder::Notification /*reason*/, const MSLane* /* enteredLane */) {
    return true; // keep the device
}

void
MSDevice_Taxi::customerEntered() {
    myState = OCCUPIED;
}


void
MSDevice_Taxi::customerArrived() {
    myCustomersServed++;
    if (myHolder.getPersonNumber() == 0) {
        myState = EMPTY;
        myCustomer = nullptr;
    }
}

void
MSDevice_Taxi::generateOutput(OutputDevice* tripinfoOut) const {
    if (tripinfoOut != nullptr) {
        tripinfoOut->openTag("taxi");
        tripinfoOut->writeAttr("customers", toString(myCustomersServed));
        tripinfoOut->writeAttr("occupiedDistance", toString(myOccupiedDistance));
        tripinfoOut->writeAttr("occupiedTime", time2string(myOccupiedTime));
        tripinfoOut->closeTag();
    }
}

std::string
MSDevice_Taxi::getParameter(const std::string& key) const {
    if (key == "customers") {
        return toString(myCustomersServed);
    } else if (key == "occupiedDistance") {
        return toString(myOccupiedDistance);
    } else if (key == "occupiedtime") {
        return toString(STEPS2TIME(myOccupiedTime));
    }
    throw InvalidArgument("Parameter '" + key + "' is not supported for device of type '" + deviceName() + "'");
}


void
MSDevice_Taxi::setParameter(const std::string& key, const std::string& value) {
    double doubleValue;
    try {
        doubleValue = StringUtils::toDouble(value);
    } catch (NumberFormatException&) {
        throw InvalidArgument("Setting parameter '" + key + "' requires a number for device of type '" + deviceName() + "'");
    }
    UNUSED_PARAMETER(doubleValue);
    throw InvalidArgument("Setting parameter '" + key + "' is not supported for device of type '" + deviceName() + "'");
}


/****************************************************************************/

