/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2007-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSDispatch.cpp
/// @author  Jakob Erdmann
/// @date    16.12.2019
///
// An algorithm that performs dispatch for the taxi device
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <limits>
#include <microsim/MSNet.h>
#include <microsim/MSEdge.h>
#include <microsim/transportables/MSTransportable.h>
#include "MSRoutingEngine.h"
#include "MSDispatch.h"

//#define DEBUG_RESERVATION
//#define DEBUG_TRAVELTIME
//#define DEBUG_DISPATCH
//#define DEBUG_SERVABLE
//#define DEBUG_COND2(obj) (obj->getID() == "p0")
#define DEBUG_COND2(obj) (true)

// ===========================================================================
// MSDispatch methods
// ===========================================================================

void
MSDispatch::addReservation(MSTransportable* person,
        SUMOTime reservationTime, 
        SUMOTime pickupTime,
        const MSEdge* from, double fromPos,
        const MSEdge* to, double toPos) 
{
    myReservations.push_back(Reservation(person, reservationTime, pickupTime, from, fromPos, to, toPos));
    myHasServableReservations = true;
#ifdef DEBUG_RESERVATION
    if (DEBUG_COND2(person)) std::cout << SIMTIME 
            << " addReservation p=" << person->getID() 
            << " rT=" << time2string(reservationTime)
            << " pT=" << time2string(pickupTime)
            << " from=" << from->getID() << " fromPos=" << fromPos
            << " to=" << to->getID() << " toPos=" << toPos
            << "\n";
#endif
}


SUMOTime
MSDispatch::computePickupTime(SUMOTime t, const MSDevice_Taxi* taxi, const Reservation& res, SUMOAbstractRouter<MSEdge, SUMOVehicle>& router) {
    ConstMSEdgeVector edges;
    if (taxi->getHolder().getEdge() != res.from || taxi->getHolder().getPositionOnLane() < res.fromPos) {
        router.compute(taxi->getHolder().getEdge(), res.from, &taxi->getHolder(), t, edges);
    } else {
        router.computeLooped(taxi->getHolder().getEdge(), res.from, &taxi->getHolder(), t, edges);
    }
    return TIME2STEPS(router.recomputeCosts(edges, &taxi->getHolder(), t));
}

// ===========================================================================
// MSDispatch_Greedy methods
// ===========================================================================

void
MSDispatch_Greedy::computeDispatch(SUMOTime now, const std::vector<MSDevice_Taxi*>& fleet) {
    int numDispatched = 0;
    int numPostponed = 0;
    // find available vehicles
    std::set<MSDevice_Taxi*> available;
    for (auto* taxi : fleet) {
        if (taxi->isEmpty()) {
            available.insert(taxi);
        }
    }
#ifdef DEBUG_DISPATCH
    std::cout << SIMTIME << " computeDispatch fleet=" << fleet.size() << " available=" << available.size() << "\n";
#endif
    // greedy assign closest vehicle in reservation order
    SUMOAbstractRouter<MSEdge, SUMOVehicle>& router = myRoutingMode == 1 ? MSRoutingEngine::getRouterTT(0) : MSNet::getInstance()->getRouterTT(0);
    std::sort(myReservations.begin(), myReservations.end(), time_sorter());
    for (auto it = myReservations.begin(); it != myReservations.end();) {
        if (available.size() == 0) {
            break;
        }
        Reservation& res = *it;
        if (res.recheck > now) {
            it++;
            numPostponed++;
            continue;
        }
        //Position pos = res.from->getLanes().front()->geometryPositionAtOffset(res.fromPos);
        MSDevice_Taxi* closest = nullptr;
        SUMOTime closestTime = SUMOTime_MAX;
        bool toEarly = false;
        for (auto* taxi : available) {
            SUMOTime travelTime = computePickupTime(now, taxi, res, router);
#ifdef DEBUG_TRAVELTIME
            if (DEBUG_COND2(person)) std::cout << SIMTIME << " taxi=" << taxi->getHolder().getID() << " person=" << res.person->getID() << " traveltime=" << time2string(travelTime) << "\n";
#endif
            if (travelTime < closestTime) {
                closestTime = travelTime;
                closest = taxi;
                SUMOTime taxiWait = res.pickupTime - (now + closestTime);
                if (taxiWait > myMaximumWaitingTime) {
                    // no need to service this customer now
                    toEarly = true;
                    res.recheck += MAX2(now + myRecheckTime, res.pickupTime - closestTime - myRecheckSafety);
                    break;
                }
            }
        }
        if (toEarly) {
            it++;
            numPostponed++;
        } else {
            closest->dispatch(res);
            available.erase(closest);
            it = myReservations.erase(it);
            numDispatched++; 
#ifdef DEBUG_DISPATCH
            if (DEBUG_COND2(person)) std::cout << SIMTIME << " dispatch taxi=" << closest->getHolder().getID() << " person=" << res.person->getID() << "\n";
#endif
        }
    }
    // check if any taxis are able to service the remaining requests
    myHasServableReservations = myReservations.size() > 0 && (available.size() < fleet.size() || numPostponed > 0 || numDispatched > 0);
#ifdef DEBUG_SERVABLE
    std::cout << SIMTIME << " reservations=" << myReservations.size() << " avail=" << available.size() 
        << " fleet=" << fleet.size() << " postponed=" << numPostponed << " dispatched=" << numDispatched << "\n";
#endif
}

