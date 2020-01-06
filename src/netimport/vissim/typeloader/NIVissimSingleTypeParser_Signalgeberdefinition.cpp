/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NIVissimSingleTypeParser_Signalgeberdefinition.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Wed, 18 Dec 2002
///
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <cassert>
#include <iostream>
#include <utils/common/StringUtils.h>
#include <utils/common/ToString.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/VectorHelper.h>
#include "../NIImporter_Vissim.h"
#include "../tempstructs/NIVissimTL.h"
#include "NIVissimSingleTypeParser_Signalgeberdefinition.h"


// ===========================================================================
// method definitions
// ===========================================================================
NIVissimSingleTypeParser_Signalgeberdefinition::NIVissimSingleTypeParser_Signalgeberdefinition(NIImporter_Vissim& parent)
    : NIImporter_Vissim::VissimSingleTypeParser(parent) {}


NIVissimSingleTypeParser_Signalgeberdefinition::~NIVissimSingleTypeParser_Signalgeberdefinition() {}


bool
NIVissimSingleTypeParser_Signalgeberdefinition::parse(std::istream& from) {
    //
    int id;
    from >> id;
    //
    std::string tag, name;
    tag = myRead(from);
    if (tag == "name") {
        name = readName(from);
        tag = myRead(from);
    }
    // skip optional "Beschriftung"
    tag = overrideOptionalLabel(from, tag);
    //
    int lsaid;
    std::vector<int> groupids;
    if (tag == "lsa") {
        int groupid;
        from >> lsaid; // type-checking is missing!
        from >> tag; // "Gruppe"
        do {
            from >> groupid;
            groupids.push_back(groupid);
            tag = myRead(from);
        } while (tag == "oder");
        //
    } else {
        from >> tag; // strecke
        WRITE_WARNING("Omitting unknown traffic light.");
        return true;
    }
    if (tag == "typ") {
        from >> tag; // typ-value
        from >> tag; // "ort"
    }

    //
    from >> tag;
    int edgeid;
    from >> edgeid;

    from >> tag;
    int laneno;
    from >> laneno;

    from >> tag;
    double position;
    from >> position;
    //
    while (tag != "fahrzeugklassen") {
        tag = myRead(from);
    }
    std::vector<int> assignedVehicleTypes = parseAssignedVehicleTypes(from, "N/A");
    //
    NIVissimTL::dictionary(lsaid); // !!! check whether someting is really done here
    NIVissimTL::NIVissimTLSignal* signal =
        new NIVissimTL::NIVissimTLSignal(id, name, groupids, edgeid,
                                         laneno, position, assignedVehicleTypes);
    if (!NIVissimTL::NIVissimTLSignal::dictionary(lsaid, id, signal)) {
        throw 1; // !!!
    }
    return true;
}



/****************************************************************************/

