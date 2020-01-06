/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEChange_Additional.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
///
// A network change in which a busStop is created or deleted
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/netelements/GNELane.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/additionals/GNEShape.h>
#include <netedit/additionals/GNEAdditional.h>
#include <netedit/demandelements/GNEDemandElement.h>
#include <netedit/GNEViewNet.h>

#include "GNEChange_Additional.h"

// ===========================================================================
// FOX-declarations
// ===========================================================================
FXIMPLEMENT_ABSTRACT(GNEChange_Additional, GNEChange, nullptr, 0)

// ===========================================================================
// member method definitions
// ===========================================================================

GNEChange_Additional::GNEChange_Additional(GNEAdditional* additional, bool forward) :
    GNEChange(additional->getViewNet()->getNet(), forward),
    myAdditional(additional),
    myParentEdges(myAdditional->getParentEdges()),
    myParentLanes(myAdditional->getParentLanes()),
    myParentShapes(myAdditional->getParentShapes()),
    myParentAdditionals(myAdditional->getParentAdditionals()),
    myParentDemandElements(myAdditional->getParentDemandElements()),
    myChildEdges(myAdditional->getChildEdges()),
    myChildLanes(myAdditional->getChildLanes()),
    myChildShapes(myAdditional->getChildShapes()),
    myChildAdditionals(myAdditional->getChildAdditionals()),
    myChildDemandElements(myAdditional->getChildDemandElements()) {
    myAdditional->incRef("GNEChange_Additional");
}


GNEChange_Additional::~GNEChange_Additional() {
    assert(myAdditional);
    myAdditional->decRef("GNEChange_Additional");
    if (myAdditional->unreferenced()) {
        // show extra information for tests
        WRITE_DEBUG("Deleting unreferenced " + myAdditional->getTagStr() + " '" + myAdditional->getID() + "'");
        // make sure that additional isn't in net before removing
        if (myNet->additionalExist(myAdditional)) {
            myNet->deleteAdditional(myAdditional, false);
            // Remove additional from parent elements
            for (const auto& i : myParentEdges) {
                i->removeChildAdditional(myAdditional);
            }
            for (const auto& i : myParentLanes) {
                i->removeChildAdditional(myAdditional);
            }
            for (const auto& i : myParentShapes) {
                i->removeChildAdditional(myAdditional);
            }
            for (const auto& i : myParentAdditionals) {
                i->removeChildAdditional(myAdditional);
            }
            for (const auto& i : myParentDemandElements) {
                i->removeChildAdditional(myAdditional);
            }
            // Remove additional from child elements
            for (const auto& i : myChildEdges) {
                i->removeParentAdditional(myAdditional);
            }
            for (const auto& i : myChildLanes) {
                i->removeParentAdditional(myAdditional);
            }
            for (const auto& i : myChildShapes) {
                i->removeChildAdditional(myAdditional);
            }
            for (const auto& i : myChildAdditionals) {
                i->removeParentAdditional(myAdditional);
            }
            for (const auto& i : myChildDemandElements) {
                i->removeParentAdditional(myAdditional);
            }
        }
        delete myAdditional;
    }
}


void
GNEChange_Additional::undo() {
    if (myForward) {
        // show extra information for tests
        WRITE_DEBUG("Removing " + myAdditional->getTagStr() + " '" + myAdditional->getID() + "' in GNEChange_Additional");
        // delete additional from net
        myNet->deleteAdditional(myAdditional, false);
        // Remove additional from parent elements
        for (const auto& i : myParentEdges) {
            i->removeChildAdditional(myAdditional);
        }
        for (const auto& i : myParentLanes) {
            i->removeChildAdditional(myAdditional);
        }
        for (const auto& i : myParentShapes) {
            i->removeChildAdditional(myAdditional);
        }
        for (const auto& i : myParentAdditionals) {
            i->removeChildAdditional(myAdditional);
        }
        for (const auto& i : myParentDemandElements) {
            i->removeChildAdditional(myAdditional);
        }
        // Remove additional from child elements
        for (const auto& i : myChildEdges) {
            i->removeParentAdditional(myAdditional);
        }
        for (const auto& i : myChildLanes) {
            i->removeParentAdditional(myAdditional);
        }
        for (const auto& i : myChildShapes) {
            i->removeChildAdditional(myAdditional);
        }
        for (const auto& i : myChildAdditionals) {
            i->removeParentAdditional(myAdditional);
        }
        for (const auto& i : myChildDemandElements) {
            i->removeParentAdditional(myAdditional);
        }
    } else {
        // show extra information for tests
        WRITE_DEBUG("Adding " + myAdditional->getTagStr() + " '" + myAdditional->getID() + "' in GNEChange_Additional");
        // insert additional into net
        myNet->insertAdditional(myAdditional);
        // add additional in parent elements
        for (const auto& i : myParentEdges) {
            i->addChildAdditional(myAdditional);
        }
        for (const auto& i : myParentLanes) {
            i->addChildAdditional(myAdditional);
        }
        for (const auto& i : myParentShapes) {
            i->addChildAdditional(myAdditional);
        }
        for (const auto& i : myParentAdditionals) {
            i->addChildAdditional(myAdditional);
        }
        for (const auto& i : myParentDemandElements) {
            i->addChildAdditional(myAdditional);
        }
        // add additional in child elements
        for (const auto& i : myChildEdges) {
            i->addParentAdditional(myAdditional);
        }
        for (const auto& i : myChildLanes) {
            i->addParentAdditional(myAdditional);
        }
        for (const auto& i : myChildShapes) {
            i->addParentAdditional(myAdditional);
        }
        for (const auto& i : myChildAdditionals) {
            i->addParentAdditional(myAdditional);
        }
        for (const auto& i : myChildDemandElements) {
            i->addParentAdditional(myAdditional);
        }
    }
    // Requiere always save additionals
    myNet->requireSaveAdditionals(true);
}