// ===========================================================================
// MSDispatch_GreedyClosest methods
// ===========================================================================

void
MSDispatch_GreedyClosest::computeDispatch(SUMOTime now, const std::vector<MSDevice_Taxi*>& fleet) {
    bool havePostponed = false;
    int numDispatched = 0;
    // find available vehicles
    std::set<MSDevice_Taxi*> available;
    for (auto* taxi : fleet) {
        if (taxi->isEmpty()) {
            available.insert(taxi);
        }
    }
#ifdef DEBUG_DISPATCH
    std::cout << SIMTIME << " computeDispatch fleet=" << fleet.size() << " available=" << available.size() << "\n";
#endif
    // greedy assign closest vehicle
    SUMOAbstractRouter<MSEdge, SUMOVehicle>& router = myRoutingMode == 1 ? MSRoutingEngine::getRouterTT(0) : MSNet::getInstance()->getRouterTT(0);
    std::vector<Reservation> activeReservations;
    for (Reservation& res : myReservations) {
        if (res.recheck <= now) {
            activeReservations.push_back(res);
        }
    }
    while (available.size() > 0 && activeReservations.size() > 0) {
        Reservation* closest = nullptr;
        MSDevice_Taxi* closestTaxi = nullptr;
        SUMOTime closestTime = SUMOTime_MAX;
        for (auto& res : activeReservations) {
            SUMOTime recheck = SUMOTime_MAX;
            for (auto* taxi : available) {
                SUMOTime travelTime = computePickupTime(now, taxi, res, router);
                SUMOTime taxiWait = res.pickupTime - (now + travelTime);
#ifdef DEBUG_TRAVELTIME
                if (DEBUG_COND2(person)) std::cout << SIMTIME << " taxi=" << taxi->getHolder().getID() << " person=" << res.person->getID() 
                    << " traveltime=" << time2string(travelTime) 
                    << " pickupTime=" << time2string(res.pickupTime)
                    << " taxiWait=" << time2string(taxiWait) << "\n";
#endif
                if (travelTime < closestTime) {
                    if (taxiWait < myMaximumWaitingTime) {
                        closestTime = travelTime;
                        closest = &res;
                        closestTaxi = taxi;
                    } else {
                        recheck = MIN2(recheck, 
                                MAX2(now + myRecheckTime, res.pickupTime - closestTime - myRecheckSafety));
                    }
                }
            }
            /*
            if (closestTaxi == nullptr) {
                // all taxis would arrive to early. postpone recheck
                res.recheck = recheck;
            }
            */
        }
        if (closestTaxi != nullptr) {
            closestTaxi->dispatch(*closest);
            available.erase(closestTaxi);
            Reservation tmp = *closest;
            activeReservations.erase(std::find(activeReservations.begin(), activeReservations.end(), tmp));
            myReservations.erase(std::find(myReservations.begin(), myReservations.end(), tmp));
            numDispatched++; 
#ifdef DEBUG_DISPATCH
            if (DEBUG_COND2(person)) std::cout << SIMTIME << " dispatch taxi=" << closestTaxi->getHolder().getID() << " person=" << closest->person->getID() << "\n";
#endif
        } else {
            // all current reservations are too early
            havePostponed = true;
            break;
        }
    }
    // check if any taxis are able to service the remaining requests
    myHasServableReservations = myReservations.size() > 0 && (available.size() < fleet.size() || havePostponed || numDispatched > 0);
#ifdef DEBUG_SERVABLE
    std::cout << SIMTIME << " reservations=" << myReservations.size() << " avail=" << available.size() 
        << " fleet=" << fleet.size() << " postponed=" << havePostponed << " dispatched=" << numDispatched << "\n";
#endif
}


/****************************************************************************/
