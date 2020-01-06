/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// activitygen module
// Copyright 2010 TUM (Technische Universitaet Muenchen, http://www.tum.de/)
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    AGFrame.cpp
/// @author  Walter Bamberger
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Mo, 13 Sept 2010
///
// Configuration of the options of ActivityGen
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#ifdef HAVE_VERSION_H
#include <version.h>
#endif

#include "AGFrame.h"
#include <utils/common/StdDefs.h>
#include <router/ROFrame.h>
#include <duarouter/RODUAFrame.h>
#include <utils/common/SystemFrame.h>
#include <utils/common/RandHelper.h>
#include <utils/options/OptionsCont.h>


// ===========================================================================
// method definitions
// ===========================================================================
void AGFrame::fillOptions() {
    OptionsCont& oc = OptionsCont::getOptions();
    // Options handling
    oc.addCallExample("--net-file <INPUT>.net.xml --stat-file <INPUT>.stat.xml --output <OUTPUT>.rou.xml --rand",
                      "generate a trips file from a stats file on a given net using arbitrary random seed");
    oc.addCallExample("--net-file <INPUT>.net.xml --stat-file <INPUT>.stat.xml --output <OUTPUT>.rou.xml --duration-d <NBR_OF_DAYS>",
                      "generate a trips file from a stats file on a given net for numerous days (with fixed random seed)");

    // Add categories and insert the standard options
    SystemFrame::addConfigurationOptions(oc);
    oc.addOptionSubTopic("Input");
    oc.addOptionSubTopic("Output");
    oc.addOptionSubTopic("Processing");
    oc.addOptionSubTopic("Time");
    SystemFrame::addReportOptions(oc);
    RandHelper::insertRandOptions();

    // Insert options
    oc.doRegister("net-file", 'n', new Option_FileName());
    oc.addSynonyme("net-file", "net");
    oc.addDescription("net-file", "Input", "Use FILE as SUMO-network to create trips for");

    oc.doRegister("stat-file", 's', new Option_FileName());
    oc.addDescription("stat-file", "Input", "Loads the SUMO-statistics FILE");

    oc.doRegister("output-file", 'o', new Option_FileName());
    oc.addSynonyme("output-file", "output", true);
    oc.addDescription("output-file", "Output", "Write generated trips to FILE");

    oc.doRegister("debug", new Option_Bool(false));
    oc.addDescription("debug", "Report",
                      "Detailed messages about every single step");

    // TODO: What time options are consistent with other parts of SUMO and
    // useful for the user?
    oc.doRegister("begin", 'b', new Option_Integer(0));
    oc.addDescription("begin", "Time", "Sets the time of beginning of the simulation during the first day (in seconds)");

    oc.doRegister("end", 'e', new Option_Integer(0));
    oc.addDescription("end", "Time", "Sets the time of ending of the simulation during the last day (in seconds)");

    oc.doRegister("duration-d", new Option_Integer(1));
    oc.addDescription("duration-d", "Time", "Sets the duration of the simulation in days");
}


bool AGFrame::checkOptions() {
    return true;
}