void
GNEChange_Additional::redo() {
    if (myForward) {
        // show extra information for tests
        WRITE_DEBUG("Adding " + myAdditional->getTagStr() + " '" + myAdditional->getID() + "' in GNEChange_Additional");
        // insert additional into net
        myNet->insertAdditional(myAdditional);
        // add additional in parent elements
        for (const auto& i : myParentEdges) {
            i->addChildAdditional(myAdditional);
        }
        for (const auto& i : myParentLanes) {
            i->addChildAdditional(myAdditional);
        }
        for (const auto& i : myParentShapes) {
            i->addChildAdditional(myAdditional);
        }
        for (const auto& i : myParentAdditionals) {
            i->addChildAdditional(myAdditional);
        }
        for (const auto& i : myParentDemandElements) {
            i->addChildAdditional(myAdditional);
        }
        // add additional in child elements
        for (const auto& i : myChildEdges) {
            i->addParentAdditional(myAdditional);
        }
        for (const auto& i : myChildLanes) {
            i->addParentAdditional(myAdditional);
        }
        for (const auto& i : myChildShapes) {
            i->addParentAdditional(myAdditional);
        }
        for (const auto& i : myChildAdditionals) {
            i->addParentAdditional(myAdditional);
        }
        for (const auto& i : myChildDemandElements) {
            i->addParentAdditional(myAdditional);
        }
    } else {
        // show extra information for tests
        WRITE_DEBUG("Removing " + myAdditional->getTagStr() + " '" + myAdditional->getID() + "' in GNEChange_Additional");
        // delete additional from net
        myNet->deleteAdditional(myAdditional, false);
        // Remove additional from parent elements
        for (const auto& i : myParentEdges) {
            i->removeChildAdditional(myAdditional);
        }
        for (const auto& i : myParentLanes) {
            i->removeChildAdditional(myAdditional);
        }
        for (const auto& i : myParentShapes) {
            i->removeChildAdditional(myAdditional);
        }
        for (const auto& i : myParentAdditionals) {
            i->removeChildAdditional(myAdditional);
        }
        for (const auto& i : myParentDemandElements) {
            i->removeChildAdditional(myAdditional);
        }
        // Remove additional from child elements
        for (const auto& i : myChildEdges) {
            i->removeParentAdditional(myAdditional);
        }
        for (const auto& i : myChildLanes) {
            i->removeParentAdditional(myAdditional);
        }
        for (const auto& i : myChildShapes) {
            i->removeChildAdditional(myAdditional);
        }
        for (const auto& i : myChildAdditionals) {
            i->removeParentAdditional(myAdditional);
        }
        for (const auto& i : myChildDemandElements) {
            i->removeParentAdditional(myAdditional);
        }
    }
    // Requiere always save additionals
    myNet->requireSaveAdditionals(true);
}


FXString
GNEChange_Additional::undoName() const {
    if (myForward) {
        return ("Undo create " + myAdditional->getTagStr()).c_str();
    } else {
        return ("Undo delete " + myAdditional->getTagStr()).c_str();
    }
}


FXString
GNEChange_Additional::redoName() const {
    if (myForward) {
        return ("Redo create " + myAdditional->getTagStr()).c_str();
    } else {
        return ("Redo delete " + myAdditional->getTagStr()).c_str();
    }
}
